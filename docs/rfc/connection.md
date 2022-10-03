# Connection

See Messaging for more information about the messages format and the channels.

## Initial Connection

> Please refer to Message Types in messaging.md for more information about the messages format.

First the client sends a `CONN_INIT` signal marker through the `main` channel.

The server replies a conn init reply or `CONN_FAILED`.

If the server replies `CONN_FAILED`, the connection is closed and the client should either retry or abort the current connection attempt.

Otherwise, the client then sends a feed init request through the `feed` channel, with its player ID provided as data.

The server replies a feed init reply or `CONN_FAILED`.

If the server replies `CONN_FAILED`, the connection is closed and the client should either retry or abort the current connection attempt.

The client should then send a final auth request through the `main` channel, with the token provided by the feed init reply.

The server replies either `CONN_OK` or `CONN_FAILED`.


> Please refer to Message Types in messaging.md for more information about the messages format.

```
main                      feed
Client        Server      Client        Server
|                |        |                |
|---CONN_INIT--->|        |                |
|<-------rep-----|        |                |  conn init rep returns a player ID and a token A
|                |        |                |
|                |        |---feed init--->|  feed init req provides a player ID and a token A
|                |        |<-----------rep-|  feed init rep provides a token B
|                |        |                |
|--final auth--->|        |                |  final auth uses the token B
|<----CONN_OK----|        |                |
|                |        |                |
```


```
main                      feed
Client        Server      Client        Server
|                |        |                |
|---CONN_INIT--->|        |                |  conn init rep returns a player ID and a token A
|<-------rep-----|        |                |
|                |        |                |
|                |        |---feed init--->|  feed init req provides player ID
|                |        |                |
|                |        |    timeout     |  the server does not reply, the client should retry
|                |        |                |
|                |        |---feed init--->|  feed init req provides player ID
|                |        |<-----------rep-|  feed init rep provides a token
...             ...       ...             ...
```

## Reconnection (main channel)

> Please refer to Message Types in messaging.md for more information about the messages format.

If the connection is lost, the client should try to reconnect.

The client sends a connection request through the `main` channel.

The server replies `CONN_OK` or `CONN_FAILED`.

If the server replies `CONN_FAILED`, the connection is closed and the connection of the feed channel is closed as well.

If the server does not reply, the client should retry, and in case of failure, the connection of the feed channel is closed as well.

## Reconnection (feed channel)

> Please refer to Message Types in messaging.md for more information about the messages format.

If the connection is lost, the client can disconnect both the `main` and the `feed` channels.