function (disallow_intree_builds)
  # Adapted from LLVM's/UTF8proc toplevel CMakeLists.txt file
  if( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
    message(SYSTEM_ERROR_FATAL "
      In-source builds are not allowed.  Please create a directory
      and run cmake from there. Building in a subdirectory is
      fine, e.g.:
      
        mkdir build
        cd build
        cmake ..
      
      This process created the file `CMakeCache.txt' and the
      directory `CMakeFiles'. Please delete them.
      
      ")
  endif()
endfunction()
