#pragma once
#include "Entity.h"

#define TEXT_BBOX_WIDTH  110
#define TEXT_BBOX_HEIGHT 8


class Text : public Entity
{
public:
	float frameW, frameH;
	float oldY;
	bool isColliCbrick;
	virtual void SetState(int state);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
	Text(float posX, float posY, float frameW, float frameH);
};


