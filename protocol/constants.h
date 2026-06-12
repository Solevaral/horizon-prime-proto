#pragma once

namespace hp {

// Bump this whenever the client/server wire format or required client
// behaviour changes. The server requires an exact match and rejects any other
// client version at connect time. Keep client and server built from the same
// proto submodule commit.
constexpr unsigned int   PROTOCOL_VERSION = 2;

constexpr unsigned short SERVER_PORT      = 7777;
constexpr int            MAX_PLAYERS      = 64;
constexpr int            MAX_PACKET_SIZE  = 4096;
constexpr int            NICKNAME_MAX_LEN = 32;
constexpr int            PASSWORD_MAX_LEN = 128;
constexpr int            MESSAGE_MAX_LEN  = 256;

constexpr int SPAWN_SECTOR_X = 0;
constexpr int SPAWN_SECTOR_Y = 0;
constexpr int SPAWN_SECTOR_Z = 0;

} // namespace hp
