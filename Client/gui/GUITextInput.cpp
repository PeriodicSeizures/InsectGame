#include "GUIElement.h"
#include "../task/Task.h"

GUITextInput::GUITextInput(int x, int y,
	int size, int label_size, int max_chars,
	std::string title, void (*callback)(std::string& s))
	: GUIElement(x, y, max_chars * size * 7, size * 11), 
		label(x, y - label_size * 11, label_size, false, title), 
		size(size),	max_chars(max_chars), callback(callback) {}

void GUITextInput::on_render() {
	// Title
	label.on_render();

	// Input below title
	Engine::drawString(text, x, y, color, size, false);
}

void GUITextInput::on_event(SDL_Event &e) {

	

	if (e.type == SDL_MOUSEBUTTONDOWN) {

		int mx = e.button.x;// .motion.x;
		int my = e.button.y;

		//SDL_GetMouseState(&mx, &my);

		if (e.button.button == SDL_BUTTON_LEFT) {
			if (is_inside(mx, my)) {
				GUITask::prompted = this;
				GUITask::any_focused = true;
			}
			else if (!GUITask::any_focused) {
				GUITask::prompted = nullptr;
			}
		}
		return;
	}

	if (GUITask::prompted != this)
		return;
		




	if (e.type == SDL_KEYDOWN) {
		switch (e.key.keysym.sym) {
		case SDLK_BACKSPACE: {
			if (!text.empty())
				text.pop_back();
			break;
		} case SDLK_RETURN: {
			if (SDL_GetModState() & KMOD_SHIFT) { // NEWLINE
				text += '\n';
				break;
			}
			callback(text);
			// disable();
			break;
		} case SDLK_ESCAPE: {
			// this->disable();
			break;
		} case SDLK_c: {
			if (SDL_GetModState() & KMOD_CTRL)
				SDL_SetClipboardText(text.c_str());
			break;
		} case SDLK_v: {
			if (SDL_GetModState() & KMOD_CTRL)
				text += SDL_GetClipboardText();
			break;
		}
		}
	}
	else if (e.type == SDL_TEXTINPUT && 
		text.size() < max_chars)
	{
		//Not copy or pasting
		if (!(SDL_GetModState() & KMOD_CTRL &&
			(e.text.text[0] == 'c' || e.text.text[0] == 'C' || e.text.text[0] == 'v' || e.text.text[0] == 'V')))
		{
			//Append character
			text += e.text.text;
		}
	}
}