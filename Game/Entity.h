	#pragma once

#include <Windows.h>
#include <d3dx9.h>
#include <vector>

#include "Sprites.h"
#include "Animations.h"
#include "debug.h"

using namespace std;

#define ID_TEX_BBOX -100		// special texture to draw object bounding box
#define ID_TEX_HUD_BG -200

class Entity;
typedef Entity* LPGAMEENTITY;

struct CCollisionEvent;
typedef CCollisionEvent* LPCOLLISIONEVENT;
struct CCollisionEvent
{
	LPGAMEENTITY obj;
	float t, nx, ny;

	float dx, dy;		// *RELATIVE* movement distance between this object and obj

	CCollisionEvent(float t, float nx, float ny, float dx = 0, float dy = 0, LPGAMEENTITY obj = NULL)
	{
		this->t = t;
		this->nx = nx;
		this->ny = ny;
		this->dx = dx;
		this->dy = dy;
		this->obj = obj;
	}

	static bool compare(const LPCOLLISIONEVENT& a, LPCOLLISIONEVENT& b)
	{
		return a->t < b->t;
	}
};




class Entity
{
public:

	float x;
	float y;
	float start_x, start_y;
	//float vx, vy;
	//float x, y;
	float dx;	// dx = vx*dt
	float dy;	// dy = vy*dt
	float vx;
	float vy;
	int health;
	DetailType detailTag;
	bool holdthing;
	bool make100;
	bool make200;
	bool make400;
	bool make600;
	bool make800;
	bool make1000;
	bool make2000;
	bool makeup;

	bool isdone;
	int id_goomba;
	int id_koopa;
	int id_broken_state;
	int nx;
	int gunDam; // gun damage

	int direction;

	int state;

	int bbARGB;

	bool isDynamic, isTrigger, isActive;

	DWORD dt;
	EntityType tag;
	LPANIMATION_SET animationSet; 

public:
	bool isEnabled, isDead;
	bool CheckAABB(float l_a, float t_a, float r_a, float b_a, float l_b, float t_b, float r_b, float b_b)
	{
		return (l_a < r_b && r_a > l_b && t_a < b_b && b_a > t_b);
	}
	void SetPosition(float x, float y) { this->x = x, this->y = y; }
	void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
	void GetPosition(float& x, float& y) { x = this->x; y = this->y; }
	void GetSpeed(float& vx, float& vy) { vx = this->vx; vy = this->vy; }
	float Getx() { return x; }
	float Gety() { return y; }
	float Getdx() { return this->dx; }
	float Getdy() { return this->dy; }
	int GetState() { return this->state; }
	int GetHealth() { return health; }
	void SetHealth(int value) { health = value; }
	int GetgunDam() { return gunDam; }
	void SetgunDam(int value) { gunDam = value; }
	void AddHealth(int BonusHealth) { health += BonusHealth; }
	void AddgunDam(int BonusgunDam) { gunDam += BonusgunDam; }
	EntityType GetType() { return tag; }

	RECT GetBBox();
	void RenderBoundingBox();
	void SetBBARGB(int x) { bbARGB = x; }
	int GetBBARGB() { return bbARGB; }

	void SetAnimationSet(LPANIMATION_SET ani_set) { animationSet = ani_set; }

	LPCOLLISIONEVENT SweptAABBEx(LPGAMEENTITY coO);
	void CalcPotentialCollisions(vector<LPGAMEENTITY>* coObjects, vector<LPCOLLISIONEVENT>& coEvents);
	void FilterCollision(
		vector<LPCOLLISIONEVENT>& coEvents,
		vector<LPCOLLISIONEVENT>& coEventsResult,
		float& min_tx,
		float& min_ty,
		float& nx,
		float& ny,
		float& rdx,
		float& rdy);

	Entity();
	virtual bool IsCollidingObject(Entity* Obj);
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) {};
	virtual void Update(DWORD dt, vector<LPGAMEENTITY>* coObjects = NULL);
	virtual void Render() {};
	virtual void SetState(int state) { this->state = state; }

	/*bool IsActive() { return this->isActive; }
	void SetActive(bool active) { this->isActive = active; }

	bool IsTrigger() { return this->isTrigger; }
	void SetTrigger(bool trigger) { this->isTrigger = trigger; }

	virtual void OnOverlapped(GameObject obj) {};
	bool IsOverlapped(GameObject otherBox);*/



	~Entity();
};