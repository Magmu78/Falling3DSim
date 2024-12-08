#version 330 core

out vec4 FragColor; // Couleur du fragment

uniform vec3 color; // Couleur de l'objet

void main() {
    FragColor = vec4(color, 1.0);
}