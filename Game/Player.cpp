﻿#include <algorithm>
#include <assert.h>
#include <string>
#include "debug.h"

#include "Player.h"
#include "Game.h"
#include "Brick.h"
#include "CBrick.h"
#include "BrokenBrick.h"
#include "Goomba.h"
#include "Koopa.h"
#include "Mushroom.h"
#include "Coin.h"
#include "Venus.h"
#include "VenusNoFire.h"
#include "Money.h"
#include "SceneManager.h"
#include "PlayScene.h"

#define END_SCENE_TIME 5000

//class PlayScene;

using namespace std;

Player::Player(float x, float y) : Entity()
{
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(ANIMATION_SET_PLAYER));
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	SetState(MARIO_ANI_SMALL_IDLE);
	start_x = x;
	start_y = y;
	backup_JumpY = 0;
	dGround = 0;
	isFly = false;
	startTickToEndScene = GetTickCount64();
	startEndScene = false;
	flyTrip = false;
	isCheckCanFly = true;
	this->x = x;
	this->y = y;
	nx = 1;
	holdthing = nullptr;
	startVentTime = GetTickCount64();
	isInGround = venting;
	keyUp = keyDown = false;
}

void Player::Reset()
{
	this->SetAnimationSet(CAnimationSets::GetInstance()->Get(ANIMATION_SET_PLAYER));
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	SetState(MARIO_ANI_SMALL_IDLE);
	start_x = x;
	start_y = y;
	backup_JumpY = 0;
	dGround = 0;
	isFly = false;
	flyTrip = false;
	isOnMoveBrick = false;
	isCheckCanFly = true;
	//this->x = 50;
	//this->y = 350;
	nx = 1;
	holdthing = nullptr;
	startVentTime = GetTickCount64();
	isInGround = venting;
	keyUp = keyDown = false;
	isDie = false;
	vx = 0;
	vy = 0;
}

Player* Player::instance = NULL;
Player* Player::GetInstance()
{
	if (instance == NULL)
		instance = new Player();
	return instance;
}

void Player::ExcuteVentProcess() {

}

void Player::BeginVentProcess(bool inGround) {
	if (keyDown && isInGround) {
		venting = true;
		startVentTime = GetTickCount64();
		isInGround = true;
	}
	else if (keyUp && !isInGround) {
		venting = true;
		startVentTime = GetTickCount64();
		isInGround = false;
	}
}

