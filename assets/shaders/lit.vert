#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
uniform mat4 transform;
uniform vec2 u_entityWorldPos;
out vec2 TexCoord;
out vec2 vWorldPos;
void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    vWorldPos = vec2(u_entityWorldPos.x + aPos.x, u_entityWorldPos.y + aPos.y);
}
