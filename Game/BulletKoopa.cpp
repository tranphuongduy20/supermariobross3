#include "BulletKoopa.h"

BulletKoopa::BulletKoopa(float _x, float _y, Player* mario)
{
	isDone = true;
	x = _x;
	y = _y;
	this->mario = mario;
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(37));
}

void BulletKoopa::Render()
{
	if (isDeath)
		return;
	if (isDone)
	{
		alpha = 0;
	}
	else
	{
		alpha = 255;
	}
	animationSet->at(0)->Render(1, x, y, alpha);
	RenderBoundingBox();
}

void BulletKoopa::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{

	if (isDeath)
		return;
	Entity::Update(dt);
	if (!isDone)
	{
		x += dx;
		y += dy;
		if (this->IsCollidingObject(mario))
		{
			if (mario->level > MARIO_LEVEL_BIG)
			{
				mario->level = MARIO_LEVEL_BIG;
				mario->StartUntouchable();
			}
			else if (mario->level == MARIO_LEVEL_BIG)
			{
				mario->level = MARIO_LEVEL_SMALL;
				mario->StartUntouchable();
			}
			else
			{
				mario->SetState(MARIO_STATE_DIE);
			}
			isDone= true;
			return;
		}
		countAttack += dt;
		if (countAttack >= 1000)
		{
			if (!isBack)
			{
				vx = -vx;
				vy = -vy;
				isBack = true;
			}
		}
		if (countAttack > 1800)
		{
			countAttack = 0;
			isDone = true;
			vx = 0;
			vy = 0;
		}
	}
}

void BulletKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (!isDone)
	{
		left = x;
		top = y;
		right = x + BULLET_BBOX_WIDTH;
		bottom = y + BULLET_BBOX_HEIGHT;
	}
}

void BulletKoopa::Attack(float x, float y)
{

	float temp = (mario->x - x) / (mario->y - y);
	if (abs(temp) > 1)
	{
		if ((mario->x - x) > 0)
			vx = 0.07f;
		else
			vx = -0.07f;
		vy = vx / temp;
	}
	else
	{
		if ((mario->y - y) > 0)
			vy = 0.07f;
		else
			vy = -0.07f;
		vx = vy * temp;
	}
	DebugOut(L"vx vy %f    %f \n ", vx, vy);
	isBack = false;
	isDone = false;
	this->x = x;
	this->y = y;
}

