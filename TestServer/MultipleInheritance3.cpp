#include <iostream>

//template<typename TextureType>
//class Sprite {
//	TextureType texture;
//
//
//};

/*
* base class
*/
struct IEntity {
	//virtual ~IEntity() = 0;
	virtual void on_render() = 0;
};

/*
* children of IEntity
*/
struct IEntityInsect : public IEntity {
	//~IEntityPlayer() {}
	virtual void on_render() {} // declare the existence of the function but be
								// defined in the other projects where needed?
};

struct EntityPlayerAnt : public IEntityInsect {
	virtual void on_render() {}
};

struct EntityPlayerSpider : public IEntityInsect {
	virtual void on_render() {}
};

int main() {

	IEntity* entity;

	// entity could be any kind of derived IEntity
	entity = new IEntityInsect();



}