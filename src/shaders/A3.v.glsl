#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
// TODO #D: add normal matrix
uniform mat3 normalMatrix;

// TODO #A: add light uniforms
uniform vec3 lightDirection;
uniform vec3 lightColor;

uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
// TODO #C: add vertex normal
layout(location = 1) in vec3 vertexNormal;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // TODO #B: compute Light vector
    vec3 lightVec = normalize(-lightDirection);

    // TODO #E: transform normal vector
    vec3 worldSpaceNormal = normalize(normalMatrix * vertexNormal);

    // TODO #F: perform diffuse calculation
    float diffuseFactor = max(dot(worldSpaceNormal, lightVec), 0.0);
    vec3 diffuseColor = lightColor * materialColor * diffuseFactor;

    // TODO #G: assign the color for this vertex
    color = diffuseColor;
}