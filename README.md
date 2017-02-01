# csce438-hw1
A chat room server and client written for csce 438.

# Requirements
- linux or mac (windows is not supported)
- g++ version supporting c++11

# Compiling
```
$ make
```

# Running the server
To run the server
```
$ ./crsd
```
The server will run in the background and will tell you the port it is running on: e.g. `127.0.0.1:1026`.

# Running the client
To start a client, you must specify the ip address and port of the server.
```
$ ./crc 127.0.0.1 1026
```

Once the client is started, you can execute several commands to create, delete, and join chat rooms.

# Client commands
Create a chatroom:
```
CREATE <name>
```
Delete a chatroom:
```
DELETE <name>
```
Join a chatroom:
```
JOIN <name>
```
