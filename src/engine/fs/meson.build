fs_lib = static_library(
  'fs', [
    'File.cc',
    'FileInfo.cc',
    'Directory.cc',
    'Path.cc',
  ],
  link_with : core_lib,
  dependencies : [fmt],
  include_directories : engine)

subdir('test')
