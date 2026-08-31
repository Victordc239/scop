#version 330 core

in vec3 vertexColor;
in vec3 vertexMaterialColor;
in vec2 texCoord;

uniform sampler2D textureSampler;
uniform float textureMix;
uniform float materialMix;

out vec4 FragColor;

void main()
{
    vec4 color = vec4(vertexColor, 1.0);
    vec4 material = vec4(vertexMaterialColor, 1.0);
    vec4 textureColor = texture(textureSampler, texCoord);
    vec4 materialColor = mix(color, material, materialMix);

    FragColor = mix(materialColor, textureColor, textureMix);
}