#include <iostream>
#include "tcp_connection.h"

//static bool verify_certificate(bool preverified,
//	asio::ssl::verify_context& ctx)
//{
//	// The verify callback can be used to check whether the certificate that is
//	// being presented is valid for the peer. For example, RFC 2818 describes
//	// the steps involved in doing this for HTTPS. Consult the OpenSSL
//	// documentation for more details. Note that the callback is called once
//	// for each certificate in the certificate chain, starting from the root
//	// certificate authority.
//
//	// In this example we will simply print the certificate's subject name.
//	char subject_name[256];
//	X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
//	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
//	std::cout << "Verifying " << subject_name << "\n";
//
//	return preverified;
//}

TCPConnection::TCPConnection(ssl_socket socket, AsyncQueue<OwnedPacket>* in_packets_s)
	: _socket(std::move(socket)), 
	in_packets_s(in_packets_s), 
	in_packets_c(nullptr),
	SIDE(Side::SERVER)
{

	_socket.set_verify_mode(asio::ssl::verify_none);
}

// https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio/example/cpp11/ssl/client.cpp
TCPConnection::TCPConnection(ssl_socket socket, AsyncQueue<Packet>* in_packets_c)
	: _socket(std::move(socket)), 
	in_packets_s(nullptr),
	in_packets_c(in_packets_c), 
	SIDE(Side::CLIENT)
{
	/*
	* clientside certificate init
	*/

	_socket.set_verify_mode(asio::ssl::verify_none);

	//_socket.set_verify_mode(asio::ssl::verify_peer);
	//_socket.set_verify_callback(
	//	std::bind(&verify_certificate, std::placeholders::_1, std::placeholders::_2));

}

TCPConnection::~TCPConnection() {
	std::cout << "deconstructor()\n";
}

ssl_socket& TCPConnection::socket() {
	return _socket;
}

bool TCPConnection::is_open() {
	// return the status
	return _socket.lowest_layer().is_open();
}

void TCPConnection::close() {
	_socket.lowest_layer().close();
	//std::error_code e;
	//_socket.lowest_layer().cancel();
	//_socket.async_shutdown([this]() { _socket.lowest_layer().close(); });
	//const char buf[] = "";
	//asio::async_write(_socket, asio::buffer(buf),
	//	[this]() {_socket.lowest_layer().close(); });
}

//UUID TCPConnection::getUUID() {
//	return uuid;
//}

void TCPConnection::handshake() {

	// perform ssl handshake
	auto self(shared_from_this());
	_socket.async_handshake(SIDE==Side::SERVER ? asio::ssl::stream_base::server :
		asio::ssl::stream_base::client,
		[this, self](const std::error_code& e)
	{
		if (!e) {
			read_header();
		}
		else {
			std::cout << "ssl handshake error: " << e.message() << "\n";
		}
	});
}

void TCPConnection::read_header() {
	std::cout << "read_header()\n";

	auto self(shared_from_this());
	asio::async_read(_socket,
		asio::buffer(&(this->temp.type), Packet::SIZE),
		[this, self](const std::error_code &e, size_t) {
		if (!e) {
			std::cout << "incoming header\n";
			read_body();
		}
		else {
			std::cout << "read header error: " << e.message() << "\n";
		}
	}
	);
}

void TCPConnection::read_body() {
	/*
	* If packet has data to be read, then read
	*/
	
	uint16_t len = 0;
	Packet::ErrorCode p_e = Packet::S(temp.type, len);

	if (p_e != Packet::ErrorCode::OK) {
		std::cout << "invalid packet\n";
	} else if (len > 0) {
		//temp.data = new char[len];
		temp.data.resize(len);

		std::cout << "r1, " << len << "\n";

		auto self(shared_from_this());
		asio::async_read(_socket,
			asio::buffer(temp.data), // len
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				if (SIDE == Side::SERVER)
					this->in_packets_s->push_back({ self, temp });
				else
					this->in_packets_c->push_back(temp);

				std::cout << "incoming body\n"; // << (uint16_t)in_packet_type << "\n";
				read_header();
			}
			else {
				std::cout << "read body error: " << e.message() << "\n";
			}
		}
		);
	}
	else {
		std::cout << "r1\n";
		read_header();
	}
}

void TCPConnection::write_header() {

	std::cout << "write_header()\n";

	auto self(shared_from_this());
	asio::async_write(_socket,
		asio::buffer(&(out_packets.front().type), Packet::SIZE),

		[this, self](const std::error_code& e, size_t) {
		if (!e) {
			write_body();
		}
		else {
			std::cout << "write header error: " << e.message() << "\n";
		}
	}
	);
}

void TCPConnection::write_body() {
	uint16_t len = Packet::sizes[(uint16_t)out_packets.front().type];
	
	if (len > 0) {
		std::cout << "w1, " << len << "\n";

		auto self(shared_from_this());
		asio::async_write(_socket,
			asio::buffer(out_packets.front().data), // len
			[this, self](const std::error_code& e, size_t) {
			if (!e) {
				out_packets.pop_front();
				std::cout << "sent something with a body\n";

				if (!out_packets.empty())
					write_header();
			}
			else {
				std::cout << "write body error: " << e.message() << "\n";
			}
		}
		);
	}
	else {
		std::cout << "w1\n";
		out_packets.pop_front();
		write_header();
	}
}