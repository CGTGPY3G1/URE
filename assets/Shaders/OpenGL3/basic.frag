// used by DebugDraw to draw lines
#version 330

in vec3 ex_Color;
out vec4 out_Color;

void main() {
     out_Color = vec4(ex_Color, 1.0);
}