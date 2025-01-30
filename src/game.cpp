#include "game.h"
#include "constants.h"

Game::Game(int window_width, int window_height, const char *title) {
	m_window_width = window_width;
	m_window_height = window_height;
	SDL_Init(SDL_INIT_VIDEO);
	m_window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		m_window_width, m_window_height,
		0
	);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

	m_game_is_running = true;
	m_last_frame_time = 0;
	m_delta_time = 0.0f;
	
	m_left_player_score = 0;
	m_right_player_score = 0;

	m_font = nullptr;
}

void Game::setup() {
	if (TTF_Init() == -1) {
		SDL_Log("Failed to initialize SDL_ttf: %s", TTF_GetError());
		m_game_is_running = false;
	}

	GameObject left_paddle(50, (m_window_height / 2) - 50, 13, 105, false);
	GameObject right_paddle(m_window_width - 60, (m_window_height / 2) - 50, 13, 105, false);
	GameObject ball((m_window_width / 2) - 10, (m_window_height / 2) - 10, 18, 18, false);
	GameObject bottom_bar(0, 590, m_window_width, 15, true);
	GameObject top_bar(0, 0, m_window_width, 15, true);
	GameObject middle_bar((m_window_width / 2), 0, 6, m_window_height, true);

	m_game_objects.push_back(left_paddle);
	m_game_objects.push_back(right_paddle);
	m_game_objects.push_back(ball);
	m_game_objects.push_back(bottom_bar);
	m_game_objects.push_back(top_bar);
	m_game_objects.push_back(middle_bar);

	m_game_objects[2].set_dx(5.0f);
	m_game_objects[2].set_dy(5.0f);

	m_font = TTF_OpenFont("assets/pong-score.ttf", 52);
	if (!m_font) {
		SDL_Log("Failed to load font: %s", TTF_GetError());
		m_game_is_running = false;
	}
}

void Game::clean_up() {
	if (m_font) {
		TTF_CloseFont(m_font);
	}
	TTF_Quit();

	if (m_renderer) {
		SDL_DestroyRenderer(m_renderer);
	}
	if (m_window) {
		SDL_DestroyWindow(m_window);
	}
	SDL_Quit();
}

void Game::run() {
	setup();
	while (m_game_is_running) {
		process_input();
		update();
		render();
	}
	clean_up();
}

void Game::process_input() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_QUIT: {
				m_game_is_running = false;
			} break;
			case SDL_KEYDOWN: {
				if (e.key.keysym.sym == SDLK_ESCAPE) {
					m_game_is_running = false;
				}
			} break;
		}
	}
	
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	m_game_objects[0].set_dy(0);  // Left Paddle
	m_game_objects[1].set_dy(0);  // Right Paddle

	// Left Paddle (W/S)
	if (keystate[SDL_SCANCODE_W]) {
		m_game_objects[0].set_dy(-8.0f);
	}
	if (keystate[SDL_SCANCODE_S]) {
		m_game_objects[0].set_dy(8.0f);
	}

	// Right Paddle (Arrow Up/Down)
	if (keystate[SDL_SCANCODE_UP]) {
		m_game_objects[1].set_dy(-8.0f);
	}
	if (keystate[SDL_SCANCODE_DOWN]) {
		m_game_objects[1].set_dy(8.0f);
	}
}

void Game::update() {
	// Frame capping
	int time_to_wait = FRAME_TARGET_DURATION - (SDL_GetTicks() - m_last_frame_time);
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_DURATION) {
		SDL_Delay(time_to_wait);
	}

	m_delta_time = (SDL_GetTicks() - m_last_frame_time) / 1000.0f;

	GameObject &ball = m_game_objects[2];
	GameObject &left_paddle = m_game_objects[0];
	GameObject &right_paddle = m_game_objects[1];

	if (ball.y() <= 15 || ball.y() + ball.h() >= 590) {
		ball.set_dy(-ball.dy());
	}

	if (check_aabb_collision(ball, left_paddle)) {
		ball.set_dx(-ball.dx());
	}

	if (check_aabb_collision(ball, right_paddle)) {
		ball.set_dx(-ball.dx());
	}

	if (ball.x() < 0) {
		m_right_player_score++;
		reset_ball();
	}
	if (ball.x() > 800) {
		m_left_player_score++;
		reset_ball();
	}

	for (auto &obj : m_game_objects) {
		if (!obj.render_only())
			obj.update(m_delta_time);
	}

	m_last_frame_time = SDL_GetTicks();
}

void Game::render() {
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);

	for (auto &obj : m_game_objects) {
		obj.render(m_renderer);
	}

	SDL_Color white = { 255, 255, 255, 255 };

	std::string score_text = std::to_string(m_left_player_score) + "                               " + std::to_string(m_right_player_score);
	SDL_Texture *score_texture = render_text(score_text, white);

	if (score_texture) {
		int text_width, text_height;
		SDL_QueryTexture(score_texture, nullptr, nullptr, &text_width, &text_height);

		SDL_Rect score_rect = { (m_window_width - text_width) / 2, 20, text_width, text_height };
		SDL_RenderCopy(m_renderer, score_texture, nullptr, &score_rect);

		SDL_DestroyTexture(score_texture);
	}

	SDL_RenderPresent(m_renderer);
}

SDL_Texture *Game::render_text(const std::string &text, SDL_Color color) {
	SDL_Surface *surface = TTF_RenderText_Solid(m_font, text.c_str(), color);
	if (!surface) {
		SDL_Log("Failed to create text surface: %s", TTF_GetError());
		return nullptr;
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);

	if (!texture) {
		SDL_Log("Failed to create text texture: %s", SDL_GetError());
	}

	return texture;
}

bool Game::check_aabb_collision(const GameObject &a, const GameObject &b) {
	return(
		a.x() < b.x() + b.w() &&
		a.x() + a.w() > b.x() &&
		a.y() < b.y() + b.h() &&
		a.y() + a.h() > b.y()
		);
}

void Game::reset_ball() {
	GameObject &ball = m_game_objects[2];
	ball.set_x(400);
	ball.set_y(300);

	// Randomize direction
	ball.set_dx((rand() % 2 == 0) ? 4.8f : -5.0f);
	ball.set_dy((rand() % 2 == 0) ? 4.8f : -5.0f);
}
