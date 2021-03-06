#include "vn.h"

vnNetworkData* latestReceivedData = NULL;

vnNetworkPeer* peer = NULL;
vnNetworkServer* server = NULL;

int32_t vnNetworkInit()
{
    if (enet_initialize() != 0)
        return -1;

    atexit(enet_deinitialize);
    return 0;
}

int32_t vnNetworkHost(uint16_t port, uint32_t max_clients)
{
    server = (vnNetworkServer*)malloc(sizeof(vnNetworkServer));
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
        return -1;
    }
    return 0;
}


int32_t vnNetworkConnect(const char* addressToConnect, uint16_t port)
{
    peer = (vnNetworkPeer*)malloc(sizeof(vnNetworkPeer));
    peer->host = enet_host_create(
        NULL,
        1, //Single peer
        2, //2 Channels
        0,
        0
    );

    if (!peer->host) {
        printf("Error occured creating a client\n");
        return -1;
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
        return -1;
    }

    ENetEvent ev;
    if (enet_host_service(peer->host, &ev, 3000) > 0 &&
        ev.type == ENET_EVENT_TYPE_CONNECT) {
        printf("Connection to %s:%d has succeeded\n", addressToConnect, port);
        enet_host_flush(peer->host);

        return 0;
    }
    else {
        printf("Connection attempt timed out\n");
        //Either the 3 seconds are up or a disconnect event was received.
        //Reset the peer in the event the 3 seconds had run out
        enet_peer_reset(peer->peer);
        return -1;
    }
}

vnNetworkData* vnNetworkEventLatest()
{
    if (!latestReceivedData) return NULL;

    return latestReceivedData;
}

void vnNetworkPollEvents()
{
    ENetEvent ev;
    ENetHost* host = NULL;
    if (server) host = server->host;
    if (peer) host = peer->host;

    assert(!((server == NULL) && (peer == NULL)));

    while (enet_host_service(host, &ev, 0) > 0) {
        switch (ev.type) {
        case ENET_EVENT_TYPE_CONNECT: {
            printf("New client connected on port %d\n", ev.peer->address.port);
        }break;

        case ENET_EVENT_TYPE_DISCONNECT: {
            printf("%d disconnected\n", ev.peer->address.host);
            ev.peer->data = NULL;
        }break;

        case ENET_EVENT_TYPE_RECEIVE: {
            printf("A packet of length %u containing %s was received from %s on channel %u.\n",
                ev.packet->dataLength,
                ev.packet->data,
                ev.peer->data,
                ev.channelID);
       
            latestReceivedData = (vnNetworkData*)malloc(sizeof(vnNetworkData));

            //latestReceivedData->sender.peer = ev.peer;
            latestReceivedData->sender = (vnNetworkPeer*)malloc(sizeof(vnNetworkPeer));
            latestReceivedData->sender->peer = ev.peer;
            latestReceivedData->data = (uint8_t*)malloc(ev.packet->dataLength * sizeof(uint8_t));
            latestReceivedData->length = ev.packet->dataLength;
            
            //Copy data received
            for (int i = 0; i < ev.packet->dataLength; i++) {
                latestReceivedData->data[i] = ev.packet->data[i];
            }
            enet_packet_destroy(ev.packet);

        }break;
        }
    }
}

void vnNetworkBroadcast(const char* data, uint32_t size)
{
    assert(server != NULL);
    if (server) {
        ENetPacket *packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(server->host, 0, packet);
    }
}

void vnNetworkSendPacket(const char* data, uint32_t size)
{
    ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer->peer, 0, packet);
}

void vnNetworkSendPacketTo(const vnNetworkPeer* peer, const char* data, uint32_t size)
{
    ENetPacket* packet = enet_packet_create(data, size, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer->peer, 0, packet);
}

int32_t vnNetworkDisconnect()
{
    if (!peer) return -1;

    ENetEvent ev;
    enet_peer_disconnect(peer->peer, 0);

    while (enet_host_service(peer->host, &ev, 3000) > 0) {
        switch (ev.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(ev.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Successfully disconnected\n");
                return 0;
                break;
        }
    }
    enet_peer_reset(peer->peer);
    return -1;
}

void vnNetworkCleanup()
{
    vnNetworkDisconnect();
    if (server) {
        enet_host_destroy(server->host);
        free(server);
    }
    if (peer) {
        enet_host_destroy(peer->host);
        free(peer);
    }
}

void vnNetworkFreeLatestData()
{
    if (latestReceivedData) {
        if (latestReceivedData->data != NULL) {
            free(latestReceivedData->data);
            latestReceivedData->data = NULL;
        }
        if (latestReceivedData->sender != NULL) {
            free(latestReceivedData->sender);
            latestReceivedData->sender = NULL;
        }

        free(latestReceivedData);
        latestReceivedData = NULL;
    }
}
