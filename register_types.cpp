#include "register_types.h"

#include "core/class_db.h"
#include "gol.h"

void register_gol_types() {
	ClassDB::register_class<GameOfLife>();
}

void unregister_gol_types() {
}
