#version 330 core
out vec4 FragColor;
in vec2 TexCord;
uniform sampler2D texture11;
uniform sampler2D texture33;
uniform sampler2D texturesmall;
uniform sampler2D textureLYE;
uniform sampler2D textureCAI;
uniform float ratio;


void main()
{
    FragColor =mix(texture(texture11,TexCord),texture(textureLYE,TexCord),ratio);

}
