#include <iostream>

/*
* base class
*/
struct IEntity {
	//virtual ~IEntity() = 0;
};

/*
* children of IEntity
*/
struct IEntityPlayer : virtual public IEntity {
	//~IEntityPlayer() {}
};

struct IEntityClientSide : virtual public IEntity {
	//~IEntityClientSide() {}

	virtual void render() {
		std::cout << "IEntityClientSide::render()\n";
	}
};

// just for reference that entities will have behaviours on serverside
struct IEntityServerSide : virtual public IEntity {

};

// player but on the client side
class EntityClientSidePlayer : public IEntityPlayer, public IEntityClientSide {
	virtual void render() override {
		std::cout << "EntityClientSidePlayer::render()\n";
	}
};

// child of ^
class EntityPlayerController : public EntityClientSidePlayer {
	void render() override {
		std::cout << "EntityPlayerController::render()\n";
	}
};

int main() {

	IEntity *entity;

	// entity could be any kind of derived IEntity
	entity = new EntityClientSidePlayer();

	// knowing that entity must be of IEntityClientSide, calling
	// render should be possible
	auto&& client_entity = dynamic_cast<IEntityClientSide*>(entity);
	client_entity->render();

}