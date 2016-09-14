#version 330

in vec2 f_tex_points;

uniform sampler2D tex;

out vec4 color;

void main() {
    vec4 out_color = texture(tex, f_tex_points);

	if (out_color.a < 0.5f) {
		discard;
	}

	color = out_color;
}
