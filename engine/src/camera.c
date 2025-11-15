#include "camera.h"
#include <math.h>
#include <vulkan_commands.h>

float value;
mat4 VULKAN_MATRIX_VIEW;

void view_matrices_update(){
    value = fmodf(value * 1.1f, 2.0f);
    glm_mat4_identity(VULKAN_MATRIX_VIEW);
    glm_mat4_scale(VULKAN_MATRIX_VIEW, value);
    vulkan_commands_graphics_records();
}