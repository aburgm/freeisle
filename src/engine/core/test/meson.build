t = executable(
  'core_test',
  [
    'TestBitmask.cc',
    'TestEnum.cc',
    'TestEnumMap.cc',
    'TestGrid.cc',
    'TestSentinel.cc',
    'TestString.cc'
  ],
  dependencies : gtest,
  link_with : core_lib,
  include_directories : engine)

test('core', t)
