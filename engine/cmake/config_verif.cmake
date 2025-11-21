set(LOCAL_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.cmake")
set(TEMPLATE_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.template.cmake")

# --- Vérification de présence ---

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR} AND NOT EXISTS "${LOCAL_CONFIG_FILE}")
    message(WARNING 
        "Le fichier de configuration '${CMAKE_CURRENT_SOURCE_DIR}/config.cmake' est absent.\n"
        "Copie du fichier template..."
    )
    file(COPY "${TEMPLATE_CONFIG_FILE}" DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
    file(RENAME "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.template.cmake"
                     "${LOCAL_CONFIG_FILE}")

    message(FATAL_ERROR 
        "Un fichier 'config.cmake' vient d'être créé.\n"
        "Merci de le compléter puis relancer CMake."
    )
endif()

# --- Load config ---

include(${LOCAL_CONFIG_FILE})

# --- Vérification du contenu ---

set(MISSING_VARS "")

foreach(var MY_TOOLCHAIN_PATH MY_COMPILER_PATH)
    if(NOT DEFINED ${var} OR "${${var}}" STREQUAL "")
        list(APPEND MISSING_VARS ${var})
    endif()
endforeach()

if(MISSING_VARS)
    message(WARNING 
        "Le fichier config.cmake est incomplet.\n"
        "Variables manquantes : ${MISSING_VARS}"
    )

    message(FATAL_ERROR 
        "Merci de compléter 'cmake/config.cmake' puis relancer CMake."
    )
endif()

# --- Log propre ---
message(STATUS "Configuration locale chargée avec succès :")
message(STATUS "  MY_TOOLCHAIN_PATH = ${MY_TOOLCHAIN_PATH}")
message(STATUS "  MY_COMPILER_PATH  = ${MY_COMPILER_PATH}")

# --- Application de la config ---
set(CMAKE_PROGRAM_PATH "${MY_TOOLCHAIN_PATH}/bin" ${CMAKE_PROGRAM_PATH})
set(CMAKE_C_COMPILER "${MY_COMPILER_PATH}/gcc")
set(CMAKE_CXX_COMPILER "${MY_COMPILER_PATH}/g++")
