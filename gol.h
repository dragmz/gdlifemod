#include "scene/3d/multimesh_instance.h"
#include "scene/3d/spatial.h"

struct map_t;

class GameOfLife : public MultiMeshInstance {
	GDCLASS(GameOfLife, MultiMeshInstance);

	map_t *map{};

	int w{};
	int h{};

protected:
	static void _bind_methods();
public:

	GameOfLife();
	~GameOfLife();

	void init();
	void step();
	void input();

	int get_w() const;
	void set_w(int w);

	int get_h() const;
	void set_h(int h);
};
