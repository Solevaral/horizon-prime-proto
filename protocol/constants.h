#pragma once

namespace hp {

// Bump whenever the wire format changes. The server requires an exact match and
// rejects any other client version at connect time.
constexpr unsigned int   PROTOCOL_VERSION = 4;   // v4: multi-location world (ship/station/hangar)

constexpr unsigned short SERVER_PORT      = 7777;
constexpr int            MAX_PLAYERS      = 64;
constexpr int            MAX_PACKET_SIZE  = 4096;
constexpr int            NICKNAME_MAX_LEN = 32;
constexpr int            PASSWORD_MAX_LEN = 128;
constexpr int            MESSAGE_MAX_LEN  = 256;

// ── World / tile grid ────────────────────────────────────────────────────────
// A sector is a flat field of TILES_X * TILES_Y tiles. Players walk one tile per
// server tick (point-and-click, OSRS-style). All authoritative positions are
// integer tile indices; the client interpolates between tiles for smooth motion.
constexpr int   SECTOR_TILES_X = 24;
constexpr int   SECTOR_TILES_Y = 24;
constexpr int   SERVER_TICK_MS = 500;   // game tick (matches design doc)

// The ship sits at a fixed tile on the station for now. Interacting with it from
// an adjacent tile makes the player "board" it (camera switches to the cockpit).
constexpr int   SHIP_TILE_X = 12;
constexpr int   SHIP_TILE_Y = 12;
constexpr uint8_t OBJECT_SHIP = 1;   // object id used in C_INTERACT

constexpr int SPAWN_SECTOR_X = 0;
constexpr int SPAWN_SECTOR_Y = 0;
constexpr int SPAWN_SECTOR_Z = 0;

// Spawn tile for a freshly-connected player.
constexpr int SPAWN_TILE_X = 4;
constexpr int SPAWN_TILE_Y = 4;

} // namespace hp
