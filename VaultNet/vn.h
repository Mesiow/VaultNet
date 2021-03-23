#pragma once

#ifndef VN_H

#include <enet/enet.h>
#include <cstdint>
#include <iostream>

struct vnNetworkServer {
	ENetHost* host;
	ENetAddress address;
	uint32_t connections;
};

struct vnNetworkPeer {
	ENetHost* host;
	ENetPeer* peer;
	uint32_t id;
};

struct vnNetworkData {
	vnNetworkPeer sender;
	uint8_t* data;
	uint32_t length;
};

int32_t vnNetworkInit();
int32_t vnNetworkHost(vnNetworkServer &host, uint16_t port, uint32_t max_clients);
int32_t vnNetworkConnect(vnNetworkPeer &peer, const char* addressToConnect, uint16_t port);

void vnNetworkBroadcast(const char* data, uint32_t size);
void vnNetworkSendPacket(const char* data, uint32_t size);
void vnNetworkSendPacketTo(const vnNetworkPeer& peer, const char* data, uint32_t size);
void vnNetworkDisconnect();
void vnNetworkCleanup();

#endif // !VN_H