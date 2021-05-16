t = executable(
  'time_test',
  ['TestDuration.cc', 'TestInstant.cc'],
  dependencies : gtest,
  link_with : time_lib,
  include_directories : engine)

test('time', t)
