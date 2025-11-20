#version 450

layout(location = 0) in vec3 inPosition; 
layout(location = 1) in uint inColorId;

layout(location = 0) out vec3 fragColor;

vec3 faceColors[6] = vec3[](
    vec3(1,0,0),
    vec3(0,1,0),
    vec3(0,0,1),
    vec3(1,1,0),
    vec3(0,1,1),
    vec3(1,0,1)
);

void main() {
    gl_Position = projection * model * vec4(inPosition, 1.0);

    fragColor = faceColors[inColorId];
}
