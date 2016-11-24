#ifndef SCENE_ASTEROID_H
#define SCENE_ASTEROID_H

#include "GameObject.h"
#include <vector>
#include "SceneBase.h"

enum class states
{
	menu,
	wave1,
	wave2,
	wave3,
};

class SceneAsteroid : public SceneBase
{
	static const int MAX_SPEED = 18;
	static const int MAX_SPEED2 = 150;
	static const int BULLET_SPEED = 50;
	static const int MISSILE_SPEED = 20;
	static const int MISSILE_POWER = 1;
	static const int SHIELD_POS = 0;
	float rotate = 0;

	states gameStates;

public:
	SceneAsteroid();
	~SceneAsteroid();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderGO(GameObject *go);

	void calculate(float angle);

	GameObject* FetchGO();
	bool useFire = false;

	float spawn;


protected:

	//Physics
	std::vector<GameObject *> m_goList;
	float m_speed;
	float m_angle;
	float m_worldWidth;
	float m_worldHeight;
	GameObject *m_ship;
	GameObject *m_asteriods;
	Vector3 m_force;
	int m_objectCount;
	int m_lives;
	int m_score;
	static const int fireRate = 5;
	float fireDebounceTimer;
	float firePOWERTimer;
	float shieldPOWERTimer;
	float blackholePOWERTimer;

	
};

#endif