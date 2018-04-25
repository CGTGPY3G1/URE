// used by DebugDraw to draw lines
// basic.vert
#version 330
layout (location = 0) in vec3 position;
layout (location = 5) in vec3 colour;
uniform mat4 viewprojection;
out vec3 ex_Color;


void main(void) {
    ex_Color = colour;
    vec4 vertexPosition = vec4(position,1.0);
    gl_Position = viewprojection * vertexPosition;
}