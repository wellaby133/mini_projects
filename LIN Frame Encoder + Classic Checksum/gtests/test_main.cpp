#include <gtest/gtest.h>
#include "lin_checksum.h"

/* ===== Tests for lin_checksum_classic ===== */

TEST(LinChecksumClassic, EmptyData)
{
    // empty data -> sum = 0 -> checksum = ~0 & 0xFF = 0xFF
    uint8_t checksum = lin_checksum_classic(NULL, 0);
    EXPECT_EQ(checksum, 0xFF);
}

TEST(LinChecksumClassic, SingleByte)
{
    // data = [0x11] -> sum = 0x11 -> checksum = ~0x11 & 0xFF = 0xEE
    uint8_t data[] = {0x11};
    uint8_t checksum = lin_checksum_classic(data, 1);
    EXPECT_EQ(checksum, 0xEE);
}

TEST(LinChecksumClassic, MultipleBytesNoOverflow)
{
    // data = [0x11, 0x22, 0x33] -> sum = 0x66 -> checksum = ~0x66 & 0xFF = 0x99
    uint8_t data[] = {0x11, 0x22, 0x33};
    uint8_t checksum = lin_checksum_classic(data, 3);
    EXPECT_EQ(checksum, 0x99);
}

TEST(LinChecksumClassic, BytesWithOverflow)
{
    // data = [0xFF, 0xFF] -> sum = 0x1FE -> (sum & 0xFF) = 0xFE -> checksum = ~0xFE & 0xFF = 0x01
    uint8_t data[] = {0xFF, 0xFF};
    uint8_t checksum = lin_checksum_classic(data, 2);
    EXPECT_EQ(checksum, 0x01);
}

TEST(LinChecksumClassic, AllZeros)
{
    // data = [0x00, 0x00, 0x00] -> sum = 0x00 -> checksum = ~0x00 & 0xFF = 0xFF
    uint8_t data[] = {0x00, 0x00, 0x00};
    uint8_t checksum = lin_checksum_classic(data, 3);
    EXPECT_EQ(checksum, 0xFF);
}

/* ===== Tests for lin_checksum_enhanced ===== */

TEST(LinChecksumEnhanced, WithPid)
{
    // pid = 0x3A, data = [0x01, 0x02] -> sum = 0x3A + 0x01 + 0x02 = 0x3D -> checksum = ~0x3D & 0xFF = 0xC2
    uint8_t data[] = {0x01, 0x02};
    uint8_t checksum = lin_checksum_enhanced(0x3A, data, 2);
    EXPECT_EQ(checksum, 0xC2);
}

TEST(LinChecksumEnhanced, PidWithEmptyData)
{
    // pid = 0x55, data = [] -> sum = 0x55 -> checksum = ~0x55 & 0xFF = 0xAA
    uint8_t checksum = lin_checksum_enhanced(0x55, NULL, 0);
    EXPECT_EQ(checksum, 0xAA);
}

TEST(LinChecksumEnhanced, PidWithOverflow)
{
    // pid = 0xFF, data = [0xFF] -> sum = 0x1FE -> (sum & 0xFF) = 0xFE -> checksum = ~0xFE & 0xFF = 0x01
    uint8_t data[] = {0xFF};
    uint8_t checksum = lin_checksum_enhanced(0xFF, data, 1);
    EXPECT_EQ(checksum, 0x01);
}

/* ===== Tests for parse_hex_byte ===== */

TEST(ParseHexByte, SimpleTwoDigit)
{
    uint8_t out;
    EXPECT_TRUE(parse_hex_byte("3A", &out));
    EXPECT_EQ(out, 0x3A);
}

TEST(ParseHexByte, WithZeroXPrefix)
{
    uint8_t out;
    EXPECT_TRUE(parse_hex_byte("0x3A", &out));
    EXPECT_EQ(out, 0x3A);
}

TEST(ParseHexByte, WithZeroXUppercase)
{
    uint8_t out;
    EXPECT_TRUE(parse_hex_byte("0X1F", &out));
    EXPECT_EQ(out, 0x1F);
}

TEST(ParseHexByte, SingleDigit)
{
    uint8_t out;
    EXPECT_TRUE(parse_hex_byte("A", &out));
    EXPECT_EQ(out, 0x0A);
}

TEST(ParseHexByte, LowerCase)
{
    uint8_t out;
    EXPECT_TRUE(parse_hex_byte("ff", &out));
    EXPECT_EQ(out, 0xFF);
}

TEST(ParseHexByte, MixedCase)
{
    uint8_t out;
    EXPECT_TRUE(parse_hex_byte("aB", &out));
    EXPECT_EQ(out, 0xAB);
}

TEST(ParseHexByte, ZeroValue)
{
    uint8_t out;
    EXPECT_TRUE(parse_hex_byte("00", &out));
    EXPECT_EQ(out, 0x00);
}

