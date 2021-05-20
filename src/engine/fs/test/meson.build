cc = meson.get_compiler('c')
dl_dep = cc.find_library('dl')

readwrite_mock = shared_library('readwrite_mock', ['mock/readwrite_mock.c'], dependencies : dl_dep)

test_env = environment()
test_env.prepend('LD_PRELOAD', readwrite_mock.full_path())

t = executable(
  'fs_test',
  ['TestDirectory.cc', 'TestFile.cc'],
  dependencies : [gtest],
  link_with : fs_lib,
  include_directories : engine)

test('fs', t, env : test_env)
