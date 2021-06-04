#include "GUIElement.h"

GUILabel::GUILabel(int x, int y, 
	int size, bool centered, 
	std::string text)
	: GUIElement(x, y, 0, 0), 
		size(size), centered(centered), 
		text(text) { }

void GUILabel::on_render() {
	// w as size, and h as centered (reuse)
	Engine::drawFormattedString(text, x, y, 255, size, centered);
}