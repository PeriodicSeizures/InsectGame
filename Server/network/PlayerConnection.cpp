#include "PlayerConnection.h"
#include "Server.h"

PlayerConnection::PlayerConnection(
	UUID uuid,
	std::shared_ptr<TCPConnection> connection,
	EntityPlayer* entity)
	: uuid(uuid), connection(connection), entity(entity) {}