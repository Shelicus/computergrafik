# CMake generated Testfile for 
# Source directory: C:/Dokumente/workspace/computergrafik/math
# Build directory: C:/Dokumente/workspace/computergrafik/math
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(MathTests "C:/Dokumente/workspace/computergrafik/math/math_test.exe")
set_tests_properties(MathTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Dokumente/workspace/computergrafik/math/CMakeLists.txt;21;add_test;C:/Dokumente/workspace/computergrafik/math/CMakeLists.txt;0;")
subdirs("extern/googletest")
