cmake_minimum_required(VERSION 3.24)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "CMake attempting in-source build")
endif()

project(PasswordManager VERSION 0.0.1 LANGUAGES CXX)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
  set(CMAKE_CXX_EXTENSIONS FALSE)

  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}"
      CACHE PATH "Installation path prefix" FORCE)
  endif()

  list(APPEND SOURCE_FILES
    src/application.cpp
    src/database.cpp
    src/main.cpp
    src/state.cpp
  )
  list(APPEND HEADER_FILES
    include/application.hpp
    include/database.hpp
    include/ncurses.hpp
    include/state.hpp
  )

  add_executable(PassMan ${SOURCE_FILES} ${HEADER_FILES})

  find_package(Curses REQUIRED)

  target_include_directories(PassMan PRIVATE include ${CURSES_INCLUDE_DIR})
  target_link_libraries(PassMan PRIVATE ${CURSES_LIBRARIES})

  install(TARGETS PassMan RUNTIME DESTINATION bin)

  if(CMAKE_EXPORT_COMPILE_COMMANDS)
    install(FILES "${CMAKE_BINARY_DIR}/compile_commands.json"
      DESTINATION "${CMAKE_SOURCE_DIR}/.vscode")
  endif()
endif()
