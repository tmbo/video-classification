set(MPSS2015_INCLUDES "")
set(MPSS2015_LIBRARIES "")

# Definition of function "find" with two mandatory arguments, "LIB_NAME" and "HEADER".
macro (find LIB_NAME HEADER)

    set(HINT_PATHS ${ARGN})

    if (${LIB_NAME} STREQUAL "mpss2015")
        set(LIB_NAME_UPPER MPSS2015)
        set(LIBNAME mpss2015)
    else()
        string(TOUPPER MPSS2015_${LIB_NAME} LIB_NAME_UPPER)
        set(LIBNAME ${LIB_NAME})
    endif()

    find_path(
	${LIB_NAME_UPPER}_INCLUDE_DIR
	${HEADER}
        ${ENV_MPSS2015_DIR}/include
        ${ENV_MPSS2015_DIR}/source/${LIB_NAME}/include
        ${MPSS2015_DIR}/include
        ${MPSS2015_DIR}/source/${LIB_NAME}/include
        ${ENV_PROGRAMFILES}/mpss2015/include
        /usr/include
        /usr/local/include
        /sw/include
        /opt/local/include
        DOC "The directory where ${HEADER} resides"
    )


    find_library(
	${LIB_NAME_UPPER}_LIBRARY_RELEASE
        NAMES ${LIBNAME}
        PATHS ${HINT_PATHS}
        DOC "The ${LIB_NAME} library"
    )
    find_library(
	${LIB_NAME_UPPER}_LIBRARY_DEBUG
        NAMES ${LIBNAME}d
        PATHS ${HINT_PATHS}
        DOC "The ${LIB_NAME} debug library"
    )
    

    if(${LIB_NAME_UPPER}_LIBRARY_RELEASE AND ${LIB_NAME_UPPER}_LIBRARY_DEBUG)
        set(${LIB_NAME_UPPER}_LIBRARY "optimized" ${${LIB_NAME_UPPER}_LIBRARY_RELEASE} "debug" ${${LIB_NAME_UPPER}_LIBRARY_DEBUG})
    elseif(${LIB_NAME_UPPER}_LIBRARY_RELEASE)
        set(${LIB_NAME_UPPER}_LIBRARY ${${LIB_NAME_UPPER}_LIBRARY_RELEASE})
    elseif(${LIB_NAME_UPPER}_LIBRARY_DEBUG)
        set(${LIB_NAME_UPPER}_LIBRARY ${${LIB_NAME_UPPER}_LIBRARY_DEBUG})
    endif()

    list(APPEND MPSS2015_INCLUDES ${${LIB_NAME_UPPER}_INCLUDE_DIR})
    list(APPEND MPSS2015_LIBRARIES ${${LIB_NAME_UPPER}_LIBRARY})

    # DEBUG MESSAGES
    # message("${LIB_NAME_UPPER}_INCLUDE_DIR     = ${${LIB_NAME_UPPER}_INCLUDE_DIR}")
    # message("${LIB_NAME_UPPER}_LIBRARY_RELEASE = ${${LIB_NAME_UPPER}_LIBRARY_RELEASE}")
    # message("${LIB_NAME_UPPER}_LIBRARY_DEBUG   = ${${LIB_NAME_UPPER}_LIBRARY_DEBUG}")
    # message("${LIB_NAME_UPPER}_LIBRARY         = ${${LIB_NAME_UPPER}_LIBRARY}")

endmacro(find)


# load standard CMake arguments (c.f. http://stackoverflow.com/questions/7005782/cmake-include-findpackagehandlestandardargs-cmake)
include(FindPackageHandleStandardArgs)

if(CMAKE_CURRENT_LIST_FILE)
    get_filename_component(MPSS2015_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
endif()

file(TO_CMAKE_PATH "$ENV{PROGRAMFILES}" ENV_PROGRAMFILES)
file(TO_CMAKE_PATH "$ENV{MPSS2015_DIR}" ENV_MPSS2015_DIR)

set(LIB_PATHS
    ${MPSS2015_DIR}/build
    ${MPSS2015_DIR}/build/Release
    ${MPSS2015_DIR}/build/Debug
    ${MPSS2015_DIR}/build-release
    ${MPSS2015_DIR}/build-debug
    ${MPSS2015_DIR}/lib
    ${ENV_MPSS2015_DIR}/lib
    ${ENV_PROGRAMFILES}/mpss2015/lib
    /usr/lib
    /usr/local/lib
    /sw/lib
    /opt/local/lib
    /usr/lib64
    /usr/local/lib64
    /sw/lib64
    /opt/local/lib64
)

# Find libraries
find(fiblib fiblib/fiblib_api.h ${LIB_PATHS})

if(MPSS2015_FIBLIB_LIBRARY)
  # add dependencies
endif()


# DEBUG
# message("MPSS2015_INCLUDES  = ${MPSS2015_INCLUDES}")
# message("MPSS2015_LIBRARIES = ${MPSS2015_LIBRARIES}")

find_package_handle_standard_args(MPSS2015 DEFAULT_MSG MPSS2015_LIBRARIES MPSS2015_INCLUDES)
mark_as_advanced(MPSS2015_FOUND)
