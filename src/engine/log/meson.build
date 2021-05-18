log_lib = static_library(
  'log', [
    'Logger.cc',
    'System.cc'
  ],
  link_with : time_lib,
  dependencies : [fmt],
  include_directories : engine)

subdir('test')
