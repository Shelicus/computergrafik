# CMake generated Testfile for 
# Source directory: D:/Dokumente/workspace/workspace_computergrafik/math
# Build directory: D:/Dokumente/workspace/workspace_computergrafik/math/out/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(MathTests "D:/Dokumente/workspace/workspace_computergrafik/math/out/build/math_test.exe")
set_tests_properties(MathTests PROPERTIES  _BACKTRACE_TRIPLES "D:/Dokumente/workspace/workspace_computergrafik/math/CMakeLists.txt;17;add_test;D:/Dokumente/workspace/workspace_computergrafik/math/CMakeLists.txt;0;")
subdirs("googletest")
