state_serialize_lib = static_library(
  'state_serialize', [
    'PlayerHandlers.cc',
    'ShopHandlers.cc',
    'StateHandlers.cc',
    'UnitHandlers.cc',
  ],
  dependencies : [json_dep, def_serialize_dep],
  link_with : [png_lib, log_lib],
  include_directories : engine)

state_serialize_dep = declare_dependency(
  link_with : state_serialize_lib,
  dependencies: [json_dep]
)

subdir('test')
