# -*- cmake -*-
#
# unice68

# ======================================================================
# cmake setup

cmake_minimum_required(VERSION 3.3)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)

# ======================================================================
# Modules

include(CheckIncludeFile)
include(CheckFunctionExists)
include(GNUInstallDirs)

# ======================================================================
# Options

option(
  BUILD_UNICE68_CLI
  "Build unice68 command line executable"
  ON)

option(
  BUILD_SHARED_LIBS
  "Prefer shared library over static"
  ON)

option(
  VERBOSE_MAKEFILE
  "Generate verbose Makefile"
  OFF)
set(CMAKE_VERBOSE_MAKEFILE ${VERBOSE_MAKEFILE})

# ======================================================================
# Project

execute_process(
  COMMAND ./vcversion.sh 2.0.0
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  TIMEOUT 15
  RESULT_VARIABLE VC_RES
  OUTPUT_VARIABLE VC_OUT
  ERROR_QUIET)
if (NOT "0" STREQUAL "${VC_RES}")
  message(
    FATAL_ERROR
    "Failed to get revision from source control:\n${VC_RES}")
endif (NOT "0" STREQUAL "${VC_RES}")
string(STRIP "${VC_OUT}" VC_OUT)

project(
  unice68
  VERSION ${VC_OUT}
  LANGUAGES C)

set(PACKAGE "${PROJECT_NAME}")
set(VERSION "${PROJECT_VERSION}")
set(PACKAGE_BUGREPORT "http://sourceforge.net/projects/sc68")
set(PACKAGE_URL "http://sc68.atari.org")
set(PACKAGE_DESC "Ice packer/depacker program and library")
set(PACKAGE_SHORTDESC "${PACKAGE_DESC}")

# ======================================================================
# Configs
#
# $$$ Shouldn't those file be by configuration instead of by build ?

set(prefix "${CMAKE_INSTALL_PREFIX}")
set(exec_prefix "\${prefix}")
set(datarootdir "\${prefix}/${CMAKE_INSTALL_DATAROOTDIR}")
set(datadir "\${datarootdir}")
set(includedir "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}")
set(libdir "\${prefix}/${CMAKE_INSTALL_LIBDIR}")

configure_file (
  "${PROJECT_SOURCE_DIR}/cm_config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  @ONLY)

configure_file (
  "${PROJECT_SOURCE_DIR}/unice68.pc.in"
  "${PROJECT_BINARY_DIR}/unice68.pc"
  @ONLY)

add_definitions(-DHAVE_CONFIG_H)

# ======================================================================
# Target library: unice68

CHECK_INCLUDE_FILE(assert.h HAVE_ASSERT_H)
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)

add_library(unice68_lib
  unice68_pack.c unice68_unpack.c unice68_version.c)

set_target_properties(
  unice68_lib PROPERTIES
  OUTPUT_NAME unice68
  C_STANDARD 99
  C_VISIBILITY_PRESET hidden
  )

install(
  TARGETS unice68_lib
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(
  FILES unice68.h DESTINATION include
  )

install(
  FILES ${PROJECT_BINARY_DIR}/unice68.pc DESTINATION lib/pkgconfig
  )

# ======================================================================
# Target binary: unice68

if (BUILD_UNICE68_CLI)
  
  CHECK_INCLUDE_FILE (libgen.h HAVE_LIBGEN_H)
  CHECK_INCLUDE_FILE (unistd.h HAVE_UNISTD_H)
  CHECK_INCLUDE_FILE (fcntl.h HAVE_FCNTL_H)
  CHECK_INCLUDE_FILE (io.h HAVE_IO_H)

  check_function_exists (basename HAVE_BASENAME)
  check_function_exists (setmode HAVE_SETMODE)
  check_function_exists (_setmode HAVE__SETMODE)

  add_executable(unice68_exe unice68.c)
  target_link_libraries(unice68_exe unice68_lib)
  set_target_properties(
    unice68_exe PROPERTIES
    OUTPUT_NAME unice68
    C_STANDARD 99
    COMPILE_DEFINITIONS "")
  install(
    TARGETS unice68_exe
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )
  
endif (BUILD_UNICE68_CLI)
