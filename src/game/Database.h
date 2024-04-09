//
// Created by bison on 12-09-23.
//

#ifndef PLATFORMER_DATABASE_H
#define PLATFORMER_DATABASE_H

#include "SparseVector.h"
#include "glm/glm.hpp"
#include "defs.h"

struct Tile {
    u32 textureId;

};

struct {
    SparseVector<glm::vec3> positionTable;
} Database;



#endif //PLATFORMER_DATABASE_H
