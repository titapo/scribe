cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(Scribe
  VERSION 0.0.1
  LANGUAGES CXX
  DESCRIPTION "Describe anything"
  )

add_subdirectory(scribe)
add_subdirectory(external/catch)

add_subdirectory(tests)
