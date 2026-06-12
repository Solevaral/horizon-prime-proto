# Horizon Prime Protocol Definitions

Shared wire-protocol definitions for client and server. Used as a git submodule
in both `horizon-prime-client` and `horizon-prime-server`.

## Files

- `constants.h` — ports, limits, spawn sector, and **`PROTOCOL_VERSION`**
- `message_types.h` — `MsgType` enum (client→server and server→client packets)
- `packets.h` — packet structs (`PktHello`, `PktLogin`, `PktAuthOk`, render
  packets, ...)

## Protocol versioning

`PROTOCOL_VERSION` (in `constants.h`) gates compatibility. On connect the client
sends `C_HELLO` with its version; the server requires an exact match and rejects
mismatched/old clients before login. **Bump `PROTOCOL_VERSION` by 1 whenever the
wire format or required client behaviour changes**, then rebuild and republish
client + server.

## Building

Header-only — included directly by the client and server build systems. Keep
client and server built from the same proto commit.
