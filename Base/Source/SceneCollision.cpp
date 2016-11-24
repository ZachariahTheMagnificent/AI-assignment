#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include <sstream>

SceneCollision::SceneCollision()
{
}

SceneCollision::~SceneCollision()
{
}

void SceneCollision::Init()
{
	SceneBase::Init();

	//Physics code here
	m_speed = 1.f;

	Math::InitRNG();

	//Exercise 1: initialize m_objectCount
	m_objectCount = 0;
	m_lives = 3;
	gameStates = states::menu;

	m_ghost = new GameObject(GameObject::GO_BALL);
	m_ghost2 = new GameObject(GameObject::GO_BALL);

	triggerzone = false;
}
void SceneCollision::unloadLvl()
{
	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active == true)
		{
			if (go->type == GameObject::GO_BALL ||go->type == GameObject::GO_WALL ||go->type == GameObject::GO_WALL2 ||go->type == GameObject::GO_WALL3 ||go->type == GameObject::GO_START ||go->type == GameObject::GO_GOAL ||go->type == GameObject::GO_PILLAR)
			{

				go->active = false;
			}
		}
	}
}

GameObject* SceneCollision::FetchGO()
{
	//Exercise 2a: implement FetchGO()

	for (std::vector<GameObject*>::iterator iter = m_goList.begin(); iter != m_goList.end(); ++iter)
	{
		GameObject *go = *iter;
		if (go->active == false)
		{
			m_objectCount++;
			go->active = true;
			go->vel.SetZero();
			return go;
		}
	}
	for (unsigned int i = 0; i < 10; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_BALL));
	}
	GameObject *go = m_goList.back();
	go->active = true;
	go->vel.SetZero();

	//Exercise 2b: increase object count every time an object is set to active
	m_objectCount++;
	return go;
}