void Player::Update(DWORD dt, vector<LPGAMEENTITY>* coObjects)
{
	if (venting) {
		ExcuteVentProcess();
	}

	//DebugOut(L"X = %f y = %f \n", x,y);
	
	//DebugOut(L"State %d \n", state);
	Entity::Update(dt);
	if (isOnMoveBrick)
	{
		//DebugOut(L"on move brick y = %f \n", vy);
		//DebugOut(L"on move brick y = %f \n", vx);
		vy = 0.09f;
		isSpin = false;
		//isKick = false;
		//isRun = false;
		isPressJump = false;
		//isCrouch = false;
		isIntoPipe = false;
		isOutPipe = false;
	}
	if (startEndScene) {
		vx = .08f;
		nx = 1;
		float delta = GetTickCount64() - startTickToEndScene;
		if (delta > END_SCENE_TIME) {
			dynamic_cast<PlayScene*>(SceneManager::GetInstance()->GetScene())->ChooseMap(STAGE_1);
			return;
		}
	}
	if (y > 610)
	{
		SetState(MARIO_STATE_DIE);
	}
	if (isIntoPipe)
	{
		vy = MARIO_GO_INTO_PIPE_SPEED_Y;
		y += dy;
		/*if (y > 113)
			isReadyToDark = true;*/
		/*if (y > 500)
		{
			DebugOut(L"State idle xxxxxxxxx %d \n", state);
			SetState(MARIO_STATE_IDLE);
		}*/
		return;
	}
	else if (isOutPipe)
	{
		vy = -MARIO_GO_INTO_PIPE_SPEED_Y;
		y += dy;
		if (level == MARIO_LEVEL_SMALL)
		{
			if (idStage == 1500)
			{
				if (y < 369.4)
				{
					isOnPipe = false;
					vx = 0;
					SetState(MARIO_STATE_IDLE);
					DebugOut(L"idle");
				}
			}
			else
			{
				if (y < 369.4)
				{
					isOnPipe = false;
					vx = 0;
					SetState(MARIO_STATE_IDLE);
					DebugOut(L"idle");
				}
			}
		}
		else
		{
			if (idStage == 1500)
			{
				if (y < 358.2)
				{
					isOnPipe = false;
					vx = 0;
					SetState(MARIO_STATE_IDLE);
					DebugOut(L"idle");
				}
			}
			else
			{
				if (y < 357.5)
				{
					isOnPipe = false;
					vx = 0;
					SetState(MARIO_STATE_IDLE);
					DebugOut(L"idle");
				}
			}
		}
		return;
	}

#pragma region Xử lý vy
	vy += MARIO_GRAVITY * dt;
	//vy = 0.05;
	//Check hightPlayer
	/*if (isJumping && backup_JumpY - y >= HIGHT_LEVER1 && isJumpHandle == false)
	{
		if (!isPressJump)
			vy = 0.09;
		isJumpHandle = true;
	}*/
	if (vy < 0 && !Game::GetInstance()->IsKeyDown(DIK_S))
	{
		vy += 0.0003 * dt;
	}

	if (vy > 0.04)
	{
		isJumping = true;
		isFly = false;
	}

#pragma endregion

#pragma region RUN
	if (isRun == true && vx != 0)
	{
		//if (abs(vx) < MARIO_VMAX)
			vx += MARIO_SPEEEDUP * nx;
	}
#pragma endregion

#pragma region Xử lý va chạm
	vector<LPCOLLISIONEVENT> coEvents;
	vector<LPCOLLISIONEVENT> coEventsResult;
	coEvents.clear();
	isWaittingPressBtn = GetTickCount64() - startWalkingComplete <= MARIO_TYPE;
	if (isWaittingPressBtn) {
		//DebugOut(L"\n true - %d", GetTickCount() - startWalkingComplete);
	}
	//else DebugOut(L"\n false - %d", GetTickCount() - startWalkingComplete);

	if (isWaittingPressBtn) {
		if (walkingDirection != nx) {
			isWalkingComplete = true;
		}
		else
			isWalkingComplete = false;
	}
	else
		isWalkingComplete = false;

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}
	if (isWaitingForAni && animationSet->at(state)->IsOver()) {
		isWaitingForAni = false;
	}

	

	//turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalcPotentialCollisions(coObjects, coEvents);

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
		float w = level == MARIO_LEVEL_SMALL ? MARIO_SMALL_BBOX_HEIGHT / 2 : MARIO_BIG_BBOX_HEIGHT / 2;

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		x += min_tx * dx + nx * 0.4f;
		y += min_ty * dy + ny * 0.001f;

		for (UINT i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];
			if (e->obj->GetType() == EntityType::BRICK)
			{
				Brick* brick = dynamic_cast<Brick*>(e->obj);
				/*x += min_tx * dx + nx * 0.4f;
				y += min_ty * dy + ny * 0.001f;*/
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isOnMoveBrick = false;
						isGround = true;
						isJumping = false;
						isFly = false;
						isCheckCanFly = true;
						vy = 0;
						if ((flyTrip && level == MARIO_LEVEL_RACCOON) || y > dGround) {
							dGround = y + w;
						}
						flyTrip = false;
					}
					else
						y += dy;
				}
				else if (e->nx != 0) {
					x += dx;
				}
			}
			//else if (e->obj->GetType() == EntityType::BROKENBRICK)
			//{
			//	BrokenBrick* brokenbrick = dynamic_cast<BrokenBrick*>(e->obj);
			//	//x += min_tx * dx + nx * 0.4f;
			//	//y += min_ty * dy + ny * 0.001f;
			//	//if (nx != 0) vx = 0;
			//	if (ny != 0) vy = 0;
			//	if (e->ny != 0)
			//	{
			//		if (e->ny == -1)
			//		{
			//			isGround = true;
			//			isJumping = false;
			//			isFly = false;
			//			isCheckCanFly = true;
			//			vy = 0;
			//			if ((flyTrip && level == MARIO_LEVEL_RACCOON) || y > dGround) {
			//				dGround = y + w;
			//			}
			//			flyTrip = false;
			//		}
			//		else
			//			y += dy;
			//	}
			//}
			else if (e->obj->GetType() == EntityType::CBRICK)  
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isOnMoveBrick = false;
						isGround = true;
						isJumping = false;
						if ((flyTrip && level == MARIO_LEVEL_RACCOON) || y > dGround) {
							dGround = y + w;
						}
						flyTrip = false;
					}
				}
				if (e->ny > 0)
				{
					brick->SetState(CBRICK_STATE_COLLISION);
				}
			}
			else if (e->obj->GetType() == EntityType::CBRICKV2)
			{
				CBrickV2* brickV2 = dynamic_cast<CBrickV2*>(e->obj);
				if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isOnMoveBrick = false;
						isGround = true;
						isJumping = false;
						if ((flyTrip && level == MARIO_LEVEL_RACCOON) || y > dGround) {
							dGround = y + w;
						}
						flyTrip = false;
					}
				}
				if (e->ny > 0)
				{
					brickV2->SetState(CBRICK_STATE_COLLISION);
				}
			}
			else if (e->obj->GetType() == EntityType::MOVEBRICK)
			{
				BrickMove* brickM = dynamic_cast<BrickMove*>(e->obj);
				if (e->nx != 0) 
				{ 
					vx = 0; 
					x -= 0.05;
					isPushed = true;
				}
				if (ny != 0) vy = 0;
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isGround = true;
						isJumping = false;
						if ((flyTrip && level == MARIO_LEVEL_RACCOON) || y > dGround) {
							dGround = y + w;
						}
						flyTrip = false;
					}
				}
				if (e->ny < 0)
				{
					isOnMoveBrick = true;
					brickM->SetState(MOVEBRICK_STATE_COLLISION);
				}
			}
			else if (e->obj->GetType() == EntityType::ENDSCENE)
			{
				EndScene* endScene = dynamic_cast<EndScene*>(e->obj);
				if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isGround = true;
						isJumping = false;
						if ((flyTrip && level == MARIO_LEVEL_RACCOON) || y > dGround) {
							dGround = y + w;
						}
						flyTrip = false;
					}
				}
				if (e->ny > 0)
				{
					endScene->SetState(ENDSCENE_STATE_COLLISION);
					startEndScene = true;
					startTickToEndScene = GetTickCount64();
				}
			}
			else if (e->obj->GetType() == EntityType::BRICKSTAND)
			{
				CBrick* brick = dynamic_cast<CBrick*>(e->obj);
				if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isGround = true;
						isJumping = false;
						if ((flyTrip && level == MARIO_LEVEL_RACCOON) || y > dGround) {
							dGround = y + w;
						}
						flyTrip = false;
					}
				}
			}
			else if (e->obj->GetType() == EntityType::BROKENBRICK)
			{
				BrokenBrick* brokenBrick = dynamic_cast<BrokenBrick*>(e->obj);
				//if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
				if (e->ny != 0)
				{
					if (e->ny == -1)
					{
						isGround = true;
						isJumping = false;
						if ((flyTrip && level == MARIO_LEVEL_RACCOON) || y > dGround) {
							dGround = y + w;
						}
						flyTrip = false;
					}
				}
				if (e->ny > 0)
				{
					brokenBrick->SetState(STATE_DESTROYED);
				}
			}
			else if (e->obj->GetType() == EntityType::VENUS)
			{
				Venus* venus = dynamic_cast<Venus*>(e->obj);
				//if (nx != 0) vx = 0;
				//if (ny != 0) vy = 0;
				if (untouchable == 0)
				{
					if (level > MARIO_LEVEL_BIG)
					{
						level = MARIO_LEVEL_BIG;
						StartUntouchable();
					}
					else if (level == MARIO_LEVEL_BIG)
					{
						level = MARIO_LEVEL_SMALL;
						StartUntouchable();
					}
					else
					{
						SetState(MARIO_STATE_DIE);
						return;
					}
					vx = 0;
					vy = 0;

				}
			}
			else if (e->obj->GetType() == EntityType::VENUSGREEN)
			{
				Venus* venus = dynamic_cast<Venus*>(e->obj);
				//if (nx != 0) vx = 0;
				//if (ny != 0) vy = 0;
				if (untouchable == 0)
				{
					if (level > MARIO_LEVEL_BIG)
					{
						level = MARIO_LEVEL_BIG;
						StartUntouchable();
					}
					else if (level == MARIO_LEVEL_BIG)
					{
						level = MARIO_LEVEL_SMALL;
						StartUntouchable();
					}
					else
					{
						SetState(MARIO_STATE_DIE);
						return;
					}
					vx = 0;
					vy = 0;

				}
			}
			else if (e->obj->GetType() == EntityType::VENUSNOFIRE)
			{
				VenusNoFire* venusNoFire = dynamic_cast<VenusNoFire*>(e->obj);
				//if (nx != 0) vx = 0;
				//if (ny != 0) vy = 0;
				if (untouchable == 0)
				{
					if (level > MARIO_LEVEL_BIG)
					{
						level = MARIO_LEVEL_BIG;
						StartUntouchable();
					}
					else if (level == MARIO_LEVEL_BIG)
					{
						level = MARIO_LEVEL_SMALL;
						StartUntouchable();
					}
					else
					{
						SetState(MARIO_STATE_DIE);
						return;
					}
					vx = 0;
					vy = 0;
				}
			}
			else if (e->obj->GetType() == EntityType::GOOMBA)
			{
				Goomba* goomba = dynamic_cast<Goomba*>(e->obj);

				if (e->ny < 0)
				{
					if (goomba->id_goomba == GOOMBA_NORMAL)     //kill goomba normal
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							goomba->SetState(GOOMBA_STATE_DIE);
							goomba->make100 = true;
							Game::GetInstance()->Score += 100;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							isJumping = true;
						}
					}
					else if (goomba->id_goomba == GOOMBA_RED)				//kill goomba red
					{
						if (goomba->hasWing)
						{
							goomba->SetState(GOOMBA_RED_STATE_NO_WING_WALK);	//khi co canh thi ve trang thai di bo							
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							goomba->make200 = true;
							Game::GetInstance()->Score += 200;
							goomba->hasWing = false;
							isJumping = true;
						}
						else
						{
							if (goomba->GetState() != GOOMBA_RED_STATE_NO_WING_DIE)
							{
								goomba->SetState(GOOMBA_RED_STATE_NO_WING_DIE);
								vy = -MARIO_JUMP_DEFLECT_SPEED;
								isJumping = true;
								goomba->make100 = true;
								Game::GetInstance()->Score += 100;
							}
						}
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable == 0)
					{
						if (goomba->id_goomba == GOOMBA_NORMAL)
						{
							if (goomba->GetState() != GOOMBA_STATE_DIE || goomba->GetState() != GOOMBA_STATE_DIE_FLY)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_BIG;
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
								{
									SetState(MARIO_STATE_DIE);
									return;
								}
							}
						}
						else if (goomba->id_goomba == GOOMBA_RED)
						{
							if (goomba->GetState() != GOOMBA_RED_STATE_NO_WING_DIE || goomba->GetState() != GOOMBA_RED_STATE_NO_WING_DIE_FLY)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_BIG;
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
								{
									SetState(MARIO_STATE_DIE);
									return;
								}
							}
						}

					}
				}
			}
			/*else if (e->obj->GetType() == EntityType::KOOPABULLET)
			{
				KoopaBullet* koopaB = dynamic_cast<KoopaBullet*>(e->obj);
				DebugOut(L"e->ny %d \n", e->ny);
				if (e->ny != 0)
				{
					koopaB->SetState(KOOPABULLET_STATE_DIE);
					koopaB->make100 = true;
					Game::GetInstance()->Score += 100;
					vy = -MARIO_JUMP_DEFLECT_SPEED;
					isJumping = true;
				}
				else if (e->nx != 0 || e->ny > 0)
				{
					if (untouchable == 0)
					{
						if (level > MARIO_LEVEL_BIG)
						{
							level = MARIO_LEVEL_BIG;
							StartUntouchable();
						}
						else if (level == MARIO_LEVEL_BIG)
						{
							level = MARIO_LEVEL_SMALL;
							StartUntouchable();
						}
						else
						{
							SetState(MARIO_STATE_DIE);
							return;
						}
					}
				}
			}*/
			else if (e->obj->GetType() == EntityType::KOOPAFLY)
			{
			KoopaFly* koopaF = dynamic_cast<KoopaFly*>(e->obj);
			if (e->ny < 0)
			{
				koopaF->SetState(KOOPA_GREEN_STATE_DIE);
				koopaF->make100 = true;
				Game::GetInstance()->Score += 100;
				vy = -MARIO_JUMP_DEFLECT_SPEED;
				isJumping = true;
			}
			else if (e->nx != 0 || e->ny > 0)
			{
				if (untouchable == 0)
				{
					if (level > MARIO_LEVEL_BIG)
					{
						level = MARIO_LEVEL_BIG;
						StartUntouchable();
					}
					else if (level == MARIO_LEVEL_BIG)
					{
						level = MARIO_LEVEL_SMALL;
						StartUntouchable();
					}
					else
					{
						SetState(MARIO_STATE_DIE);
						return;
					}
				}
			}
			}
			else if (e->obj->GetType() == EntityType::KOOPA)
			{
				Koopa* koopa = dynamic_cast<Koopa*>(e->obj);
				/*x += min_tx * dx + nx * 0.4f;
				y += min_ty * dy + ny * 0.001f;*/
				if (nx != 0) vx = 0;
				if (ny != 0) vy = 0;
				if (koopa->id_koopa == KOOPA_RED)
				{
					if (e->ny < 0)
					{
						if (koopa->GetState() == KOOPA_RED_STATE_WALKING_RIGHT || koopa->GetState() == KOOPA_RED_STATE_WALKING_LEFT)
						{
							koopa->SetState(KOOPA_RED_STATE_DIE);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							isJumping = true;
						}
						else if (koopa->GetState() == KOOPA_RED_STATE_DIE_AND_MOVE)
						{
							koopa->SetState(KOOPA_RED_STATE_DIE);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							isJumping = true;
						}
						else if (koopa->GetState() == KOOPA_RED_STATE_DIE_AND_MOVE_UP)
						{
							koopa->SetState(KOOPA_RED_STATE_DIE_UP);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							isJumping = true;
						}
						else if (koopa->GetState() == KOOPA_RED_STATE_DIE)
						{
							koopa->SetState(KOOPA_RED_STATE_DIE_AND_MOVE);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							if (lastnx > 0)
							{
								koopa->nx = 1;
							}
							else
								koopa->nx = -1;
							y += dy;

						}
						else if (koopa->GetState() == KOOPA_RED_STATE_DIE_UP)
						{
							koopa->SetState(KOOPA_RED_STATE_DIE_AND_MOVE_UP);
							
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							if (lastnx > 0)
							{
								koopa->nx = 1;
							}
							else
								koopa->nx = -1;
							y += dy;
						}
					}
					else if (Game::GetInstance()->IsKeyDown(DIK_A) && e->nx != 0 && (koopa->GetState() == KOOPA_RED_STATE_DIE || koopa->GetState() == KOOPA_RED_STATE_DIE_UP))// xac dinh dang nhan giu phim A va cham vs koopas 
					{
						e->obj->nx = nx;
						if (koopa->last_state == KOOPA_RED_STATE_DIE)
							koopa->SetState(KOOPA_RED_STATE_HOLDING);
						else if (koopa->last_state == KOOPA_RED_STATE_DIE_UP)
							koopa->SetState(KOOPA_RED_STATE_HOLDING_UP);
						holdthing = e->obj;
					}
					else if (koopa->GetState() == KOOPA_RED_STATE_DIE || koopa->GetState() == KOOPA_RED_STATE_DIE_UP)
					{
						if (e->nx > 0)
						{
							isKick = true;
							if (koopa->last_state == KOOPA_RED_STATE_DIE)
								koopa->SetState(KOOPA_RED_STATE_DIE_AND_MOVE);
							else if (koopa->last_state == KOOPA_RED_STATE_DIE_UP)
								koopa->SetState(KOOPA_RED_STATE_DIE_AND_MOVE_UP);
							koopa->nx = -1;
						}
						else
						{
							isKick = true;
							if (koopa->last_state == KOOPA_RED_STATE_DIE)
								koopa->SetState(KOOPA_RED_STATE_DIE_AND_MOVE);
							else if (koopa->last_state == KOOPA_RED_STATE_DIE_UP)
								koopa->SetState(KOOPA_RED_STATE_DIE_AND_MOVE_UP);
							koopa->nx = 1;
						}
					}
					else if (e->nx != 0)
					{
						if (untouchable == 0)
						{
							if (koopa->GetState() != KOOPA_RED_STATE_DIE && koopa->GetState() != KOOPA_RED_STATE_DIE_UP && !holdthing)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_BIG;
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
								{
									SetState(MARIO_STATE_DIE);
									return;
								}
							}
						}
					}
				}
				else if (koopa->id_koopa == KOOPA_GREEN)
				{
					if (e->ny < 0)
					{
						if (koopa->GetState() == KOOPA_GREEN_STATE_HAS_WING_FLY_RIGHT || koopa->GetState() == KOOPA_GREEN_STATE_HAS_WING_FLY_LEFT)
						{
							if (koopa->nx > 0)
								koopa->SetState(KOOPA_GREEN_STATE_WALKING_RIGHT);
							else
								koopa->SetState(KOOPA_GREEN_STATE_WALKING_LEFT);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							koopa->hasWing = false;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							isJumping = true;
							DebugOut(L"rot");
						}
						else if (koopa->GetState() == KOOPA_GREEN_STATE_WALKING_LEFT || koopa->GetState() == KOOPA_GREEN_STATE_WALKING_RIGHT)
						{
							koopa->SetState(KOOPA_GREEN_STATE_DIE);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							isJumping = true;
						}
						else if (koopa->GetState() == KOOPA_GREEN_STATE_DIE_AND_MOVE)
						{
							koopa->SetState(KOOPA_GREEN_STATE_DIE);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							isJumping = true;
						}
						else if (koopa->GetState() == KOOPA_GREEN_STATE_DIE_AND_MOVE_UP)
						{
							koopa->SetState(KOOPA_GREEN_STATE_DIE_UP);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							vy = -MARIO_JUMP_DEFLECT_SPEED;
							isJumping = true;
						}
						else if (koopa->GetState() == KOOPA_GREEN_STATE_DIE)
						{

							koopa->SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							if (lastnx > 0)
							{
								koopa->nx = 1;
							}
							else
								koopa->nx = -1;
							y += dy;

						}
						else if (koopa->GetState() == KOOPA_GREEN_STATE_DIE_UP)
						{
							koopa->SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE_UP);
							koopa->make100 = true;
							Game::GetInstance()->Score += 100;
							if (lastnx > 0)
							{
								koopa->nx = 1;
							}
							else
								koopa->nx = -1;
							y += dy;
						}
					}
					else if (Game::GetInstance()->IsKeyDown(DIK_A) && e->nx != 0 && (koopa->GetState() == KOOPA_GREEN_STATE_DIE || koopa->GetState() == KOOPA_GREEN_STATE_DIE_UP)) 
					{
						e->obj->nx = nx;
						holdthing = e->obj;
						if (koopa->last_state == KOOPA_GREEN_STATE_DIE)
							koopa->SetState(KOOPA_GREEN_STATE_HOLDING);
						else if (koopa->last_state == KOOPA_GREEN_STATE_DIE_UP)
							koopa->SetState(KOOPA_GREEN_STATE_HOLDING_UP);
					}
					else if (koopa->GetState() == KOOPA_GREEN_STATE_DIE || koopa->GetState() == KOOPA_GREEN_STATE_DIE_UP)
					{
						if (e->nx > 0)
						{
							isKick = true;
							if (koopa->last_state == KOOPA_GREEN_STATE_DIE)
								koopa->SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE);
							else if (koopa->last_state == KOOPA_GREEN_STATE_DIE_UP)
								koopa->SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE_UP);
							koopa->nx = -1;
						}
						else
						{
							isKick = true;
							if (koopa->last_state == KOOPA_GREEN_STATE_DIE)
								koopa->SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE);
							else if (koopa->last_state == KOOPA_GREEN_STATE_DIE_UP)
								koopa->SetState(KOOPA_GREEN_STATE_DIE_AND_MOVE_UP);
							koopa->nx = 1;
						}
					}
					else if (e->nx != 0 || e->ny > 0)
					{
						if (untouchable == 0)
						{
							if (koopa->GetState() != KOOPA_GREEN_STATE_DIE && koopa->GetState() != KOOPA_GREEN_STATE_DIE_UP && !holdthing)
							{
								if (level > MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_BIG;
									StartUntouchable();
								}
								else if (level == MARIO_LEVEL_BIG)
								{
									level = MARIO_LEVEL_SMALL;
									StartUntouchable();
								}
								else
								{
									SetState(MARIO_STATE_DIE);
									return;
								}
							}
						}
					}
				}
			}
			else if (e->obj->GetType() == EntityType::COIN)
			{
				Coin* coin = dynamic_cast<Coin*>(e->obj);
				if (IsCollidingObject(e->obj))
				{
					coin->isActive = false;
				}
			}
			else if (e->obj->GetType() == EntityType::MUSH)
			{
				Mushroom* mush = dynamic_cast<Mushroom*>(e->obj);
				listitems.push_back(mush);
				Collision_items(&listitems);
			}
		}

	}
	for (UINT i = 0; i < coEvents.size(); i++) delete coEvents[i];


	float k, l;
	GetPosition(k, l);
	if (!Game::GetInstance()->IsKeyDown(DIK_A))
	{
		holdthing = nullptr;
	}
	if (holdthing && Game::GetInstance()->IsKeyDown(DIK_A))
	{
		if (level == MARIO_LEVEL_SMALL)
			holdthing->SetPosition(nx == 1 ? k + nx * 10 : k + nx * 13, l - 5);
		else
			holdthing->SetPosition(nx == 1 ? k + nx * 10 : k + nx * 13, l - 5);
	}


