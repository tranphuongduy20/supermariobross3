#include "KoopaFly.h"
#include "Koopa.h"
#include "Point.h"
#include "BrokenBrick.h"
#include "Brick.h"
#include "CBrick.h"
#include "Goomba.h"
#include "Leaf.h"

KoopaFly::KoopaFly(Player* mario)
{
	tag = EntityType::KOOPAFLY;
	Mario = mario;
	directionY = 1;
	SetState(KOOPA_GREEN_STATE_HAS_WING_FLY_LEFT);
	timeToFly = GetTickCount64();

}

void KoopaFly::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (isDeath)
		return;
	left = x;
	top = y;
	bottom = top + KOOPAS_BBOX_HEIGHT_DIE;
	right = x + KOOPAS_BBOX_WIDTH;

}

void KoopaFly::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	Entity::Update(dt);
	if (isDeath)
	{
		vy += 0.0005f * dt;
		x += dx;
		y += dy;
		return;
	}

	if (vy < -0.055)
	{
		vy = 0;
		directionY = 0;
	}
	if (vy > 0.055)
	{
		vy = 0;
		directionY = 1;
	}
	if (directionY == 1)
		vy -= 0.00027;
	else
		vy += 0.00027;
	x += dx;
	y += dy;
}

void KoopaFly::Render()
{
	if (isDeath)
	{
		animationSet->at(20)->Render(nx, x, y);
		return;
	}
	//DebugOut(L"nx %d ani %d \n", this->nx, state);
	animationSet->at(20)->Render(-this->nx, x, y);
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Render();
	}
	//DebugOut(L"gia tri state %d \n", state);
	RenderBoundingBox();
}

void KoopaFly::SetState(int State)
{
	Entity::SetState(State);   // last_state de xac dinh trang thai truoc do cua koopas roi thuc hien cac truong hop
	switch (State)
	{
	case KOOPA_GREEN_STATE_DIE:
		isDeath = true;
		vx = 0;
		vy = 0.04f;
		break;
	case KOOPA_GREEN_STATE_DIE_UP:
		if (Mario->nx > 0)
			vx = 0.05;
		else
			vx = -0.05;
		if (isDeath == false)
			vy = -0.15;
		isDeath = true;
		break;
	}

}