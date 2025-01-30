#pragma once

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "game_object.h"

class Game {
public:
	Game(int window_width, int window_height, const char *title);

	void setup();
	void clean_up();
	void run();
	void process_input();
	void update();
	void render();

	SDL_Texture *render_text(const std::string &text, SDL_Color color);
	bool check_aabb_collision(const GameObject &a, const GameObject &b);
	void reset_ball();

	int left_player_score() const { return m_left_player_score; }
	int right_player_score() const { return m_right_player_score; }
	bool is_running() const { return m_game_is_running; }

	// You should't have more than one instance of this class.
	// But just in case.
	Game(const Game &) = delete;
	Game &operator=(const Game &) = delete;

private:
	SDL_Window *m_window;
	SDL_Renderer *m_renderer;
	TTF_Font *m_font;

	int m_window_width;
	int m_window_height;

	bool m_game_is_running;
	int m_last_frame_time;
	float m_delta_time;

	int m_left_player_score;
	int m_right_player_score;

	std::vector<GameObject> m_game_objects;
};
