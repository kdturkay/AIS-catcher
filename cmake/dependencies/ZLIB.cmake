# Find zlib
if(ZLIB)
    if(MSVC)
        find_path(ZLIB_INCLUDE_DIR zlib.h HINTS ${POTHOSSDR_INCLUDE_DIR})
        find_library(ZLIB_LIBRARY zlib.lib HINTS ${POTHOSSDR_LIBRARY_DIR})
        find_file(ZLIB_DLL zlib1.dll HINTS ${POTHOSSDR_BINARY_DIR})

        if(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY AND ZLIB_DLL)
            Message(STATUS "ZLIB: found (PothosSDR) - " ${ZLIB_INCLUDE_DIR}, ${ZLIB_LIBRARY}, ${ZLIB_DLL})
            set(COPY_ZLIB_DLL TRUE)
            add_definitions(-DHASZLIB)

            set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})
            set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
        else()
            Message(STATUS "ZLIB: not found (PothosSDR) - " ${ZLIB_INCLUDE_DIR}, ${ZLIB_LIBRARY}, ${ZLIB_DLL})
        endif()

        add_definitions(-DHASZLIB)

    else()
        pkg_check_modules(PKG_ZLIB zlib)
        find_path(ZLIB_INCLUDE_DIR zlib.h HINTS ${PKG_ZLIB_INCLUDE_DIRS})
        find_library(ZLIB_LIBRARY libz.so HINTS ${PKG_ZLIB_LIBRARY_DIRS})
        find_library(ZLIB_LIBRARY libz.dylib HINTS ${PKG_ZLIB_LIBRARY_DIRS})
        find_library(ZLIB_LIBRARY libz.tbd HINTS ${PKG_ZLIB_LIBRARY_DIRS})
        find_library(ZLIB_LIBRARY libz.dll HINTS ${PKG_ZLIB_LIBRARY_DIRS})

        if(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)

            message(STATUS "ZLIB: found - ${ZLIB_INCLUDE_DIR}, ${ZLIB_LIBRARY}")
            add_definitions(-DHASZLIB)

            set(ZLIB_INCLUDE_DIRS ${ZLIB_INCLUDE_DIR})
            set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})

        else()
            message(STATUS "ZLIB: not found - ${ZLIB_INCLUDE_DIR}, ${ZLIB_LIBRARY}")
        endif()
    endif()
endif()
