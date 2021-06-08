def_serialize_lib = static_library(
  'def_serialize', [
    'MapDefHandlers.cc',
    'ScenarioHandlers.cc',
    'ShopDefHandlers.cc',
    'UnitDefHandlers.cc',
  ],
  dependencies : [json_dep],
  link_with : [png_lib, log_lib],
  include_directories : engine)

def_serialize_dep = declare_dependency(
  link_with : def_serialize_lib,
  dependencies: [json_dep]
)

subdir('test')
