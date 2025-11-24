#CMakeLists.txt

# PROJECT
cmake_minimum_required(VERSION 3.20)
project(BlazeUtils)
# END PROJECT

# CMAKE COMPONENTS
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# CMAKE COMPONENTS

include(vars)
include(dependencies)
include(target)
include(properties)