json_lib = static_library(
  'json', [
    'Loader.cc', 'Saver.cc', 'LoadUtil.cc', 'SaveUtil.cc',
  ],
  link_with : [core_lib, fs_lib, base64_lib],
  dependencies : [fmt, jsoncpp],
  include_directories : engine)

json_dep = declare_dependency(
  link_with : json_lib,
  dependencies: [jsoncpp]
)

subdir('test')
