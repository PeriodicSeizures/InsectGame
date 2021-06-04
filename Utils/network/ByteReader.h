#ifndef BYTE_READER_H
#define BYTE_READER_H

#include <stdint.h>
#include <string>

class ByteReader
{
private:
	char* buf;
	unsigned int pos;

public:
	ByteReader(char* buf);

public:
	char read();
	//int8_t readByte();
	uint8_t readUnsignedByte();
	int16_t readShort();
	uint16_t readUnsignedShort();
	int32_t readInt();
	uint32_t readUnsignedInt();
	int64_t readLong();
	uint64_t readUnsignedLong();
	float readFloat();
	double readDouble();
	void readAsciiString(char *ret);
	void readString(std::string &ret);

	template<typename T>
	T readEnumUnsignedShort() {
		return static_cast<T>(readUnsignedShort());
	};
};

#endif