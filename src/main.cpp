#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "gl_helper.h"
#include "hashmap.h"
#include "earr.h"

u8 tex_points[] = {
	1, 0, 3, 2
};

f32 points[] = {
	-0.5,  0.5,
    -0.5, -0.5,
     0.5,  0.5,
     0.5, -0.5,
};

u16 map_width = 20;
u16 map_height = 20;
u16 map_size = map_width * map_height;

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

	GLuint v_tex_points;
	glGenBuffers(1, &v_tex_points);
	glBindBuffer(GL_ARRAY_BUFFER, v_tex_points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_points), tex_points, GL_STATIC_DRAW);

	Entity *tile_map = (Entity *)malloc(sizeof(Entity) * map_size);
	for (u32 x = 0; x < map_width; x++) {
		for (u32 y = 0; y < map_height; y++) {
			tile_map[COMPRESS_THREE(x, y, 0, map_width, map_height)] = new_entity(1, glm::vec3((f32)x, (f32)y, 0.0f));
		}
	}

	EntityArr *entity_map = new_earr();
	Entity player = new_entity(9, glm::vec3(0.0f, 0.0f, 1.0f));
	ea_push(entity_map, player);

	GLuint v_entity;
	glGenBuffers(1, &v_entity);

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
	GLuint a_tex_points = glGetAttribLocation(shader, "tex_points");
	GLuint a_pos = glGetAttribLocation(shader, "pos");
	GLuint a_tex_idx = glGetAttribLocation(shader, "tex_idx");

	GLuint u_pv = glGetUniformLocation(shader, "pv");
	GLuint u_tex = glGetUniformLocation(shader, "tex");

	glViewport(0, 0, screen_width, screen_height);

	f32 current_time = (f32)SDL_GetTicks() / 60.0;
	f32 t = 0.0;

	glm::vec3 cam_pos = glm::vec3(-map_width / 2.0f, -map_height / 2.0f, 0.0);
	f32 cam_speed = 1.0f;
	f32 scale = 0.75f;

	bool running = true;
	while (running) {
		SDL_Event event;

		f32 new_time = (f32)SDL_GetTicks() / 60.0;
		f32 dt = new_time - current_time;
		current_time = new_time;
		t += dt;

		SDL_PumpEvents();
        const u8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_UP]) {
			cam_pos.y += cam_speed * dt;
		}
		if (state[SDL_SCANCODE_DOWN]) {
			cam_pos.y -= cam_speed * dt;
		}
		if (state[SDL_SCANCODE_LEFT]) {
			cam_pos.x -= cam_speed * dt;
		}
		if (state[SDL_SCANCODE_RIGHT]) {
			cam_pos.x += cam_speed * dt;
		}

		if (state[SDL_SCANCODE_W]) {
			entity_map->arr[0].pos.y += cam_speed * dt;
		}
		if (state[SDL_SCANCODE_S]) {
 			entity_map->arr[0].pos.y -= cam_speed * dt;
		}
		if (state[SDL_SCANCODE_A]) {
 			entity_map->arr[0].pos.x -= cam_speed * dt;
		}
		if (state[SDL_SCANCODE_D]) {
 			entity_map->arr[0].pos.x += cam_speed * dt;
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

		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shader);

		glEnableVertexAttribArray(a_points);
		glEnableVertexAttribArray(a_pos);
		glEnableVertexAttribArray(a_tex_points);
		glEnableVertexAttribArray(a_tex_idx);

		f32 s_ratio = (f32)screen_width / (f32)screen_height;
		glm::mat4 projection = glm::ortho(-10.0f * s_ratio, 10.f * s_ratio, -10.0f, 10.0f, -2.0f, 2.0f);
		glm::mat4 view = glm::mat4(1.0);
		view = glm::scale(view, glm::vec3(scale, scale, scale));
		view = glm::translate(view, cam_pos);
		glm::mat4 pv = projection * view;

		glBindBuffer(GL_ARRAY_BUFFER, v_points);
		glVertexAttribPointer(a_points, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, v_tex_points);
		glVertexAttribIPointer(a_tex_points, 1, GL_UNSIGNED_BYTE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, v_entity);
   		glVertexAttribPointer(a_pos, 3, GL_FLOAT, GL_FALSE, sizeof(Entity), (void *)STRUCT_OFFSET(Entity, pos));
   		glVertexAttribIPointer(a_tex_idx, 1, GL_UNSIGNED_BYTE, sizeof(Entity), (void *)STRUCT_OFFSET(Entity, sprite_id));
		glVertexAttribDivisor(a_pos, 1);
		glVertexAttribDivisor(a_tex_idx, 1);

		glBindTexture(GL_TEXTURE_2D, atlas_tex);
		glActiveTexture(GL_TEXTURE0);

		glUniform1i(u_tex, 0);
		glUniformMatrix4fv(u_pv, 1, GL_FALSE, &pv[0][0]);

		glBindBuffer(GL_ARRAY_BUFFER, v_entity);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Entity) * map_size, tile_map, GL_STATIC_DRAW);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, sizeof(points), map_size);

		glBindBuffer(GL_ARRAY_BUFFER, v_entity);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Entity) * entity_map->len, entity_map->arr, GL_STATIC_DRAW);
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, sizeof(points), entity_map->len);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_Quit();
	return 0;
}
