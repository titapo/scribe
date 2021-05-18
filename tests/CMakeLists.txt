add_library(catch INTERFACE)
target_include_directories(catch INTERFACE ${CATCH_SINGLE_INCLUDE_DIR})

add_executable(unit_test
  Array.cc
  basic_types.cc
  BasicPathFormat.cc
  Command.cc
  ComplexTypeNotion.cc
  EntityFormatter.cc
  Function.cc
  Leaf.cc
  main.cc
  Meta.cc
  Node.cc
  Path.cc
  Tree.cc
  TypeName.cc
  TypeRegistry.cc
)
target_link_libraries(unit_test
  PRIVATE scribe
  PRIVATE catch
  )
