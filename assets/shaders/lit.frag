#version 330 core
uniform sampler2D tex;
uniform vec3 color;
uniform vec3 u_ambientColor;
uniform float u_ambientIntensity;
uniform int u_lightCount;
uniform vec3 u_lightPos[4];
uniform vec3 u_lightColor[4];
uniform float u_lightRadius[4];
uniform float u_lightIntensity[4];
in vec2 TexCoord;
in vec2 vWorldPos;
out vec4 FragColor;
void main() {
    vec4 texel = texture(tex, TexCoord);
    vec3 light = u_ambientColor * u_ambientIntensity;
    for (int i = 0; i < u_lightCount; i++) {
        float d = distance(vWorldPos, u_lightPos[i].xy);
        float atten = u_lightIntensity[i] / (1.0 + (d*d)/(u_lightRadius[i]*u_lightRadius[i]));
        light += u_lightColor[i] * atten;
    }
    light = clamp(light, 0.0, 1.0);
    FragColor = vec4(texel.rgb * color * light, texel.a);
}
