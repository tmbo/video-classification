
# Generate meta-header
configure_file(mpss2015-version.h.in ${CMAKE_CURRENT_BINARY_DIR}/mpss2015-version.h)

# We do NOT want to deploy this header (at least not for binary packages)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mpss2015/mpss2015-version.h DESTINATION ${INSTALL_INCLUDE}/mpss2015)

# Include directories
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Libraries
set(IDE_FOLDER "")
add_subdirectory(ofextract)

# Applications
set(IDE_FOLDER "Applications")
add_subdirectory(ofextractcmd)

# Tests
set(IDE_FOLDER "Tests")
add_subdirectory(tests)
