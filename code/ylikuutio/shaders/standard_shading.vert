#version 120

// Input vertex data, different for all executions of this shader.
attribute vec3 vertex_position_modelspace;
attribute vec2 vertexUV;
attribute vec3 vertex_normal_modelspace;

// Output data ; will be interpolated for each fragment.
varying vec2 UV;
varying vec3 position_worldspace;
varying vec3 normal_cameraspace;
varying vec3 eye_direction_cameraspace;
varying vec3 LightDirection_cameraspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

void main()
{
    // Output position of the vertex, in clip space : MVP * position
    gl_Position = MVP * vec4(vertex_position_modelspace, 1);

    // Position of the vertex, in worldspace : M * position
    position_worldspace = (M * vec4(vertex_position_modelspace, 1)).xyz;

    // Vector that goes from the vertex to the camera, in camera space.
    // In camera space, the camera is at the origin (0, 0, 0).
    vec3 vertexPosition_cameraspace = (V * M * vec4(vertex_position_modelspace, 1)).xyz;
    eye_direction_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;

    // Vector that goes from the vertex to the light, in camera space. M is ommited because it's identity.
    vec3 LightPosition_cameraspace = (V * vec4(LightPosition_worldspace, 1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace + eye_direction_cameraspace;

    // Normal of the the vertex, in camera space
    normal_cameraspace = (V * M * vec4(vertex_normal_modelspace, 0)).xyz; // Only correct if ModelMatrix does not scale the model ! Use its inverse transpose if not.

    // UV of the vertex. No special space for this one.
    UV = vertexUV;
}
