#ifndef BYTE_WRITER_H
#define BYTE_WRITER_H

#include <stdint.h>
#include <string>

class ByteWriter
{
private:
	char* buf;
	int pos;

public:
	ByteWriter(char* buf);

public:
	unsigned int getOffset();

	void write(char c);
	//int8_t writeByte();
	void writeUnsignedByte(uint8_t c);
	void writeShort(int16_t c);
	void writeUnsignedShort(uint16_t c);
	void writeInt(int32_t c);
	void writeUnsignedInt(uint32_t c);
	void writeLong(int64_t c);
	void writeUnsignedLong(uint64_t c);
	void writeFloat(float c);
	void writeDouble(double c);
	void writeAsciiString(char* ascii, unsigned int len);
	void writeString(const std::string& message);

	template<typename T>
	void writeEnumUnsignedShort(T _enum) {
		writeUnsignedShort(static_cast<unsigned short>(_enum));
	};
};

#endif