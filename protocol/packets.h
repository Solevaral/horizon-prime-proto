#pragma once
#include <cstdint>
#include <cstring>
#include "message_types.h"
#include "constants.h"

namespace hp {

#pragma pack(push, 1)

struct PacketHeader {
    MsgType  type;
    uint16_t body_len;
};

// ── Client -> Server ────────────────────────────────────────────────────────

// First packet the client sends after connecting. The server verifies
// `version` against PROTOCOL_VERSION and rejects mismatches before auth.
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

struct PktClientInput {
    PacketHeader header;
    char text[MESSAGE_MAX_LEN];  // raw command line from player
};

// ── Server -> Client ────────────────────────────────────────────────────────

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

struct PktPlayerEvent {
    PacketHeader header;
    uint32_t player_id;
    char     nickname[NICKNAME_MAX_LEN];
    int32_t  sector_x;
    int32_t  sector_y;
    int32_t  sector_z;
};

struct PlayerInfo {
    uint32_t player_id;
    char     nickname[NICKNAME_MAX_LEN];
    int32_t  sector_x;
    int32_t  sector_y;
    int32_t  sector_z;
};

struct PktWorldState {
    PacketHeader header;
    uint16_t     player_count;
    PlayerInfo   players[MAX_PLAYERS];
};

// ── Terminal render packets (Server -> Client) ──────────────────────────────

// S_TERM_CLEAR: body is empty

// S_TERM_TEXT: append one styled line to terminal buffer
// color: 0xRRGGBB packed in 3 bytes (R,G,B)
struct PktTermText {
    PacketHeader header;
    uint8_t      r, g, b;        // text color
    uint8_t      flags;          // 0x01=bold, 0x02=dim
    char         text[480];      // UTF-8 text (null-terminated)
};

// S_TERM_PROMPT: change the prompt string shown on input line
struct PktTermPrompt {
    PacketHeader header;
    char         text[64];
};

// S_TERM_SCENE: trigger a built-in fullscreen scene
// scene_id: 0=starfield, 1=warp, 2=planet, 255=off (return to terminal)
struct PktTermScene {
    PacketHeader header;
    uint8_t      scene_id;
    uint8_t      duration_sec;   // 0 = indefinite, N = auto-dismiss after N seconds
    char         params[128];    // scene-specific params (JSON-like string)
};

// S_TERM_ANIM: replace the last `line_count` lines with new content
// Used for progress bars, loading animations, live-updating displays
struct PktTermAnim {
    PacketHeader header;
    uint8_t      line_count;     // how many lines to replace (from bottom of buffer)
    uint8_t      r, g, b;
    char         text[480];
};

#pragma pack(pop)

} // namespace hp
