cmake_minimum_required(VERSION 3.20)
project(spinbench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FindPkgConfig)

# numactl
pkg_check_modules(numactl REQUIRED IMPORTED_TARGET numa)
  get_target_property(
  NUMACTL_INC
  PkgConfig::numactl
  INTERFACE_INCLUDE_DIRECTORIES
)

# zlib
pkg_check_modules(zlib REQUIRED IMPORTED_TARGET zlib)
  get_target_property(
  ZLIB_INC
  PkgConfig::zlib
  INTERFACE_INCLUDE_DIRECTORIES
)

# tclap
pkg_check_modules(tclap REQUIRED IMPORTED_TARGET tclap)
  get_target_property(
  TCLAP_INC
  PkgConfig::tclap
  INTERFACE_INCLUDE_DIRECTORIES
)

# libprim
pkg_check_modules(libprim REQUIRED IMPORTED_TARGET libprim)
  get_target_property(
  LIBPRIM_INC
  PkgConfig::libprim
  INTERFACE_INCLUDE_DIRECTORIES
)

# librnd
pkg_check_modules(librnd REQUIRED IMPORTED_TARGET librnd)
  get_target_property(
  LIBRND_INC
  PkgConfig::librnd
  INTERFACE_INCLUDE_DIRECTORIES
)

# libdes
pkg_check_modules(libdes REQUIRED IMPORTED_TARGET libdes)
  get_target_property(
  LIBDES_INC
  PkgConfig::libdes
  INTERFACE_INCLUDE_DIRECTORIES
)

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
  PkgConfig::numactl
  PkgConfig::zlib
  PkgConfig::tclap
  PkgConfig::libprim
  PkgConfig::librnd
  PkgConfig::libdes
  )

include(GNUInstallDirs)

install(
  TARGETS
  spinbench
  )

