#pragma once
#include <cstdint>

namespace hp {

enum class MsgType : uint8_t {
    // Client -> Server
    C_LOGIN         = 0x01,
    C_REGISTER      = 0x02,
    C_CHAT          = 0x03,
    C_DISCONNECT    = 0x04,
    C_INPUT         = 0x05,  // terminal command input

    // Server -> Client (legacy)
    S_AUTH_OK       = 0x10,
    S_AUTH_FAIL     = 0x11,
    S_REGISTER_OK   = 0x12,
    S_REGISTER_FAIL = 0x13,
    S_CHAT          = 0x14,
    S_PLAYER_JOIN   = 0x15,
    S_PLAYER_LEAVE  = 0x16,
    S_WORLD_STATE   = 0x17,
    S_ERROR         = 0x1F,

    // Server -> Client (terminal render)
    S_TERM_CLEAR    = 0x20,  // clear screen
    S_TERM_TEXT     = 0x21,  // append styled text line
    S_TERM_PROMPT   = 0x22,  // set prompt string
    S_TERM_SCENE    = 0x23,  // fullscreen 3D scene (type + params)
    S_TERM_ANIM     = 0x24,  // animation frame (replaces last N lines)
    S_TERM_SOUND    = 0x25,  // play a sound effect (id byte)
    S_TERM_CHAT     = 0x26,  // chat message (separate from terminal output)
    S_STATS         = 0x27,  // player statistics (for overlay)
    S_LOGOUT        = 0x30,  // force client back to login screen
};

} // namespace hp
