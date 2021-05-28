# apt-get install libgtest-dev libfmt-dev libjsoncpp-dev libpng-dev
gtest = dependency('gtest', main : true)
fmt = dependency('fmt') 
jsoncpp = dependency('jsoncpp') 
libpng = dependency('libpng') 

# only required for mocking system and library calls in unit tests.
# TODO(armin): allow this to be not found and disable the corresponding
# tests in that case.
cc = meson.get_compiler('c')
dl = cc.find_library('dl')

subdir('engine')
