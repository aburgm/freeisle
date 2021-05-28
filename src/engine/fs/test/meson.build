readwrite_mock = shared_library(
  'readwrite_mock',
  ['mock/readwrite_mock.c'],
  dependencies : dl)

t = executable(
  'fs_test',
  ['TestDirectory.cc', 'TestFile.cc', 'TestPath.cc'],
  dependencies : [gtest],
  link_with : fs_lib,
  include_directories : engine)

test_env = environment()
test_env.prepend('LD_PRELOAD', readwrite_mock.full_path())
test('fs', t, env : test_env)