TEST(ParseHexByte, InvalidTooLong)
{
    uint8_t out;
    EXPECT_FALSE(parse_hex_byte("123", &out));
}

TEST(ParseHexByte, InvalidNonHex)
{
    uint8_t out;
    EXPECT_FALSE(parse_hex_byte("GG", &out));
}

TEST(ParseHexByte, InvalidEmpty)
{
    uint8_t out;
    EXPECT_FALSE(parse_hex_byte("", &out));
}

TEST(ParseHexByte, InvalidNullPointer)
{
    uint8_t out;
    EXPECT_FALSE(parse_hex_byte(NULL, &out));
}

/* ===== Tests for parse_hex_line ===== */

TEST(ParseHexLine, SingleByte)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("FF", out, 64, &count));
    EXPECT_EQ(count, 1);
    EXPECT_EQ(out[0], 0xFF);
}

TEST(ParseHexLine, MultipleBytesSpaceSeparated)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("3A 04 11 22 33 44", out, 64, &count));
    EXPECT_EQ(count, 6);
    EXPECT_EQ(out[0], 0x3A);
    EXPECT_EQ(out[1], 0x04);
    EXPECT_EQ(out[2], 0x11);
    EXPECT_EQ(out[5], 0x44);
}

TEST(ParseHexLine, TabSeparated)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("AA\tBB\tCC", out, 64, &count));
    EXPECT_EQ(count, 3);
    EXPECT_EQ(out[0], 0xAA);
    EXPECT_EQ(out[1], 0xBB);
    EXPECT_EQ(out[2], 0xCC);
}

TEST(ParseHexLine, MixedSeparators)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("01 02\t03 \t04", out, 64, &count));
    EXPECT_EQ(count, 4);
    EXPECT_EQ(out[0], 0x01);
    EXPECT_EQ(out[3], 0x04);
}

TEST(ParseHexLine, WithTrailingNewline)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("1A 2B 3C\n", out, 64, &count));
    EXPECT_EQ(count, 3);
    EXPECT_EQ(out[0], 0x1A);
}

TEST(ParseHexLine, WithLeadingWhitespace)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("  55 66", out, 64, &count));
    EXPECT_EQ(count, 2);
    EXPECT_EQ(out[0], 0x55);
}

TEST(ParseHexLine, WithPrefixes)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("0xAA 0xBB 0xCC", out, 64, &count));
    EXPECT_EQ(count, 3);
    EXPECT_EQ(out[0], 0xAA);
}

TEST(ParseHexLine, Empty)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_FALSE(parse_hex_line("", out, 64, &count));
}

TEST(ParseHexLine, OnlyWhitespace)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_FALSE(parse_hex_line("   \t  ", out, 64, &count));
}

TEST(ParseHexLine, InvalidHexInMiddle)
{
    uint8_t out[64];
    size_t count = 0;
    EXPECT_FALSE(parse_hex_line("11 GG 33", out, 64, &count));
}

TEST(ParseHexLine, TooManyBytes)
{
    uint8_t out[4];
    size_t count = 0;
    EXPECT_FALSE(parse_hex_line("01 02 03 04 05", out, 4, &count));
}

TEST(ParseHexLine, NullOutputBuffer)
{
    size_t count = 0;
    EXPECT_FALSE(parse_hex_line("AA BB", NULL, 64, &count));
}

/* ===== Integration tests ===== */

TEST(Integration, FullLinFrame)
{
    // Simulate parsing and checksum of a full frame: PID=3A, LEN=04, DATA=[11 22 33 44]
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("3A 04 11 22 33 44", out, 64, &count));
    EXPECT_EQ(count, 6);

    uint8_t pid = out[0];
    uint8_t len = out[1];
    EXPECT_EQ(pid, 0x3A);
    EXPECT_EQ(len, 0x04);

    // Classic checksum: sum of data [11 22 33 44] = 0xAA -> checksum = ~0xAA = 0x55
    uint8_t classic = lin_checksum_classic(&out[2], len);
    EXPECT_EQ(classic, 0x55);

    // Enhanced checksum: sum of [3A 11 22 33 44] = 0xE4 -> checksum = ~0xE4 = 0x1B
    uint8_t enhanced = lin_checksum_enhanced(pid, &out[2], len);
    EXPECT_EQ(enhanced, 0x1B);
}

TEST(Integration, ShortFrame)
{
    // PID=10, LEN=01, DATA=[20]
    uint8_t out[64];
    size_t count = 0;
    EXPECT_TRUE(parse_hex_line("10 01 20", out, 64, &count));
    EXPECT_EQ(count, 3);

    uint8_t pid = out[0];
    uint8_t len = out[1];
    uint8_t classic = lin_checksum_classic(&out[2], len);
    // sum = 0x20 -> checksum = ~0x20 = 0xDF
    EXPECT_EQ(classic, 0xDF);
}
