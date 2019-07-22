#version 330 core
in vec2 Tex;
out vec4 color;

uniform sampler2D img;

void main() {
    color = texture(img, Tex);
}