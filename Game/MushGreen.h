#pragma once
#include "Entity.h"

#define MUSHROOM_WALKING_SPEED 0.04f;

#define MUSHROOM_BBOX_WIDTH		16
#define MUSHROOM_BBOX_HEIGHT	16


#define MUSHROOM_STATE_WALKING	100

#define MUSHROOM_ANI_WALKING		0

class MushGreen : public Entity
{
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();

public:
	DWORD timeDelay;
	vector <LPGAMEENTITY> listEffect;
	int alpha;
	float oldY;
	bool isDone;
	bool isCollis;
	bool isOnTop = false;
	bool isStart = false;
	bool isMove = false;
	MushGreen(float posX, float posY);
	virtual void SetState(int state);
};

