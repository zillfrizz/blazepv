#TARGET.CMAKE

# SOURCES
file(GLOB_RECURSE SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)
# END SOURCES

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in
    ${CMAKE_BINARY_DIR}/config.h
)

# TARGETS
add_library(blaze_engine STATIC ${SRC})
# END TARGETS

# LIB
target_include_directories(blaze_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_BINARY_DIR})
target_link_libraries(blaze_engine PUBLIC ${DEPENDENCIES})
#END LIB