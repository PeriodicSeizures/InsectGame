#pragma once

#include <string>
#include <memory>
#include "../Common.h"
#include "../impl/EntityImpl.h"
#include "../network/tcp_connection.h"

namespace Entity {

	enum class Type {
		PLAYER,
		MISC
	};

};

class IEntity : public std::enable_shared_from_this<IEntity>
{private:
	bool force_moved = false;

public:
	// ptr
	typedef std::shared_ptr<IEntity> ptr;

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

	EntityImpl* impl;

public:
	IEntity(UUID uuid, EntityImpl* impl);
	virtual ~IEntity();

public:
	virtual void on_physics();

	virtual void on_tick() = 0;

	//void send_transform(TCPConnection::ptr conn); // send location
	//virtual void send_new(TCPConnection::ptr conn) = 0; // send existence
	//virtual void send_data(TCPConnection::ptr conn) = 0;

	void set_transform(float x, float y,
		float vx, float vy,
		float ax, float ay,
		float angle);

	/*
	* Return packet containing certain data
	*/

	// transform packet
	Packet packet_transform();

	// new entity packet
	virtual Packet packet_new() = 0;

	// remove entity packet
	Packet packet_delete();
};

struct EntityPlayer : public IEntity {
public:
	typedef std::shared_ptr<EntityPlayer> ptr;

private:
	std::string name;

public:
	EntityPlayer(UUID uuid, std::string name, EntityImpl* impl);

public:
	void on_tick() override;

	Packet packet_new() override;


};