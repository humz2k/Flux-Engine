/**
 * @file empty_lights.fs
 **/

#version 330

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main()
{
    vec4 base_color = texture(texture0, fragTexCoord) * colDiffuse;

    // gamma correction
    finalColor = base_color;
}