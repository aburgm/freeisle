json_lib = static_library(
  'json', [
    'Loader.cc',
  ],
  link_with : [core_lib, fs_lib],
  dependencies : [fmt, jsoncpp],
  include_directories : engine)

json_dep = declare_dependency(
  link_with : json_lib,
  dependencies: [jsoncpp]
)

subdir('test')
