#pragma once
#include "Entity.h"
#include "Enemy.h"
#include "Player.h"
#include "BulletKoopa.h"
#define KOOPABULLET_WALKING_SPEED 0.03f;

#define KOOPABULLET_BBOX_WIDTH 16
#define KOOPABULLET_BBOX_HEIGHT 24
#define KOOPABULLET_BBOX_HEIGHT_DIE 9

#define KOOPABULLET_STATE_WALKING	0
#define KOOPABULLET_STATE_DIE		1
#define KOOPABULLET_STATE_DIE_UP	2


class KoopaBullet : public Entity
{
public:
	DWORD countAttack = 0;
	BulletKoopa* bullet;
	Player* Mario;
	bool isDeath;
	bool isDoneDeath;
	int ani;
	int oldY;
	bool isActive = false;
	bool isAttack = false;
	bool isReadyAttack = false;
	DWORD countReady = 0;
	vector <LPGAMEENTITY> listEffect;
	float GetDistance(D3DXVECTOR2 pos, D3DXVECTOR2 target)
	{
		return (float)sqrt(pow(pos.x - target.x, 2) + pow(pos.y - target.y, 2));
	}
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects);
	virtual void Render();
	void Activation();
	void SetSpeed();
	void SetDirection();
	void Attack();
	DWORD timeRenderAniDie;
	KoopaBullet(Player* mario);
	virtual void SetState(int state);

};

