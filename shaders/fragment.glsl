#version 330 core

in vec3 vertexColor;
in vec2 texCoord;

uniform sampler2D textureSampler;
uniform float textureMix;

out vec4 FragColor;

void main()
{
    vec4 color = vec4(vertexColor, 1.0);
    vec4 textureColor = texture(textureSampler, texCoord);
    FragColor = mix(color, textureColor, textureMix);
}