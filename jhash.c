/*********************************************************************
* Filename:   jhash.c
* Author:     Bartholomew Joyce
*********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "jhash.h"

static void jhash_push_hash(JHASH_CTX *ctx);
static void jhash_join_hashes(JHASH_CTX *ctx);

void jhash_init(JHASH_CTX *ctx) {
    sha256_init(&ctx->sha_ctx);
    ctx->length = 0;
    ctx->hash_count = 0;
}

void jhash_update(JHASH_CTX *ctx, const unsigned char* data, size_t len) {
    int space_remaining_in_block = JHASH_BLOCK_SIZE - ctx->length % JHASH_BLOCK_SIZE;

    // New data arriving fits in the current block
    if (len < space_remaining_in_block) {
        sha256_update(&ctx->sha_ctx, data, len);
        ctx->length += len;
        return;
    }

    // We have reached the end of the block, so we want to create the block

    sha256_update(&ctx->sha_ctx, data, space_remaining_in_block);
    ctx->length += space_remaining_in_block;

    const unsigned char* spillover_data = data + space_remaining_in_block;
    size_t spillover_len = len - space_remaining_in_block;

    jhash_push_hash(ctx);
    jhash_join_hashes(ctx);

    if (spillover_len > 0) {
        jhash_update(ctx, spillover_data, spillover_len);
    }
}

void jhash_final(JHASH_CTX *ctx, char* hash) {

    // Finalise the final block, if it exists
    int final_block_size = ctx->length % JHASH_BLOCK_SIZE;
    if (final_block_size > 0) {
        jhash_push_hash(ctx);
    }

    // Tag the final hash with the highest level, and join all
    ctx->hash_levels[ctx->hash_count - 1] = JHASH_MAX_COUNT;
    jhash_join_hashes(ctx);

    // Construct result
    sprintf(hash, "jh%d:%ld:", JHASH_BLOCK_SIZE, ctx->length);
    hash += strlen(hash);
    for (int i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        sprintf(hash, "%02x", ctx->hashes[i]);
        hash += 2;
    }
}

void jhash_push_hash(JHASH_CTX *ctx) {
    assert(ctx->hash_count < JHASH_MAX_COUNT);
    unsigned char* hash = &ctx->hashes[ctx->hash_count * SHA256_BLOCK_SIZE];
    sha256_final(&ctx->sha_ctx, hash);
    sha256_init(&ctx->sha_ctx);
    ctx->hash_levels[ctx->hash_count] = 1;
    ctx->hash_count += 1;
}

void jhash_join_hashes(JHASH_CTX *ctx) {
    while (ctx->hash_count > 1) {

        // Inspect the two top most hashes, where A is the top most, and B is the one below.
        int a_index = ctx->hash_count - 1;
        int b_index = ctx->hash_count - 2;
        int a_level = ctx->hash_levels[a_index];
        int b_level = ctx->hash_levels[b_index];

        // If A's level is less than B's level, we cannot join these hashes
        if (a_level < b_level) {
            break;
        }

        // Pop the two hashes off the stack, concatenate them and hash them again
        unsigned char* b_hash = &ctx->hashes[b_index * SHA256_BLOCK_SIZE];
        sha256_update(&ctx->sha_ctx, b_hash, SHA256_BLOCK_SIZE);

        unsigned char* a_hash = &ctx->hashes[a_index * SHA256_BLOCK_SIZE];
        sha256_update(&ctx->sha_ctx, a_hash, SHA256_BLOCK_SIZE);

        sha256_final(&ctx->sha_ctx, b_hash);
        sha256_init(&ctx->sha_ctx);
        ctx->hash_count--;

        ctx->hash_levels[ctx->hash_count - 1] = a_level + 1;
    }
}
