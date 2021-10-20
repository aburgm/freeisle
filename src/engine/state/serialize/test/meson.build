t = executable(
  'state_serialize_test',
  [
    'TestPlayerHandlers.cc',
    'TestSerialize.cc',
    'TestShopHandlers.cc',
    'TestStateHandlers.cc',
    'TestUnitHandlers.cc',
  ],
  dependencies : [state_serialize_dep, gtest],
  include_directories : engine)

test('state_serialize', t, workdir : meson.current_source_dir())
