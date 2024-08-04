set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_LIBRARY_LINKAGE dynamic)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)

set(VCPKG_CXX_FLAGS "-stdlib=libc++")
set(VCPKG_C_FLAGS "")
set(VCPKG_LINKER_FLAGS "-fuse-ld=lld -stdlib=libc++")

set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/../toolchains/clang.cmake)
