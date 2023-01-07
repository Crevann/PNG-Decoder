#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "png_decoder.h"
#include "zlib.h"

const char* path = ".\\image\\palette.png";
const char* signature = "\x89PNG\x0D\x0A\x1a\x0A";

//Image data

int width;
int height;
char bit_depth; //Currently not used
char color_type; //Currently not used
char compression_method; //Currently not used
char filter_method; //Currently not used
char interlace_method; //Currently not used

//Decompression

static alloc_func zalloc = (alloc_func)0;
static free_func zfree = (free_func)0;

//!WARNING! The code is temporarily hardcoded to work on palette.png
int main(int argc, char **argv){
    printf("%s\n", path);
    FILE *fp = fopen(path, "rb");
    check_signature(fp, signature);
    
    char *chunk_end = "";
    chunk_t **chunks = malloc(sizeof(chunk_t) * 4); //Temporary size, needs to be changed with a linked list for unknown chunk number
    
    int i = 0;
    //Gather chunks
    while(strcmp(chunk_end, "IEND") != 0){
        chunk_t *chunk = read_chunk(fp);
        chunks[i] = chunk;
        chunk_end = chunk->chunk_type;
        i++;
    }
    fclose(fp);

    for (int i = 0; i < 4; i++){
        printf("%s ", chunks[i]->chunk_type);
    }

    //Read IHDR chunk to get image sizes, IHDR is always the first chunk
    chunk_t *IHDR_chunk = chunks[0];
    char width_data[] = {IHDR_chunk->chunk_data[0], IHDR_chunk->chunk_data[1], IHDR_chunk->chunk_data[2], IHDR_chunk->chunk_data[3]};
    width = big_endian_to_integer(width_data, 4);
    char height_data[] = {IHDR_chunk->chunk_data[4], IHDR_chunk->chunk_data[5], IHDR_chunk->chunk_data[6], IHDR_chunk->chunk_data[7]};
    height = big_endian_to_integer(height_data, 4);
    printf("\n%d %d\n", width, height);
    
    //Gather IDAT data
    chunk_t *IDAT_chunk = chunks[2];

    //Decompression
    unsigned long uncompressed_size = ((width * height) * BYTES_PIXEL) + height;
    unsigned char* uncompressed_data = malloc(uncompressed_size);
    
    //printf("%s\n", IDAT_chunk->chunk_data);
    for (int i = 0; i < IDAT_chunk->length; i++){
        printf("%X ", IDAT_chunk->chunk_data[i]);
    }
    int result = uncompress(uncompressed_data, &uncompressed_size, IDAT_chunk->chunk_data, IDAT_chunk->length);
    if (result != Z_OK)
    {
        printf("unable to uncompress: error %d\n", result);
        free(uncompressed_data);
        return -1;
    }
    
    printf("\n");
    
    //Pixel reconstruction
    unsigned char *filtered_pixels = malloc(sizeof(unsigned char) * (width * height));
    
    int stride = width * BYTES_PIXEL;
    unsigned char filter_type;
    
    int p = 0;
    int current_pixel = 0;
    printf("\n");
    for(int r = 0; r < height; r++){
        filter_type = uncompressed_data[p];
        p++;
        for(int c = 0; c < stride; c++){
            unsigned char pixel_to_filter = uncompressed_data[p];
            p++;
            //Filtering
            unsigned char filtered_pixel;
            switch (filter_type){
            case NONE:
                filtered_pixel = uncompressed_data[p];
                break;
            case SUB:
                filtered_pixel = uncompressed_data[p] + recon_a(filtered_pixels, stride, r, c);
                break;
            case UP:
                filtered_pixel = uncompressed_data[p] + recon_b(filtered_pixels, stride, r, c);
                break;
            case AVERAGE:
                filtered_pixel = uncompressed_data[p] + (recon_a(filtered_pixels, stride, r, c) + recon_b(filtered_pixels, stride, r, c)) / 2;
                break;
            case PAETH:
                filtered_pixel = uncompressed_data[p] + paeth_prediction(recon_a(filtered_pixels, stride, r, c), recon_b(filtered_pixels, stride, r, c), recon_c(filtered_pixels, stride, r, c));
                break;
            default:
                printf("No filtering type, cope");
                break;
            }
            filtered_pixels[current_pixel] = filtered_pixel & 0xff;
            printf("%d ", filtered_pixels[current_pixel]);
            current_pixel++;
        }
    }

    //Draw pixels
    
}