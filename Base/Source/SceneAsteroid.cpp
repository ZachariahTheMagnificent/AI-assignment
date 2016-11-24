#include "SceneAsteroid.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneAsteroid::SceneAsteroid()
{
}

SceneAsteroid::~SceneAsteroid()
{
}

void SceneAsteroid::Init()
{
	SceneBase::Init();

	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	//Physics code here
	m_speed = 1.f;
	m_angle = 0.f;
	
	Math::InitRNG();

	//Exercise 2a: Construct 100 GameObject with type GO_ASTEROID and add into m_goList
	for (int i = 0; i < 100; ++i)
	{
		GameObject* go;
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
	}
	for (int i = 0; i < 100; ++i)
	{
		GameObject* go;
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID2));
	}
	//Exercise 2b: Initialize m_lives, m_score and timer
	m_lives = 3;
	m_score = 0;

	fireDebounceTimer = 0.f;
	firePOWERTimer = 0.f;
	shieldPOWERTimer = 0.f;
	blackholePOWERTimer = 0.f;
	//Exercise 2c: Construct m_ship, set active, type, scale and pos
	m_ship = new GameObject(GameObject::GO_SHIP);
	m_ship->active = true;
	m_ship->scale.Set(1,1,1);
	m_ship->vel.Set(0,0,0);
	m_ship->pos.Set(m_worldWidth / 2, m_worldHeight / 2);

	spawn = 0;
	waveTimer = 60;
	titleTimer = 5;

	gameStates = states::wave1;

}

