
# Generate meta-header
configure_file(mpss2015-version.h.in ${CMAKE_CURRENT_BINARY_DIR}/mpss2015-version.h)

# We do NOT want to deploy this header (at least not for binary packages)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mpss2015/mpss2015-version.h DESTINATION ${INSTALL_INCLUDE}/mpss2015)

# Include directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

find_package(OpenCV REQUIRED)

# Libraries
set(IDE_FOLDER "")
if (CUDA_VERSION)
    add_subdirectory(ofextract)
endif ()

# Applications
set(IDE_FOLDER "Applications")
if (CUDA_VERSION)
    add_subdirectory(ofextractcmd)
endif ()

# Tests
set(IDE_FOLDER "Tests")
add_subdirectory(tests)
