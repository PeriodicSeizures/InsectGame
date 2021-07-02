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
		PING,
		PONG,
		CHAT64,
		S2C_CLIENT_MOTION,
		S2C_ENTITY_MOTION,
		C2S_CLIENT_INPUT,
		S2C_PLAYER_NEW,
		S2C_ENTITY_DELETE,
		S2C_CLIENT_IDENTITY,

		count
	};

	/*******************************************************************\
	*																	
	* 	Packet declarations																
	*																	
	* 	structure naming etiquette:											
	* 		- [S2C] / [C2S]: prefix for src->dest or nothing to imply bidirectional 
	* 		- [name]: general name of packet (name should explain briefly but understandably)
	*																	
	\*******************************************************************/

	struct Ping {
		static constexpr Packet::Type TYPE = Packet::Type::PING;
	};

	struct Pong {
		static constexpr Packet::Type TYPE = Packet::Type::PONG;
	};

	struct Chat64 {
		static constexpr Packet::Type TYPE = Packet::Type::CHAT64;
		char message[64] = "\0";
		UUID uuid;
	};

	struct S2CClientMotion {
		static constexpr Packet::Type TYPE = Packet::Type::S2C_CLIENT_MOTION;
		uint32_t last_processed_input;
		float x, y;
		float vx, vy;
		float ax, ay;
		float angle;
	};

	struct S2CEntityMotion {
		static constexpr Packet::Type TYPE = Packet::Type::S2C_ENTITY_MOTION;
		UUID uuid;
		float x, y;
		float vx, vy;
		float ax, ay;
		float angle;
	};

	struct C2SClientInput {
		static constexpr Packet::Type TYPE = Packet::Type::C2S_CLIENT_INPUT;
		uint32_t last_processed_input;
		uint16_t input_mask;
	};

	struct S2CPlayerNew {
		static constexpr Packet::Type TYPE = Packet::Type::S2C_PLAYER_NEW;
		UUID uuid;
		char name[16] = "\0";
	};

	struct S2CEntityDelete {
		static constexpr Packet::Type TYPE = Packet::Type::S2C_ENTITY_DELETE;
		UUID uuid;
	};

	struct S2CClientIdentity {
		static constexpr Packet::Type TYPE = Packet::Type::S2C_CLIENT_IDENTITY;
		UUID uuid;
	};

	/*******************************************************************\
	*																	*
	* 																	*
	*					Utility methods and types						*
	* 																	*
	*																	*
	\*******************************************************************/

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
		sizeof(Ping),
		sizeof(Pong),
		sizeof(Chat64),
		sizeof(S2CClientMotion),
		sizeof(S2CEntityMotion),
		sizeof(C2SClientInput),
		sizeof(S2CPlayerNew),
		sizeof(S2CEntityDelete),
		sizeof(S2CClientIdentity)
	};

	/*
	* Member variables
	*/
	Type type;
	//char* data;
	std::vector<char> data;

};

#endif