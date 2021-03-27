#pragma once

#ifndef VN_H

#include <enet/enet.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NETWORK_EVENTS 10

	typedef enum eNetworkEvent {
		NetworkEventConnected,
		NetworkEventDisconnected
	}eNetworkEvent;

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
		vnNetworkPeer* sender;
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
	int32_t vnNetworkHost(uint16_t port, uint32_t max_clients);


	/*
		Connect a client to a remote host
	*/
	int32_t vnNetworkConnect(const char* addressToConnect, uint16_t port);


	/*
		Gets latest received network data
	*/
	vnNetworkData* vnNetworkEventLatest();

	/*
		Poll all network events
	*/
	void vnNetworkPollEvents();

	/*
		Broadcast a packet to all peers
	*/
	void vnNetworkBroadcast(const char* data, uint32_t size);

	/*
		Send a packet to the server
	*/
	void vnNetworkSendPacket(const char* data, uint32_t size);

	/*
		Send a packet as the server to a connected peer
	*/
	void vnNetworkSendPacketTo(const vnNetworkPeer* peer, const char* data, uint32_t size);

	int32_t vnNetworkDisconnect();
	void vnNetworkCleanup();

	/*
		Must be called after done reading a newly received packet
	*/
	void vnNetworkFreeLatestData();

#ifdef __cplusplus
}
#endif

#endif // !VN_H