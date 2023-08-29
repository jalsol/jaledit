#pragma once

#include "common/vector.h"
#include "cursor.h"
#include "rope/rope.h"

#include <stdbool.h>

typedef struct Snapshot {
    Rope *rope;
    Cursor cursor;
    bool dirty;
} Snapshot;

DECLARE_VEC(Snapshot)