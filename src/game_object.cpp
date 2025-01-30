#include "game_object.h"

GameObject::GameObject(int x_pos, int y_pos, int width, int height, bool render_only) {
	m_rect.x = x_pos;
	m_rect.y = y_pos;
	m_rect.w = width;
	m_rect.h = height;

	m_dx = 0;
	m_dy = 0;

	m_render_only = render_only;
}

void GameObject::render(SDL_Renderer *renderer) {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &m_rect);
}

void GameObject::update(float delta_time) {
	m_rect.x += m_dx;
	m_rect.y += m_dy;

	// Prevent paddles from going off the screen
	if (m_rect.y < 15) m_rect.y = 15;
	if (m_rect.y + m_rect.h > 590) m_rect.y = 590 - m_rect.h;
}
