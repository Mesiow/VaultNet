#include "vn.h"

int32_t vnNetworkInit()
{
    if (enet_initialize() != 0)
        return -1;

    atexit(enet_deinitialize);
    return 0;
}

int32_t vnNetworkHost(vnNetworkServer& host, uint16_t port, uint32_t max_clients)
{
    //Bind server
    host.address.host = ENET_HOST_ANY;
    host.address.port = port;

    host.host = enet_host_create(
        &host.address, max_clients,
        2, //2 Channels
        0,
        0
    );

    if (host.host == nullptr) {
        std::cerr << "Error occured creating a vnNetwork Host\n";
        return -1;
    }
    
    return 0;
}

int32_t vnNetworkConnect(vnNetworkPeer& peer, const char* addressToConnect, uint16_t port)
{
    peer.host = enet_host_create(
        nullptr,
        1, //Single peer
        2, //2 Channels
        0,
        0
    );

    if (!peer.host) {
        std::cerr << "Error occured creating a client\n";
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
    peer.peer = enet_host_connect(peer.host, &addr, 2, 0);
    if (peer.peer == nullptr) {
        std::cerr << "No available peers for creating a connection(client)\n";
        return -1;
    }

    ENetEvent ev;
    if (enet_host_service(peer.host, &ev, 3000) > 0 &&
        ev.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "Connection to " << addressToConnect << ":" << port << " succeeded\n";
        enet_host_flush(peer.host);
    }
    else {
        std::cerr << "Connection attempt timed out\n";
        //Either the 3 seconds are up or a disconnect event was received.
        //Reset the peer in the event the 3 seconds had run out
        enet_peer_reset(peer.peer);
        return -1;
    }

    return 0;
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
