#include <stdio.h>

#define BYTES_PIXEL 4

typedef enum filter_types{
    NONE = 0,
    SUB,
    UP,
    AVERAGE,
    PAETH
} filter_types;

//Chunk
typedef struct chunk_t{
    unsigned int length; //4 bytes to read
    unsigned char* chunk_type; //4 bytes to read
    unsigned char* chunk_data; //data
    unsigned int crc; //4 bytes to read
} chunk_t;

//File reading

int check_file_existance(FILE*);
int check_signature(FILE*, const char*);
chunk_t* read_chunk(FILE*);

//Utilities

int big_endian_to_integer(char[], size_t);
//Pixel reconstruction

int paeth_prediction(int, int, int);
unsigned char recon_a(unsigned char*, int, int, int);
unsigned char recon_b(unsigned char*, int, int, int);
unsigned char recon_c(unsigned char*, int, int, int);
