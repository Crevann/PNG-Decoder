#include <stdio.h>
#include <string.h>

#include "png_decoder.h"

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

    //Read IHDR chunk to get image sizes
    chunk_t *IHDR_chunk = chunks[0];
    char width_data[] = {IHDR_chunk->chunk_data[0], IHDR_chunk->chunk_data[1], IHDR_chunk->chunk_data[2], IHDR_chunk->chunk_data[3]};
    width = big_endian_to_integer(width_data, 4);
    char height_data[] = {IHDR_chunk->chunk_data[4], IHDR_chunk->chunk_data[5], IHDR_chunk->chunk_data[6], IHDR_chunk->chunk_data[7]};
    height = big_endian_to_integer(height_data, 4);
    printf("\n%d %d\n", width, height);
    
    //Gather IDAT data
    pixel_t *pixels = malloc(sizeof(pixel_t) * (width * height));
    chunk_t *IDAT_chunk = chunks[3];
    int p = 0;
    for (int i = 0; i < IDAT_chunk->length; i = i + 4){
        pixels[p].r = IDAT_chunk->chunk_data[i];
        pixels[p].g = IDAT_chunk->chunk_data[i + 1];
        pixels[p].b = IDAT_chunk->chunk_data[i + 2];
        pixels[p].a = IDAT_chunk->chunk_data[i + 3];
        printf("%d %d %d %d \n", pixels[p].r, pixels[p].g, pixels[p].b, pixels[p].a);
        p++;
    }
    
}