t = executable(
  'def_serialize_test',
  [
    'TestCollectionLoaders.cc',
    'TestCollectionSavers.cc',
    'TestMapDefHandlers.cc',
    'TestScenarioHandlers.cc',
    'TestShopDefHandlers.cc',
    'TestUnitDefHandlers.cc',
  ],
  dependencies : [def_serialize_dep, gtest],
  include_directories : engine)

test('def_serialize', t, workdir : meson.current_source_dir())
