#ifndef SCENE_COLLISION_H
#define SCENE_COLLISION_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

enum class states
{
	menu,
	level1,
	level2,
	level3,
	win,
	lose,
};
class SceneCollision : public SceneBase
{
	static const int MAX_SPEED = 10;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;

	
public:
	SceneCollision();
	~SceneCollision();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);
	virtual bool CheckCollision(GameObject* go1, GameObject *other, double dt);
	float CheckCollision2(GameObject* go1, GameObject *other);
	bool CheckCollision3(GameObject *go, GameObject *other, double dt);
	void CollisionResponse(GameObject *go, GameObject *other);
	void unloadLvl();
	float m_estimatedTime, m_timeTaken;
	bool m_timerStarted;
	GameObject* FetchGO();

	states gameStates;
	
protected:

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ghost;
	GameObject *m_ghost2;
	
	int m_objectCount;
	int m_lives;

	bool reset = 0;

	//Auditing
	float m1, m2;
	Vector3 u1, u2, v1, v2;
	bool triggerzone;
};

#endif