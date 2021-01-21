#include "Text.h"
void Text::Render()
{
	animationSet->at(0)->Render(nx, x, y);
	//RenderBoundingBox();
}

void Text::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + frameW;
	b = y + frameH;
}

Text::Text(float posX, float posY, float frameW, float frameH)
{
	x = posX;
	y = posY;
	oldY = posY;
	isColliCbrick = false;
	tag = EntityType::TEXT;
	this->frameH = frameH;
	this->frameW = frameW;
}

void Text::SetState(int state)
{
	Entity::SetState(state);

}

void Text::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	Entity::Update(dt, coObjects);
	x += dx;
	y += dy;
	if (y >= oldY)
	{
		y = oldY;
		vy = 0;
	}
}