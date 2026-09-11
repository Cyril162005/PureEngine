#version 330 core
uniform sampler2D tex;
uniform vec3 color;
in vec2 TexCoord;
out vec4 FragColor;
void main() {
    vec4 texel = texture(tex, TexCoord);
    FragColor = vec4(texel.rgb * color, texel.a);
}