#pragma endregion
}

void Player::Collision_items(vector<LPGAMEENTITY>* coObjects)
{
}


void Player::Render()
{
	int ani = -1;
	int alpha = 255;
	if (untouchable) alpha = 128;
	if (isDie == true) {
		ani = MARIO_ANI_SMALL_DIE;
	}
	else if (level == MARIO_LEVEL_BIG)
	{
		if (isOnPipe)
		{
			ani = MARIO_ANI_BIG_IDLE_FRONT;
		}
		else if (isOnMoveBrick)
		{
			if (isWalkingComplete == true)
			{
				ani = MARIO_ANI_BIG_SKID;
			}
			else if (isCrouch == true) {
				ani = MARIO_ANI_BIG_CROUCH;
			}
			else
			{
				if (vx == 0)
					ani = MARIO_ANI_BIG_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_BIG_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_BIG_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_BIG_RUN;
			}
		}
		else if (holdthing)
		{
			if (vx == 0)
				ani = MARIO_ANI_BIG_IDLE_HOLD;
			else
				ani = MARIO_ANI_BIG_HOLD;
		}
		else if (isCrouch == true) {
			ani = MARIO_ANI_BIG_CROUCH;
		}
		else if (isWalkingComplete == true && vy == 0)
		{
			ani = MARIO_ANI_BIG_SKID;
		}
		else if (isKick == true) {
			ani = MARIO_ANI_BIG_KICK;
		}
		else
		{
			if (isPushed == true)
			{
				ani = MARIO_ANI_BIG_WALKING;
			}
			else if (isJumping == false)
			{
				if (vx == 0)
					ani = MARIO_ANI_BIG_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_BIG_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_BIG_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_BIG_RUN;
			}
			else
			{
				if (vy > 0)
					ani = MARIO_ANI_BIG_JUMP_DOWN;
				else
					ani = MARIO_ANI_BIG_JUMP_UP;
			}
		}
	}
	else if (level == MARIO_LEVEL_SMALL)
	{
		if (isOnPipe)
		{
			ani = MARIO_ANI_SMALL_IDLE_FRONT;
		}
		else if (isOnMoveBrick)
		{
			if (isWalkingComplete == true)
			{
				ani = MARIO_ANI_SMALL_SKID;
			}
			else
			{
				if (vx == 0)
					ani = MARIO_ANI_SMALL_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_SMALL_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_SMALL_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_SMALL_RUN;
			}
		}
		else if (holdthing)
		{
			if (vx == 0)
				ani = MARIO_ANI_SMALL_IDLE_HOLD;
			else
				ani = MARIO_ANI_SMALL_HOLD;
		}
		else if (isWalkingComplete == true && vy == 0) {
			ani = MARIO_ANI_SMALL_SKID;
		}
		else if (isKick == true) {
			ani = MARIO_ANI_SMALL_KICK;
		}
		else 
		{
			if (isPushed == true)
			{
				ani = MARIO_ANI_SMALL_WALKING;
			}
			else if (isJumping == false)
			{
				if (vx == 0)
					ani = MARIO_ANI_SMALL_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_SMALL_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_SMALL_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_SMALL_RUN;
			}
			else
				ani = MARIO_ANI_SMALL_JUMP;
		}

	}
	else if (level == MARIO_LEVEL_RACCOON)
	{
		if (isOnPipe)
		{
			ani = MARIO_ANI_RACCOON_IDLE_FRONT;
		}
		else if (isOnMoveBrick)
		{
			DebugOut(L"isWalkingComplet %d \n", isCrouch);
			if (isWalkingComplete == true)
			{
				ani = MARIO_ANI_RACCOON_SKID;
			}
			else if (isCrouch == true) {
				ani = MARIO_ANI_RACCOON_CROUCH;
			}
			else
			{
				if (vx == 0)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_IDLE;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_WALKING;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_RUN;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (MARIO_VMAX < abs(vx))
				{
					ani = MARIO_ANI_RACCOON_SPEEDUP;
				}
			}
		}
		else if (isFly)
		{
			ani = MARIO_ANI_RACCOON_FLY;
		}
		else if (holdthing)
		{
			if (vx == 0)
				ani = MARIO_ANI_RACCOON_IDLE_HOLD;
			else
				ani = MARIO_ANI_RACCOON_HOLD;
		}
		else if (isCrouch)
			ani = MARIO_ANI_RACCOON_CROUCH;
		else if (isSpin)
			ani = MARIO_ANI_RACCOON_SPIN;
		else if (isWalkingComplete && vy == 0)
			ani = MARIO_ANI_RACCOON_SKID;
		else if (isKick == true) {
			ani = MARIO_ANI_RACCOON_KICK;
		}
		else
		{
			if (isPushed == true)
			{
				ani = MARIO_ANI_RACCOON_WALKING;
			}
			else if (isJumping == false)
			{
				if (vx == 0)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_IDLE;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_WALKING;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_RUN;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else if (MARIO_VMAX < abs(vx))
				{
					ani = MARIO_ANI_RACCOON_SPEEDUP;
				}
			}
			else
			{
				if (vy > MARIO_VY_TO_CONFIRM_JUM)
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_JUMP_DOWN;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
				else
				{
					if (isAttack == false)
						ani = MARIO_ANI_RACCOON_JUMP_UP;
					else ani = MARIO_ANI_RACCOON_SPIN_SINGLE;
				}
			}
		}
		if (ani == MARIO_ANI_RACCOON_IDLE_FRONT) //racoon idle front
			animationSet->at(ani)->Render(nx, x , y, alpha);
		else
		{
			if (nx > 0)
			{
				if (animationSet->at(MARIO_ANI_RACCOON_SPIN_SINGLE)->GetCurrentFrame() == 1 || animationSet->at(MARIO_ANI_RACCOON_SPIN_SINGLE)->GetCurrentFrame() == 2 || animationSet->at(MARIO_ANI_RACCOON_SPIN_SINGLE)->GetCurrentFrame() == 3)
					animationSet->at(ani)->Render(nx, x, y, alpha);
				else
					animationSet->at(ani)->Render(nx, x - 8, y, alpha);
			}
			else
			{
				if (animationSet->at(MARIO_ANI_RACCOON_SPIN_SINGLE)->GetCurrentFrame() == 2)
					animationSet->at(ani)->Render(nx, x - 8, y, alpha);
				else
					animationSet->at(ani)->Render(nx, x, y, alpha);
			}
		}
		RenderBoundingBox();
		if (animationSet->at(MARIO_ANI_RACCOON_KICK)->GetCurrentFrame() == 2)
			isKick = false;
		if (animationSet->at(MARIO_ANI_RACCOON_SPIN_SINGLE)->GetCurrentFrame() == 4)
			isAttack = false;
		return;
	}
	else if (level == MARIO_LEVEL_FIRE)
	{
		if (isOnPipe)
		{
			ani = MARIO_ANI_FIRE_IDLE_FRONT;
		}
		else if (isOnMoveBrick)
		{
			if (isWalkingComplete == true)
			{
				ani = MARIO_ANI_FIRE_SKID;
			}
			else if (isCrouch == true) {
				ani = MARIO_ANI_FIRE_CROUCH;
			}
			else
			{
				if (vx == 0)
					ani = MARIO_ANI_FIRE_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_FIRE_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_FIRE_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_FIRE_RUN;
			}
		}
		else if (holdthing)
		{
			if (vx == 0)
				ani = MARIO_ANI_FIRE_IDLE_HOLD;
			else
				ani = MARIO_ANI_FIRE_HOLD;
		}
		else if (isBullet == true && GetTickCount64() - shootTime < SHOOT_ANIMATION_TIME) 
		{
			ani = MARIO_ANI_FIRE_THROW;
		}
		else if (isCrouch == true) {
			ani = MARIO_ANI_FIRE_CROUCH;
		}
		else if (isWalkingComplete == true && vy == 0) {
			ani = MARIO_ANI_FIRE_SKID;
		}
		else if (isKick == true) {
			ani = MARIO_ANI_FIRE_KICK;
		}
		else
		{
			if (isPushed == true)
			{
				ani = MARIO_ANI_RACCOON_WALKING;
			}
			else if (isJumping == false)
			{
				if (vx == 0)
					ani = MARIO_ANI_FIRE_IDLE;
				else if (0 < abs(vx) && abs(vx) <= MARIO_VMAX / 2)
					ani = MARIO_ANI_FIRE_WALKING;
				else if (MARIO_VMAX / 2 < abs(vx) && abs(vx) <= MARIO_VMAX)
					ani = MARIO_ANI_FIRE_WALK_RUN;
				else if (MARIO_VMAX < abs(vx))
					ani = MARIO_ANI_FIRE_RUN;
			}
			else
			{
				if (vy > 0)
					ani = MARIO_ANI_FIRE_JUMP_DOWN;
				else
					ani = MARIO_ANI_FIRE_JUMP_UP;
			}
		}
	}
	animationSet->at(ani)->Render(nx, x, y, alpha);
	if (animationSet->at(MARIO_ANI_SMALL_KICK)->GetCurrentFrame() == 2)
		isKick = false;
	if (animationSet->at(MARIO_ANI_BIG_KICK)->GetCurrentFrame() == 2)
		isKick = false;
	if (animationSet->at(MARIO_ANI_FIRE_KICK)->GetCurrentFrame() == 2)
		isKick = false;
	//DebugOut(L"[vx] %f \n", vx);
	RenderBoundingBox();
}

