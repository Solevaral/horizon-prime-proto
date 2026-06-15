#pragma once
#include <cstdint>
#include <cstdlib>   // std::abs
#include "constants.h"

// ── Multi-location world (OSRS-style "floors") ───────────────────────────────
// The world is split into separate locations, each its own trimmed tile grid.
// A player is in exactly one location at a time; moving between them teleports
// the player and swaps the grid (the client just renders the new layout, with
// the starfield backdrop showing past the walls).
//
//   LOC_STATION : where you spawn. A big hub with a LIFT in the centre.
//   LOC_HANGAR  : reached via the lift. Has a TERMINAL that boards your ship.
//   LOC_SHIP    : your personal ship interior — a walled hull with compartments
//                 and a single airlock. Reached from the hangar terminal.
//
// Both server (movement/pathfinding) and client (rendering) derive everything
// from these helpers, so no map data travels over the wire — only the location
// id and grid size in S_SECTOR_LOAD.
namespace hp {

enum Location : int32_t {
    LOC_STATION = 0,
    LOC_HANGAR  = 1,
    LOC_SHIP    = 2,
};

// Object ids used in C_INTERACT and S_OPEN_MENU.
constexpr uint8_t OBJ_LIFT     = 1;   // station: opens floor menu
constexpr uint8_t OBJ_TERMINAL = 2;   // hangar: opens board menu
constexpr uint8_t OBJ_AIRLOCK  = 3;   // ship: leave the ship (back to hangar)

// ── Grid sizes ───────────────────────────────────────────────────────────────
// Ship is the base unit; station/hangar are larger. (Ship ~ old station * 1.5;
// station ~ ship * 2 in footprint terms.)
constexpr int SHIP_TILES    = 36;
constexpr int STATION_TILES = 48;
constexpr int HANGAR_TILES  = 48;

inline int loc_tiles(int32_t loc) {
    switch (loc) {
        case LOC_SHIP:    return SHIP_TILES;
        case LOC_HANGAR:  return HANGAR_TILES;
        default:          return STATION_TILES;
    }
}

// ── Object tile positions per location ───────────────────────────────────────
inline void lift_tile(int& tx, int& ty) {           // station centre
    tx = STATION_TILES / 2; ty = STATION_TILES / 2;
}
inline void terminal_tile(int& tx, int& ty) {        // hangar
    tx = HANGAR_TILES / 2; ty = HANGAR_TILES / 2 + 6;
}
inline void hangar_ship_tile(int& tx, int& ty) {     // decorative ship in hangar
    tx = HANGAR_TILES / 2; ty = HANGAR_TILES / 2 - 8;
}
inline void ship_airlock_tile(int& tx, int& ty) {    // the one door in the hull
    tx = SHIP_TILES / 2; ty = SHIP_TILES - 4;
}

// Spawn tile for entering / arriving in a location.
inline void loc_spawn(int32_t loc, int& tx, int& ty) {
    switch (loc) {
        case LOC_STATION:  tx = STATION_TILES / 2; ty = STATION_TILES / 2 + 4; break;  // by the lift
        case LOC_HANGAR:   tx = HANGAR_TILES / 2;  ty = HANGAR_TILES / 2 + 9;  break;  // by the terminal
        case LOC_SHIP:     ship_airlock_tile(tx, ty); ty -= 1;                 break;  // just inside the airlock
        default:           tx = ty = 4; break;
    }
}

// ── Wall layouts ─────────────────────────────────────────────────────────────
// Each returns true if the tile is a solid wall in that location.

// A rectangular room with a wall ring and a gap (door) on one side.
inline bool ring_wall(int tx, int ty, int x0, int y0, int x1, int y1,
                      int door_x, int door_y) {
    bool on_edge = (tx == x0 || tx == x1 || ty == y0 || ty == y1);
    if (!on_edge) return false;
    if (tx == door_x && ty == door_y) return false;   // doorway gap
    if (tx < x0 || tx > x1 || ty < y0 || ty > y1) return false;
    return true;
}

// STATION: a big walled hub. Outer ring + four short interior spurs framing the
// central lift, leaving wide walkways. Two doorways on the outer ring.
inline bool station_wall(int tx, int ty) {
    const int x0 = 2, y0 = 2, x1 = STATION_TILES - 3, y1 = STATION_TILES - 3;
    if (tx < x0 || tx > x1 || ty < y0 || ty > y1) return false;
    bool edge = (tx == x0 || tx == x1 || ty == y0 || ty == y1);
    if (edge) {
        int mid = STATION_TILES / 2;
        if ((ty == y0 || ty == y1) && tx == mid) return false;  // N/S doors
        return true;
    }
    // Interior spurs around the central plaza (with the lift in the middle).
    int cx = STATION_TILES / 2, cy = STATION_TILES / 2;
    // four L-shaped stubs at the corners of a central square (radius 8)
    int r = 8;
    bool spur = false;
    if (std::abs(tx - cx) == r && std::abs(ty - cy) <= r - 3) spur = true;
    if (std::abs(ty - cy) == r && std::abs(tx - cx) <= r - 3) spur = true;
    return spur;
}

// HANGAR: a walled bay. Outer ring + a partition splitting the ship pad from the
// terminal area, with a doorway.
inline bool hangar_wall(int tx, int ty) {
    const int x0 = 2, y0 = 2, x1 = HANGAR_TILES - 3, y1 = HANGAR_TILES - 3;
    if (tx < x0 || tx > x1 || ty < y0 || ty > y1) return false;
    bool edge = (tx == x0 || tx == x1 || ty == y0 || ty == y1);
    if (edge) return true;
    // Partition wall across the middle with a central doorway.
    int my = HANGAR_TILES / 2 - 2;
    if (ty == my && tx >= x0 + 2 && tx <= x1 - 2) {
        if (tx == HANGAR_TILES / 2) return false;   // doorway
        return true;
    }
    return false;
}

// SHIP: a built-up hull. Outer ring with ONE airlock gap, plus interior bulkhead
// walls dividing it into compartments, each with a doorway.
inline bool ship_wall(int tx, int ty) {
    const int x0 = 2, y0 = 2, x1 = SHIP_TILES - 3, y1 = SHIP_TILES - 3;
    if (tx < x0 || tx > x1 || ty < y0 || ty > y1) return false;
    int ax, ay; ship_airlock_tile(ax, ay);
    bool edge = (tx == x0 || tx == x1 || ty == y0 || ty == y1);
    if (edge) {
        if (tx == ax && ty == y1) return false;   // single airlock in the aft wall
        return true;
    }
    // Bulkheads: one horizontal (fore/aft split) and one vertical (port/star),
    // each with a doorway, carving 4 compartments.
    int mx = SHIP_TILES / 2, my = SHIP_TILES / 2;
    if (ty == my && tx > x0 && tx < x1) {
        if (tx == mx) return false;                // doorway
        return true;
    }
    if (tx == mx && ty > y0 && ty < my) {          // vertical bulkhead, fore half
        if (ty == (y0 + my) / 2) return false;     // doorway
        return true;
    }
    return false;
}

inline bool tile_is_wall(int32_t loc, int tx, int ty) {
    switch (loc) {
        case LOC_SHIP:    return ship_wall(tx, ty);
        case LOC_HANGAR:  return hangar_wall(tx, ty);
        default:          return station_wall(tx, ty);
    }
}

// Solid object tiles (impassable) per location: the lift pillar, the terminal,
// the decorative hangar ship.
inline bool tile_is_object(int32_t loc, int tx, int ty) {
    int ox, oy;
    if (loc == LOC_STATION) { lift_tile(ox, oy); return tx == ox && ty == oy; }
    if (loc == LOC_HANGAR) {
        terminal_tile(ox, oy);
        if (tx == ox && ty == oy) return true;
        // Decorative ship occupies a 3x5 footprint.
        int sx, sy; hangar_ship_tile(sx, sy);
        if (tx >= sx - 1 && tx <= sx + 1 && ty >= sy - 2 && ty <= sy + 2) return true;
        return false;
    }
    return false;
}

inline bool tile_walkable(int32_t loc, int tx, int ty) {
    int n = loc_tiles(loc);
    if (tx < 0 || ty < 0 || tx >= n || ty >= n) return false;
    if (tile_is_wall(loc, tx, ty))   return false;
    if (tile_is_object(loc, tx, ty)) return false;
    return true;
}

// Which interactable object (if any) sits on this tile, for click hit-testing.
// Returns 0 if none.
inline uint8_t object_at(int32_t loc, int tx, int ty) {
    int ox, oy;
    if (loc == LOC_STATION) { lift_tile(ox, oy);     if (tx == ox && ty == oy) return OBJ_LIFT; }
    if (loc == LOC_HANGAR)  { terminal_tile(ox, oy); if (tx == ox && ty == oy) return OBJ_TERMINAL; }
    if (loc == LOC_SHIP)    { ship_airlock_tile(ox, oy); if (tx == ox && ty == oy) return OBJ_AIRLOCK; }
    return 0;
}

// The tile a player should stand on to use an object (adjacent, walkable).
inline void object_use_tile(int32_t loc, uint8_t obj, int& tx, int& ty) {
    int ox, oy;
    if (obj == OBJ_LIFT)     { lift_tile(ox, oy);        tx = ox; ty = oy + 1; return; }
    if (obj == OBJ_TERMINAL) { terminal_tile(ox, oy);    tx = ox; ty = oy + 1; return; }
    if (obj == OBJ_AIRLOCK)  { ship_airlock_tile(ox, oy);tx = ox; ty = oy - 1; return; }
    tx = ty = 4;
}

} // namespace hp
