#include "base64/Base64.hh"

#include <stdexcept>

#include <cassert>

namespace {

const uint8_t alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
};

const uint8_t inv_alphabet[] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 62,   0xff, 0xff, 0xff, 63,
    52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0,    1,    2,    3,    4,    5,    6,
    7,    8,    9,    10,   11,   12,   13,   14,   15,   16,   17,   18,
    19,   20,   21,   22,   23,   24,   25,   0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 26,   27,   28,   29,   30,   31,   32,   33,   34,   35,   36,
    37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47,   48,
    49,   50,   51,   0xff, 0xff, 0xff, 0xff, 0xff,
};

static_assert(sizeof(alphabet) == 64, "Base-64 alphabet has incorrect size");
static_assert(sizeof(inv_alphabet) == 128,
              "Base-64 inverse alphabet has incorrect size");

uint8_t inv(uint8_t v) {
  if (v >= 0x80) {
    throw std::invalid_argument("Non-ASCII input");
  }

  if (inv_alphabet[v] == 0xff) {
    throw std::invalid_argument("Illegal character: " +
                                std::string(&v, &v + 1));
  }

  return inv_alphabet[v];
}

} // namespace

namespace freeisle::base64 {

uint64_t encode(const uint8_t *data, uint64_t len, uint8_t *result) {
  uint8_t *res = result;
  while (len >= 3) {
    const uint8_t b1 = (data[0] & 0b1111100) >> 2;
    const uint8_t b2 =
        ((data[0] & 0b00000011) << 4) | ((data[1] & 0b11110000) >> 4);
    const uint8_t b3 =
        ((data[1] & 0b00001111) << 2) | ((data[2] & 0b11000000) >> 6);
    const uint8_t b4 = (data[2] & 0b00111111);

    data += 3;
    len -= 3;

    res[0] = alphabet[b1];
    res[1] = alphabet[b2];
    res[2] = alphabet[b3];
    res[3] = alphabet[b4];

    res += 4;
  }

  assert(len < 3);
  switch (len) {
  case 0:
    break;
  case 1:
    res[0] = alphabet[(data[0] & 0b11111100) >> 2];
    res[1] = alphabet[(data[0] & 0b00000011) << 4];
    res[2] = '=';
    res[3] = '=';
    res += 4;
    break;
  case 2:
    res[0] = alphabet[(data[0] & 0b1111100) >> 2];
    res[1] =
        alphabet[((data[0] & 0b00000011) << 4) | ((data[1] & 0b11110000) >> 4)];
    res[2] = alphabet[((data[1] & 0b00001111) << 2)];
    res[3] = '=';
    res += 4;
    break;
  }

  return res - result;
}

uint64_t decode(const uint8_t *data, uint64_t len, uint8_t *result) {
  // TODO(armin): skip whitespace
  while (len > 0 && data[len - 1] == '=') {
    --len;
  }

  uint8_t *res = result;
  while (len >= 4) {
    const uint8_t b1 = inv(data[0]);
    const uint8_t b2 = inv(data[1]);
    const uint8_t b3 = inv(data[2]);
    const uint8_t b4 = inv(data[3]);

    data += 4;
    len -= 4;

    res[0] = (b1 << 2) | (b2 >> 4);
    res[1] = (b2 << 4) | (b3 >> 2);
    res[2] = (b3 << 6) | b4;
    res += 3;
  }

  assert(len < 4);
  switch (len) {
  case 0:
    break;
  case 1:
    throw std::invalid_argument("Preliminary end of input");
  case 2:
    res[0] = (inv(data[0]) << 2) | (inv(data[1]) >> 4);
    res += 1;
    break;
  case 3:
    res[0] = (inv(data[0]) << 2) | (inv(data[1]) >> 4);
    res[1] = (inv(data[1]) << 4) | (inv(data[2]) >> 2);
    res += 2;
    break;
  }

  return res - result;
}

} // namespace freeisle::base64