bool SceneCollision::CheckCollision(GameObject *go, GameObject *other, double dt)
{
	switch (other->type)
	{
	case GameObject::GO_BALL:
	{
		float distanceSquared = ((other->pos + other->vel * dt) - (go->pos + go->vel * dt)).LengthSquared();
		float combinedRadiusSquared = (go->scale.x + other->scale.x) * (go->scale.x + other->scale.x);
		Vector3 relativeVelocity = go->vel - other->vel;
		Vector3 relativeDisplacement = other->pos - go->pos;
		return (distanceSquared < combinedRadiusSquared && relativeVelocity.Dot(relativeDisplacement) > 0);
		break;
	}
	case GameObject::GO_WALL:
	{
		Vector3 w0 = other->pos;
		Vector3 b1 = go->pos + go->vel * dt;
		Vector3 N = other->normal;
		//Vector3 NP = N.Cross(Vector3(0, 0, 1));
		Vector3 NP(-N.y, N.x);
		float r = go->scale.x;
		float h = other->scale.x;
		float l = other->scale.y;

		Vector3 relativeVelocity = go->vel - other->vel;
		Vector3 relativeDisplacement = w0 - b1;

		if (relativeDisplacement.Dot(N) < 0)
			N = -N;

		if (relativeVelocity.Dot(N) > 0)
			return ((abs((w0 - b1).Dot(N)) < (r + h / 2)) && ((abs(relativeDisplacement.Dot(NP)) < (l / 2))));
		break;
	}
	case GameObject::GO_WALL2:
	{
		//Vector3 w0 = other->pos;
		//Vector3 b1 = go->pos + go->vel * dt;
		//Vector3 N = other->normal;
		//Vector3 NP(-N.y, N.x);
		//float r = go->scale.x;
		//float h = other->scale.x;
		//float l = other->scale.y;

		//Vector3 relativeVelocity = go->vel - other->vel;
		//Vector3 relativeDisplacement = w0 - b1;

		//if (relativeDisplacement.Dot(N) < 0)
		//	N = -N;

		//if (relativeVelocity.Dot(N) > 0)
		//	return ((abs((w0 - b1).Dot(N)) < (r + h / 2)) && ((abs(relativeDisplacement.Dot(NP)) < (l / 2))));
		Vector3 w0 = other->pos;
		Vector3 b1 = go->pos + go->vel * dt;
		Vector3 N = other->normal;
		Vector3 relativeDisplacement = w0 - b1;
		if (relativeDisplacement.Dot(N) < 0)
			N = -N;
		//Vector3 NP = N.Cross(Vector3(0, 0, 1));
		Vector3 NP(-N.y, N.x);
		float r = go->scale.x;
		float h = other->scale.x;
		float l = other->scale.y;

		Vector3 relativeVelocity = go->vel - other->vel;
		if (relativeVelocity.Dot(N) > 0)
			return ((abs((w0 - b1).Dot(N)) < (r + h / 2)) && ((abs(relativeDisplacement.Dot(NP)) < (l / 2))));
		break;
	}
	case GameObject::GO_WALL3:
	{
		Vector3 w0 = other->pos;
		Vector3 b1 = go->pos + go->vel * dt;
		Vector3 N = other->normal;
		//Vector3 NP = N.Cross(Vector3(0, 0, 1));
		Vector3 NP(-N.y, N.x);
		float r = go->scale.x;
		float h = other->scale.x;
		float l = other->scale.y;

		Vector3 relativeVelocity = go->vel - other->vel;
		Vector3 relativeDisplacement = w0 - b1;

		if (relativeDisplacement.Dot(N) < 0)
			N = -N;

		if (relativeVelocity.Dot(N) > 0)
			return ((abs((w0 - b1).Dot(N)) < (r + h / 2)) && ((abs(relativeDisplacement.Dot(NP)) < (l / 2))));
		break;
	}
	case GameObject::GO_GOAL:
	{
		Vector3 p1 = go->pos;
		Vector3 p2 = other->pos;
		float r1 = go->scale.x;
		float r2 = other->scale.x;
		float combinedRadius = r1 + r2;
		Vector3 u = go->vel;
		return (p2 - p1).LengthSquared() < combinedRadius * combinedRadius && (p2 - p1).Dot(u) > 0;

		break;
	}
	case GameObject::GO_PILLAR:
	{
 		Vector3 p1 = go->pos;
		Vector3 p2 = other->pos;
		float r1 = go->scale.x;
		float r2 = other->scale.x;
		float combinedRadius = r1 + r2;
		Vector3 u = go->vel;
		return (p2 - p1).LengthSquared() < combinedRadius * combinedRadius && (p2 - p1).Dot(u) > 0;

		break;
	}
	}
	return false;
	/*float distanceSquared = ((other->pos + other->vel * dt) - (go->pos + go->vel * dt)).LengthSquared();
	float combinedRadiusSquared = (go->scale.x + other->scale.x) * (go->scale.x + other->scale.x);
	Vector3 relativeVelocity = go->vel - other->vel;
	Vector3 relativeDisplacement = other->pos - go->pos;
	return (distanceSquared < combinedRadiusSquared && relativeVelocity.Dot(relativeDisplacement) > 0);*/
}

float SceneCollision::CheckCollision2(GameObject *go1, GameObject *go2)
{
	switch (go2->type)
	{
		case GameObject::GO_BALL:
		{
			Vector3 rel = go1->vel - go2->vel;
			Vector3 dir = go1->pos = go2->pos;
			float r = go1->scale.x + go2->scale.x;
			float dot = rel.Dot(dir);
			if (dot > 0)
				return -1;
			float a = rel.LengthSquared();
			float b = 2 * rel.Dot(dir);
			float c = dir.LengthSquared() - r * r;
			float d = b * b - 4 * a * c;
			if (d < 0)
				return -1;
			float t = (-b - sqrt(d)) / (2 * a);
			if (t < 0)
				float t = (-b + sqrt(d)) / (2 * a);
			return t;
		}
		break;
	case GameObject::GO_WALL:
	{
			Vector3 w0 = go2->pos;
			Vector3 b1 = go1->pos;
			Vector3 N = go2->normal;

			float r = go1->scale.x;
			float h = go2->scale.x;
			float l = go2->scale.y;

			Vector3 U = go1->vel;
			Vector3 rd = w0 - b1;

			if (rd.Dot(N) < 0)
				N = -N;

			if (U.Dot(N) < 0)
				return -1;

			w0 += -(r + h * 0.5f) * N;

			float distance = (w0 + b1).Dot(N);
			float speed = U.Dot(N);
			float th = distance / speed;

			Vector3 NP(-N.y, N.x);
			Vector3 w1 = w0 + (1 * 0.5) * NP;
			Vector3 w2 = w0 - (1 * 0.5f) * NP;

			Vector3 bh = b1 + U * th;
			if ((w1-bh).Dot(w2-bh) > 0)
			{
				return -1;
			}
			return th;
	}
	break;
	}
}

