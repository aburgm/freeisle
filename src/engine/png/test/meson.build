png_mock = shared_library(
  'png_mock',
  ['mock/png_mock.c'],
  dependencies : [dl, libpng])

t = executable(
  'png_test',
  ['TestPng.cc'],
  dependencies : gtest,
  link_with : [time_lib, log_lib, fs_lib, png_lib],
  include_directories : engine)

test_env = environment()
test_env.prepend('LD_PRELOAD', png_mock.full_path())
test('png', t, env : test_env, workdir : meson.current_source_dir())
