#include "GUIElement.h"

GUIElement::GUIElement(int x, int y, int w, int h)
	: x(x), y(y), w(w), h(h) {}

void GUIElement::on_render() {

}

void GUIElement::on_event(SDL_Event& e) {

}

bool GUIElement::is_inside(int x, int y) {
	if (x < this->x || y < this->y ||
		x > this->x + w || y > this->y + h)
		return false;
	return true;
}