bool SceneCollision::CheckCollision3(GameObject *go, GameObject *other, double dt)
{
	switch (other->type)
	{
	case GameObject::GO_BALL:
	{
								float distanceSquared = ((go->pos + go->vel * dt) - (other->pos - other->vel * dt)).LengthSquared();
								float combinedRadiusSquared = (go->scale.x + other->scale.x) * (go->scale.x + other->scale.x);
								Vector3 relativeVelocity = go->vel - other->vel;
								Vector3 relativeDisplacement = other->pos - go->pos;
								return (distanceSquared < combinedRadiusSquared && relativeVelocity.Dot(relativeDisplacement) > 0);
								break;
	}
	case GameObject::GO_WALL:
	{
								Vector3 w0 = other->pos;
								Vector3 b1 = go->pos;
								Vector3 N = other->normal;
								//Vector3 NP = N.Cross(Vector3(0, 0, 1));
								Vector3 NP(-N.y, N.x);
								float r = go->scale.x;
								float h = other->scale.x;
								float l = other->scale.y;

								return ((abs((w0 - b1).Dot(N)) < (r + h / 2)) && ((abs((w0 - b1).Dot(NP)) < (r + l / 2))));
								break;
	}
	case GameObject::GO_WALL2:
	{
		Vector3 w0 = other->pos;
		Vector3 b1 = go->pos;
		Vector3 N = other->normal;
		//Vector3 NP = N.Cross(Vector3(0, 0, 1));
		Vector3 NP(-N.y, N.x);
		float r = go->scale.x;
		float h = other->scale.x;
		float l = other->scale.y;

		return ((abs((w0 - b1).Dot(N)) < (r + h / 2)) && ((abs((w0 - b1).Dot(NP)) < (r + l / 2))));
		break;
	}
	case GameObject::GO_WALL3:
	{
		Vector3 w0 = other->pos;
		Vector3 b1 = go->pos;
		Vector3 N = other->normal;
		//Vector3 NP = N.Cross(Vector3(0, 0, 1));
		Vector3 NP(-N.y, N.x);
		float r = go->scale.x;
		float h = other->scale.x;
		float l = other->scale.y;
		return ((abs((w0 - b1).Dot(N)) < (r + h / 2)) && ((abs((w0 - b1).Dot(NP)) < (r + l / 2))));

		break;
	}
	case GameObject::GO_GOAL:
	{
		Vector3 w0 = other->pos;
		Vector3 b1 = go->pos;
		Vector3 N = other->normal;
		//Vector3 NP = N.Cross(Vector3(0, 0, 1));
		Vector3 NP(-N.y, N.x);
		float r = go->scale.x;
		float h = other->scale.x;
		float l = other->scale.y;

		return ((abs((w0 - b1).Dot(N)) < (r + h / 2)) && ((abs((w0 - b1).Dot(NP)) < (r + l / 2))));
		break;
	}
	case GameObject::GO_PILLAR:
	{
		float distanceSquared = ((go->pos + go->vel * dt) - (other->pos - other->vel * dt)).LengthSquared();
		float combinedRadiusSquared = (go->scale.x + other->scale.x) * (go->scale.x + other->scale.x);
		Vector3 relativeVelocity = go->vel - other->vel;
		Vector3 relativeDisplacement = other->pos - go->pos;
		return (distanceSquared < combinedRadiusSquared && relativeVelocity.Dot(relativeDisplacement) > 0);
		break;
	}
	}
	return false;
}

