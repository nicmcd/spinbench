cmake_minimum_required(VERSION 3.10)
project(spinbench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindPkgConfig)

# numactl
find_library(NUMACTL_A NAMES libnuma.a PATHS ${CMAKE_INSTALL_RPATH})
if(NUMACTL_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libnuma.a not found")
endif()
string(REPLACE lib/libnuma.a include/ NUMACTL_INC ${NUMACTL_A})
if(NOT EXISTS ${NUMACTL_INC})
  message(FATAL_ERROR "numactl include dir doesn't exist")
endif()

# zlib
find_library(ZLIB_A NAMES libz.a PATHS ${CMAKE_INSTALL_RPATH})
if(ZLIB_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libz.a not found")
endif()
string(REPLACE lib/libz.a include/ ZLIB_INC ${ZLIB_A})
if(NOT EXISTS ${ZLIB_INC})
  message(FATAL_ERROR "zlib include dir doesn't exist")
endif()

# tclap
pkg_search_module(TCLAP REQUIRED tclap)
pkg_get_variable(
  TCLAP_INC
  tclap
  includedir
)

# libprim
find_library(LIBPRIM_A NAMES libprim.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBPRIM_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libprim.a not found")
endif()
string(REPLACE lib/libprim.a include/ LIBPRIM_INC ${LIBPRIM_A})
if(NOT EXISTS ${LIBPRIM_INC})
  message(FATAL_ERROR "libprim include dir doesn't exist")
endif()

# librnd
find_library(LIBRND_A NAMES librnd.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBRND_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "librnd.a not found")
endif()
string(REPLACE lib/librnd.a include/ LIBRND_INC ${LIBRND_A})
if(NOT EXISTS ${LIBRND_INC})
  message(FATAL_ERROR "librnd include dir doesn't exist")
endif()

# libdes
find_library(LIBDES_A NAMES libdes.a PATHS ${CMAKE_INSTALL_RPATH})
if(LIBDES_A MATCHES "NOTFOUND")
  message(FATAL_ERROR "libdes.a not found")
endif()
string(REPLACE lib/libdes.a include/ LIBDES_INC ${LIBDES_A})
if(NOT EXISTS ${LIBDES_INC})
  message(FATAL_ERROR "libdes include dir doesn't exist")
endif()

add_executable(
  spinbench
  ${PROJECT_SOURCE_DIR}/src/main.cc
  )

target_include_directories(
  spinbench
  PUBLIC
  ${PROJECT_SOURCE_DIR}/src
  ${NUMACTL_INC}
  ${ZLIB_INC}
  ${TCLAP_INC}
  ${LIBPRIM_INC}
  ${LIBRND_INC}
  ${LIBDES_INC}
  )

target_link_libraries(
  spinbench
  ${NUMACTL_A}
  ${ZLIB_A}
  ${LIBPRIM_A}
  ${LIBRND_A}
  ${LIBDES_A}
  ${LIBRND_A}
  ${LIBPRIM_A}
  ${ZLIB_A}
  ${NUMACTL_A}
  )

include(GNUInstallDirs)

install(
  TARGETS
  spinbench
  )

