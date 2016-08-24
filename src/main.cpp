#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "gl_helper.h"

f32 points[] = {
	-1.0,  1.0,
    -1.0, -1.0,
     1.0,  1.0,
     1.0, -1.0,
};

int main() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	int screen_height = 480;
	int screen_width = 640;

	SDL_Window *window = SDL_CreateWindow("Icicle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_GetDrawableSize(window, &screen_width, &screen_height);

    GLuint shader = load_and_build_program("src/vert.vsh", "src/frag.fsh");
	if (shader == -1)
		return 1;

	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint v_points;
	glGenBuffers(1, &v_points);
	glBindBuffer(GL_ARRAY_BUFFER, v_points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

	GLuint atlas_tex;
	SDL_Surface *atlas_surf = IMG_Load("assets/atlas.png");
	if (atlas_surf == NULL) {
		printf("%s\n", IMG_GetError());
		return 1;
	}

	glGenTextures(1, &atlas_tex);
	glBindTexture(GL_TEXTURE_2D, atlas_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas_surf->w, atlas_surf->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas_surf->pixels);
	free(atlas_surf);

	GLuint a_points = glGetAttribLocation(shader, "points");

	GLuint u_model = glGetUniformLocation(shader, "model");
	GLuint u_pv = glGetUniformLocation(shader, "pv");
	GLuint u_tex = glGetUniformLocation(shader, "tex");

	glViewport(0, 0, screen_width, screen_height);
    glEnable(GL_DEPTH_TEST);

	f32 current_time = (f32)SDL_GetTicks() / 60.0;
	f32 t = 0.0;

	glm::vec3 cam_pos = glm::vec3(0.0, 0.0, 0.0);
	f32 cam_speed = 1.0f;
	f32 scale = 1.0f;

	bool running = true;
	while (running) {
		SDL_Event event;

		f32 new_time = (f32)SDL_GetTicks() / 60.0;
		f32 dt = new_time - current_time;
		current_time = new_time;
		t += dt;

		SDL_PumpEvents();
        const u8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_W]) {
			cam_pos.y += cam_speed * dt;
		}
		if (state[SDL_SCANCODE_S]) {
			cam_pos.y -= cam_speed * dt;
		}
		if (state[SDL_SCANCODE_A]) {
			cam_pos.x -= cam_speed * dt;
		}
		if (state[SDL_SCANCODE_D]) {
			cam_pos.x += cam_speed * dt;
		}
		if (state[SDL_SCANCODE_LSHIFT]) {
			scale += cam_speed * dt;
		}
		if (state[SDL_SCANCODE_SPACE]) {
			scale -= cam_speed * dt;
		}

		if (scale < 0.25f) {
			scale = 0.25f;
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE: {
							running = false;
						} break;
					}
				} break;
				case SDL_MOUSEBUTTONDOWN: {
				} break;
				case SDL_QUIT: {
					running = false;
				} break;
			}
		}

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);

		glEnableVertexAttribArray(a_points);
		glVertexAttribPointer(a_points, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindTexture(GL_TEXTURE_2D, atlas_tex);
		glActiveTexture(GL_TEXTURE0);

		f32 s_ratio = (f32)screen_width / (f32)screen_height;
		glm::mat4 projection = glm::ortho(-10.0f * s_ratio, 10.f * s_ratio, -10.0f, 10.0f, -1.0f, 1.0f);
		glm::mat4 view = glm::mat4(1.0);
		view = glm::scale(view, glm::vec3(scale, scale, scale));
		view = glm::translate(view, cam_pos);
		glm::mat4 pv = projection * view;
		glm::mat4 model = glm::mat4(1.0);

		glUniform1i(u_tex, 0);
		glUniformMatrix4fv(u_pv, 1, GL_FALSE, &pv[0][0]);
		glUniformMatrix4fv(u_model, 1, GL_FALSE, &model[0][0]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(points));

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_Quit();
	return 0;
}