GameObject* SceneAsteroid::FetchGO()
{
	//Exercise 3a: Fetch a game object from m_goList and return it
	for (std::vector<GameObject*>::iterator iter = m_goList.begin(); iter != m_goList.end(); ++iter)
	{
		GameObject *go = *iter;
		if (go->active == false)
		{
			go->active = true;
			return go;
		}
	}
	for (unsigned int i = 0; i < 10; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID));
	}
	for (unsigned int i = 0; i < 10; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_ASTEROID2));
	}
	GameObject *go = m_goList.back();
	go->active = true;
	return go;
}
void SceneAsteroid::calculate(float angle)
{
	m_ship->vel.x = (m_speed * cos(angle));
	m_ship->vel.y = (m_speed * sin(angle));
}
void SceneAsteroid::Update(double dt)
{
	SceneBase::Update(dt);

	rotate += 15;
	if (gameStates == states::menu)
	{

	}
	if (gameStates == states::wave1)
	{
		spawn += dt;
		if (spawn > dt*90)
		{
			GameObject *go = FetchGO();
			go->type = GameObject::GO_ASTEROID2;
			go->scale.Set(1, 1, 1);
			go->pos.Set(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), 0);
			go->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
			spawn = 0;
		}
	}
	if (gameStates == states::wave2)
	{
		spawn += dt;
		if (spawn > dt * 90)
		{
			GameObject *go = FetchGO();
			go->type = GameObject::GO_ASTEROID;
			go->scale.Set(1, 1, 1);
			go->pos.Set(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), 0);
			go->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);

			GameObject *go2 = FetchGO();
			go2->type = GameObject::GO_ASTEROID2;
			go2->scale.Set(1, 1, 1);
			go2->pos.Set(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), 0);
			go2->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);

			spawn = 0;
		}
	}
	if (gameStates == states::wave3)
	{
		spawn += dt;
		if (spawn > dt * 90)
		{
			GameObject *go = FetchGO();
			go->type = GameObject::GO_ASTEROID;
			go->scale.Set(1, 1, 1);
			go->pos.Set(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), 0);
			go->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);

			GameObject *go2 = FetchGO();
			go2->type = GameObject::GO_ASTEROID2;
			go2->scale.Set(1, 1, 1);
			go2->pos.Set(Math::RandFloatMinMax(0.f, m_worldWidth), Math::RandFloatMinMax(0, m_worldHeight), 0);
			go2->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);

			spawn = 0;
		}
	}

	if (waveTimer <= 0)
	{
		if (gameStates == states::wave1)
		{
			gameStates = states::wave2;
			waveTimer = 60;
			titleTimer = 5;
		}
		else if (gameStates == states::wave2)
		{
			gameStates = states::wave3;
			waveTimer = 60;
			titleTimer = 5;
		}
		if (gameStates == states::wave3)
		{
			gameStates = states::menu;
		}
	}

	if (Application::IsKeyPressed(VK_OEM_PLUS))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed(VK_OEM_MINUS))
	{
		m_speed += 0.1f;
	}
	m_force.SetZero();
	//Exercise 6: set m_force values based on WASD
	if (Application::IsKeyPressed('W'))
	{
		//m_force.Set(0, 100, 0);
		m_speed += 0.2f;
	}
	if (Application::IsKeyPressed('A'))
	{
		//m_force.Set(-100, 0, 0);
		m_angle += 0.1f;
	}
	if (Application::IsKeyPressed('S'))
	{
		//m_force.Set(0, -100, 0);
		m_speed -= 0.2f;
	}
	if (Application::IsKeyPressed('D'))
	{
		//m_force.Set(100, 0, 0);
		m_angle -= 0.1f;
	}

	calculate(m_angle);
	//Exercise 8: use 2 keys to increase and decrease mass of ship
	if (Application::IsKeyPressed('9'))
	{
		m_ship->mass = Math::Max(0.1f, m_ship->mass - 0.1f);
	}
	if (Application::IsKeyPressed('0'))
	{
		m_ship->mass += 0.1f;
	}
	
	if (Application::IsKeyPressed('Z'))
	{
		m_ship->pos;
		GameObject *go = FetchGO();
		go->type = GameObject::GO_BALL;
		go->scale.Set(1, 1, 1);
	}

	fireDebounceTimer += dt;
	firePOWERTimer += dt;
	shieldPOWERTimer += dt;
	blackholePOWERTimer += dt;
	//Exercise 14: use a key to spawn a bullet
	if (Application::IsKeyPressed(VK_SPACE) && fireDebounceTimer > 1.f / fireRate)
	{
		fireDebounceTimer = 0;
		GameObject *bullet = FetchGO();
		bullet->type = GameObject::GO_BULLET;
		bullet->scale.Set(0.2f, 0.2f, 0.2f);
		bullet->pos = m_ship->pos;
		if (m_ship->vel.IsZero())
			bullet->vel.Set(BULLET_SPEED, 0, 0);
		else
			bullet->vel = m_ship->vel.Normalized()*BULLET_SPEED;
	}
	if (Application::IsKeyPressed('N') && blackholePOWERTimer > 10.f / fireRate)
	{
		blackholePOWERTimer = 0;
		GameObject *bullet = FetchGO();
		bullet->type = GameObject::GO_BLACKHOLE;
		bullet->scale.Set(0.2f, 0.2f, 0.2f);
		bullet->pos = m_ship->pos;
		if (m_ship->vel.IsZero())
			bullet->vel.Set(BULLET_SPEED, 0, 0);
		else
			bullet->vel = m_ship->vel.Normalized()*BULLET_SPEED;
	}
	if (Application::IsKeyPressed('O') && shieldPOWERTimer > 5.f / fireRate)
	{
		shieldPOWERTimer = 0;
		GameObject *shield = FetchGO();
		shield->type = GameObject::GO_SHIELD;
		shield->scale.Set(1.2f, 1.2f, 1.2f);
	}
	if (Application::IsKeyPressed('I') && firePOWERTimer > 20.f / fireRate)
	{
		firePOWERTimer = 0;
		GameObject *fire = FetchGO();
		fire->type = GameObject::GO_FIRE;
		fire->scale.Set(1.2f, 1.2f, 1.2f);
	}

	if (Application::IsKeyPressed('M') && fireDebounceTimer > 5.f / fireRate)
	{
		fireDebounceTimer = 0;
		GameObject *bullet = FetchGO();
		bullet->type = GameObject::GO_MISSILE;
		bullet->scale.Set(0.5f, 0.5f, 0.5f);
		bullet->pos = m_ship->pos;

		if (m_ship->vel.IsZero())
			bullet->vel.Set(MISSILE_SPEED, 0, 0);
		else
			bullet->vel = m_ship->vel.Normalized()*MISSILE_SPEED;
	}
	//Exercise 15: limit the spawn rate of bullets

	//Mouse Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;
	}

	//Physics Simulation Section

	//Exercise 7: Update ship's velocity based on m_force
	// F = m * a
	Vector3 acceleration(0, 0, 0);
	if (m_ship->mass > Math::EPSILON)
		acceleration = m_force * (1.f / m_ship->mass);
	m_ship->vel += acceleration * dt;

	if (useFire == false)
	{
		if (m_ship->vel.LengthSquared() > (MAX_SPEED)*(MAX_SPEED))
		{
			m_ship->vel = m_ship->vel.Normalized()* MAX_SPEED;
		}
	}
	else
	{
		m_ship->vel = m_ship->vel.Normalized()* MAX_SPEED2;
	}
	m_ship->pos += m_ship->vel*dt;
	//Exercise 9: wrap ship position if it leaves screen
	if (m_ship->pos.x > m_worldWidth)
	{
		m_ship->pos.x -= m_worldWidth;
	}
	if (m_ship->pos.x < 0)
	{
		m_ship->pos.x += (m_worldWidth);
	}
	if (m_ship->pos.y > m_worldHeight)
	{
		m_ship->pos.y -= m_worldHeight;
	}
	if (m_ship->pos.y < 0)
	{
		m_ship->pos.y += m_worldHeight;
	}

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;

		if (go->active)
		{
			go->pos += go->vel * dt;
			//Exercise 12: handle collision between GO_SHIP and GO_ASTEROID using simple distance-based check
			if (go->type == GameObject::GO_ASTEROID)
			{
				float distanceSquared = (go->pos - m_ship->pos).LengthSquared();
				float combinedRadiusSquared = (4 * (go->scale.x + m_ship->scale.x)) * (4 * (go->scale.x + m_ship->scale.x));
				if (distanceSquared < combinedRadiusSquared)
				{
					go->active = false;
					m_lives--;
					if (m_lives == 0)
					{
						m_lives = 3;
						m_score = 0;
						//Reset game logic here
					}
					m_ship->pos.Set(m_worldWidth / 2, m_worldHeight / 2, 0);
					m_ship->vel.SetZero();
				}


				//Exercise 13: asteroids should wrap around the screen like the ship
				if (go->pos.x > m_worldWidth)
				{
					go->pos.x -= m_worldWidth;
				}
				if (go->pos.x < 0)
				{
					go->pos.x += (m_worldWidth);
				}
				if (go->pos.y > m_worldHeight)
				{
					go->pos.y -= m_worldHeight;
				}
				if (go->pos.y < 0)
				{
					go->pos.y += m_worldHeight;
				}
			}
			if (go->type == GameObject::GO_ASTEROID2)
			{
				float distanceSquared = (go->pos - m_ship->pos).LengthSquared();
				float combinedRadiusSquared = (4 * (go->scale.x + m_ship->scale.x)) * (4 * (go->scale.x + m_ship->scale.x));
				if (distanceSquared < combinedRadiusSquared)
				{
					go->active = false;
					m_lives--;
					if (m_lives == 0)
					{
						m_lives = 3;
						m_score = 0;
						//Reset game logic here
					}
					m_ship->pos.Set(m_worldWidth / 2, m_worldHeight / 2, 0);
					m_ship->vel.SetZero();
				}


				//Exercise 13: asteroids should wrap around the screen like the ship
				if (go->pos.x > m_worldWidth)
				{
					go->pos.x -= m_worldWidth;
				}
				if (go->pos.x < 0)
				{
					go->pos.x += (m_worldWidth);
				}
				if (go->pos.y > m_worldHeight)
				{
					go->pos.y -= m_worldHeight;
				}
				if (go->pos.y < 0)
				{
					go->pos.y += m_worldHeight;
				}
			}

			//Exercise 16: unspawn bullets when they leave screen
			if (go->type == GameObject::GO_BULLET)
			{
				//Exercise 18: collision check between GO_BULLET and GO_ASTEROID
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject *other = (GameObject *)*it2;
					if (other->type == GameObject::GO_ASTEROID)
					{
						if (other->active == true)
						{
							float distanceSquared = (other->pos - go->pos).LengthSquared();
							float combinedRadiusSquared = (3.5*(go->scale.x + other->scale.x))*(3.5*(go->scale.x + other->scale.x));
							if (distanceSquared < combinedRadiusSquared)
							{
								other->active = false;
								go->active = false;
								m_score += 2;
								int RNGspawn = Math::RandIntMinMax(2, 3);
								for (int i = 0; i < RNGspawn; ++i)
								{
									GameObject *go = FetchGO();
									go->type = GameObject::GO_ASTEROID2;
									go->scale.Set(1, 1, 1);
									go->pos.Set(other->pos.x, other->pos.y, other->pos.z);
									go->vel.Set(Math::RandFloatMinMax(-5, 5), Math::RandFloatMinMax(-5, 5), 0);
								}
								break;
							}
						}
					}

				}

				if (go->pos.y > m_worldHeight || go->pos.y < 0 || go->pos.x > m_worldWidth || go->pos.x < 0)
					go->active = false;

			}
			if (go->type == GameObject::GO_BULLET)
			{
				//Exercise 18: collision check between GO_BULLET and GO_ASTEROID
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject *other = (GameObject *)*it2;
					if (other->type == GameObject::GO_ASTEROID2)
					{
						if (other->active == true)
						{
							float distanceSquared = (other->pos - go->pos).LengthSquared();
							float combinedRadiusSquared = (3.5*(go->scale.x + other->scale.x))*(3.5*(go->scale.x + other->scale.x));
							if (distanceSquared < combinedRadiusSquared)
							{
								other->active = false;
								go->active = false;
								m_score += 1;
								break;
							}
						}
					}
				}
				if (go->pos.y > m_worldHeight || go->pos.y < 0 || go->pos.x > m_worldWidth || go->pos.x < 0)
					go->active = false;
			}

			else if (go->type == GameObject::GO_BLACKHOLE)
			{
				if (go->pos.y > m_worldHeight || go->pos.y < 0 || go->pos.x > m_worldWidth || go->pos.x < 0)
				{
					go->active = false;
				}

				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject *other = (GameObject *)*it2;
					if (other->type == GameObject::GO_ASTEROID)
					{
						if (other->active == true)
						{
							float distanceSquared = (other->pos - go->pos).LengthSquared();
							float combinedRadiusSquared = (3.5*(go->scale.x + other->scale.x))*(3.5*(go->scale.x + other->scale.x));
							//when it hits
							if (distanceSquared < combinedRadiusSquared)
							{
								other->active = false;
								go->active = false;
								m_score += 1;

								GameObject *blackhole = FetchGO();
								blackhole->type = GameObject::GO_BLACKHOLE2;
								blackhole->pos = go->pos;
								blackhole->vel.Set(0, 0, 0);
								blackhole->scale.Set(1, 1, 1);
								blackhole->active = true;

								break;
							}
						}
					}
					else if (other->type == GameObject::GO_ASTEROID2)
					{
						if (other->active == true)
						{
							float distanceSquared = (other->pos - go->pos).LengthSquared();
							float combinedRadiusSquared = (3.5*(go->scale.x + other->scale.x))*(3.5*(go->scale.x + other->scale.x));
							//when it hits
							if (distanceSquared < combinedRadiusSquared)
							{
								other->active = false;
								go->active = false;
								m_score += 1;

								GameObject *blackhole = FetchGO();
								blackhole->type = GameObject::GO_BLACKHOLE2;
								blackhole->pos = go->pos;
								blackhole->vel.Set(0, 0, 0);
								blackhole->scale.Set(1, 1, 1);
								blackhole->active = true;

								break;
							}
						}
					}
				}
			}

			else if (go->type == GameObject::GO_SHIELD)
			{
				go->pos = m_ship->pos;
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject *other = (GameObject *)*it2;
					if (other->type == GameObject::GO_ASTEROID)
					{
						if (other->active == true)
						{
							float distanceSquared = (other->pos - go->pos).LengthSquared();
							float combinedRadiusSquared = (5 * (go->scale.x + other->scale.x))*(5 * (go->scale.x + other->scale.x));
							//when it hits
							if (distanceSquared < combinedRadiusSquared)
							{
								other->active = false;
								go->active = false;
								m_score += 1;

								GameObject *shield = FetchGO();
								shield->type = GameObject::GO_SHIELD2;
								shield->pos = go->pos;
								shield->vel.Set(0, 0, 0);
								shield->scale.Set(1, 1, 1);
								shield->active = true;
								break;
							}
						}
					}
					if (other->type == GameObject::GO_ASTEROID2)
					{
						if (other->active == true)
						{
							float distanceSquared = (other->pos - go->pos).LengthSquared();
							float combinedRadiusSquared = (5 * (go->scale.x + other->scale.x))*(5 * (go->scale.x + other->scale.x));
							//when it hits
							if (distanceSquared < combinedRadiusSquared)
							{
								other->active = false;
								go->active = false;
								m_score += 1;

								GameObject *shield = FetchGO();
								shield->type = GameObject::GO_SHIELD2;
								shield->pos = go->pos;
								shield->vel.Set(0, 0, 0);
								shield->scale.Set(1, 1, 1);
								shield->active = true;
								break;
							}
						}
					}
				}
			}
			else if (go->type == GameObject::GO_FIRE)
			{
				m_speed = 150;
				go->pos = m_ship->pos;


				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject *other = (GameObject *)*it2;
					if (other->type == GameObject::GO_ASTEROID)
					{
						if (other->active == true)
						{
							float distanceSquared = (other->pos - go->pos).LengthSquared();
							float combinedRadiusSquared = (5 * (go->scale.x + other->scale.x))*(5 * (go->scale.x + other->scale.x));
							//when it hits
							if (distanceSquared < combinedRadiusSquared)
							{
								other->active = false;
								m_score += 1;

								GameObject *fire = FetchGO();
								fire->type = GameObject::GO_FIRE;
								fire->pos = go->pos;
								fire->vel.Set(0, 0, 0);
								fire->active = true;
								break;
							}
						}
					}
				}
			}

			else if (go->type == GameObject::GO_MISSILE)
			{
				float closestDistanceSquared = FLT_MAX;
				GameObject* closestAsteroid = nullptr;
				for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
				{
					GameObject* other = *it2;
					if (other->type == GameObject::GO_ASTEROID)
					{
						if (other->active == true)
						{
							float lengthSquared = (go->pos - other->pos).LengthSquared();
							if (lengthSquared < closestDistanceSquared)
							{
								closestDistanceSquared = lengthSquared;
								closestAsteroid = other;
							}

							float combinedRadiusSquared = (go->scale.x + other->scale.x)*(go->scale.x + other->scale.x);
							if (lengthSquared < combinedRadiusSquared)
							{
								go->active = false;
								other->active = false;
								m_score += 1;
								break;
							}
						}
					}

					if (closestAsteroid)
					{
						go->vel = (closestAsteroid->pos - go->pos).Normalize(); //add physics
						go->vel *= MISSILE_SPEED;
					}
					if (go->pos.y > m_worldHeight || go->pos.y < 0 || go->pos.x > m_worldWidth || go->pos.x < 0)
						go->active = false;

				}
				//After getting closest Asteroid
				if (closestAsteroid != nullptr)
				{
					/*float angleBetween = Math::RadianToDegree(atan2((go->pos.y - closestAsteroid->pos.y), (go->pos.x - closestAsteroid->pos.x)));
					Mtx44 rotation;
					rotation.SetToRotation(angleBetween, 1, 0, 0);
					Vector3  missleVel = go->vel.Normalize();
					missleVel = (rotation * missleVel) * MISSILE_SPEED;
					go->vel.Set(missleVel.x, missleVel.y, missleVel.z);
					go->vel = displacement * MISSILE_SPEED;*/

					go->vel = Vector3(closestAsteroid->pos - go->pos).Normalized() *MISSILE_SPEED;
				}

				//if (go->type == gameobject)
				//{
				//	//logic
				//}
			}

			if (go->type == GameObject::GO_BLACKHOLE2)
			{
				if (go->active)
				{
					go->time += dt;

					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject *other = (GameObject *)*it2;
						if (other->type == GameObject::GO_ASTEROID)
						{
							if (other->active == true)
							{
								float distanceSquared = (other->pos - go->pos).LengthSquared();
								float combinedRadiusSquared = (50 * (go->scale.x + other->scale.x))*(50 * (go->scale.x + other->scale.x));
								float combinedRadiusSquared2 = (10 * (go->scale.x + other->scale.x))*(10 * (go->scale.x + other->scale.x));
								float yDistance = go->pos.y - other->pos.y;
								float xDistance = go->pos.x - other->pos.x;
								float angleDiff = atan2(yDistance, xDistance);
								//when it hits
								if (distanceSquared < combinedRadiusSquared)
								{
									other->vel.x += (distanceSquared * cos(angleDiff)) / (distanceSquared);
									other->vel.y += (distanceSquared * sin(angleDiff)) / (distanceSquared);
								}
								if (distanceSquared < combinedRadiusSquared2)
								{
									other->active = false;
									m_score += 1;
									break;
								}
							}
						}
						else if (other->type == GameObject::GO_ASTEROID2)
						{
							if (other->active == true)
							{
								float distanceSquared = (other->pos - go->pos).LengthSquared();
								float combinedRadiusSquared = (50 * (go->scale.x + other->scale.x))*(50 * (go->scale.x + other->scale.x));
								float combinedRadiusSquared2 = (10 * (go->scale.x + other->scale.x))*(10 * (go->scale.x + other->scale.x));
								float yDistance = go->pos.y - other->pos.y;
								float xDistance = go->pos.x - other->pos.x;
								float angleDiff = atan2(yDistance, xDistance);
								//when it hits
								if (distanceSquared < combinedRadiusSquared)
								{
									other->vel.x += (distanceSquared * cos(angleDiff)) / (distanceSquared);
									other->vel.y += (distanceSquared * sin(angleDiff)) / (distanceSquared);
								}
								if (distanceSquared < combinedRadiusSquared2)
								{
									other->active = false;
									m_score += 1;
									break;
								}
							}
						}
					}
				}

				if (go->time >= 3)
				{
					go->time = 0;
					go->active = false;
					break;
				}
			}
			if (go->type == GameObject::GO_SHIELD2)
			{
				if (go->active)
				{
					go->time += dt;

					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject *other = (GameObject *)*it2;
						if (other->type == GameObject::GO_ASTEROID)
						{
							if (other->active == true)
							{
								float distanceSquared = (other->pos - go->pos).LengthSquared();
								float combinedRadiusSquared = (10 * (go->scale.x + other->scale.x))*(10 * (go->scale.x + other->scale.x));
								if (distanceSquared < combinedRadiusSquared)
								{
									other->active = false;
									m_score += 1;
									break;
								}
							}
						}
					}
				}

				if (go->time >= 3)
				{
					go->time = 0;
					go->active = false;
					break;
				}

			}
			if (go->type == GameObject::GO_FIRE)
			{
				if (go->active)
				{
					go->time += dt;
					useFire = true;

					for (std::vector<GameObject*>::iterator it2 = m_goList.begin(); it2 != m_goList.end(); ++it2)
					{
						GameObject *other = (GameObject *)*it2;
						if (other->type == GameObject::GO_ASTEROID)
						{
							if (other->active == true)
							{
								float distanceSquared = (other->pos - go->pos).LengthSquared();
								float combinedRadiusSquared = (5 * (go->scale.x + other->scale.x))*(5 * (go->scale.x + other->scale.x));
								if (distanceSquared < combinedRadiusSquared)
								{
									other->active = false;
									m_score += 1;
									break;
								}
							}
						}
					}
				}

				if (go->time >= 0.5)
				{
					go->time = 0;
					go->active = false;
					useFire = false;
					break;
				}

			}
		}
	}

}

