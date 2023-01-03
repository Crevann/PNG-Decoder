#include <stdio.h>

//Chunk
typedef struct chunk_t{
    unsigned int length; //4 bytes
    char* chunk_type; //4 bytes
    char* chunk_data; //data
    unsigned int crc;
} chunk_t;

typedef struct pixel_t{
    char r;
    char g;
    char b;
    char a;
} pixel_t;

//File reading

int check_file_existance(FILE*);
int check_signature(FILE*, const char*);
chunk_t* read_chunk(FILE*);

//Utilities

int big_endian_to_integer(char[], size_t);

//Pixel reconstruction

int paeth_prediction(int, int, int);
int recon_a(int, int);
int recon_b(int, int);
int recon_c(int, int);
