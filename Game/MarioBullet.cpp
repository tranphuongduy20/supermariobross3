﻿#include "MarioBullet.h"
#include "Brick.h"
#include "CBrick.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Player.h"
#include "Venus.h"
#include "VenusNoFire.h"
#include "KoopaBullet.h"
#include "KoopaFly.h"

MarioBullet::MarioBullet()
{
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(ANIMATION_SET_BULLET));
	x = 0;
	y = 0;
	alpha = 0;
	isCollision = 0;
	isDone = true;
	damage = 1;
	timeDelayed = 0;
	timeDelayMax = BULLET_DELAY;
}

MarioBullet::~MarioBullet() {}

void MarioBullet::Update(DWORD dt, vector<LPGAMEENTITY>* colliable_objects)
{
	if (isDone == true)
	{
		alpha = 0;
		return;
	}
	else
	{
		if (!isCollision)
		{
			vy += BULLET_GRAVITY * dt;
			vx = BULLET_SPEED * direction;
		}
		timeDelayed += dt;
		Entity::Update(dt);
	
#pragma region Xử lý va chạm
		vector<LPCOLLISIONEVENT> coEvents;
		vector<LPCOLLISIONEVENT> coEventsResult;

		coEvents.clear();

		CalcPotentialCollisions(colliable_objects, coEvents);

		if (coEvents.size() == 0)
		{
			x += dx;
			y += dy;
		}
		else
		{
			float min_tx, min_ty, nx = 0, ny;
			float rdx = 0;
			float rdy = 0;

			FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

			for (UINT i = 0; i < coEventsResult.size(); i++)
			{
				x += min_tx * dx + nx * 0.4f;
				y += min_ty * dy + ny * 0.4f;
				LPCOLLISIONEVENT e = coEventsResult[i];
				/*if (e->nx != 0)
				{
					isCollision = 1;
					vx = 0;
					vy = 0;
				}
				if (e->ny != 0)
					vy = -BULLET_DEFLECT_SPEED_Y;*/
				if (e->obj->GetType() == EntityType::BRICK)
				{
					Brick* brick = dynamic_cast<Brick*>(e->obj);
					/*x += min_tx * dx + nx * 0.4f;
					y += min_ty * dy + ny * 0.001f;*/
					if (e->nx != 0)
					{
						isCollision = 1;
						vx = 0;
						vy = 0;
					}
					if (e->ny != 0)
						vy = -BULLET_DEFLECT_SPEED_Y;
				}
				else if (e->obj->GetType() == EntityType::CBRICK)
				{
					CBrick* cbrick = dynamic_cast<CBrick*>(e->obj);
					/*x += min_tx * dx + nx * 0.4f;
					y += min_ty * dy + ny * 0.001f;*/
					if (e->nx != 0)
					{
						isCollision = 1;
						vx = 0;
						vy = 0;
					}
					if (e->ny != 0)
						vy = -BULLET_DEFLECT_SPEED_Y;
				}
				else if (e->obj->GetType() == EntityType::BRICKSTAND)
				{
					CBrick* cbrick = dynamic_cast<CBrick*>(e->obj);
					/*x += min_tx * dx + nx * 0.4f;
					y += min_ty * dy + ny * 0.001f;*/
					if (e->nx != 0)
					{
						isCollision = 1;
						vx = 0;
						vy = 0;
					}
					if (e->ny != 0)
						vy = -BULLET_DEFLECT_SPEED_Y;
				}
				else if (e->obj->GetType() == EntityType::GOOMBA)
				{
					Goomba* goomba = dynamic_cast<Goomba*>(e->obj);
					if (goomba->id_goomba == GOOMBA_NORMAL)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							goomba->SetState(GOOMBA_STATE_DIE_FLY);
						}
					}
					else if (goomba->id_goomba == GOOMBA_RED)
					{
							goomba->SetState(GOOMBA_RED_STATE_NO_WING_DIE_FLY);
							goomba->hasWing = false;
					}
					isDone = true;
					goomba->make100 = true;
					Game::GetInstance()->Score += 100;
				}
				else if (e->obj->GetType() == EntityType::KOOPA)
				{
					Koopa* koopa = dynamic_cast<Koopa*>(e->obj);
					if (koopa->id_koopa == KOOPA_RED)
						koopa->SetState(KOOPA_RED_STATE_DIE_UP);
					else if (koopa->id_koopa == KOOPA_GREEN)
						koopa->SetState(KOOPA_GREEN_STATE_DIE_UP);
					koopa->hitByWeapon = true;
					isDone = true;
					koopa->make100 = true;
					Game::GetInstance()->Score += 100;
				}
				else if (e->obj->GetType() == EntityType::KOOPABULLET)
				{
					KoopaBullet* koopaB = dynamic_cast<KoopaBullet*>(e->obj);
					koopaB->SetState(KOOPABULLET_STATE_DIE_UP);
					koopaB->make100 = true;
					Game::GetInstance()->Score += 100;

				}
				else if (e->obj->GetType() == EntityType::KOOPAFLY)
				{
					KoopaFly* koopaF = dynamic_cast<KoopaFly*>(e->obj);
					koopaF->SetState(KOOPA_GREEN_STATE_DIE_UP);
					koopaF->make100 = true;
					Game::GetInstance()->Score += 100;
					isDone = true;
				}
				else if (e->obj->GetType() == EntityType::VENUS)
				{
					Venus* venus = dynamic_cast<Venus*>(e->obj);
					venus->isDeath = true;
					isDone = true;
					venus->make100 = true;
					Game::GetInstance()->Score += 100;
				}
				else if (e->obj->GetType() == EntityType::VENUSGREEN)
				{
					Venus* venus = dynamic_cast<Venus*>(e->obj);
					venus->isDeath = true;
					isDone = true;
				}
				else if (e->obj->GetType() == EntityType::VENUSNOFIRE)
				{
					VenusNoFire* venusNoFire = dynamic_cast<VenusNoFire*>(e->obj);
					venusNoFire->isDeath = true;
					isDone = true;
				}
			}
		}
		for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];


#pragma endregion
	}
}

void MarioBullet::Render()
{
	RenderBoundingBox();
	int ani;
	if (timeDelayed >= timeDelayMax)
	{
		isDone = true;
		timeDelayed = 0;
	}
	else
	{
		if (!isCollision)
		{
			ani = BULLET_ANI_FIRE;
			animationSet->at(0)->Render(nx, x, y, alpha);
		}
		else
		{
			ani = BULLET_ANI_BANG;
			animationSet->at(ani)->Render(nx, x, y - DISTANCE_TO_BANG, alpha);
			if (animationSet->at(ani)->GetCurrentFrame() == 4)
			{
				isDone = true;
				timeDelayed = 0;
				Player::GetInstance()->isBullet = false;
			}
		}
	}
}

void MarioBullet::GetBoundingBox(float& l, float& t, float& r, float& b)
{
	l = x;
	t = y;
	r = x + BULLET_BBOX_HEIGHT/2;
	b = y + BULLET_BBOX_WIDTH/2;

	if (isCollision) {
		l = 0;
		t = 0;
		r = 0;
		b = 0;
	}
}