void Player::SetState(int state)
{
	Entity::SetState(state);

	switch (state)
	{
	case MARIO_STATE_WALKING_RIGHT:
		isCrouch = false;
		//isKick = false;
		nx = 1;
		if (isRun == true)
			return;
		// vx = MARIO_WALKING_SPEED;
		vx += MARIO_WALKING_ACC * dt;
		if (vx > MARIO_WALKING_SPEED) vx = MARIO_WALKING_SPEED;
		break;
	case MARIO_STATE_WALKING_LEFT:
		isCrouch = false;
		//isKick = false;
		nx = -1;
		if (isRun == true)
			return;
		vx -= MARIO_WALKING_ACC * dt;
		if (vx < -MARIO_WALKING_SPEED) vx = -MARIO_WALKING_SPEED;
		break;
	case MARIO_STATE_IDLE:
		isSpin = false;
		//isKick = false;
		isRun = false;
		isPressJump = false;
		isCrouch = false;
		isIntoPipe = false;
		isOutPipe = false;
		if (isOnMoveBrick)
			vx = 0;
		if (vx > MARIO_WALKING_SPEED)
			vx = MARIO_WALKING_SPEED;
		if (vx < -MARIO_WALKING_SPEED)
			vx = -MARIO_WALKING_SPEED;
		if (vx > 0) 
		{
			vx -= 1.65 * MARIO_WALKING_ACC * dt;
			if (vx < 0)
				vx = 0;
		}
		else if (vx < 0) 
		{
			vx += 1.65 * MARIO_WALKING_ACC * dt;
			if (vx > 0)
				vx = 0;
		}
		//vx = 0;
		break;
	case MARIO_STATE_JUMP:
		isPressJump = true;
		isCrouch == true;
		if (level == MARIO_LEVEL_RACCOON)
		{
			if ((abs(vx) > MARIO_VMAX || isFly == true) && vy < 0.01 && isCheckCanFly == true)
			{
				isCrouch = false;
				isFly = true;
				flyTrip = true;
				vy = -MARIO_VY_FLY;
			}
			else
			{
				if (isOnMoveBrick)
				{
					DebugOut(L"Jumping");
					isJumpHandle = false;
					isJumping = true;
					vy = -MARIO_JUMP_SPEED_YMAX;
					backup_JumpY = y;
					isOnMoveBrick = false;
				}
				else
				{
					if (isJumping == true)
					{
						if (vy < 0)
							return;
						else
							vy = -MARIO_STOP_FALLING;
					}
					else
					{
						DebugOut(L"Jumping");
						isJumpHandle = false;
						isJumping = true;
						vy = -MARIO_JUMP_SPEED_YMAX;
						backup_JumpY = y;
					}
				}
			}
		}
		else
		{

			if (isOnMoveBrick)
			{
				DebugOut(L"Jumping");
				isJumpHandle = false;
				isJumping = true;
				vy = -MARIO_JUMP_SPEED_YMAX;
				backup_JumpY = y;
				isOnMoveBrick = false;
			}
			else
			{
				if (isJumping == true)
					return;
				else
				{
					isJumpHandle = false;
					isJumping = true;
					vy = -MARIO_JUMP_SPEED_YMAX;
					backup_JumpY = y;
				}
			}
		}
		break;
	case MARIO_STATE_CROUCH:
		isCrouch = true;
		//vx = 0;
		if (vx > 0) 
		{
			
			vx -= MARIO_WALKING_ACC * dt;
			if (vx < 0)
				vx = 0;
		}
		else if (vx < 0) 
		{
			vx += MARIO_WALKING_ACC * dt;
			if (vx > 0)
				vx = 0;
		}
		break;
	case MARIO_STATE_SPIN:
		isSpin = true;
		break;
	case MARIO_STATE_DIE:
		isDie = true;
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_STATE_KICK:
		isKick = true;
		break;
	case MARIO_STATE_BULLET:
		break;
	case MARIO_STATE_INTO_PIPE:
		oldY = y;
		isIntoPipe = true;
		isOutPipe = false;
		break;
	case MARIO_STATE_OUT_PIPE:
		oldY = y;
		isIntoPipe = false;
		isOutPipe = true;
		break;
	}
}

