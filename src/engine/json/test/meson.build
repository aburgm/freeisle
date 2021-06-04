t = executable(
  'json_test',
  [
    'BitmaskLoaderTest.cc',
    'BitmaskSaverTest.cc',
    'EnumMapLoaderTest.cc',
    'EnumMapSaverTest.cc',
    'LoaderTest.cc',
    'SaverTest.cc',
    'MappedContainerLoaderTest.cc',
    'MappedContainerSaverTest.cc'
  ],
  dependencies : [gtest, json_dep],
  include_directories : engine)

test_env = environment()
test('json', t, env : test_env, workdir : meson.current_source_dir())
