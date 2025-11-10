#TARGET.CMAKE

# SOURCES
file(GLOB_RECURSE SRC 
${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)
# END SOURCES

# TARGETS
add_library(blaze_engine STATIC ${SRC})
# END TARGETS

# LIB
target_include_directories(blaze_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(blaze_engine PUBLIC ${DEPENDENCIES})
#END LIB