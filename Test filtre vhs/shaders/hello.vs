#version 330 
layout (location = 0) in vec2 a_position;

out vec2 TexCoords;

void main() {
    TexCoords = (a_position + 1.0) * 0.5;  // Transforme de [-1,1] à [0,1]
    gl_Position = vec4(a_position, 0.0, 1.0);
}

