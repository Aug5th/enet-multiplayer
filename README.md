# enet-multiplayer


## Build Steps:
 
1. Run `genprj.bat`
2. Run server project at `prj/enet_server/enet_server.sln`
2. Run client project at `prj/enet_client/enet_client.sln`
3. Set `enet_client` and `enet_server` as start up project ( finding the way to automatice config this *** )

## Base on this repo code, try to implement "Guess Number" game:
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
