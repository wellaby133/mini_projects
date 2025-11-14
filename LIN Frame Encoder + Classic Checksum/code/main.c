#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "lin_checksum.h"

int main()
{
    const char *default_filename = "inputs.txt";
    const char *in_filename = default_filename;
    bool write_out = false;
    bool enhanced = false; /* enhanced checksum includes PID */

    /* Simple command-line parsing: [program] [input_file] [--mode enhanced] [--write] */
    for (int i = 1; i < __argc; ++i) {
        if (strcmp(__argv[i], "--write") == 0) {
            write_out = true;
        } else if (strcmp(__argv[i], "--mode") == 0 && i + 1 < __argc) {
            if (strcmp(__argv[i+1], "enhanced") == 0) {
                enhanced = true;
            }
            ++i; /* skip next */
        } else if (in_filename == default_filename) {
            in_filename = __argv[i];
        } else {
            /* ignore */
        }
    }

    FILE *input_file = fopen(in_filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: cannot open input file '%s'\n", in_filename);
        return 2;
    }

    char out_filename[512];
    FILE *output_file = NULL;

    char line[1024];
    unsigned long line_no = 0;
    while (fgets(line, sizeof(line), input_file)) {
        ++line_no;
        /* Trim leading/trailing whitespace */
        char *p = line;
        while (isspace((unsigned char)*p)) ++p;
        if (*p == '\0' || *p == '\n' || *p == '#') {
            continue; 
        }

        uint8_t bytes[64];
        size_t bytes_count = 0;

        if (!parse_hex_line(line, bytes, sizeof(bytes), &bytes_count)) {
            fprintf(stderr, "Skipping line %lu: parse error\n", line_no);
            continue;
        }

        if (bytes_count < 2) {
            fprintf(stderr, "Skipping line %lu: need at least PID and length\n", line_no);
            continue;
        }

        uint8_t pid = bytes[0];
        uint8_t len = bytes[1];
        if (bytes_count < (size_t)(2 + len)) {
            fprintf(stderr, "Skipping line %lu: declared length %u but only %zu bytes present\n", line_no, (unsigned)len, bytes_count-2);
            continue;
        }

        /* compute checksum */
        uint8_t checksum;
        if (enhanced) {
            checksum = lin_checksum_enhanced(pid, &bytes[2], len);
        } else {
            checksum = lin_checksum_classic(&bytes[2], len);
        }

        /* Reconstruct original tokens for output: we'll print the original bytes and appended checksum */
        /* Print to stdout */
        printf("Line %lu:", line_no);
        for (size_t i = 0; i < bytes_count; ++i) {
            printf(" %02X", bytes[i]);
        }
        printf(" %02X\n", checksum);

        if (output_file) {
            fprintf(output_file, ""); /* keep file pointer in sync (no-op) */
            for (size_t i = 0; i < bytes_count; ++i) {
                fprintf(output_file, "%02X", bytes[i]);
                if (i + 1 < bytes_count) fprintf(output_file, " ");
            }
            fprintf(output_file, " %02X\n", checksum);
        }
    }

    fclose(input_file);
    if (output_file) fclose(output_file);

    return 0;
}
