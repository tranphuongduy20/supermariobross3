#pragma once
#include "Entity.h"
#include "Player.h"
#include "Point.h"

#define KOOPAS_WALKING_SPEED 0.03f;

#define KOOPAS_BBOX_WIDTH 17
#define KOOPAS_BBOX_HEIGHT 27
#define KOOPAS_BBOX_HEIGHT_DIE 16


#define KOOPA_GREEN_STATE_WALKING_LEFT	 10
#define KOOPA_GREEN_STATE_WALKING_RIGHT  11
#define KOOPA_GREEN_STATE_DIE			 12
#define KOOPA_GREEN_STATE_DIE_AND_MOVE	 13
#define KOOPA_GREEN_STATE_REVIVE	     14 
#define KOOPA_GREEN_STATE_DIE_UP		 15


class KoopaFly : public Entity
{

public:
	bool directionY;
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
	KoopaFly(Player* mario);
	vector<LPGAMEENTITY> listLeaf;
	int last_state;
	Player* Mario;
	vector<LPGAMEENTITY> listEnemies;
	virtual void SetState(int state);
};


