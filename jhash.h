/*********************************************************************
* Filename:   jhash.h
* Author:     Bartholomew Joyce
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding JHASH implementation.
*********************************************************************/

#ifndef JHASH_H
#define JHASH_H

#include "sha256.h"

#define JHASH_BLOCK_SIZE      1024   // JHASH uses a default block size of 1024
#define JHASH_RESULT_MAX_SIZE 87     // JHASH results are between 73 and 87 characters in length including NULL terminal
#define JHASH_MAX_COUNT       40     // A depth of 40 allows for the generation of hashes of up to 1TiB

typedef struct {
    SHA256_CTX sha_ctx;
    size_t length;
    unsigned char hashes[JHASH_MAX_COUNT * SHA256_BLOCK_SIZE];
    char hash_levels[JHASH_MAX_COUNT];
    int hash_count;

    unsigned char* output_buffer;
    size_t output_buffer_size;
    size_t output_buffer_length;
} JHASH_CTX;

void jhash_init(JHASH_CTX* ctx);
void jhash_init_with_output_buffer(JHASH_CTX* ctx, unsigned char* output_buffer, size_t output_buffer_size);
void jhash_update(JHASH_CTX* ctx, const unsigned char* data, size_t len);
void jhash_final(JHASH_CTX* ctx, char* hash);

size_t jhash_output_buffer_read(JHASH_CTX* ctx);


#endif // JHASH_H
