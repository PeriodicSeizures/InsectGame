#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <string>
#include <assert.h>
#include "ByteReader.h"
#include "ByteWriter.h"
#include "../Common.h"

/* * * * * * * * * * * * * * * * * * * * * * * *

					PACKETS

 * * * * * * * * * * * * * * * * * * * * * * * */



struct Packet {

	enum class Type : uint16_t {
		SRC_SERVER_CONNECTION_REFUSED,
		SRC_SERVER_CONNECTION_VERSION,	// version of server
		SRC_CLIENT_CONNECTION_LOGIN,	// identity of client
		CHAT32,		// sent by client to server
		CHAT64,
		CHAT128,
		CHAT256,
		TRANSFORM,
		//SRC_CLIENT_UNTRUSTED_MOTION,
		SRC_SERVER_ENTITY_NEW,
		//SRC_CLIENT_REQUEST_TEAM,
		SRC_SERVER_CLIENT_IDENTITY,
		count // kind of hacky
	};

	/*
	* 
	* Packet declarations
	* 
	*/

	struct ConnectionRefused {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_CONNECTION_REFUSED;
		char message[16];
	};

	struct ConnectionVersion {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_CONNECTION_VERSION;
		char message[16];
	};

	struct ConnectionLogin {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_CLIENT_CONNECTION_LOGIN;
		char username[16];
		char password[16];
	};

	struct Chat32 {
		static constexpr Packet::Type TYPE = Packet::Type::CHAT32;
		char message[32];
		//char target[16] = ""; // optional 
		UUID target;
	};

	struct Chat64 {
		static constexpr Packet::Type TYPE = Packet::Type::CHAT64;
		char message[64];
		//char target[16] = ""; // optional 
		UUID target;
	};

	struct Chat128 {
		static constexpr Packet::Type TYPE = Packet::Type::CHAT128;
		char message[128];
		//char target[16] = ""; // optional 
		UUID target;
	};

	struct Chat256 {
		static constexpr Packet::Type TYPE = Packet::Type::CHAT256;
		char message[256];
		//char target[16] = ""; // optional 
		UUID target;
	};

	// trust the client motion
	struct Transform { //  (cheaper for server)
		static constexpr Packet::Type TYPE = Packet::Type::TRANSFORM;
		UUID target; // optional due to client
		float x, y;
		float vx, vy;
		float ax, ay;
		float angle;
	};

	// fuck the clients motion
	//struct UnTrustedMotion { // (expensive for server)
	//	static constexpr Packet::Type TYPE = Packet::Type::SRC_CLIENT_UNTRUSTED_MOTION;
	//	Input input;
	//};

	// declare the existence of a particular entity
	struct EntityNew {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_ENTITY_NEW;
		UUID uuid;
		Entity::Type type;
		char name[16] = "";
	};

	//struct RequestTeam {
	//	static constexpr Packet::Type TYPE = Packet::Type::SRC_CLIENT_REQUEST_TEAM;
	//	Entity::Type type;
	//};

	// sent by the server to inform the client on who they should be in the game
	struct ClientIdentity {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_CLIENT_IDENTITY;
		UUID uuid;
		Entity::Type type;
	};

	/*
	* 
	* Utility methods and types
	* 
	*/

	enum class ErrorCode : uint16_t {
		OK = 0,
		INVALID_HEADER
	};

	// Size of header
	static constexpr unsigned int SIZE = sizeof(Type);

	/*
	* Return the sizeof(...) of a packet 
	* that is correlated to @type
	*/
	static ErrorCode S(Type type, uint16_t &ret);

	template<typename T>
	static T* deserialize(Packet& in) {
		return static_cast<T*>((void*)in.data);
	}



	static constexpr size_t sizes[] = {
		sizeof(ConnectionRefused),
		sizeof(ConnectionVersion),
		sizeof(ConnectionLogin),
		sizeof(Chat32),
		sizeof(Chat64),
		sizeof(Chat128),
		sizeof(Chat256),
		sizeof(Transform),
		//sizeof(UnTrustedMotion),
		sizeof(EntityNew),
		//sizeof(RequestTeam),
		sizeof(ClientIdentity)
	};

	/*
	* Member variables
	*/
	Type type;
	char* data;

};

//struct OwnedPacket {
//	UUID uuid;
//	Packet packet;
//};

#endif