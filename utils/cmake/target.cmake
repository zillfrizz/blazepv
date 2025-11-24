#TARGET.CMAKE

# SOURCES
file(GLOB_RECURSE SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)
# END SOURCES

# TARGETS
add_library(blaze_utils STATIC ${SRC})
# END TARGETS

# LIB
target_include_directories(blaze_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(blaze_utils PUBLIC ${DEPENDENCIES})
#END LIB