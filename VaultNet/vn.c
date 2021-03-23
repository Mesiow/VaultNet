#include "vn.h"

int32_t vnNetworkInit()
{
    if (enet_initialize() != 0)
        return -1;

    atexit(enet_deinitialize);
    return 0;
}

vnNetworkServer* vnNetworkHost(uint16_t port, uint32_t max_clients)
{
    vnNetworkServer* server = (vnNetworkServer*)malloc(sizeof(vnNetworkServer));
    server->address.host = ENET_HOST_ANY;
    server->address.port = port;
    server->connections = max_clients;
    server->host = enet_host_create(
        &server->address, max_clients,
        2, //2 Channels
        0,
        0
    );

    if (server->host == NULL) {
        printf("Error occured creating a vnNetwork Host\n");
        return NULL;
    }
    return server;
}

void vnNetworkHostDestroy(vnNetworkServer* host)
{
    assert(host != NULL);
    if (host) {
        free(host);
    }
}

vnNetworkPeer* vnNetworkConnect(const char* addressToConnect, uint16_t port)
{
    vnNetworkPeer* peer = (vnNetworkPeer*)malloc(sizeof(vnNetworkPeer));
    peer->host = enet_host_create(
        NULL,
        1, //Single peer
        2, //2 Channels
        0,
        0
    );

    if (!peer->host) {
        printf("Error occured creating a client\n");
        return NULL;
    }

    /*
          The host must be specified in network byte-order,
          and the port must be in host byte-order
   */
    ENetAddress addr;
    enet_address_set_host(&addr, addressToConnect);
    addr.port = port;

    //Connect client to the specified address and port
    peer->peer = enet_host_connect(peer->host, &addr, 2, 0);
    if (peer->peer == NULL) {
        printf("No available peers for creating a connection(client)\n");
        return NULL;
    }

    ENetEvent ev;
    if (enet_host_service(peer->host, &ev, 3000) > 0 &&
        ev.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to %s:%d has succeeded", addressToConnect, port);
        enet_host_flush(peer->host);
    }
    else {
        printf("Connection attempt timed out\n");
        //Either the 3 seconds are up or a disconnect event was received.
        //Reset the peer in the event the 3 seconds had run out
        enet_peer_reset(peer->peer);
        return NULL;
    }

    return peer;
}

void vnNetworkPeerDestroy(vnNetworkPeer* peer)
{
    assert(peer != NULL);
    if (peer) {
        free(peer);
    }
}

void vnNetworkPollServer(vnNetworkServer *server)
{
    ENetEvent ev;
    while (enet_host_service(server->host, &ev, 0) > 0) {
        switch (ev.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                 printf("New client connected on port %d", ev.peer->address.port);
            }break;
        }
    }
}

void vnNetworkPollPeer(vnNetworkPeer* peer)
{
    ENetEvent ev;
    while (enet_host_service(peer->host, &ev, 0) > 0) {

    }
}

void vnNetworkBroadcast(vnNetworkServer* host, const char* data, uint32_t size)
{
    assert(host != NULL);
    if (host) {
        ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(host->host, 0, packet);
    }
}

void vnNetworkSendPacket(const char* data, uint32_t size)
{

}

void vnNetworkSendPacketTo(const vnNetworkPeer* peer, const char* data, uint32_t size)
{

}

void vnNetworkDisconnect()
{

}

void vnNetworkCleanup()
{

}
