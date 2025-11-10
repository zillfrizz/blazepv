#PROPERTIES.CMAKE

# Activer LTO si supporté
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported OUTPUT error)

if(lto_supported)
    set_target_properties(blaze_engine PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "LTO (Interprocedural Optimization) is supported and has been enabled.")
else()
    message(STATUS "LTO (Interprocedural Optimization) is not supported on this platform/with this compiler.")
    message(STATUS "Details: ${error}")
endif()
