#pragma once

#ifndef VN_H

#include <enet/enet.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

typedef struct vnNetworkServer {
	ENetHost* host;
	ENetAddress address;
	uint32_t connections;
}vnNetworkServer;

typedef struct vnNetworkPeer {
	ENetHost* host;
	ENetPeer* peer;
	uint32_t id;
}vnNetworkPeer;

typedef struct vnNetworkData {
	vnNetworkPeer sender;
	uint8_t* data;
	uint32_t length;
}vnNetworkData;



/*
	Initialize the library
*/
int32_t vnNetworkInit();

/*
	Create a network server/host at a specific port and allow max_clients
	number of connections
*/
vnNetworkServer* vnNetworkHost(uint16_t port, uint32_t max_clients);

/*
	Free the memory from creating a server/host
*/
void vnNetworkHostDestroy(vnNetworkServer* host);

/*
	Connect a client to a remote host
*/
vnNetworkPeer* vnNetworkConnect(const char* addressToConnect, uint16_t port);

/*
	Free the memory from creating a client/peer
*/
void vnNetworkPeerDestroy(vnNetworkPeer* peer);


void vnNetworkPollServer(vnNetworkServer *host);
void vnNetworkPollPeer(vnNetworkPeer* peer);

void vnNetworkBroadcast(vnNetworkServer *host, const char* data, uint32_t size);
void vnNetworkSendPacket(const char* data, uint32_t size);
void vnNetworkSendPacketTo(const vnNetworkPeer* peer, const char* data, uint32_t size);
void vnNetworkDisconnect();
void vnNetworkCleanup();

#endif // !VN_H