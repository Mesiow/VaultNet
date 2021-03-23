#include "vn.h"

int32_t vnNetworkInit()
{
    return int32_t();
}

int32_t vnNetworkHost(ENetHost* host, uint16_t port, uint32_t max_clients)
{
    return int32_t();
}

int32_t vnNetworkConnect(const char* addressToConnect, uint16_t port)
{
    return int32_t();
}

void vnNetworkBroadcast(const char* data, uint32_t size)
{

}

void vnNetworkSendPacket(const char* data, uint32_t size)
{

}

void vnNetworkSendPacketTo(const vnNetworkPeer& peer, const char* data, uint32_t size)
{

}

void vnNetworkDisconnect()
{

}

void vnNetworkCleanup()
{

}
