#include "base64/Base64.hh"

#include <gtest/gtest.h>

TEST(Base64, RoundToNextMultiple) {
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(0), 0);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(1), 4);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(2), 4);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(3), 4);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(4), 4);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(5), 8);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(6), 8);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(7), 8);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(8), 8);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<4>(9), 12);

  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<16>(9), 16);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<16>(22), 32);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<16>(96), 96);
  EXPECT_EQ(freeisle::base64::round_to_next_multiple_of<16>(97), 112);
}

TEST(Base64, EncodeEmpty) {
  uint8_t out[8];
  EXPECT_EQ(
      freeisle::base64::encode(reinterpret_cast<const uint8_t *>(""), 0, out),
      0);
}

TEST(Base64, Encode_f) {
  uint8_t out[8];
  EXPECT_EQ(
      freeisle::base64::encode(reinterpret_cast<const uint8_t *>("f"), 1, out),
      4);
  EXPECT_EQ(std::string(out, out + 4), "Zg==");
}

TEST(Base64, Encode_fo) {
  uint8_t out[8];
  EXPECT_EQ(
      freeisle::base64::encode(reinterpret_cast<const uint8_t *>("fo"), 2, out),
      4);
  EXPECT_EQ(std::string(out, out + 4), "Zm8=");
}

TEST(Base64, Encode_foo) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::encode(reinterpret_cast<const uint8_t *>("foo"),
                                     3, out),
            4);
  EXPECT_EQ(std::string(out, out + 4), "Zm9v");
}

TEST(Base64, Encode_foob) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::encode(reinterpret_cast<const uint8_t *>("foob"),
                                     4, out),
            8);
  EXPECT_EQ(std::string(out, out + 8), "Zm9vYg==");
}

TEST(Base64, Encode_fooba) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::encode(reinterpret_cast<const uint8_t *>("fooba"),
                                     5, out),
            8);
  EXPECT_EQ(std::string(out, out + 8), "Zm9vYmE=");
}

TEST(Base64, Encode_foobar) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::encode(
                reinterpret_cast<const uint8_t *>("foobar"), 6, out),
            8);
  EXPECT_EQ(std::string(out, out + 8), "Zm9vYmFy");
}

TEST(Base64, DecodeEmpty) {
  uint8_t out[8];
  EXPECT_EQ(
      freeisle::base64::decode(reinterpret_cast<const uint8_t *>(""), 0, out),
      0);
}

TEST(Base64, DecodePadded_f) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(reinterpret_cast<const uint8_t *>("Zg=="),
                                     4, out),
            1);
  EXPECT_EQ(std::string(out, out + 1), "f");
}

TEST(Base64, DecodeUnpadded_f) {
  uint8_t out[8];
  EXPECT_EQ(
      freeisle::base64::decode(reinterpret_cast<const uint8_t *>("Zg"), 2, out),
      1);
  EXPECT_EQ(std::string(out, out + 1), "f");
}

TEST(Base64, DecodePadded_fo) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(reinterpret_cast<const uint8_t *>("Zm8="),
                                     4, out),
            2);
  EXPECT_EQ(std::string(out, out + 2), "fo");
}

TEST(Base64, DecodeUnpadded_fo) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(reinterpret_cast<const uint8_t *>("Zm8"),
                                     3, out),
            2);
  EXPECT_EQ(std::string(out, out + 2), "fo");
}

TEST(Base64, Decode_foo) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(reinterpret_cast<const uint8_t *>("Zm9v"),
                                     4, out),
            3);
  EXPECT_EQ(std::string(out, out + 3), "foo");
}

TEST(Base64, DecodePadded_foob) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(
                reinterpret_cast<const uint8_t *>("Zm9vYg=="), 8, out),
            4);
  EXPECT_EQ(std::string(out, out + 4), "foob");
}

TEST(Base64, DecodeUnpadded_foob) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(
                reinterpret_cast<const uint8_t *>("Zm9vYg"), 6, out),
            4);
  EXPECT_EQ(std::string(out, out + 4), "foob");
}

TEST(Base64, DecodePadded_fooba) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(
                reinterpret_cast<const uint8_t *>("Zm9vYmE="), 8, out),
            5);
  EXPECT_EQ(std::string(out, out + 5), "fooba");
}

TEST(Base64, DecodeUnpadded_fooba) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(
                reinterpret_cast<const uint8_t *>("Zm9vYmE"), 7, out),
            5);
  EXPECT_EQ(std::string(out, out + 5), "fooba");
}

TEST(Base64, Decode_foobar) {
  uint8_t out[8];
  EXPECT_EQ(freeisle::base64::decode(
                reinterpret_cast<const uint8_t *>("Zm9vYmFy"), 8, out),
            6);
  EXPECT_EQ(std::string(out, out + 6), "foobar");
}

TEST(Base64, DecodeNonAscii) {
  uint8_t out[8];
  const uint8_t in[8] = {'Z', '9', 'm', 0xa3, 'Y', 'm', 'F', 'y'};
  EXPECT_THROW(freeisle::base64::decode(in, 8, out), std::invalid_argument);
}

TEST(Base64, DecodeIllegalCharacter) {
  uint8_t out[8];
  const uint8_t in[8] = {'Z', '9', 'm', ')', 'Y', 'm', 'F', 'y'};
  EXPECT_THROW(freeisle::base64::decode(in, 8, out), std::invalid_argument);
}

TEST(Base64, DecodeShortPadded) {
  uint8_t out[8];
  const uint8_t in[8] = {'Z', '9', 'm', 'v', 'Y', '=', '=', '='};
  EXPECT_THROW(freeisle::base64::decode(in, 8, out), std::invalid_argument);
}

TEST(Base64, DecodeShortUnpadded) {
  uint8_t out[8];
  const uint8_t in[5] = {'Z', '9', 'm', 'v', 'Y'};
  EXPECT_THROW(freeisle::base64::decode(in, 5, out), std::invalid_argument);
}

TEST(Base64, Encode_above128) {
  uint8_t out[8];
  uint8_t in[4] = {137, 80, 78, 71};
  EXPECT_EQ(freeisle::base64::encode(in, sizeof(in), out), 8);
  EXPECT_EQ(std::string(out, out + 8), "iVBORw==");
}

TEST(Base64, Decode_above128) {
  uint8_t out[4];
  const uint8_t in[8] = {'i', 'V', 'B', 'O', 'R', 'w', '=', '='};
  ASSERT_EQ(freeisle::base64::decode(in, 8, out), 4);

  EXPECT_EQ(out[0], 137);
  EXPECT_EQ(out[1], 80);
  EXPECT_EQ(out[2], 78);
  EXPECT_EQ(out[3], 71);
}
