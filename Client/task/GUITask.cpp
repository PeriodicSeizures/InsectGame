#include "Task.h"

/*
* definitions
*/
GUITextInput *GUITask::prompted;
bool GUITask::any_focused;

GUITask::GUITask() {

}

void GUITask::on_render() {
	for (const auto& element : elements) {
		element->on_render();
	}
}

void GUITask::on_update(float delta) {

}

void GUITask::on_tick() {

}

void GUITask::on_event(SDL_Event& e) {
	// run all events on guis
	any_focused = false;
	for (const auto& element : elements) {
		element->on_event(e);
	}
}