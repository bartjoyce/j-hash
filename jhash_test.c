#include "sha256.h"
#include "jhash.h"
#include <string.h>
#include <stdio.h>

#define BUFFER_SIZE 4096

int main(int argc, const char** argv) {

    if (argc < 2 || argc > 3) {
        printf(
            "J-HASH C TEST UTILITY\n"
            "\n"
            "   ./jhash [file-in] [optional file-out]\n"
            "   Given a file will compute the jhash and print it to stdout.\n"
            "   If an additional output file is given, the entire merkle tree will be\n"
            "   written to the file."
        );
        return 0;
    }

    FILE* in = fopen(argv[1], "rb");
    if (in == NULL) {
        printf("Couldn't read file '%s'\n", argv[1]);
        return 1;
    }

    if (argc == 2) {

        // Simple version, no output file
        JHASH_CTX ctx;
        jhash_init(&ctx);

        unsigned char buffer[BUFFER_SIZE];
        size_t length;
        while ((length = fread(buffer, 1, BUFFER_SIZE, in)) > 0) {
            jhash_update(&ctx, buffer, length);
        }

        char hash[JHASH_RESULT_MAX_SIZE];
        jhash_final(&ctx, hash);
        printf("%s\n", hash);

    } else {

        // Advanced version, with output file

        FILE* out = NULL;
        if (argc == 3) {
            out = fopen(argv[2], "wb");
            if (out == NULL) {
                printf("Couldn't open file '%s' for writing\n", argv[2]);
                return 1;
            }
        }

        unsigned char input_buffer[BUFFER_SIZE];
        unsigned char output_buffer[BUFFER_SIZE];

        JHASH_CTX ctx;
        jhash_init_with_output_buffer(&ctx, output_buffer, BUFFER_SIZE);

        size_t length_in, length_out;
        while ((length_in = fread(input_buffer, 1, BUFFER_SIZE, in)) > 0) {
            jhash_update(&ctx, input_buffer, length_in);

            if ((length_out = jhash_output_buffer_read(&ctx)) > 0) {
                fwrite(output_buffer, 1, length_out, out);
            }
        }

        char hash[JHASH_RESULT_MAX_SIZE];
        jhash_final(&ctx, hash);
        printf("%s\n", hash);

        if ((length_out = jhash_output_buffer_read(&ctx)) > 0) {
            fwrite(output_buffer, 1, length_out, out);
        }
        fclose(out);

    }

    fclose(in);
    return 0;
}
