#version 330

in vec2 points;

uniform mat4 model;
uniform mat4 pv;

out vec2 f_tex_points;

void main() {
	gl_Position = pv * model * vec4(points, 0.0, 1.0);
    f_tex_points = points;
}
