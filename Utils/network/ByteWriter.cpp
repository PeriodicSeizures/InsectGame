#include "ByteWriter.h"
#include <vcruntime_string.h>
#include <algorithm>

ByteWriter::ByteWriter(char* buf) : buf(buf), pos(0) {}

unsigned int ByteWriter::getOffset() {
	return pos;
}

void ByteWriter::write(char c) { 
	buf[pos++] = c;
}
void ByteWriter::writeUnsignedByte(uint8_t c) { 
	write(static_cast<char>(c));
}
void ByteWriter::writeShort(int16_t c) {
	write(c >> 8);
	write(c >> 0);
}
void ByteWriter::writeUnsignedShort(uint16_t c) {
	writeUnsignedByte(c >> 8);
	writeUnsignedByte(c >> 0);
}
void ByteWriter::writeInt(int32_t c) {
	write(c >> 24);
	write(c >> 16);
	write(c >> 8);
	write(c >> 0);
}
void ByteWriter::writeUnsignedInt(uint32_t c) {
	writeUnsignedByte(c >> 24);
	writeUnsignedByte(c >> 16);
	writeUnsignedByte(c >> 8);
	writeUnsignedByte(c >> 0);
}
void ByteWriter::writeLong(int64_t c) {
	memcpy(buf + pos, &c, sizeof(int64_t));
	pos += sizeof(int64_t);
}
void ByteWriter::writeUnsignedLong(uint64_t c) {
	memcpy(buf + pos, &c, sizeof(uint64_t));
	pos += sizeof(uint64_t);
}
void ByteWriter::writeFloat(float c) {
	memcpy(buf + pos, &c, sizeof(float));
	pos += sizeof(float);
}	
void ByteWriter::writeDouble(double c) {
	memcpy(buf + pos, &c, sizeof(double));
	pos += sizeof(double);
}
void ByteWriter::writeAsciiString(char* ascii, unsigned int len) {
	memcpy(buf + pos, ascii, static_cast<size_t>(len) + 1);
	pos += len + 1;
}
void ByteWriter::writeString(const std::string& message) {

	if (message.empty()) {
		write('\0');
	}
	else {
		memcpy(buf + pos, message.c_str(), message.length() + 1);
		pos += message.length() + 1;
	}
}