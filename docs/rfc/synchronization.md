# Synchronization process

Synchronization messages follow the basic message process. Refer to Messaging for more information.

## Synchronization

Both the client and the server can request resynchronization (see Synchronization context), the process should be implemented bilaterally. Either the client asks the server, or the server asks the client.

The only difference is that the server can refuse the sync request for any reason, in that case, the client is expected to send back a synchronization request (see Synchronization context) as soon as possible.

Outside of a sync context (see Synchronization context), the client is not required to handle sync messages (see Synchronization message).

The channel used is the `main` channel.

### Synchronization message (a.k.a sync message)

A synchronization message can be sent anytime to force the other party to synchronize a specific element.

The server can refuse the sync message for any reason. The client should handle it accordingly, and should request for resync as soon as possible on inconsistency.

Refer to the Messaging section about Synchronization messages.

### Synchronization context (a.k.a. request for resync)

The requester (0) should send a `SYNC_REQ` signal marker, called a sync request.

The replier (1) should send a `SYNC_START` signal marker, called a sync start.

The requester (0) will then receive multiple sync messages from the replier (1).

At the end of synchronization, the replier (1) should send a `SYNC_END` signal marker, called a sync end, or a sync validation.

Once the marker is sent, it means that the requester (0) has done synchronizing. Other messages can still be sent in the meantime.

The requester (0) should acknowledge this marker and send back a `SYNC_OK` signal marker, called a sync confirmation or a sync validation.

> Between the signal markers, every messages are still handled as normally!! Signal marker are just messages containing only a single byte!

```
main
0                 1
|---SYNC_REQ----->|
|<---SYNC_START---|
|                 |
|<--sync message--|
|<--sync message--|
|<--sync message--|
|                 |
|<---SYNC_END-----|
|----SYNC_OK----->|
```

```
main
0                 1
|---SYNC_REQ----->|
|<---SYNC_START---|
|                 |
|<--sync message--|
|<--sync message--|
|                 |
|<--other message-|   in the case an important message is sent during sync,
|---other reply-->|   we should still be able to handle it
|                 |
|<--sync message--|
|                 |
|<---SYNC_END-----|
|----SYNC_OK----->|
```

```
main
0                 1
|---SYNC_REQ----->|
|<---SYNC_START---|
|                 |
|<--sync message--|
|<--sync message--|
|                 |
|---SYNC_REQ----->|   if a SYNC_REQ is already pending (not ended),
|<---REFUSED------|   the request will be refused
|                 |
|<--SYNC_REQ------|   same here, but the other way
|----REFUSED----->|
|                 |
|<--sync message--|
|                 |
|<---SYNC_END-----|
|----SYNC_OK----->|
```