#ifndef LIN_CHECKSUM_H
#define LIN_CHECKSUM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Compute LIN classic checksum (data bytes only, inverted sum).
 * @param data pointer to data bytes (excluding PID and length)
 * @param len number of data bytes
 * @return 8-bit checksum (inverted sum modulo 256)
 */
uint8_t lin_checksum_classic(const uint8_t *data, size_t len);

/**
 * Compute LIN enhanced checksum (PID + data bytes, inverted sum).
 * @param pid message ID byte
 * @param data pointer to data bytes (excluding PID and length)
 * @param len number of data bytes
 * @return 8-bit checksum (inverted sum of PID+data modulo 256)
 */
uint8_t lin_checksum_enhanced(uint8_t pid, const uint8_t *data, size_t len);

/**
 * Parse a single hex byte token (e.g., "3A" or "0x3A").
 * @param token null-terminated string
 * @param out_byte pointer to store parsed byte (only set on success)
 * @return true if parsing succeeded, false otherwise
 */
bool parse_hex_byte(const char *token, uint8_t *out_byte);

/**
 * Parse a line of hex tokens separated by whitespace into a byte array.
 * @param line null-terminated line (may contain leading/trailing whitespace)
 * @param out_bytes pointer to output byte array
 * @param max_bytes max size of out_bytes array
 * @param out_count pointer to store number of parsed bytes
 * @return true if parsing succeeded, false otherwise
 */
bool parse_hex_line(const char *line, uint8_t *out_bytes, size_t max_bytes, size_t *out_count);

#endif
