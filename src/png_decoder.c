#include "png_decoder.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

// Defined chunk byte sizes

#define LENGTH_BYTES 4
#define TYPE_BYTES 4
#define CRC_BYTES 4
#define BYTES_PIXEL 4

// Checks if the file is valid
int check_signature(FILE *fp, const char *signature) {
    if (check_file_existance(fp) == 1) {
        return 1;
    }

    char s_buffer[9];
    fread(s_buffer, 8, 1, fp);
    printf("Signature: %s\nHeader: %s\n", signature, s_buffer);
    if (strcmp(signature, s_buffer) == 0) {
        printf("Is a PNG file\n\n");
        return 0;
    }
    printf("File is invalid\n");
    return 1;
}

// Reads chunks, doesn't read the header
chunk_t *read_chunk(FILE *fp) {
    if (check_file_existance(fp) == 1) {
        return NULL;
    }
    // Gather length

    char *length_buffer = malloc(LENGTH_BYTES);
    unsigned int chunk_length = 0;
    fread(length_buffer, LENGTH_BYTES, 1, fp);  // Length
    chunk_length = big_endian_to_integer(length_buffer, LENGTH_BYTES);
    printf("Length: %d\n", chunk_length);

    // Gather type

    char *chunk_type = malloc(TYPE_BYTES + 1);
    fread(chunk_type, TYPE_BYTES, 1, fp);  // Type
    chunk_type[4] = '\0';
    printf("Type: %s\n", chunk_type);

    // Gather data

    unsigned char *chunk_data = malloc(chunk_length);
    fread(chunk_data, chunk_length, 1, fp);  // Data
    printf("Data: ");
    for (int i = 0; i < chunk_length; i++) {
        printf("%X ", chunk_data[i]);
    }
    printf("\n");

    // Gather CRC
    char *crc_buffer = malloc(CRC_BYTES);
    unsigned int chunk_crc = 0;
    fread(crc_buffer, CRC_BYTES, 1, fp);  // Crc
    chunk_crc = big_endian_to_integer(crc_buffer, CRC_BYTES);
    printf("CRC: %X\n\n", chunk_crc);

    // Build struct and return

    chunk_t *chunk = malloc(sizeof(chunk_t));
    chunk->length = chunk_length;
    chunk->chunk_type = chunk_type;
    chunk->chunk_data = chunk_data;
    chunk->crc = chunk_crc;
    return chunk;
}

// Check if the file exists
int check_file_existance(FILE *fp) {
    if (fp == NULL) {
        printf("%s\n", strerror(errno));
        return 1;
    }
    printf("File found\n");
    return 0;
}

// Get integer from big endian value
int big_endian_to_integer(char values[], size_t size) {
    unsigned int value = 0;
    for (int i = 0; i < size; i++) {
        value += values[i] << (size - 1 - i) * 8;
    }
    return value;
}

// Reconstruction functions

int paeth_prediction(int a, int b, int c) {
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);
    if (pa <= pb && pa <= pc) {
        return a;
    } else if (pb <= pc) {
        return b;
    } else {
        return c;
    }
};

unsigned char recon_a(unsigned char *recon, int stride, int r, int c) {
    if (c >= BYTES_PIXEL) {
        return recon[r * stride + c - BYTES_PIXEL];
    } else {
        return 0;
    }
}

unsigned char recon_b(unsigned char *recon, int stride, int r, int c) {
    if (r > 0) {
        return recon[(r - 1) * stride + c];
    } else {
        return 0;
    }
}

unsigned char recon_c(unsigned char *recon, int stride, int r, int c) {
    if (r > 0 && c >= BYTES_PIXEL) {
        return recon[(r - 1) * stride + c - BYTES_PIXEL];
    } else {
        return 0;
    }
}

static int image_graphics_init(SDL_Window **window, SDL_Renderer **renderer, SDL_Texture **texture, int image_width, int image_height, unsigned char *pixel_data) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    *window = SDL_CreateWindow("Image", 100, 100, image_width, image_height, 0);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    int width = image_width;
    int height = image_height;
    int channels;
    unsigned char *pixels = pixel_data;
    if (!pixels) {
        SDL_Log("Unable to open image");
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    SDL_Log("Image width: %d height: %d channels: %d", width, height, channels);

    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STATIC, width, height);
    if (!texture) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        free(pixels);
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        SDL_Quit();
        return -1;
    }

    SDL_UpdateTexture(*texture, NULL, pixels, width * 4);
    free(pixels);
    return 0;
}
