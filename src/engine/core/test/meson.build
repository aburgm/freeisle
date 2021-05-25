t = executable(
  'core_test',
  ['TestBitmask.cc', 'TestEnum.cc', 'TestGrid.cc', 'TestPtr.cc', 'TestString.cc'],
  dependencies : gtest,
  link_with : core_lib,
  include_directories : engine)

test('core', t)
