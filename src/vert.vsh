#version 330

in vec2 points;
in vec3 pos;
in uint tex_points;
in uint tex_idx;

uniform mat4 pv;

out vec2 f_tex_points;

void main() {
    mat4 model = mat4(vec4(1.0, 0.0, 0.0, 0.0),
 					  vec4(0.0, 1.0, 0.0, 0.0),
					  vec4(0.0, 0.0, 1.0, 0.0),
				      vec4(pos, 1.0));

	gl_Position = pv * model * vec4(points, 0.0, 1.0);

	uint atlas_width = 4u;

	float scalar = 1.0f / float(atlas_width);
	float y = ((tex_idx - 1u) % atlas_width) * scalar;
	float x = ((tex_idx - 1u) / atlas_width) * scalar;

	switch (tex_points) {
		case 0: {
			f_tex_points = vec2(x, y);
		} break;
		case 1: {
			f_tex_points = vec2(x + scalar, y);
		} break;
		case 2: {
			f_tex_points = vec2(x, y + scalar);
		} break;
		case 3: {
			f_tex_points = vec2(x + scalar, y + scalar);
		} break;
	}
}
