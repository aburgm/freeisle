t = executable(
  'serialize_test',
  ['TestMapDefHandlers.cc'],
  dependencies : [serialize_dep, gtest],
  include_directories : engine)

test('serialize', t, workdir : meson.current_source_dir())