void Player::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (level == MARIO_LEVEL_BIG)
	{
		left = x;
		top = y;
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
		if (isCrouch) {
			top = y + 12;
		}
	}
	else if (level == MARIO_LEVEL_SMALL)
	{
		left = x;
		top = y;
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
	else if (level == MARIO_LEVEL_RACCOON)
	{
		left = x;
		top = y;
		right = x + MARIO_RACCOON_BBOX_WIDTH;
		bottom = y + MARIO_RACCOON_BBOX_HEIGHT;
		if (isCrouch) {
			top = y + 12;
		}
	}
	else if (level == MARIO_LEVEL_FIRE)
	{
		left = x;
		top = y;
		right = x + MARIO_FIRE_BBOX_WIDTH;
		bottom = y + MARIO_FIRE_BBOX_HEIGHT;
		if (isCrouch) {
			top = y + 12;
		}
	}
}

void Player::ResetBIG()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	dGround = 0;
	SetSpeed(0, 0);
	nx = 1;
}

void Player::ResetRACCOON()
{
	SetState(MARIO_ANI_RACCOON_IDLE);
	SetLevel(MARIO_LEVEL_RACCOON);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	dGround = 0;
	nx = 1;
}

void Player::ResetFIRE()
{
	SetState(MARIO_ANI_FIRE_IDLE);
	SetLevel(MARIO_LEVEL_FIRE);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
	dGround = 0;
	nx = 1;
}

