#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <string>
#include <assert.h>
#include <vector>
#include <typeinfo>
#include "../Common.h"

/* * * * * * * * * * * * * * * * * * * * * * * *

					PACKETS

 * * * * * * * * * * * * * * * * * * * * * * * */



struct Packet {

	/*
	* Packet systems:
	*  - template structure based
	*      simple future development
	*      no variable length, no special behaviour
	*  - inheritance byte based, read and write to and from
	*      repetitive and confusing code for read/write
	*      variable length, and allow for special behaviour
	*/

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
		SRC_SERVER_PLAYER_NEW,
		//SRC_SERVER_ENTITY_NEW, // explicitly declare which entity to create
		SRC_SERVER_ENTITY_DELETE,
		//SRC_CLIENT_REQUEST_TEAM,
		SRC_SERVER_PLAYER_IDENTITY,
		count // kind of hacky
	};

	/*
	* 
	* Packet declarations
	* 
	*/

	struct ConnectionRefused {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_CONNECTION_REFUSED;
		char message[32];
	};

	struct ConnectionVersion {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_CONNECTION_VERSION;
		char message[8]; // 1.80.02\0

	};

	struct ConnectionLogin {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_CLIENT_CONNECTION_LOGIN;
		char username[16];
		char password[16];
	};

	struct Chat32 {
		static constexpr Packet::Type TYPE = Packet::Type::CHAT32;
		char message[32];
		UUID uuid;
	};

	struct Chat64 {
		static constexpr Packet::Type TYPE = Packet::Type::CHAT64;
		char message[64];
		UUID uuid; // to or from
	};

	struct Transform { //  (cheaper for server)
		static constexpr Packet::Type TYPE = Packet::Type::TRANSFORM;
		UUID uuid; // for server
		float x, y;
		float vx, vy;
		float ax, ay;
		float angle;
	};
	/*
	* metadata should be somehow organized and have variable length
	* but, how?
	* stl structures with templates should be good enough to
	* get size and data, depends
	* packet structs arent variable sized because of simplicity of
	* needing only 1 struct and no complicated, error prone across client and server
	* 
	* a rewritten manual packet system would allow for full control over packet lwngth,
	* and for special exceptions, such as var lwngth metadata depending on a type.
	* how to organize it?
	* 
	* packets that are 
	*/
	// tell the client of a new player (uuid, name)
	//struct EntityNew {
	//	static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_ENTITY_NEW;
	//	UUID uuid;
	//	Entity::Type type;
	//	char meta[128];
	//	//char name[16] = "";
	//};

	struct PlayerNew {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_PLAYER_NEW;
		UUID uuid;
		char name[16] = "";
	};

	struct EntityDelete {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_ENTITY_DELETE;
		UUID uuid;
	};

	// tell the client their identity
	struct PlayerIdentity {
		static constexpr Packet::Type TYPE = Packet::Type::SRC_SERVER_PLAYER_IDENTITY;
		UUID uuid;
		//char name[16] = "";
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

	/*
	* Turn data structure into packet
	*/
	template<class T>
	static Packet serialize(T structure) {
		
		//static_assert()
		// might want to include some static asserts to make
		// compiling issues easier to debug and fix
		// since errors like "'TYPE' is not a member of Packet" is annoying
		// and takes time to backtrack where it's used

		static_assert(!std::is_same<T, Packet>::value, "must not be a Packet type");

		Packet packet = { structure.TYPE };
		if (sizeof(T)) {
			packet.data.resize(sizeof(T));
			std::memcpy(packet.data.data(), (void*)&structure, sizeof(T));
		}
		return std::move(packet);
	}

	/*
	* Turn Packet into data structure
	*/
	template<typename T>
	static T* deserialize(Packet& packet) {
		//return static_cast<T*>((void*)in.data);
		return static_cast<T*>((void*)packet.data.data());
	}



	static constexpr size_t sizes[] = {
		sizeof(ConnectionRefused),
		sizeof(ConnectionVersion),
		sizeof(ConnectionLogin),
		sizeof(Chat32),
		sizeof(Chat64),
		sizeof(Transform),
		//sizeof(UnTrustedMotion),
		sizeof(PlayerNew),
		sizeof(EntityDelete),
		//sizeof(RequestTeam),
		sizeof(PlayerIdentity)
	};

	/*
	* Member variables
	*/
	Type type;
	//char* data;
	std::vector<char> data;

};

#endif