void SceneCollision::CollisionResponse(GameObject *go, GameObject *other)
{
	switch (other->type)
	{
	case GameObject::GO_BALL:
	{
		m1 = go->mass;
		m2 = other->mass;
		u1 = go->vel;
		u2 = other->vel;

		//Vector3 momentum1 = m1 * u1;
		//Vector3 momentum2 = m2 * u2;
		//
		//go->vel = momentum2 * (1.0f / go->mass);
		//other->vel = momentum1 * (1.0f / other->mass);

		//go->vel = (u1 * (m1 - m2) + 2 * m2 * u2) * (1.0f / (m1 + m2));
		//other->vel = (u2 * (m2 - m1) + 2 * m1 * u1) * (1.0f / (m1 + m2));

		Vector3 u1N, u2N, N;
		N = (go->pos - other->pos).Normalized();

		u1N = u1.Dot(N)  * N;
		u2N = u2.Dot(N)  * N;

		go->vel = u1 + (2 * m2) / (m1 + m2) * (u2N - u1N);
		other->vel = u2 + (2 * m1) / (m1 + m2) * (u1N - u2N);

		v1 = go->vel;
		v2 = other->vel;
		break;
	}
	
	case GameObject::GO_WALL:
	{
								Vector3 u = go->vel;
								Vector3 N = other->normal;
								Vector3 uN = u.Dot(N) * N;
								go->vel = u - 2 * uN;
								break;
	}
	case GameObject::GO_WALL2:
	{
		Vector3 u = go->vel;
		Vector3 N = other->normal;
		Vector3 uN = u.Dot(N) * N;
		go->vel = u - 2 * uN;
		break;
	}
	case GameObject::GO_WALL3:
	{
		Vector3 u = go->vel;
		Vector3 N = other->normal;
		Vector3 uN = u.Dot(N) * N;
		go->vel = u - 2 * uN;
		m_lives--;
		go->active = false;
		break;
	}
	case GameObject::GO_GOAL:
	{
		//Vector3 u = go->vel;
		//Vector3 N = (other->pos - go->pos).Normalized();
		//go->vel = u - 2 * u.Dot(N) * N;
		go->vel = other->pos - go->pos;
		go->scale *= 0.95f;

		if (go->scale.x <= 0.1f)
		{
			go->active = false;
			if (gameStates == states::level1)
			{
				gameStates = states::level2;
				reset = 0;
			}
			else if (gameStates == states::level2)
			{
				gameStates = states::level3;
				reset = 0;
			}
			else if (gameStates == states::level3)
			{
				gameStates = states::win;
				reset = 0;
			}
		}

		break;
	}
	case GameObject::GO_PILLAR:
	{
								  Vector3 u = go->vel;
								  Vector3 N = (other->pos - go->pos).Normalized();
								  go->vel = u - 2 * u.Dot(N) * N;
	}
		break;
	}

}

