#DEPENDENCIES.CMAKE

# PACKAGES
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(volk REQUIRED)
find_package(json-c REQUIRED)
find_package(cglm CONFIG REQUIRED)
# END PACKAGES

set(DEPENDENCIES
    blaze_utils
    cglm::cglm
    volk::volk
    json-c::json-c
    SDL2::SDL2 
    SDL2_image::SDL2_image-static
    SDL2_mixer::SDL2_mixer-static
    SDL2_ttf::SDL2_ttf-static
)