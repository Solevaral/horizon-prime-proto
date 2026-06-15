#pragma once
#include <cstdint>
#include <cstring>
#include "message_types.h"
#include "constants.h"

namespace hp {

#pragma pack(push, 1)

struct PacketHeader {
    MsgType  type;
    uint16_t body_len;   // network byte order on the wire
};

// ── Client -> Server ────────────────────────────────────────────────────────

// First packet after connecting. Server verifies `version` against
// PROTOCOL_VERSION and rejects mismatches before auth.
struct PktHello {
    PacketHeader header;
    uint32_t     version;   // network byte order
};

struct PktLogin {
    PacketHeader header;
    char nickname[NICKNAME_MAX_LEN];
    char password[PASSWORD_MAX_LEN];
};

struct PktRegister {
    PacketHeader header;
    char nickname[NICKNAME_MAX_LEN];
    char password[PASSWORD_MAX_LEN];
};

struct PktClientChat {
    PacketHeader header;
    char text[MESSAGE_MAX_LEN];
};

struct PktClientCommand {
    PacketHeader header;
    char text[MESSAGE_MAX_LEN];   // raw ship-terminal command line
};

// Intent: walk to a tile. Server validates bounds, paths there one tile/tick.
struct PktMoveTo {
    PacketHeader header;
    int32_t      tile_x;    // network byte order
    int32_t      tile_y;
};

// Intent: interact with an object (e.g. board the ship).
struct PktInteract {
    PacketHeader header;
    uint8_t      object_id; // OBJECT_SHIP, ...
    uint8_t      action;    // 0 = default action
};

// ── Server -> Client ────────────────────────────────────────────────────────

// Body layout matches the v2 client parser: player_id(4), sector_x(4),
// sector_y(4), sector_z(4), access(4), nickname(32). All ints network order.
struct PktAuthOk {
    PacketHeader header;
    uint32_t player_id;
    int32_t  sector_x;
    int32_t  sector_y;
    int32_t  sector_z;
    int32_t  access;             // 0=admin, 1=mod, 2=helper, 3=user
    char     nickname[NICKNAME_MAX_LEN];
};

struct PktAuthFail {
    PacketHeader header;
    char reason[128];
};

struct PktServerChat {
    PacketHeader header;
    char sender[NICKNAME_MAX_LEN];
    char text[MESSAGE_MAX_LEN];
};

// S_SECTOR_LOAD: describes the sector the player just entered.
struct PktSectorLoad {
    PacketHeader header;
    int32_t  sector_x, sector_y, sector_z;  // network order
    int32_t  location_id;                   // LOC_STATION / LOC_HANGAR / LOC_SHIP
    int32_t  tiles_x, tiles_y;              // grid dimensions
    int32_t  ship_tile_x, ship_tile_y;      // (unused legacy field; kept for layout)
    char     star_class;                    // procedural star spectral class
    char     star_name[48];                 // procedural star name
};

// One entity in the sector. `tile_x/y` is the authoritative tile; the client
// interpolates toward it. `riding` = sitting in a ship.
struct EntityInfo {
    uint32_t player_id;      // network order
    int32_t  tile_x;         // network order
    int32_t  tile_y;
    uint8_t  riding;         // 0 = on foot, 1 = in ship
    uint8_t  access;
    char     nickname[NICKNAME_MAX_LEN];
};

// S_ENTITY_STATE: full snapshot of everyone in this player's sector.
struct PktEntityState {
    PacketHeader header;
    uint16_t     count;            // network order
    EntityInfo   entities[MAX_PLAYERS];
};

// S_RIDE_STATE: this client boarded/left a ship. The client switches camera to
// the cockpit panel when riding == 1.
struct PktRideState {
    PacketHeader header;
    uint8_t      riding;           // 0 = on foot, 1 = in cockpit
};

// S_TERM_TEXT: one styled line for the on-board ship terminal.
struct PktTermText {
    PacketHeader header;
    uint8_t      r, g, b;
    char         text[480];
};

// ── Interaction menus (lift floors, hangar terminal) ─────────────────────────
constexpr int MENU_MAX_ITEMS = 6;
constexpr int MENU_LABEL_LEN = 48;

struct MenuItem {
    uint8_t action;                 // opaque id passed back in C_MENU_PICK
    uint8_t enabled;                // 0 = greyed out ("You are here")
    char    label[MENU_LABEL_LEN];
};

// S_OPEN_MENU: tell the client to show a menu. `menu_id` identifies which object
// opened it so C_MENU_PICK can be routed back to the right handler.
struct PktOpenMenu {
    PacketHeader header;
    uint8_t      menu_id;           // OBJ_LIFT / OBJ_TERMINAL / ...
    uint8_t      count;
    char         title[MENU_LABEL_LEN];
    MenuItem     items[MENU_MAX_ITEMS];
};

// C_MENU_PICK: the player chose an item.
struct PktMenuPick {
    PacketHeader header;
    uint8_t      menu_id;
    uint8_t      action;
};

#pragma pack(pop)

} // namespace hp
