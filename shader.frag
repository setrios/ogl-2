#version 330 core

// don't need layout as do not actually pull data out of buffer

// INSERTED - variables to track if model has texture
uniform bool useTexture;
uniform vec3 solidColor;
////////

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{

    // FragColor = texture(texture_diffuse1, TexCoords);

    // INSERTED - variables to track if model has texture
    vec4 color;
    if (useTexture)
        color = texture(texture_diffuse1, TexCoords);
    else
        color = vec4(solidColor, 1.0);

    FragColor = color;
    ////////
} 