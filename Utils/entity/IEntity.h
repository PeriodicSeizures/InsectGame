#pragma once
#include <string>
#include "../Common.h"
#include "../impl/EntityImplement.h"

class IEntity
{
private:
	

public:
	UUID uuid;

	float x = 0, y = 0,
		vx = 0, vy = 0,
		ax = 0, ay = 0,
		angle = 0;

	float x_prev = 0, y_prev = 0,
		ax_prev = 0, ay_prev = 0;

	static constexpr float max_speed = 150.f;
	static constexpr float fric = 12.f;
	static constexpr bool is_kinematic = true;

	EntityImplement* impl;

public:
	IEntity(UUID uuid, EntityImplement* impl);

private:
	// used by both server and client
	void on_physics();

public:
	virtual void on_tick() = 0;

	// have client override this
	//virtual void on_render();

	void set_transform(float x, float y,
		float vx, float vy,
		float ax, float ay,
		float angle);

	//void setImpl(EntityImplement *impl);
};

struct EntityPlayer : public IEntity {
private:
	std::string name;

public:
	EntityPlayer(UUID uuid, std::string name, EntityImplement* impl);

public:
	void on_tick() override;

	// will never override on_render()
	//void on_render() override;

};