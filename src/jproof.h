/*********************************************************************
* Filename:   jproof.h
* Author:     Bartholomew Joyce
* Copyright:
* Disclaimer: This code is presented "as is" without any guarantees.
* Details:    Defines the API for the corresponding JPROOF implementation.
*********************************************************************/

#ifndef JPROOF_H
#define JPROOF_H

#include "jhash.h"

#define JHASH_MAX_LENGTH      (1024 * 2147483648) // Size must not exceed 2TiB

typedef int JPROOF_INST;

typedef struct {
    size_t length;
    size_t region_in_point;
    size_t region_out_point;
    size_t payload_length;
    unsigned char* payload;
} JPROOF_VALUE;

typedef struct {
    // General info
    size_t length;
    size_t region_in_point;
    size_t region_out_point;
    int num_hashes;
    int num_blocks_total;
    int num_blocks_region;

    // Generator state
    size_t hash_offsets[JHASH_MAX_COUNT];
    size_t head_in_point, head_size;
    size_t tail_in_point, tail_size;
} JPROOF_GENERATE_CTX;

typedef struct {
    const JPROOF_VALUE* value;

    // General info
    int num_hashes;
    int num_blocks_total;
    int num_blocks_region;
    size_t head_in_point, head_size;
    size_t tail_in_point, tail_size;

    // Verify state
    int program_size, program_counter;
    JPROOF_INST* program;
    unsigned char stack[JHASH_MAX_COUNT * SHA256_BLOCK_SIZE];
    int stack_idx;

    int state;
    SHA256_CTX sha_ctx;
    size_t input_length;

} JPROOF_VERIFY_CTX;

void jproof_generate(JPROOF_GENERATE_CTX* ctx, size_t length, size_t region_in_point, size_t region_out_point);
JPROOF_VALUE jproof_create_value(JPROOF_GENERATE_CTX* ctx);

void jproof_verify_init(JPROOF_VERIFY_CTX* ctx, const JPROOF_VALUE* value);
void jproof_verify_update(JPROOF_VERIFY_CTX* ctx, const unsigned char* data, size_t len);
int  jproof_verify_check_error(JPROOF_VERIFY_CTX* ctx);
int  jproof_verify_final(JPROOF_VERIFY_CTX* ctx, JHASH_VALUE* value);
void jproof_verify_free(JPROOF_VERIFY_CTX* ctx);

int jproof_decode(const char* string, JPROOF_VALUE* value);
char* jproof_encode(const JPROOF_VALUE* value);

#endif // JPROOF_H