void SceneAsteroid::RenderGO(GameObject *go)
{
	float angle;

	switch(go->type)
	{
	case GameObject::GO_SHIP:
		//Exercise 4a: render a sphere with radius 1
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		angle = Math::RadianToDegree(atan2(go->vel.y, go->vel.x));
		modelStack.Rotate(angle, 0, 0, 1);

		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		//Exercise 17a: render a ship texture or 3D ship model
		RenderMesh(meshList[GEO_SHIP], false);
		modelStack.PopMatrix();

		//Exercise 17b:	re-orientate the ship with velocity
		break;
	case GameObject::GO_ASTEROID:
		//Exercise 4b: render a cube with length 2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROID], false);
		modelStack.PopMatrix();

	case GameObject::GO_ASTEROID2:
		//Exercise 4b: render a cube with length 2
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_ASTEROID2], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BULLET:
		//Exercise 4b: render a bullet
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;
	case GameObject::GO_MISSILE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BLACKHOLE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(rotate, 0,0,1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BLACKHOLE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_BLACKHOLE2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(rotate, 0, 0, 1);
		modelStack.Scale(go->scale.x * 3.5, go->scale.y * 3.5, go->scale.z * 3.5);
		RenderMesh(meshList[GEO_BLACKHOLE], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_SHIELD:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_SHIELD], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_SHIELD2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(rotate, 0, 0, 1);
		modelStack.Scale(go->scale.x * 3.5, go->scale.y * 3.5, go->scale.z * 3.5);
		RenderMesh(meshList[GEO_SHIELD], false);
		modelStack.PopMatrix();
		break;

	case GameObject::GO_FIRE:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		//modelStack.Rotate(m_angle += 0.1f, 0, 0, 1);
		modelStack.Scale(go->scale.x *2, go->scale.y *2, go->scale.z *2);
		RenderMesh(meshList[GEO_FIRE], false);
		modelStack.PopMatrix();
		break;
	}

}

