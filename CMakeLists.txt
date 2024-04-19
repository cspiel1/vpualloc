cmake_minimum_required(VERSION 3.22)

project(vpualloc)

add_executable(vpualloc 
vpualloc.c)

target_link_libraries(vpualloc fslvpuwrap)

install(TARGETS vpualloc
  RUNTIME
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT Applications
)
