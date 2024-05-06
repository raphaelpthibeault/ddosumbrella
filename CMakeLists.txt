cmake_minimum_required(VERSION 3.28)
project(ddosumbrella C)

set(CMAKE_C_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBNL3 REQUIRED libnl-3.0 libnl-genl-3.0 libnl-route-3.0)
pkg_check_modules(LIBMNL REQUIRED libmnl)

add_executable(ddosumbrella
        detection.c
        umbrella.h)

target_include_directories(ddosumbrella PRIVATE ${LIBNL3_INCLUDE_DIRS} ${LIBMNL_INCLUDE_DIRS})

target_link_libraries(ddosumbrella ${LIBNL3_LIBRARIES} ${LIBMNL_LIBRARIES})

target_link_options(ddosumbrella PRIVATE ${LIBNL3_LDFLAGS} ${LIBMNL_LDFLAGS})

target_compile_definitions(ddosumbrella PRIVATE _GNU_SOURCE)