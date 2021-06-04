#include "Task.h"

/*
* static defines
*/

Task* Task::current_task = nullptr;
Task* Task::prev_task = nullptr;
asio::io_context Task::_io_context;
std::shared_ptr<TCPConnection> Task::connection;

std::thread Task::run_thread;
std::condition_variable Task::cv_run;
std::mutex Task::mux_run;
bool Task::running = true;
bool Task::do_stops = false;

//std::vector<Engine::Sprite*> Task::sprite_archive;
//std::unordered_map<std::string, Engine::Sprite**> Task::sprites_to_load;

//bool Task::DEBUG;
bool Task::GAME_ALIVE = true;



void Task::init() {
	connection = std::make_shared<TCPConnection>(Task::_io_context);
	
	Task::run_thread = std::thread([]() {
		while (Task::running) {
			std::unique_lock<std::mutex> u1(Task::mux_run);
			Task::cv_run.wait(u1);

			Task::_io_context.run();
			Task::_io_context.restart(); // this will only run if the io_context is stopped
		}
	});

	Engine::Sprite::queueLoad("resources/ant.json", &EntityPlayerAnt::sprite);
	Engine::Sprite::queueLoad("resources/spider.json", &EntityPlayerSpider::sprite);
	Engine::Sprite::queueLoad("resources/brick.json", &WorldTask::brick_sprite);

	// load all sprites into sprite_archive
	//for (auto&& pair : Task::sprites_to_load) {
	//	Engine::Sprite *sprite = new Engine::Sprite(pair.first);
	//	pair.second = &sprite;
	//}

	//sprites_to_load.clear();
}

void Task::uninit() {
	// connection is a smart ptr, no need to free it
	Task::running = false;
	Task::cv_run.notify_one();
	if (Task::do_stops) {
		Task::_io_context.stop();
	}
	Task::run_thread.join();
}

void Task::focus() {
	prev_task = current_task;
	current_task = this;
}