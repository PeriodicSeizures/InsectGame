#pragma once
#include <asio.hpp>
#include <memory>
#include <deque>
#include <unordered_map>
//#include ""

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE

using namespace asio::ip;

class TCPConnection : public std::enable_shared_from_this<TCPConnection>
{
public:
	typedef std::shared_ptr<TCPConnection> pointer;

	static pointer create(asio::io_context& io_context);

	tcp::socket& socket();

	void start();

private:
	TCPConnection(asio::io_context& io_context);

	void handle_write(const asio::error_code& /*error*/);
	void handle_read_header(const asio::error_code& /*error*/);
	void handle_read_body(const asio::error_code& /*error*/);

	//void send(const Packet& packet);

	tcp::socket socket_;
	std::string message_;
	std::string recv_message_;

	std::deque<std::string> message_queue;

	//const unsigned int id;

	// zip ~ 62%

	static constexpr uint32_t HEADER_SIZE = 2; // fixed
	uint16_t PACKET_SIZE; // based on header data

	// packet about 300-400 bytes (512 safe)
	// 512 bytes is on the larger side for an ordinary tcp tx/rx packet
	char *read_buf; // delete data on dc

};

