# IPC using Message Passing

This folder contains two programs - `msg_reciever.c` and `msg_send.c`  
The reciever waits for the sender to send messages to message queue. The sender sends message to queue.

## Known Issues

The `msg_reciever` program crashes when END is sent to it.