void SceneCollision::Update(double dt)
{
	SceneBase::Update(dt);

	Vector3 baguaCentre(50, 50, 0);
	Vector3 baguaRadiusOffset(35, 0, 0);
	Mtx44 baguaRotation;

	if (Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}
	if (Application::IsKeyPressed('R'))
	{
		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (go->active && go->type == GameObject::GO_BALL)
			{
				go->active - false;
				--m_objectCount;
			}
		}
	}

	static bool SpaceButtonState = false;
	if (!SpaceButtonState && Application::IsKeyPressed(VK_SPACE))
	{
		SpaceButtonState = true;
	}
	//else if (SpaceButtonState && !Application::IsKeyPressed(VK_SPACE))
	//{
	//	SpaceButtonState = false;
	//	int w = Application::GetWindowWidth();
	//	float m_worldSizeX = m_worldWidth / w;

	//	GameObject* ball = FetchGO();
	//	ball->type = GameObject::GO_BALL;
	//	ball->scale.Set(5, 5, 5);
	//	ball->pos = (m_worldSizeX, 0.9f, 10);
	//	ball->vel = Vector3(0, 10, 0);
	//	ball->mass = 10.f;
	//}
	if (gameStates == states::menu && reset == 0)
	{
		unloadLvl();
		if (Application::IsKeyPressed(VK_RETURN))
		{
			gameStates = states::level1;
		}
	}
	if (gameStates == states::level1 && reset == 0)
	{
		unloadLvl();
		reset = 1;
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float worldX = x * m_worldWidth / w;
		float worldY = (h - y) * m_worldHeight / h;

		GameObject* start = FetchGO();
		start->type = GameObject::GO_START;
		start->pos = Vector3(6, 91, 0);
		start->scale.Set(10, 10, 1);

		GameObject* wall = FetchGO();
		//wall->pos = baguaCentre;
		//wall->normal = Vector3(0, 1, 0);
		//wall->type = GameObject::GO_WALL;
		//wall->scale.Set(4, 36, 1);

		wall = FetchGO();
		wall->type = GameObject::GO_WALL;
		wall->pos = Vector3(67, 100, 0);
		wall->normal = Vector3(0, 1, 0);
		wall->scale.Set(4, 134, 1);

		wall = FetchGO();
		wall->type = GameObject::GO_WALL;
		wall->pos = Vector3(67, 0, 0);
		wall->normal = Vector3(0, 1, 0);	
		wall->scale.Set(4, 134, 1);
		//for (int i = 0; i < 8; ++i)
		//{
		//	GameObject* wall = FetchGO();
		//	baguaRotation.SetToRotation(i * 45.0f, 0, 0, 1);
		//	wall->pos = (baguaRotation *  baguaRadiusOffset) + baguaCentre;
		//	wall->normal = baguaRotation * Vector3(1, 0, 0);
		//	wall->type = GameObject::GO_WALL;
		//	//wall->pos = (m_worldWidth / 2, m_worldHeight / 2, 0);
		//	wall->active = true;
		//	wall->scale.Set(4, 36, 1);

		//}
		GameObject* wall2 = FetchGO();
		wall2->type = GameObject::GO_WALL3;
		wall2->pos = Vector3(88, 50, 0);
		wall2->normal = Vector3(1, 0, 0);
		wall2->scale.Set(4, 36, 1);

		wall2 = FetchGO();
		wall2->type = GameObject::GO_WALL3;
		wall2->pos = Vector3(30, 70, 0);
		wall2->normal = Vector3(1, 0, 0);
		wall2->scale.Set(4, 60, 1);

		wall2 = FetchGO();
		wall2->type = GameObject::GO_WALL3;
		wall2->pos = Vector3(60, 50, 0);
		wall2->normal = Vector3(1, 0, 0);
		wall2->scale.Set(4, 60, 1);

		wall2 = FetchGO();
		wall2->type = GameObject::GO_WALL3;
		wall2->pos = Vector3(120, 70, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->scale.Set(4, 60, 1);

		wall2 = FetchGO();
		wall2->type = GameObject::GO_WALL3;
		wall2->pos = Vector3(120, 30, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->scale.Set(4, 60, 1);

		GameObject* pillar = FetchGO();
		pillar->type = GameObject::GO_PILLAR;
		pillar->pos = Vector3(20, 20, 0);
		pillar->scale.Set(2, 2, 1);

		pillar = FetchGO();
		pillar->type = GameObject::GO_PILLAR;
		pillar->pos = Vector3(150, 50, 0);
		pillar->scale.Set(2, 2, 1);

		GameObject* goal = FetchGO();
		goal->type = GameObject::GO_GOAL;
		goal->pos = Vector3(100, 50, -1);
		goal->scale.Set(10, 10, 1);
		//goal->vel.Set(0, 0, 0);
	}

	if (gameStates == states::level2 && reset == 0)
	{
		unloadLvl();
		reset = 1;
		
		GameObject* start = FetchGO();
		start->type = GameObject::GO_START;
		start->pos = Vector3(10, 50, 0);
		start->vel.Set(0, 0, 0);
		start->scale.Set(10, 10, 1);

		GameObject* goal = FetchGO();
		goal->type = GameObject::GO_GOAL;
		goal->pos = Vector3(98, 50, -1);
		goal->scale.Set(10, 10, 1);

		GameObject* wall = FetchGO();
		wall = FetchGO();
		wall->pos = Vector3(20, 39, 0);
		wall->normal = Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 45, 1);

		wall = FetchGO();
		wall->pos = Vector3(12, 60, 0);
		wall->normal = Vector3(0, 1, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 20, 1);

		wall = FetchGO();
		wall->pos = Vector3(51, 44, 0);
		wall->normal = Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 80, 1);

		wall = FetchGO();
		wall->pos = Vector3(112, 54, 0);
		wall->normal = Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 55, 1);

		wall = FetchGO();
		wall->pos = Vector3(130, 30, 0);
		wall->normal = Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 75, 1);

		wall = FetchGO();
		baguaRotation.SetToRotation(45.0f, 0, 0, 1);
		wall->pos = Vector3(140, 72, 0);
		wall->normal = baguaRotation * Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 30, 1);

		wall = FetchGO();
		wall->pos = Vector3(91, 82, 0);
		wall->normal = Vector3(0, 1, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 80, 1);

		wall = FetchGO();
		wall->pos = Vector3(99, 38, 0);
		wall->normal = Vector3(0, 1, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 28, 1);

		GameObject* wall2 = FetchGO();
		wall2->pos = Vector3(35, 2, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 36, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(35, 82, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 36, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(85, 12, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 36, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(35, 42, 0);
		wall2->normal = Vector3(1, 0, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 36, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(83, 51, 0);
		wall2->normal = Vector3(1, 0, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 30, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(156, 51, 0);
		wall2->normal = Vector3(1, 0, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 30, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(176,31, 0);
		wall2->normal = Vector3(1, 0, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 30, 1);

		GameObject* pillar = FetchGO();
		pillar->type = GameObject::GO_PILLAR;
		pillar->pos = Vector3(10, 71, 0);
		pillar->scale.Set(2, 2, 1);
	}

	if (gameStates == states::level3 && reset == 0)
	{
		unloadLvl();
		reset = 1;
		GameObject* start = FetchGO();
		start->type = GameObject::GO_START;
		start->pos = Vector3(5, 5, 0);
		start->vel.Set(0, 0, 0);
		start->scale.Set(10, 10, 1);

		GameObject* goal = FetchGO();
		goal->type = GameObject::GO_GOAL;
		goal->pos = Vector3(55, 90, -1);
		goal->scale.Set(8, 8, 1);

		GameObject* wall = FetchGO();
		wall = FetchGO();
		wall->pos = Vector3(12, 10, 0);
		wall->normal = Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 20, 1);

		wall = FetchGO();
		wall->pos = Vector3(30, 68, 0);
		wall->normal = Vector3(0, 1, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 40, 1);

		wall = FetchGO();
		wall->pos = Vector3(71, 80, 0);
		wall->normal = Vector3(0, 1, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 60, 1);

		wall = FetchGO();
		wall->pos = Vector3(120, 50, 0);
		wall->normal = Vector3(0, 1, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 40, 1);

		wall = FetchGO();
		wall->pos = Vector3(50, 30, 0);
		wall->normal = Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 80, 1);

		wall = FetchGO();
		wall->pos = Vector3(100, 50, 0);
		wall->normal = Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		wall->scale.Set(4, 80, 1);


		GameObject* wall2 = FetchGO();
		wall2->pos = Vector3(15, 32, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 36, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(24, 50, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(18, 22, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(75, 10, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(18, 22, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(75, 50, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(18, 22, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(58, 50, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 15, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(24, 74, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(30, 5, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(44, 90, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 5, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(120, 10, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(150, 10, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(150, 40, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(150, 70, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(150, 100, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(120, 40, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(120, 70, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(120, 100, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(135, 25, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(135, 55, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		wall2->pos = Vector3(135, 85, 0);
		wall2->normal = Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(20, 3, 1);

		wall2 = FetchGO();
		baguaRotation.SetToRotation(50.0f, 0, 0, 1);
		wall2->pos = Vector3(34, 21, 0);
		wall2->normal = baguaRotation * Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 20, 1);


		wall2 = FetchGO();
		baguaRotation.SetToRotation(-50.0f, 0, 0, 1);
		wall2->pos = Vector3(24, 21, 0);
		wall2->normal = baguaRotation * Vector3(0, 1, 0);
		wall2->type = GameObject::GO_WALL3;
		wall2->scale.Set(4, 20, 1);
	}

	if (gameStates == states::lose)
	{
		unloadLvl();
		if (Application::IsKeyPressed(VK_RETURN))
		{
			gameStates = states::menu;
			m_lives = 3;
			reset = 0;
		}
	}
	if (gameStates == states::win)
	{
		unloadLvl();
		if (Application::IsKeyPressed(VK_RETURN))
		{
			gameStates = states::menu;
			m_lives = 3;
			reset = 0;
		}
	}
	//Mouse Section
	static bool bLButtonState = false;
	if (!bLButtonState && Application::IsMousePressed(0))
	{
		bLButtonState = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float worldX = x * m_worldWidth / w;
		float worldY = (h - y) * m_worldHeight / h;
		m_ghost->pos.Set(worldX, worldY, 0);
		m_ghost->active = true;
		m_ghost2->active = true;
		
	}
	else if (bLButtonState && !Application::IsMousePressed(0))
	{
		bLButtonState = false;
		std::cout << "LBUTTON UP" << std::endl;

		//Exercise 6: spawn small GO_BALL
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float worldX = x * m_worldWidth / w;
		float worldY = (h - y) * m_worldHeight / h;
		m_ghost->active = false;

		GameObject* wall = FetchGO();
		wall->type = GameObject::GO_WALL2;
		wall->vel = Vector3(0, 0, 0);
		wall->pos = (m_ghost->pos + m_ghost2->pos) * 0.5;
		wall->normal = (m_ghost->pos - m_ghost2->pos).Cross(Vector3(0, 0, 1)).Normalize();
		wall->scale = Vector3(2, (m_ghost->pos - m_ghost2->pos).Length() * 1, 1);
		wall->time = 0;

		//ball->type = GameObject::GO_BALL;
		//Vector3 size = Vector3(worldX, worldY, 0) - m_ghost->pos;
		//float radius = Math::Clamp(size.Length(), 2.f, 10.f);
		//ball->scale.Set(radius, radius, 1.0f);
		//ball->pos = m_ghost->pos;
		//ball->vel = m_ghost->pos - Vector3(worldX, worldY, 0);
		//ball->mass = radius;
		m_ghost->active = false;
		m_ghost2->active = false;

		m_estimatedTime = -1;
	}
	static bool bRButtonState = false;
	if (!bRButtonState && Application::IsMousePressed(1))
	{
		bRButtonState = true;
		std::cout << "RBUTTON DOWN" << std::endl;
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float worldX = x * m_worldWidth / w;
		float worldY = (h - y) * m_worldHeight / h;
		m_ghost->pos.Set(worldX, worldY, 0);

		for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		{
			GameObject *go = (GameObject *)*it;
			if (go->type == GameObject::GO_START)
			{
				if (worldX > go->pos.x - go->scale.x / 2)
				{
					if (worldX < go->pos.x + go->scale.x / 2)
					{
						if (worldY > go->pos.y - go->scale.y / 2)
						{
							if (worldY < go->pos.y + go->scale.y / 2)
							{
								triggerzone = true;
							}
						}
					}
				}
			}
		}

	}
	else if (bRButtonState && !Application::IsMousePressed(1))
	{
		bRButtonState = false;
		std::cout << "RBUTTON UP" << std::endl;

		//Exercise 10: spawn large GO_BALL
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float worldX = x * m_worldWidth / w;
		float worldY = (h - y) * m_worldHeight / h;

		if (triggerzone == true)
		{
			GameObject* ball = FetchGO();
			ball->type = GameObject::GO_BALL;
			ball->scale.Set(1.5f, 1.5f, 1.5f);
			ball->pos = m_ghost->pos;
			ball->vel = m_ghost->pos - Vector3(worldX, worldY, 0);
			ball->mass = 1.5f * 1.5f * 1.5f;
			triggerzone = false;
		}
		
		m_ghost->active = false;
	}

	if (m_ghost2->active)
	{
		double x, y;
		Application::GetCursorPos(&x, &y);
		int w = Application::GetWindowWidth();
		int h = Application::GetWindowHeight();
		float worldX = x * m_worldWidth / w;
		float worldY = (h - y) * m_worldHeight / h;
		m_ghost2->pos.Set(worldX, worldY, 0);
	}

	//Physics Simulation Section

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;

		if (!go->active)
			continue;
		//if (go->active)
		{
			go->pos += go->vel * m_speed * dt;

			//Exercise 7: handle out of bound game objects
			if (go->type == GameObject::GO_BALL)
			{
				if (go->pos.x > m_worldWidth - go->scale.x)
				{
					if (go->vel.x > 0)
						go->vel.x = -go->vel.x;
				}
				else if (go->pos.x < 0 + go->scale.x)
				{
					if (go->vel.x < 0)
						go->vel.x = -go->vel.x;
				}
				if (go->pos.x > m_worldWidth + go->scale.x || go->pos.x < 0 - go->scale.x)
				{
					go->active = false;
					m_objectCount--;
				}
				if (go->pos.y > m_worldHeight - go->scale.y)
				{
					if (go->vel.y > 0)
						go->vel.y = -go->vel.y;
				}
				else if (go->pos.y < 0 + go->scale.y)
				{
					if (go->vel.y < 0)
						go->vel.y = -go->vel.y;
				}
				if (go->pos.y > m_worldHeight + go->scale.y || go->pos.y < 0 - go->scale.y)
				{
					go->active = false;
					m_objectCount--;
				}
			}
			//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap

			for (std::vector<GameObject *>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
			{
				GameObject *other = (GameObject*)*it2;
				if (!other->active)
					continue;

				GameObject *goA = go, *goB = other;
				if (go->type != GameObject::GO_BALL)
				{
					if (other->type != GameObject::GO_BALL)
						continue;

					else
					{
						goA = other;
						goB = go;
					}
				}

				if (CheckCollision(goA, goB, dt))
				{
					//Exercise 8b: store values in auditing variables
					CollisionResponse(goA, goB);

					break;
				}
			}

			if (go->type == GameObject::GO_WALL2)
			{
				go->time += dt;
				if (go->time >= 2.0f)
				{
					go->time = 0;
					go->active = false;
				}
			}
		
			if (m_lives == 0)
			{
				gameStates = states::lose;
				
			}
			//Exercise 10: handle collision using momentum swap instead

			//Exercise 12: improve inner loop to prevent double collision

			//Exercise 13: improve collision detection algorithm [solution to be given later] 
		}
	}
}


void SceneCollision::RenderGO(GameObject *go)
{
	switch (go->type)
	{
	case GameObject::GO_GOAL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_GOAL], false);
		modelStack.PopMatrix();

		break;
	case GameObject::GO_BALL:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();

		//Exercise 11: think of a way to give balls different colors
		break;

	case GameObject::GO_START:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE3], false);
		modelStack.PopMatrix();

		break;

	case GameObject::GO_WALL:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();

		break;

	case GameObject::GO_WALL2:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();

		break;

	case GameObject::GO_WALL3:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE2], false);
		modelStack.PopMatrix();

		break;

	case GameObject::GO_PILLAR:
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Rotate(Math::RadianToDegree(atan2(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CYLINDER], false);
		modelStack.PopMatrix();

		break;
	}
}

void SceneCollision::Render()
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

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
		{
			RenderGO(go);
		}
	}

	if (m_ghost->active)
	{
		RenderGO(m_ghost);
	}

	if (m_ghost2->active)
	{
		RenderGO(m_ghost2);
	}

	//On screen text

	//Exercise 5: Render m_objectCount
	/*std::ostringstream ss3;
	ss3.precision(5);
	ss3 << "m_objectCount: " << m_objectCount;
	RenderTextOnScreen(meshList[GEO_TEXT], ss3.str(), Color(0, 1, 0), 3, 0, 9);*/

	//Exercise 8c: Render initial and final momentum


	std::ostringstream ss2;
	//ss2.precision(5);
	//ss2 << "Initial Energy: " << 0.5f * m1 * u1.LengthSquared() + 0.5f * m2 * u2.LengthSquared();
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 15);

	//ss2.str("");
	//ss2.precision(5);
	//ss2 << "Estimated Time: " << m_estimatedTime;
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 18);

	//ss2.str("");
	//ss2.precision(5);
	//ss2 << "Final energy: " << 0.5f * m1 * v1.LengthSquared() + 0.5f * m2 * v2.LengthSquared();
	//RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 12);

	/*ss2.str("");
	ss2.precision(5);
	ss2 << "Lives: " << m_lives;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 9);*/
	if (gameStates == states::lose)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU LOST", Color(0, 1, 0), 5, 25, 30);
	}
	if (gameStates == states::menu)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Wall-Bounce", Color(0, 1, 0), 5, 15, 40);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press Enter to Start", Color(0, 1, 0), 3, 15, 10);
	}
	if (gameStates == states::win)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "YOU WON", Color(0, 1, 0), 5, 15, 40);
		RenderTextOnScreen(meshList[GEO_TEXT], "Press Enter to go back", Color(0, 1, 0), 3, 15, 10);
	}
	ss2.str("");
	ss2.precision(3);
	ss2 << "Lives: " << m_lives;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 6);

	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);

	RenderTextOnScreen(meshList[GEO_TEXT], "Wall-Bounce", Color(0, 1, 0), 3, 0, 0);
}

void SceneCollision::Exit()
{
	SceneBase::Exit();
	//Cleanup GameObjects
	while (m_goList.size() > 0)
	{
		GameObject *go = m_goList.back();
		delete go;
		m_goList.pop_back();
	}
	if (m_ghost)
	{
		delete m_ghost;
		m_ghost = NULL;
	}
}