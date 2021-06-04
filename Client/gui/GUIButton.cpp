#include "GUIElement.h"

GUIButton::GUIButton(int x, int y,
	int size,
	std::string text,
	int w, int h,
	void (*callback)())
	: GUIElement(x, y, w, h), 
		label(x, y, size, true, text),
		callback(callback) {

}

void GUIButton::on_render() {

	// w as size, and h as centered (reuse)
	SDL_Rect rect = {x, y, w, h};
	//Engine::fillRect(color, rect);

	label.on_render();
}

void GUIButton::on_event(SDL_Event& e) {
	if (e.type != SDL_MOUSEBUTTONDOWN)
		return;

	int mx = e.button.x;// .motion.x;
	int my = e.button.y;

	//SDL_GetMouseState(&mx, &my);

	if (is_inside(mx, my) && e.button.button == SDL_BUTTON_LEFT)
		callback();
}