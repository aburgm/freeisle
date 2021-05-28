#include "png/Png.hh"

#include <png.h>

namespace freeisle::png {

namespace {

struct PngHelper {
  png_structp png = nullptr;
  png_infop info = nullptr;

  ~PngHelper() { png_destroy_read_struct(&png, &info, nullptr); }
};

struct ExtraData {
  jmp_buf jmpbuf;
  std::string error_message;
  log::Logger *logger;
};

struct PngReadIo {
  const uint8_t *const data;
  const uint64_t len;
  uint64_t cur;
};

struct PngWriteIo {
  std::vector<uint8_t> data;
};

void user_error_fn(png_structp png_ptr, png_const_charp error_msg) {
  ExtraData *extra = static_cast<ExtraData *>(png_get_error_ptr(png_ptr));
  extra->error_message = error_msg;
  extra->logger->error(error_msg);
  longjmp(extra->jmpbuf, 1);
}

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg) {
  ExtraData *extra = static_cast<ExtraData *>(png_get_error_ptr(png_ptr));
  extra->logger->warning(warning_msg);
}

void user_read_fn(png_structp png_ptr, png_bytep buf, png_size_t len) {
  PngReadIo *io = static_cast<PngReadIo *>(png_get_io_ptr(png_ptr));
  if (len > io->len - io->cur) {
    png_error(png_ptr, "preliminary end of data");
  }

  std::copy(io->data + io->cur, io->data + io->cur + len, buf);
  io->cur += len;
}

void user_write_fn(png_structp png_ptr, png_bytep buf, png_size_t len) {
  PngWriteIo *io = static_cast<PngWriteIo *>(png_get_io_ptr(png_ptr));
  io->data.insert(io->data.end(), buf, buf + len);
}

} // namespace

core::Grid<core::color::Rgb8> decode_rgb8(const uint8_t *data, uint64_t len,
                                          log::Logger logger) {
  ExtraData extra = {.logger = &logger};

  PngHelper png;

  png.png = png_create_read_struct(PNG_LIBPNG_VER_STRING, &extra, user_error_fn,
                                   user_warning_fn);
  if (png.png == nullptr) {
    throw std::runtime_error("failed to allocate PNG read struct");
  }

  png.info = png_create_info_struct(png.png);
  if (png.info == nullptr) {
    throw std::runtime_error("failed to allocate PNG info struct");
  }

  if (setjmp(extra.jmpbuf)) {
    throw std::runtime_error(extra.error_message);
  }

  PngReadIo io = {
      .data = data,
      .len = len,
      .cur = 0,
  };

  png_set_read_fn(png.png, &io, user_read_fn);

  png_read_info(png.png, png.info);

  const uint32_t width = png_get_image_width(png.png, png.info);
  const uint32_t height = png_get_image_height(png.png, png.info);

  png_set_strip_16(png.png);
  png_set_strip_alpha(png.png);
  png_set_palette_to_rgb(png.png);
  png_set_expand_gray_1_2_4_to_8(png.png);
  png_set_gray_to_rgb(png.png);

  png_read_update_info(png.png, png.info);

  core::Grid<core::color::Rgb8> result(width, height);
  std::vector<png_bytep> row_pointers(height);
  for (uint32_t i = 0; i < height; ++i) {
    row_pointers[i] = reinterpret_cast<png_bytep>(&result(0, i));
  }

  png_read_image(png.png, row_pointers.data());
  return result;
}

std::vector<uint8_t> encode_rgb8(const core::Grid<core::color::Rgb8> &image,
                                 log::Logger logger) {
  ExtraData extra = {.logger = &logger};

  PngHelper png;

  png.png = png_create_write_struct(PNG_LIBPNG_VER_STRING, &extra,
                                    user_error_fn, user_warning_fn);
  if (png.png == nullptr) {
    throw std::runtime_error("failed to allocate PNG read struct");
  }

  png.info = png_create_info_struct(png.png);
  if (png.info == nullptr) {
    throw std::runtime_error("failed to allocate PNG info struct");
  }

  if (setjmp(extra.jmpbuf)) {
    throw std::runtime_error(extra.error_message);
  }

  PngWriteIo io;
  png_set_write_fn(png.png, &io, user_write_fn, nullptr);

  png_set_IHDR(png.png, png.info, image.width(), image.height(), 8,
               PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

  png_write_info(png.png, png.info);

  std::vector<png_bytep> row_pointers(image.height());
  for (uint32_t i = 0; i < image.height(); ++i) {
    png_const_bytep ptr = reinterpret_cast<png_const_bytep>(&image(0, i));
    row_pointers[i] = const_cast<png_bytep>(ptr);
  }

  png_write_image(png.png, row_pointers.data());
  png_write_end(png.png, NULL);

  return io.data;
}

} // namespace freeisle::png
