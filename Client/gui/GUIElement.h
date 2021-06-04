#ifndef GUIELEMENT_H
#define GUIELEMENT_H

#include <string>
#include <memory>
#include <SDL.h>
#include "../engine/Engine.h"

class GUIElement
{
public:
	typedef std::shared_ptr<GUIElement> pointer;

protected:
	int x, y, w, h;

public:
	GUIElement(int x, int y, int w, int h);

	virtual void on_render();
	virtual void on_event(SDL_Event& e);
	bool is_inside(int x, int y);
};

/*
* just text
*/
class GUILabel final : public GUIElement {
public:
	int size;
	bool centered;
	std::string text;

	// text color
	static constexpr SDL_Color color = {
		127, 127, 127, 255
	};

public:
	GUILabel(int x, int y,
		int size, bool centered, 
		std::string text);

	void on_render() override;
};

class GUIButton final : public GUIElement {
private:
	int size;
	GUILabel label;
	void (*callback)();

	// backer color
	static constexpr SDL_Color color = {
		255, 255, 255, 255
	};

public:
	GUIButton(int x, int y,
		int size,
		std::string text,
		int w, int h,
		void (*callback)());

	void on_render() override;
	void on_event(SDL_Event& e) override;
};

class GUITextInput final : public GUIElement {
public:
	int size;
	int max_chars;
	GUILabel label;
	std::string text;
	void (*callback)(std::string& s);

	// text color
	static constexpr SDL_Color color = {
		127, 127, 127, 255
	};

public:
	GUITextInput(int x, int y,
		int size, int label_size, int max_chars,
		std::string title,
		void (*callback)(std::string &s));

	void on_render() override;
	void on_event(SDL_Event& e) override;
};

#endif