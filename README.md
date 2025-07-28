# enet-multiplayer


## Build Steps:
 
1. Run `genprj.bat`
2. Run server project at `prj/enet_server/enet_server.sln`
2. Run client project at `prj/enet_client/enet_client.sln`
3. Set `enet_client` and `enet_server` as start up project ( finding the way to automatice config this *** )

---

# Practice

## 1) Base on this repo code, try to implement "Guess Number" game:
### Turn-Based:
- Server picks a random number from 1 - 100.
- Players take turns to guess.
- After each guess, server announces the result of guess "PlayerX's guess is" + "Smaller" / "A Litter Small" / "Correct" / "A Litte Large" / "Larger". (Server say "A Litter Small" if guess's number smaller within 10. Similar to "A Litte Large")
- Repeat until end game or quit.

#### Simple flow example: 
Server: Random number is 52

--

Client A: YOUR_TURN

Client A: GUESS: 14

--

Server (announce to all clients): PlayerA's guess is Smaller

--

Client B: YOUR_TURN

Client B: GUESS: 60

--

Server (announce to all clients): PlayerB's guess is A Litter Large

--

Client A: YOUR_TURN

Client A: GUESS: 52

--

Server (announce to all clients): PlayerA's guess is Correct

End game!

### Real-time:
Rule is as same as with Turn-Based, but no need to wait each other to send a guess.

## 2) Implement room system (You can using Enet).
- Create
- Join
- Leave
- Find/Search

Here an example code setup and implement create room feature:

server.cpp
```C++
#include <enet/enet.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

struct Room {
    std::string name;
    ENetPeer* host; // the client that created the room
};

std::vector<Room> rooms;

void sendMessage(ENetPeer* peer, const std::string& msg) {
    ENetPacket* packet = enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);
}

int main() {
    if (enet_initialize() != 0) {
        std::cerr << "ENet initialization failed.\n";
        return 1;
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    ENetHost* server = enet_host_create(&address, 32, 2, 0, 0); // allow up to 32 clients
    if (!server) {
        std::cerr << "Server creation failed.\n";
        return 1;
    }

    std::cout << "Server is running. Waiting for clients...\n";

    while (true) {
        ENetEvent event;
        while (enet_host_service(server, &event, 1000) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Client connected from "
                          << event.peer->address.host << ":" << event.peer->address.port << "\n";
                break;

            case ENET_EVENT_TYPE_RECEIVE:
            {
                std::string msg(reinterpret_cast<char*>(event.packet->data));
                std::cout << "Received: " << msg << "\n";

                if (msg.rfind("CREATE_ROOM:", 0) == 0) {
                    std::string roomName = msg.substr(12);
                    auto it = std::find_if(rooms.begin(), rooms.end(), [&](const Room& r) {
                        return r.name == roomName;
                    });

                    if (it != rooms.end()) {
                        sendMessage(event.peer, "ERROR: Room already exists");
                    } else {
                        Room room{ roomName, event.peer };
                        rooms.push_back(room);
                        sendMessage(event.peer, "ROOM_CREATED:" + roomName);
                        std::cout << "Room '" << roomName << "' created.\n";
                    }
                }

                enet_packet_destroy(event.packet);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Client disconnected.\n";
                // Clean up rooms hosted by this peer
                rooms.erase(std::remove_if(rooms.begin(), rooms.end(),
                    [&](const Room& r) {
                        return r.host == event.peer;
                    }), rooms.end());
                break;

            default:
                break;
            }
        }
    }

    enet_host_destroy(server);
    enet_deinitialize();
    return 0;
}
```
