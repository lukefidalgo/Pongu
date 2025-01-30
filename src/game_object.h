#pragma once

#include <SDL2/SDL.h>

class GameObject {
public:
	GameObject(int x_pos, int y_pos, int width, int height, bool render_only);
	~GameObject() = default;

	float dx() const { return m_dx; }
	float dy() const { return m_dy; }
	int x() const { return m_rect.x; }
	int y() const { return m_rect.y; }
	int w() const { return m_rect.w; }
	int h() const { return m_rect.h; }

	void set_dx(float value) { m_dx = value; }
	void set_dy(float value) { m_dy = value; }
	void set_x(int value) { m_rect.x = value; }
	void set_y(int value) { m_rect.y = value; }

	bool render_only() const { return m_render_only; }

	void render(SDL_Renderer *renderer);
	void update(float delta_time);

private:
	SDL_Rect m_rect;
	float m_dx;
	float m_dy;
	bool m_render_only;
};
