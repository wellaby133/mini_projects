#include "lin_checksum.h"
#include <string.h>
#include <ctype.h>

uint8_t lin_checksum_classic(const uint8_t *data, size_t len)
{
    unsigned sum = 0;
    for (size_t i = 0; i < len; ++i) {
        sum += data[i];
    }
    return (uint8_t)(~(sum & 0xFF) & 0xFF);
}

uint8_t lin_checksum_enhanced(uint8_t pid, const uint8_t *data, size_t len)
{
    unsigned sum = pid;
    for (size_t i = 0; i < len; ++i) {
        sum += data[i];
    }
    return (uint8_t)(~(sum & 0xFF) & 0xFF);
}

bool parse_hex_byte(const char *token, uint8_t *out_byte)
{
    if (!token || !out_byte) return false;

    const char *s = token;
    /* Skip 0x or 0X prefix */
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2;
    }

    size_t len = strlen(s);
    if (len == 0 || len > 2) {
        return false;
    }

    uint8_t v = 0;
    for (size_t i = 0; i < len; ++i) {
        char c = s[i];
        int nib;
        if (c >= '0' && c <= '9') {
            nib = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            nib = c - 'A' + 10;
        } else if (c >= 'a' && c <= 'f') {
            nib = c - 'a' + 10;
        } else {
            return false;
        }
        v = (v << 4) | nib;
    }
    *out_byte = v;
    return true;
}

bool parse_hex_line(const char *line, uint8_t *out_bytes, size_t max_bytes, size_t *out_count)
{
    if (!line || !out_bytes || !out_count || max_bytes == 0) {
        return false;
    }

    /* Make a copy for strtok */
    char buf[1024];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    /* Remove trailing newline */
    char *nl = strchr(buf, '\n');
    if (nl) *nl = '\0';

    *out_count = 0;
    char *tok = strtok(buf, " \t");
    while (tok) {
        if (*out_count >= max_bytes) {
            return false; /* too many tokens */
        }
        if (!parse_hex_byte(tok, &out_bytes[*out_count])) {
            return false; /* invalid hex */
        }
        ++(*out_count);
        tok = strtok(NULL, " \t");
    }

    return *out_count > 0;
}
