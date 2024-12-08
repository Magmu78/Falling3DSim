#version 330 core

layout (location = 0) in vec3 aPos; // Position du sommet

uniform mat4 model; // Matrice de transformation pour l'objet
uniform mat4 camMatrix;

void main() {
    gl_Position = model * camMatrix * vec4(aPos, 1.0);
}