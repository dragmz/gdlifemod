#include "gol.h"

#include <algorithm>
#include <execution>

struct tile_t {
	struct health_t {
		int current;
		int next;
	} health;

	int life;

	tile_t *other[8];

	int value() const;

	int get_count() {
		return other[0]->value() +
			   other[1]->value() +
			   other[2]->value() +
			   other[3]->value() +
			   other[4]->value() +
			   other[5]->value() +
			   other[6]->value() +
			   other[7]->value();
	}
};

int tile_t::value() const {
	return health.current;
}

struct map_t {
	tile_t defaultTile{};

	uint16_t w;
	uint16_t h;
	tile_t *tiles;

	tile_t &at(const uint16_t x, const uint16_t y) {
		tile_t *t = at_or_null(x, y);
		_ASSERT(t);

		return *t;
	}

	tile_t *at_or_null(const uint16_t x, const uint16_t y) {
		if (x < 0 || y < 0 || x >= w || y >= h)
			return {};
		return &tiles[x + y * w];
	}

	tile_t *at_or_default(const uint16_t x, const uint16_t y) {
		if (tile_t *t = at_or_null(x, y))
			return t;

		return &defaultTile;
	}

	void init(const uint16_t w, const uint16_t h) {
		tiles = (tile_t *)malloc(sizeof(tile_t) * w * h);

		this->w = w;
		this->h = h;

		for (int y = 0; y < h; ++y) {
			for (int x = 0; x < w; ++x) {
				tile_t &t = at(x, y);

				t.health = {};
				t.life = 0;

				t.other[0] = at_or_default(x - 1, y - 1);
				t.other[1] = at_or_default(x, y - 1);
				t.other[2] = at_or_default(x + 1, y - 1);
				t.other[3] = at_or_default(x + 1, y);
				t.other[4] = at_or_default(x + 1, y + 1);
				t.other[5] = at_or_default(x, y + 1);
				t.other[6] = at_or_default(x - 1, y + 1);
				t.other[7] = at_or_default(x - 1, y);
			}
		}
	}
};

void GameOfLife::_bind_methods() {
	ClassDB::bind_method(D_METHOD("init"), &GameOfLife::init);
	ClassDB::bind_method(D_METHOD("step"), &GameOfLife::step);
	ClassDB::bind_method(D_METHOD("input"), &GameOfLife::input);

	ClassDB::bind_method(D_METHOD("set_w", "w"), &GameOfLife::set_w);
	ClassDB::bind_method(D_METHOD("get_w"), &GameOfLife::get_w);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "w", PROPERTY_HINT_NONE), "set_w", "get_w");

	ClassDB::bind_method(D_METHOD("set_h", "h"), &GameOfLife::set_h);
	ClassDB::bind_method(D_METHOD("get_h"), &GameOfLife::get_h);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "h", PROPERTY_HINT_NONE), "set_h", "get_h");
}

GameOfLife::GameOfLife() {
	map = memnew(map_t);
}

GameOfLife::~GameOfLife() {
	memdelete(map);
}

void GameOfLife::init() {
	MultiMesh *mm = *get_multimesh();

	map->init(w, h);

	mm->set_transform_format(MultiMesh::TRANSFORM_3D);
	mm->set_color_format(MultiMesh::COLOR_FLOAT);
	mm->set_instance_count(map->w * map->h);

	int i{};
	for (int y = 0; y < map->h; ++y) {
		for (int x = 0; x < map->w; ++x) {
			mm->set_instance_transform(i, Transform(Basis(), Vector3(x, rand() % 5 * 0.1f, y)));
			++i;
		}
	}
}

void GameOfLife::step() {
	std::for_each(std::execution::par_unseq, map->tiles, map->tiles + (size_t)map->w * (size_t)map->h,
			[](tile_t &t) {
				const int value = t.get_count();
				if (t.health.current)
					t.health.next = value == 2 || value == 3;
				else
					t.health.next = value == 3;
			});

	std::for_each(std::execution::par_unseq, map->tiles, map->tiles + (size_t)map->w * (size_t)map->h,
			[](tile_t &t) {
				t.health.current = t.health.next;

				if (t.health.current)
					t.life = 20;
				else
					t.life = std::max(0, t.life - 1);
			});

	MultiMesh *mm = *get_multimesh();
	tile_t *t = map->tiles;

	for (int i = 0, count = map->w * map->h; i < count; ++i) {
		if (t->health.current)
			mm->set_instance_color(i, Color(1.0, 0, 0));
		else
			mm->set_instance_color(i, Color(0, t->life / 50.0f, 0));

		t++;
	}
}

void GameOfLife::input() {
	map->at(25, 25).health.current = true;
	map->at(24, 25).health.current = true;
	map->at(26, 25).health.current = true;
	map->at(25, 24).health.current = true;
}

int GameOfLife::get_w() const {
	return w;
}

void GameOfLife::set_w(int w) {
	this->w = w;
}

int GameOfLife::get_h() const {
	return h;
}

void GameOfLife::set_h(int h) {
	this->h = h;
}
