# Messaging

## Definitions

- an int16 is defined as an integer number, encoded in **2 bytes**, in **big endian**.
- an int32 is defined as an integer number, encoded in **4 bytes**, in **big endian**.
- an int64 is defined as an integer number, encoded in **8 bytes**, in **big endian**.
- a string, is defined as a sequence of bytes. it should be **ASCII** compliant.
- `CODE` always refer to a **single byte** code.
- `STATUS` always refer to a **single byte** status.
- `SID` is a shared identifier that refers to **the same element for both the server and every client**, it is encoded as an **int16**.

## Channels

Network communication is separated into two main channels: `main` and `feed` channels.

### `main` channel

`main` is in TCP. It is used to transmit data in a safe way and transmit resynchronization messages and important messages.

### `feed` channel

`feed` is in UDP. It is used to transmit real-time state changes faster, with possible loss of precision. Using it in pair with the `main` channel synchronization messages can compensate the loss of precision.

Any message sent on the `feed` channel should be preceded by a sequence number, to ensure that older messages are not received after newer ones.

```
000AAAAAAAABBBBBBBBBBBBBBBBBBBBBBBB
 |   |            |
 | SEQ        CONTENT
MAGIC

MAGIC is a magic number, used to ensure that the message is valid. its value is 0x0fficecoffeedefec.
SEQ is the sequence number, encoded as a long, it is incremented by one for each message sent.
CONTENT is the message content.
```

## Message types

### Signal markers

They can be sent both in `main` and `feed` channels.

Signal markers are single byte messages.

```
AAAA
 |
CODE

CODE is the signal marker message code.
```

### Update message (specifically `UPDATE_MSG`)

The channel used is the `feed` channel.

```
AAAABBBBBBBBCCCCCCCCCCCC
 |      |         |
CODE   SID       DATA

CODE is the message code, it has the value of `UPDATE_MSG` (refer to the relevant code table).
SID is the shared identifier, it should refer to the same element for the server and for every client.
DATA is the serialized data which is relevant for this SID. its size in bytes is unspecified and can depend on the relevant SID.
```

### Synchronization message (specifically `SYNC_MSG`)

The channel used is the `main` channel.

```
AAAABBBBBBBBCCCCCCCCCCCC
 |      |         |
CODE   SID       DATA

CODE is the message code, it has the value of `SYNC_MSG` (refer to the relevant code table).
SID is the shared identifier, it should refer to the same element for the server and for every client.
DATA is the serialized data which is relevant for this SID. its size in bytes is unspecified and can depend on the relevant SID.
```

### Query message (specifically `QUERY_MSG`)

The channel used can be either the `main` channel or the `feed` channel.

```
AAAABBBBBBBBB
 |      |
CODE   DATA

CODE is the message code, it has the value of a message code which is of query type (refer to the relevant code table).
DATA is the serialized data which is relevant for this query. its size in bytes is defined according to the specified CODE.
```

### Reply message

The channel used can be either the `main` channel or the `feed` channel.

A reply message cannot and should not be sent without having a query message received before.

```
AAAABBBBBCCCCCCCCCC
 |    |       |
CODE STATUS  DATA

CODE is the message code, it has the value of a message code which is of reply type (refer to the relevant code table).
STATUS is the status code, it has the value of any status code (refer to the relevant code table).
DATA is the serialized data which is relevant for this reply. its size in bytes is defined according to the specified CODE.
```

### Connection init reply (specifically `CONN_INIT_REP`)

Replies a player ID and a token.

```
AAAABBBBBBBBBBCCCCCCCCCCCCC
 |    |             |
CODE PLAYERID     TOKEN

CODE is the message code, it has the value of `CONN_INIT_REP` (refer to the relevant code table).
PLAYERID is the player ID, it is encoded as an int16.
TOKEN is the token, it is encoded as an int32.
```


### Feed init request (specifically `FEED_INIT`)

Sent by the client to the server to request the `feed` channel init.

```
AAAABBBBBBBBBBCCCCCCCCCCCCC
 |      |         |
CODE PLAYERID   TOKEN

CODE is the message code, it has the value of `FEED_INIT` (refer to the relevant code table).
PLAYERID is the player ID, it is encoded as an int16.
TOKEN is the token, it is encoded as an int32.
```

### Feed init reply (specifically `FEED_INIT_REP`)

Sent by the server to the client to reply the `feed` channel init.

```
AAAACCCCCCCCCCCCC
 |       |
CODE   TOKEN

CODE is the message code, it has the value of `FEED_INIT_REP` (refer to the relevant code table).
TOKEN is the token, it is encoded as an int32.
```