/*********************************************************************
* Filename:   jproof_generate.c
* Author:     Bartholomew Joyce
*********************************************************************/

#include "jproof.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

static void traverse_node(JPROOF_GENERATE_CTX* ctx, int node_idx, int node_from, int node_to, int target_block_from, int target_block_to);
static int  largest_second_power(int value);

void jproof_generate(JPROOF_GENERATE_CTX* ctx, size_t length, size_t region_in_point, size_t region_out_point) {
    assert(length < JHASH_MAX_LENGTH);
    assert(0 < region_in_point && region_in_point < region_out_point && region_out_point <= length);

    // Prepare general state
    ctx->length = length;
    ctx->region_in_point = region_in_point;
    ctx->region_out_point = region_out_point;
    ctx->num_hashes = 0;

    ctx->num_blocks_total = length / JHASH_BLOCK_SIZE;
    if (length % JHASH_BLOCK_SIZE > 0) ctx->num_blocks_total++;

    int block_from = region_in_point  / JHASH_BLOCK_SIZE;
    int block_to   = region_out_point / JHASH_BLOCK_SIZE;
    if (block_to % JHASH_BLOCK_SIZE > 0) block_to++;
    ctx->num_blocks_region = block_to - block_from;

    ctx->head_in_point = block_from * JHASH_BLOCK_SIZE;
    ctx->head_size     = region_in_point - ctx->head_in_point;
    ctx->tail_in_point = region_out_point;
    ctx->tail_size     = block_to * JHASH_BLOCK_SIZE - region_out_point;
    if (ctx->tail_in_point + ctx->tail_size > ctx->length) {
        ctx->tail_size = ctx->length - ctx->tail_in_point;
    }

    // Traverse merkle tree, and compute all hashes that will need to be embedded in the proof
    traverse_node(ctx, 0, 0, ctx->num_blocks_total, block_from, block_to);
}

JPROOF_VALUE jproof_create_value(JPROOF_GENERATE_CTX* ctx) {
    JPROOF_VALUE value;
    value.length = ctx->length;
    value.region_in_point = ctx->region_in_point;
    value.region_out_point = ctx->region_out_point;
    value.payload_length = ctx->head_size + ctx->tail_size + ctx->num_hashes * SHA256_BLOCK_SIZE;
    value.payload = (unsigned char*)malloc(value.payload_length);
    return value;
}

void traverse_node(JPROOF_GENERATE_CTX* ctx, int node_idx, int node_from, int node_to, int target_block_from, int target_block_to) {

    if (node_to <= target_block_from || node_from >= target_block_to) {
        // This node appears either before or after the target region, push the node's hash
        int num_hashes_in_node = (node_to - node_from) * 2 - 1;
        int hash_idx = node_idx + num_hashes_in_node - 1;
        size_t hash_offset = hash_idx * SHA256_BLOCK_SIZE;
        ctx->hash_offsets[ctx->num_hashes++] = hash_offset;
        return;
    }

    if (node_from >= target_block_from && node_to <= target_block_to) {
        // The target region covers this node and all its children, so we can ignore it
        return;
    }

    // This node doesn't quite cover the target yet -> split and recurse
    int split = largest_second_power(node_to - node_from);

    int node_l_from = node_from;
    int node_l_to   = node_from + split;
    int node_l_idx  = node_idx;

    int node_r_from = node_from + split;
    int node_r_to   = node_to;
    int node_r_idx  = node_idx + (2 * split - 1);

    traverse_node(ctx, node_l_idx, node_l_from, node_l_to, target_block_from, target_block_to);
    traverse_node(ctx, node_r_idx, node_r_from, node_r_to, target_block_from, target_block_to);
}

int largest_second_power(int value) {
    // This function determines the largest power of 2 that is strictly smaller than value
    int x = 0b01000000000000000000000000000000;
    if (value <= 1 || value > x) return 0;
    while (x >= value) x >>= 1;
    return x;
}
