serialize_lib = static_library(
  'serialize', [
    'MapDefHandlers.cc',
  ],
  dependencies : [json_dep],
  link_with : [png_lib, log_lib],
  include_directories : engine)

serialize_dep = declare_dependency(
  link_with : serialize_lib,
  dependencies: [json_dep]
)

subdir('test')
