#ifndef MAP_DATA_H
#define MAP_DATA_H

// Map dimensions based on the doc.txt
#define MAP_WIDTH 42
#define MAP_HEIGHT 22

// ASCII Map Data
// C: Forest/Boundary
// A: Forest/Boundary
// *: White Flowers
// ☆: Red Flowers
// ◯: Gazebo
// ◼︎: Tiles (Represented as '#')
// ◻︎: Asphalt (Represented as '=')
// W: Grass
// B: Bridge
// ●: Water (Represented as '~')
// D: Forest/Boundary

// We will use single characters for simplicity in C
// # : Tile
// = : Asphalt
// ~ : Water
// W : Grass
// B : Bridge
// G : Gazebo (◯)
// R : Red Flower (☆)
// w : White Flower (★)
// T : Tree/Boundary (C, A, D, E)

static const char *map_layout[] = {
    "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT",
    "T   wwwwwwwwwwwwwwR==####==Rwwwwwwwwwwwww   TE",
    "T   wGwwwwwwwwwwGwR==##==RwwGwwwwwwwwGw   TE",
    "T   wwwwwwwwwwwwwwR==##==Rwwwwwwwwwwwww   TE",
    "T   wwwwwwwwwwwwwwR==##==Rwwwwwwwwwwwww   TE",
    "T   wGwwwwwwwwwwGwR==##==RwwGwwwwwwwwGw   T",
    "T   wwwwwwwwwwwwwwR==##==Rwwwwwwwwwwwww   T",
    "T   wwwwwwwwwwwwwwR==##==Rwwwwwwwwwwwww   T",
    "T   RRRRRRRRRRRRRRR=========RRRRRRRRRRRRRR   T",
    "T   WWWWWWWWWWWWWWW==WWWWW==WWWWWWWWWWWWWW   T",
    "T   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   T",
    "T   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   T",
    "T   WWWWWWWWWWWWWWW==WWWWW==WWWWWWWWWWWWWW   T",
    "T   RRRRRRRRRRRRRRR=========RRRRRRRRRRRRRR   T",
    "T   wwwwwwwwwwwwwwR==#####==Rwwwwwwwwwwwww   T",
    "T   wGwwwwwwwwwwGwR==#####==RwwGwwwwwwwwGw   T",
    "T   wwwwwwwwwwwwwwR==#####==Rwwwwwwwwwwwww   T",
    "T   wwwwwwwwwwwwwwR==#####==Rwwwwwwwwwwwww   T",
    "T   wGwwwwwwwwwwGwR==#####==RwwGwwwwwwwwGw   T",
    "T   wwwwwwwwwwwwwwR==#####==Rwwwwwwwwwwwww   T",
    "T   wwwwwwwwwwwwwwR==#####==Rwwwwwwwwwwwww   T",
    "TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT"};

#endif
