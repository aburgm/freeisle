clock_gettime_mock = shared_library('clock_gettime_mock', ['mock/clock_gettime_mock.c'])

test_env = environment()
test_env.prepend('LD_PRELOAD', clock_gettime_mock.full_path())

t = executable(
  'time_test',
  ['TestDuration.cc', 'TestInstant.cc', 'TestStopWatch.cc', 'TestSystemClock.cc'],
  dependencies : [gtest],
  link_with : time_lib,
  include_directories : engine)

test('time', t, env : test_env)
