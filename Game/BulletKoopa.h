#include <ctime>
#include <map>
#include "Timer.h"
#include "Entity.h"
#include "Player.h"
#define BULLET_BBOX_WIDTH					20
#define BULLET_BBOX_HEIGHT					20

class BulletKoopa : public Entity
{
public:
	Player* mario;
	int alpha = 0;
	bool isDone;
	bool isDeath = false;
	bool isBack = false;
	DWORD countAttack = 0;
	BulletKoopa(float x, float y, Player* mario);
	void Render();
	void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	void Attack(float x, float y);
};

