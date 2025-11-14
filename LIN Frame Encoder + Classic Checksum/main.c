#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

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

        /* Make a copy we can tokenize */
        char buf[1024];
        strncpy(buf, p, sizeof(buf)-1);
        buf[sizeof(buf)-1] = '\0';
        /* remove trailing newline */
        char *nl = strchr(buf, '\n');
        if (nl) *nl = '\0';

        uint8_t bytes[64];
        size_t bytes_count = 0;

        char *tok = strtok(buf, " \t");
        bool parse_error = false;
        while (tok) {
            /* allow tokens like '3A' or '0x3A' */
            char *s = tok;
            if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;

            size_t len = strlen(s);
            if (len == 0 || len > 2) { parse_error = true; break; }
            int hi = 0, lo = 0;
            int v = 0;
            for (size_t i = 0; i < len; ++i) {
                char c = s[i];
                int nib;
                if (c >= '0' && c <= '9') nib = c - '0';
                else if (c >= 'A' && c <= 'F') nib = c - 'A' + 10;
                else if (c >= 'a' && c <= 'f') nib = c - 'a' + 10;
                else { parse_error = true; break; }
                v = (v << 4) | nib;
            }
            if (parse_error) break;
            if (bytes_count < sizeof(bytes)) {
                bytes[bytes_count++] = (uint8_t)(v & 0xFF);
            } else {
                parse_error = true;
                break;
            }
            tok = strtok(NULL, " \t");
        }

        uint8_t pid = bytes[0];
        uint8_t len = bytes[1];
        if (bytes_count < (size_t)(2 + len)) {
            fprintf(stderr, "Skipping line %lu: declared length %u but only %zu bytes present\n", line_no, (unsigned)len, bytes_count-2);
            continue;
        }

        /* compute checksum */
        unsigned sum = 0;
        size_t data_start = 2;
        if (enhanced) {
            sum += pid;
        }
        for (size_t i = 0; i < len; ++i) {
            sum += bytes[data_start + i];
        }
        uint8_t checksum = (uint8_t)(~(sum & 0xFF) & 0xFF);

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