# CMAKE generated file: DO NOT EDIT!
# Generated by CMake Version 3.28
cmake_policy(SET CMP0009 NEW)

# MY_SOURCES at CMakeLists.txt:23 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Engineering/OpenglCppProjects/Falling3DObject/src/*.cpp")
set(OLD_GLOB
  "D:/Engineering/OpenglCppProjects/Falling3DObject/src/demoShaderLoader.cpp"
  "D:/Engineering/OpenglCppProjects/Falling3DObject/src/main.cpp"
  "D:/Engineering/OpenglCppProjects/Falling3DObject/src/openglDebug.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Engineering/OpenglCppProjects/Falling3DObject/out/build/x64-Debug/CMakeFiles/cmake.verify_globs")
endif()