void SceneAsteroid::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Calculating aspect ratio
	m_worldHeight = 100.f;
	m_worldWidth = m_worldHeight * (float)Application::GetWindowWidth() / Application::GetWindowHeight();

	// Projection matrix : Orthographic Projection
	Mtx44 projection;
	projection.SetToOrtho(0, m_worldWidth, 0, m_worldHeight, -10, 10);
	projectionStack.LoadMatrix(projection);
	
	// Camera matrix
	viewStack.LoadIdentity();
	viewStack.LookAt(
						camera.position.x, camera.position.y, camera.position.z,
						camera.target.x, camera.target.y, camera.target.z,
						camera.up.x, camera.up.y, camera.up.z
					);
	// Model matrix : an identity matrix (model will be at the origin)
	modelStack.LoadIdentity();
	

	RenderMesh(meshList[GEO_AXES], false);

	RenderGO(m_ship);

	for(std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if(go->active)
		{
			RenderGO(go);
		}
	}
	modelStack.PushMatrix();
	modelStack.Translate(70, 50, -5);
	modelStack.Scale(24, 10, -10);
	RenderMesh(meshList[GEO_BACKGROUND], false);
	modelStack.PopMatrix();
	//On screen text

	//Exercise 5a: Render m_lives, m_score

	//Exercise 5b: Render position, velocity & mass of ship

	std::ostringstream ss2;
	ss2.precision(3);
	if (m_ship->vel.LengthSquared() >= (MAX_SPEED)*(MAX_SPEED))
	{
		ss2 << "Speed: " << MAX_SPEED;
	}
	else if (useFire == true)
	{
		ss2 << "Speed: BURST!!";
	}
	else
		ss2 << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 6);
	
	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);

	ss.str("");
	ss.precision(5);
	ss << "Lives: " << m_lives;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 9);

	ss.str("");
	ss.precision(5);
	ss << "Score: " << m_score;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 12);

	ss.str("");
	ss.precision(5);
	ss << "Position: " << m_ship->pos;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 15);

	if (firePOWERTimer > (firePOWERTimer > 20.f/ fireRate))
	{
		ss.str("");
		ss.precision(5);
		ss << "I = Burst";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 18);
	}
	else
	{
		ss.str("");
		ss.precision(5);
		ss << "Burst Recharging";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 18);
	}

	if (shieldPOWERTimer > (shieldPOWERTimer > 5.f / fireRate))
	{
		ss.str("");
		ss.precision(5);
		ss << "O = Shield";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 21);
	}
	else
	{
		ss.str("");
		ss.precision(5);
		ss << "Shield Recharging";
		RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 21);
	}

	if (gameStates == states::wave1)
	{
		if (titleTimer > 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "- WAVE 1 -", Color(0, 1, 0), 3, 28, 50);
		}
		else if (titleTimer <= 0)
		{
			std::ostringstream ss;
			ss.precision(5);
			ss << "TIME LEFT: " << waveTimer;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 18, 50);
		}
	}
	if (gameStates == states::wave2)
	{

		if (titleTimer > 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "- WAVE 2 -", Color(0, 1, 0), 3, 28, 50);
		}
		else if (titleTimer <= 0)
		{
			std::ostringstream ss;
			ss.precision(5);
			ss << "TIME LEFT: " << waveTimer;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 18, 50);
		}
	}
	if (gameStates == states::wave3)
	{

		if (titleTimer > 0)
		{
			RenderTextOnScreen(meshList[GEO_TEXT], "- WAVE 3 -", Color(0, 1, 0), 3, 28, 50);
		}
		else if (titleTimer <= 0)
		{
			std::ostringstream ss;
			ss.precision(5);
			ss << "TIME LEFT: " << waveTimer;
			RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 18, 50);
		}
	}

	RenderTextOnScreen(meshList[GEO_TEXT], "Asteroid", Color(0, 1, 0), 3, 0, 0);
}

void SceneAsteroid::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while(m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if(m_ship)
	{
		delete m_ship;
		m_ship = NULL;
	}
}
