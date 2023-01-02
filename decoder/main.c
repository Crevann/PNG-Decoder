#include <stdio.h>
#include <string.h>

#include "png_decoder.h"

const char* path = ".\\image\\palette.png";
const char* signature = "\x89PNG\x0D\x0A\x1a\x0A";

//File testing

/*void read_file(const char* path){
    int buffer_size = 256;
    char *buffer;
    buffer = malloc(sizeof(buffer) * buffer_size);
    FILE *file = fopen(path, "r");
    char data = fread(buffer, 256, 1, file);
    printf("%s", data);
}*/

int main(int argc, char **argv){
    printf("%s\n", path);
    FILE *fp = fopen(path, "rb");
    check_signature(fp, signature);
    
    char *chunk_end = "";
    chunk_t **chunks = malloc(sizeof(chunk_t) * 4);
    
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
    
}