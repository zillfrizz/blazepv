#version 450

layout(set = 0, binding = 0) uniform UniformData {
    mat4 mat;
} ubo;

layout(location = 0) in vec3 inPosition; 
layout(location = 1) in uint inColorId;

layout(location = 0) out vec3 fragColor;

const vec3 faceColors[6] = {
    vec3(1,0,0),
    vec3(0,1,0),
    vec3(0,0,1),
    vec3(1,1,0),
    vec3(0,1,1),
    vec3(1,0,1)
};

void main() {

    gl_Position = ubo.mat * vec4(inPosition, 1.0);

    fragColor = faceColors[inColorId];
}
