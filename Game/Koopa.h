#pragma once

#include "Entity.h"
#include "Player.h"
#include "Point.h"
#include "Goomba.h"


#define KOOPAS_WALKING_SPEED 0.03f;

#define KOOPAS_BBOX_WIDTH 17
#define KOOPAS_BBOX_HEIGHT 27
#define KOOPAS_BBOX_HEIGHT_DIE 28

#define KOOPA_RED 1
#define KOOPA_GREEN 2

#define KOOPA_RED_STATE_WALKING_LEFT	 0
#define KOOPA_RED_STATE_WALKING_RIGHT	 1
#define KOOPA_RED_STATE_DIE				 2
#define KOOPA_RED_STATE_DIE_AND_MOVE	 3
#define KOOPA_RED_STATE_DIE_UP			 4
#define KOOPA_RED_STATE_REVIVE_UP		 5
#define KOOPA_RED_STATE_REVIVE			 6 
#define KOOPA_RED_STATE_DIE_AND_MOVE_UP	 7
#define KOOPA_RED_STATE_HOLDING		 	 8
#define KOOPA_RED_STATE_HOLDING_UP       9

#define KOOPA_GREEN_STATE_WALKING_LEFT	 10
#define KOOPA_GREEN_STATE_WALKING_RIGHT  11
#define KOOPA_GREEN_STATE_DIE			 12
#define KOOPA_GREEN_STATE_DIE_AND_MOVE	 13
#define KOOPA_GREEN_STATE_REVIVE	     14 
#define KOOPA_GREEN_STATE_DIE_UP		 15
#define KOOPA_GREEN_STATE_DIE_AND_MOVE_UP 16
#define KOOPA_GREEN_STATE_REVIVE_UP		 17
#define KOOPA_GREEN_STATE_HOLDING		 18
#define KOOPA_GREEN_STATE_HOLDING_UP	 19
#define KOOPA_GREEN_STATE_HAS_WING_FLY_LEFT	20
#define KOOPA_GREEN_STATE_HAS_WING_FLY_RIGHT 21
//#define KOOPA_GREEN_STATE_DIE			 22

class Koopa : public Entity
{

public:
	float bLeft, bRight;
	bool isDeath;
	bool isDoneDeath;
	int ani;
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();
	void CheckCollisionWithBrick(vector<LPGAMEENTITY>* coObjects);
	vector <LPGAMEENTITY> listEffect;
	DWORD timeToRevive;
	DWORD timeToFly;
	bool hitByTail;
	bool hasWing = true;
	bool hitByWeapon;
	int startx, widthtogo;
	Koopa(Player* mario, int id_Koopa);
	vector<LPGAMEENTITY> listLeaf;
	int last_state;
	Player* Mario;
	vector<LPGAMEENTITY> listEnemies;
	virtual void SetState(int state);
};