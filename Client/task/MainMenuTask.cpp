#include "Task.h"

MainMenuTask MAIN_MENU_TASK;



MainMenuTask::MainMenuTask() {

	GUITextInput* input = new GUITextInput(15, 20, 2, 2, 21, "address",
		[](std::string &s) {
			size_t at = s.find(':');
			std::string ip = s.substr(0, at);
			std::string port = s.substr(at + 1);

			if (Task::do_stops) {
				Task::_io_context.stop();
			}

			try {
				Task::connection->connect_to_server(Task::_io_context, ip, port);
				Task::cv_run.notify_one();
				Task::do_stops = true;

				WORLD_TASK.focus();
			}
			catch (const std::system_error& ec) {
				std::cout << "connect error: " << ec.what() << "\n";
			}

		}
	);

														// 255 to include \0 char
	GUITextInput* chat = new GUITextInput(15, 600-22, 2, 2, 255, "chat",
		[](std::string& s) {

			std::cout << "chat: " << s << "\n";

			Packet packet;

			if (s.length() < 32) {
				Packet::Chat32 chat;
				std::memcpy(chat.message, s.c_str(), s.length() + 1);
				Task::connection->dispatch(std::move(chat));
			}
			else if (s.length() < 64) {
				Packet::Chat64 chat;
				std::memcpy(chat.message, s.c_str(), s.length() + 1);
				Task::connection->dispatch(std::move(chat));
			}
			else if (s.length() < 128) {
				Packet::Chat128 chat;
				std::memcpy(chat.message, s.c_str(), s.length() + 1);
				Task::connection->dispatch(std::move(chat));
			}
			else if (s.length() < 256) {
				Packet::Chat256 chat;
				std::memcpy(chat.message, s.c_str(), s.length() + 1);
				Task::connection->dispatch(std::move(chat));
			}
			s.clear();
		}
	);

	elements.push_back(input);
	elements.push_back(chat);
}

void MainMenuTask::on_render() {
	for (const auto& element : elements) {
		element->on_render();
	}
}

void MainMenuTask::on_update(float delta) {

}

void MainMenuTask::on_event(SDL_Event& e) {
	// run all events on guis
	for (const auto& element : elements) {
		element->on_event(e);
	}
}