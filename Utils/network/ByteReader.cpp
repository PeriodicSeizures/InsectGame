#include "ByteReader.h"

ByteReader::ByteReader(char* buf) : buf(buf), pos(0) {}

char ByteReader::read() {
	return buf[pos++];
	
}

// https://quick-bench.com/q/YWMyCU9Nhkayd7MhKdipjXXR1q0
//surprisingly enough ... funky
uint8_t ByteReader::readUnsignedByte() {
	return static_cast<uint8_t>(read());
}
int16_t ByteReader::readShort() {
	char ch1 = read();
	char ch2 = read();
	return (ch1 << 8) + (ch2 << 0);
}
uint16_t ByteReader::readUnsignedShort() {
	uint8_t ch1 = readUnsignedByte();
	uint8_t ch2 = readUnsignedByte();
	return (ch1 << 8) | (ch2 << 0);
}
int32_t ByteReader::readInt() {
	char ch1 = read();
	char ch2 = read();
	char ch3 = read();
	char ch4 = read();
	return (ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0);
}
uint32_t ByteReader::readUnsignedInt() {
	uint8_t ch1 = readUnsignedByte();
	uint8_t ch2 = readUnsignedByte();
	uint8_t ch3 = readUnsignedByte();
	uint8_t ch4 = readUnsignedByte();
	return (ch1 << 24) | (ch2 << 16) | (ch3 << 8) | (ch4 << 0);
}
int64_t ByteReader::readLong() {
	int64_t g;
	memcpy(&g, buf + pos, sizeof(int64_t));
	pos += sizeof(int64_t);
	return g;
}
uint64_t ByteReader::readUnsignedLong() {
	uint64_t g;
	memcpy(&g, buf + pos, sizeof(uint64_t));
	pos += sizeof(uint64_t);
	return g;
}
float ByteReader::readFloat() {
	float f;
	memcpy(&f, buf + pos, sizeof(float));
	pos += sizeof(float);
	return f;
}
double ByteReader::readDouble() {
	double f;
	memcpy(&f, buf + pos, sizeof(double));
	pos += sizeof(double);
	return f;
}
void ByteReader::readAsciiString(char* ret) {
	// iterate until a \0 character is found (0)
	while (buf[pos] != '\0') {
		ret++[0] = buf[pos++];
	}

	// finalize the null char
	ret[0] = '0';
	pos++;
}
void ByteReader::readString(std::string& ret) {
	//ret.resize(64);
	//strcpy(&ret., )
	if (buf[pos] != '\0') {
		ret = std::string(buf + pos);
	}
	pos += ret.length() + 1;
}