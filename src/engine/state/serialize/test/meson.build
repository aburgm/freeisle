t = executable(
  'serialize_test',
  [
    'TestCollectionLoaders.cc',
    'TestCollectionSavers.cc',
    'TestMapDefHandlers.cc',
    'TestScenarioHandlers.cc',
    'TestShopDefHandlers.cc',
    'TestUnitDefHandlers.cc',
  ],
  dependencies : [serialize_dep, gtest],
  include_directories : engine)

test('serialize', t, workdir : meson.current_source_dir())
