#pragma once
#include <cstdint>

namespace hp {

// v3 — tile-based 3D world ("OSRS in space"). The player is an on-foot
// character that walks a tile grid on a station and can sit in a ship.
enum class MsgType : uint8_t {
    // ── Client -> Server ────────────────────────────────────────────────────
    C_HELLO         = 0x06,  // first packet: announces PROTOCOL_VERSION
    C_LOGIN         = 0x01,
    C_REGISTER      = 0x02,
    C_CHAT          = 0x03,  // chat message (sector channel)
    C_DISCONNECT    = 0x04,
    C_MOVE_TO       = 0x07,  // intent: walk to tile (tx, ty)
    C_INTERACT      = 0x08,  // intent: interact with an object (object_id + action)
    C_COMMAND       = 0x09,  // ship-terminal text command ("::warp ...")
    C_MENU_PICK     = 0x0A,  // chose an item in a server-opened menu

    // ── Server -> Client ────────────────────────────────────────────────────
    S_AUTH_OK       = 0x10,
    S_AUTH_FAIL     = 0x11,
    S_CHAT          = 0x14,
    S_SECTOR_LOAD   = 0x18,  // load the sector: grid size, station, ship spawn
    S_ENTITY_STATE  = 0x19,  // all players currently in this sector (tile positions)
    S_ENTITY_LEAVE  = 0x1A,  // a player left the sector
    S_RIDE_STATE    = 0x1B,  // this player entered/left a ship (camera switch)
    S_OPEN_MENU     = 0x1C,  // open an interaction menu (lift floors / terminal)
    S_TERM_TEXT     = 0x21,  // a line for the on-board ship terminal
    S_ERROR         = 0x1F,
    S_LOGOUT        = 0x30,  // force client back to login screen
};

} // namespace hp
