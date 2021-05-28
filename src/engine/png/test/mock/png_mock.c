#define _GNU_SOURCE

#include <png.h>

#include <assert.h>
#include <dlfcn.h>

static int create_read_struct_fail = 0;
static int create_write_struct_fail = 0;
static int create_info_struct_fail = 0;
static int create_info_struct_warn = 0;
static int write_info_fail = 0;

void set_create_read_struct_fail() { create_read_struct_fail = 1; }

void set_create_write_struct_fail() { create_write_struct_fail = 1; }

void set_create_info_struct_fail() { create_info_struct_fail = 1; }

void set_create_info_struct_warn() { create_info_struct_warn = 1; }

void set_write_info_fail() { write_info_fail = 1; }

typedef png_structp (*png_create_read_struct_func)(png_const_charp user_png_ver,
                                                   png_voidp error_ptr,
                                                   png_error_ptr error_fn,
                                                   png_error_ptr warn_fn);
typedef png_structp (*png_create_write_struct_func)(
    png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn,
    png_error_ptr warn_fn);
typedef png_infop (*png_create_info_struct_func)(png_const_structp png_ptr);
typedef void (*png_write_info_func)(png_structp png_ptr,
                                    png_const_infop info_ptr);

png_structp png_create_read_struct(png_const_charp user_png_ver,
                                   png_voidp error_ptr, png_error_ptr error_fn,
                                   png_error_ptr warn_fn) {
  if (create_read_struct_fail) {
    create_read_struct_fail = 0;
    return NULL;
  }

  png_create_read_struct_func f =
      (png_create_read_struct_func)dlsym(RTLD_NEXT, "png_create_read_struct");
  assert(f != NULL);

  return f(user_png_ver, error_ptr, error_fn, warn_fn);
}

png_structp png_create_write_struct(png_const_charp user_png_ver,
                                    png_voidp error_ptr, png_error_ptr error_fn,
                                    png_error_ptr warn_fn) {
  if (create_write_struct_fail) {
    create_write_struct_fail = 0;
    return NULL;
  }

  png_create_write_struct_func f =
      (png_create_write_struct_func)dlsym(RTLD_NEXT, "png_create_write_struct");
  assert(f != NULL);

  return f(user_png_ver, error_ptr, error_fn, warn_fn);
}

png_infop png_create_info_struct(png_const_structp png_ptr) {
  if (create_info_struct_fail) {
    create_info_struct_fail = 0;
    return NULL;
  }

  if (create_info_struct_warn) {
    create_info_struct_warn = 0;
    png_warning(png_ptr, "Mock warning");
  }

  png_create_info_struct_func f =
      (png_create_info_struct_func)dlsym(RTLD_NEXT, "png_create_info_struct");
  assert(f != NULL);

  return f(png_ptr);
}

void png_write_info(png_structp png_ptr, png_const_infop info_ptr) {
  if (write_info_fail) {
    write_info_fail = 0;
    png_error(png_ptr, "Mock error");
  }

  png_write_info_func f =
      (png_write_info_func)dlsym(RTLD_NEXT, "png_write_info");
  assert(f != NULL);

  return f(png_ptr, info_ptr);
}
