#include "sha256.h"
#include "jhash.h"
#include <string.h>
#include <stdio.h>

static void to_hex_string(char* output_buffer, const unsigned char* data, int length);

#define BUFFER_SIZE 4096

int main(int argc, const char** argv) {

    if (argc != 2) {
        printf(
            "J-HASH C TEST UTILITY\n"
            "\n"
            "   ./jhash [file]\n"
            "   Given a file will compute the jhash and print it to stdout.\n"
        );
        return 0;
    }

    FILE* f = fopen(argv[1], "rb");
    if (f == NULL) {
        printf("Couldn't read file '%s'", argv[1]);
        return 1;
    }
    
    JHASH_CTX ctx;
    jhash_init(&ctx);

    unsigned char buffer[BUFFER_SIZE];
    size_t length;
    while ((length = fread(buffer, 1, BUFFER_SIZE, f)) > 0) {
        jhash_update(&ctx, buffer, length);
    }

    char hash[JHASH_RESULT_MAX_SIZE];
    jhash_final(&ctx, hash);
    printf("%s\n", hash);

    return 0;
}

static void to_hex_string(char* output_buffer, const unsigned char* data, int length) {
    const unsigned char* data_end = data + length;
    while (data < data_end) {
        sprintf(output_buffer, "%02x", *data++);
        output_buffer += 2;
    }
}
