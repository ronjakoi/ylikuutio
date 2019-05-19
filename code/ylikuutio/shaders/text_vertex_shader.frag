#version 130

// Interpolated values from the vertex shaders
varying vec2 UV;

// Values that stay constant for the whole mesh.
uniform sampler2D texture_sampler;

out vec4 color;

void main()
{
    color = texture(texture_sampler, UV);
}
