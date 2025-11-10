#APP:TARGET.CMAKE

# SOURCES
file(GLOB_RECURSE SRC 
${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)
# END SOURCES

# TARGETS
add_executable(blaze_app ${SRC})
# END TARGETS

# LIB
target_include_directories(blaze_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(blaze_app PUBLIC ${DEPENDENCIES})
#END LIB