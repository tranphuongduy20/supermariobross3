#include "KoopaBullet.h"
#include "Brick.h"
#include "CBrick.h"
#include "BrickStand.h"
#include "Point.h"

KoopaBullet::KoopaBullet(Player* mario)
{
	tag = EntityType::KOOPABULLET;
	SetState(KOOPABULLET_STATE_WALKING);
	Mario = mario;
	bullet = new BulletKoopa(x, y, Mario);
}

void KoopaBullet::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (!isDeath)
	{
		left = x;
		top = y;
		bottom = top + KOOPABULLET_BBOX_HEIGHT;
		right = left + KOOPABULLET_BBOX_WIDTH;
	}
}

void KoopaBullet::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	//DebugOut(L"is count attack %d \n", countAttack);
	//DebugOut(L"is count ready %d \n", countReady);
	vy += 0.0005f * dt;
	Entity::Update(dt);
	bullet->Update(dt, coObjects);
	if (isDeath)
	{
		//bullet->isDeath= true;
		x += dx;
		y += dy;
		return;
	}
	if (isActive == true)
	{
		/*if (bullet->isBack)
		{
			float temp = (x - bullet->x) / (y - bullet->y);
			if (temp > 1)
			{
				if ((x - bullet->x) > 0)
					bullet->vx = 0.05f;
				else
					bullet->vx = -0.05f;
				bullet->vy = vx / temp;
			}
			else
			{
				if ((x - bullet->x) > 0)
					bullet->vy = 0.05f;
				else
					bullet->vy = -0.05f;
				bullet->vx = vy / temp;
			}
		}*/
		if (bullet->isDone == true && isAttack == false)
		{
			isReadyAttack = false;
			countReady = 0;
			countAttack += dt;
		}
		if (countAttack >= 1500 && isAttack == false)
		{
			countAttack = 0;
			bullet->alpha = 255;
			isAttack = true;
		}
		if (isAttack)
		{
			bullet->x = x;
			bullet->y = y;
			countReady += dt;
			if (countReady > 400)
			{
				isReadyAttack = true;
				countReady = 0;
				Attack();
			}
			return;
		}
	}
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	vector<LPGAMEENTITY> bricks;

	coEvents.clear();
	bricks.clear();
	coEvents.clear();
	for (UINT i = 0; i < coObjects->size(); i++)
		if (coObjects->at(i)->GetType() == EntityType::BRICK || coObjects->at(i)->GetType() == EntityType::CBRICK)
			bricks.push_back(coObjects->at(i));

	CalcPotentialCollisions(coObjects, coEvents);
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;
	}
	else
	{
		float min_tx, min_ty, nx = 0, ny = 0;
		float rdx = 0;
		float rdy = 0;
		// TODO: This is a very ugly designed function!!!!
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// block every object first!
		y += min_ty * dy + ny * 0.4f;
		x += min_tx * dx + nx * 0.4f;

		if (GetDistance(D3DXVECTOR2(this->x, this->y), D3DXVECTOR2(Mario->x, Mario->y)) <= 120)// Kiểm tra bán kính xung quanh xem có player không
		{
			SetDirection();
		}
		else // Đụng tường hay hết brick quay lại
		{
			if (nx != 0)
			{
				this->nx = -this->nx;
				//DebugOut(L"\nnx:  %d", nx);
			}
			if (ny != 0)
			{
				vy = 0;
				for (int i = 0; i < coEventsResult.size(); i++)
				{
					LPCOLLISIONEVENT e = coEventsResult.at(i);
					if (dynamic_cast<Brick*>(e->obj))
					{
						Brick* brick = dynamic_cast<Brick*>(e->obj);
						if (e->nx != 0)
						{
							x += dx;
						}
						else if (e->ny < 0)
						{
							if (x <= brick->x)
							{
								x = brick->x;
								nx = 1;

							}
							else if (x >= (brick->x + brick->frameW - 1 - KOOPABULLET_BBOX_WIDTH))
							{
								x = brick->x + brick->frameW - 1 - KOOPABULLET_BBOX_WIDTH;
								nx = -1;
							}
						}
					}
				}
				//DebugOut(L"\nny:  %d", ny);
			}
		}
	}

	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Update(dt, coObjects);
	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];
	Activation();
	
}

void KoopaBullet::Render()
{
	bullet->Render();
	if (isDeath)
	{ 
		animationSet->at(1)->Render(nx, x, y);
		return;
	}
	animationSet->at(0)->Render(nx, x, y);
	for (int i = 0; i < listEffect.size(); i++)
	{
		listEffect[i]->Render();
	}
	RenderBoundingBox();
}

void KoopaBullet::SetSpeed()
{
	if (nx > 0)
		vx = KOOPABULLET_WALKING_SPEED;
	if (nx < 0)
		vx = -KOOPABULLET_WALKING_SPEED;
}

void KoopaBullet::SetDirection()
{
	if ((Mario->x - this->x) > 0)
		nx = 1;
	else
		nx = -1;
}

void KoopaBullet::SetState(int state)
{
	Entity::SetState(state);
	switch (state)
	{
	case KOOPABULLET_STATE_WALKING:
		SetSpeed();
		break;
	case KOOPABULLET_STATE_DIE:
		isDeath = true;
		vy = 0;
		vx = 0;
		break;
	case KOOPABULLET_STATE_DIE_UP:
		
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

void KoopaBullet::Activation()
{
	if (!isActive)
	{
		vx = 0;
	}
	else
	{
		SetState(KOOPABULLET_STATE_WALKING);
	}

	if (GetDistance(D3DXVECTOR2(this->x, this->y), D3DXVECTOR2(Mario->x, Mario->y)) <= 120)
	{
		isActive = true;
	}
}

void KoopaBullet::Attack()
{
	if (isReadyAttack)
	{
		isReadyAttack = false;
		isAttack = false;
		bullet->Attack(x, y);
	}
	
}
