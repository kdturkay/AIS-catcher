# Find libcurl
if(CURL)
    if(MSVC AND NOT MSVC_VCPKG)
        find_path(CURL_INCLUDE_DIR curl/curl.h HINTS ${POTHOSSDR_INCLUDE_DIR})
        find_library(CURL_LIBRARY libcurl.lib HINTS ${POTHOSSDR_LIBRARY_DIR})
        find_file(CURL_DLL libcurl.dll HINTS ${POTHOSSDR_BINARY_DIR})
        find_file(ZLIB1_DLL zlib1.dll HINTS ${POTHOSSDR_BINARY_DIR})

        if(CURL_INCLUDE_DIR AND CURL_LIBRARY AND CURL_DLL AND ZLIB1_DLL)
            Message(STATUS "CURL: found (PothosSDR) - " ${CURL_INCLUDE_DIR}, ${CURL_LIBRARY}, ${CURL_DLL}, ${ZLIB1_DLL})
            set(COPY_CURL_DLL TRUE)
            set(COPY_ZLIB1_DLL TRUE)
            add_definitions(-DHASCURL)

            set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR})
            set(CURL_LIBRARIES ${CURL_LIBRARY})
        else()
            Message(STATUS "CURL: NOT found (PothosSDR) - " ${CURL_INCLUDE_DIR}, ${CURL_LIBRARY}, ${CURL_DLL})
        endif()
    else()
        pkg_check_modules(PKG_CURL libcurl)
        find_path(CURL_INCLUDE_DIR curl/curl.h HINTS ${PKG_CURL_INCLUDE_DIRS})
        find_library(CURL_LIBRARY libcurl.so HINTS ${PKG_CURL_LIBRARY_DIRS})
        find_library(CURL_LIBRARY libcurl.dylib HINTS ${PKG_CURL_LIBRARY_DIRS})
        find_library(CURL_LIBRARY curl HINTS ${PKG_CURL_LIBRARY_DIRS})

        if(CURL_INCLUDE_DIR AND CURL_LIBRARY)

            message(STATUS "CURL: found - ${CURL_INCLUDE_DIR}, ${CURL_LIBRARY}")
            add_definitions(-DHASCURL)

            set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIR})
            set(CURL_LIBRARIES ${CURL_LIBRARY})

        else()
            message(STATUS "CURL: not found - ${CURL_INCLUDE_DIR}, ${CURL_LIBRARY}")
        endif()
    endif()
endif()
