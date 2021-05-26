t = executable(
  'json_test',
  ['LoaderTest.cc'],
  dependencies : [gtest, json_dep],
  include_directories : engine)

test_env = environment()
test('json', t, env : test_env, workdir : meson.current_source_dir())
