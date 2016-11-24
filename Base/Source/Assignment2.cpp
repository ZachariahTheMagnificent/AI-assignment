#include "SceneCollision.h"
#include "GL\glew.h"
#include "Application.h"
#include "MeshBuilder.h"
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

	m_ghost = new GameObject(GameObject::GO_POINT);
	m_ghost2 = new GameObject(GameObject::GO_POINT);
	Vector3 baguaCentre(50, 50, 0);
	Vector3 baguaRadiusOffset(35, 0, 0);
	Mtx44 baguaRotation;
	for (int i = 0; i < 8; ++i)
	{
		GameObject* wall = FetchGO();
		baguaRotation.SetToRotation(i * 45.0f, 0, 0, 1);
		wall->pos = (baguaRotation *  baguaRadiusOffset) + baguaCentre;
		wall->normal = baguaRotation * Vector3(1, 0, 0);
		wall->type = GameObject::GO_WALL;
		//wall->pos = (m_worldWidth / 2, m_worldHeight / 2, 0);
		wall->active = true;
		wall->scale.Set(2, 20, 1);

	}
	GameObject* wall = FetchGO();
	baguaRotation.SetToRotation(90, 0, 0, 1);
	wall->pos = baguaCentre;
	wall->normal = baguaRotation * Vector3(1, 0, 0);
	wall->type = GameObject::GO_WALL;
	wall->active = true;
	wall->scale.Set(2, 18, 1);
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
			return go;
		}
	}
	for (unsigned int i = 0; i < 10; ++i)
	{
		m_goList.push_back(new GameObject(GameObject::GO_BALL));
	}
	GameObject *go = m_goList.back();
	go->active = true;

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
								Vector3 b1 = go->pos;
								Vector3 N = other->normal;
								float r = go->scale.x;
								float h = other->scale.x;
								float l = other->scale.y;

								Vector3 NP(-N.y, N.x); //(N.y, -N.x)
								return (abs((w0 - b1).Dot(N)) < r + h / 2 && abs((w0 - b1).Dot(NP)) < r + 1 / 2);

								break;
	}
	}
	return false;
}
float SceneCollision::CheckCollision2(GameObject *go, GameObject *other)
{
	Vector3 rel = go->vel - other->vel;
	Vector3 dir = go->pos - other->pos;
	float r = go->scale.x + other->scale.x;
	float dot = rel.Dot(dir);

	if (dot > 0)
	{
		return -1;
	}
	float a = rel.LengthSquared();
	float b = 2 * rel.Dot(dir);
	float c = dir.LengthSquared() - r * r;
	float d = b * b - 4 * a * c;

	if (d < 0)
		return -1;

	float t = (-b - sqrt(d)) / (2 * a);

	if (t < 0)
		t = (-b + sqrt(d)) / (2 * a);

	return t;
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
								N = (other->pos - go->pos).Normalized();

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
	}

}
void SceneCollision::Update(double dt)
{
	SceneBase::Update(dt);

	if (Application::IsKeyPressed('9'))
	{
		m_speed = Math::Max(0.f, m_speed - 0.1f);
	}
	if (Application::IsKeyPressed('0'))
	{
		m_speed += 0.1f;
	}

	static bool SpaceButtonState = false;
	if (!SpaceButtonState && Application::IsKeyPressed(VK_SPACE))
	{
		SpaceButtonState = true;
	}
	else if (SpaceButtonState && !Application::IsKeyPressed(VK_SPACE))
	{
		SpaceButtonState = false;
		int w = Application::GetWindowWidth();
		float m_worldSizeX = m_worldWidth / w;

		GameObject* ball = FetchGO();
		ball->type = GameObject::GO_BALL;
		ball->scale.Set(5, 5, 5);
		ball->pos = (m_worldSizeX, 0.9f, 10);
		ball->vel = Vector3(0, 10, 0);
		ball->mass = 10.f;
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
		//m_ghost->pos.Set(worldX, worldY, 0);
		//m_ghost->active = true;

		if (!m_ghost)
		{
			m_ghost = new GameObject(GameObject::GO_POINT);
			m_ghost->pos.Set(worldX, worldY, 0);
			m_ghost->active = true;
			m_ghost->scale.Set(1, 1, 1);
			m_ghost->color = Color(0, 1, 0);
			m_ghost->mesh = MeshBuilder::GenerateSphere("ball", Color(1, 1, 1), 10, 10, 1.f);
		}
		else
		{
			if (m_ghost2)
				delete m_ghost2;
			m_ghost2 = new GameObject(GameObject::GO_POINT);
			m_ghost2->pos.Set(worldX, worldY, 0);
			m_ghost2->active = true;
			m_ghost2->scale.Set(1, 1, 1);
			m_ghost2->color = Color(0, 1, 0);
			m_ghost2->mesh = MeshBuilder::GenerateSphere("ball", Color(1, 1, 1), 10, 10, 1.f);
		}
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
		//m_ghost->active = false;

		//GameObject* ball = FetchGO();
		//ball->type = GameObject::GO_BALL;
		//Vector3 size = Vector3(worldX, worldY, 0) - m_ghost->pos;
		//float radius = Math::Clamp(size.Length(), 2.f, 10.f);
		//ball->scale.Set(radius, radius, 1.0f);
		//ball->pos = m_ghost->pos;
		//ball->vel = m_ghost->pos - Vector3(worldX, worldY, 0);
		//ball->mass = radius;
		//m_ghost->active = false;

		//m_estimatedTime = -1;
		//for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
		//{
		//	GameObject *other = (GameObject *)*it;
		//	if (other->active && ball != other)
		//	{
		//		//float t = CheckCollision2(ball, other);
		//		//if (t > 0)
		//		//	m_estimatedTime = t;
		//	}
		//}

		GameObject* point = FetchGO();
		Vector3 dist = m_ghost->pos - m_ghost2->pos;
		//baguaRotation.SetToRotation(90, 0, 0, 1);
		point->pos = (m_ghost->pos + m_ghost2->pos) * 0.5;
		//wall->normal = baguaRotation * Vector3(1, 0, 0);
		point->type = GameObject::GO_WALL;
		point->active = true;
		point->scale.Set(1, dist.Length(), 1);
		point->normal = dist.Cross(Vector3(0, 0, 1));
		point->mesh = MeshBuilder::GenerateCube("wall", Color(1, 1, 1), 1);
		m_ghost2->active = false;
		m_ghost->active = false;
		delete m_ghost;
		m_ghost = nullptr;
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

		GameObject* ball = FetchGO();
		ball->type = GameObject::GO_BALL;
		ball->scale.Set(1.5f, 1.5f, 1.5f);
		ball->pos = m_ghost->pos;
		ball->vel = m_ghost->pos - Vector3(worldX, worldY, 0);
		ball->mass = 1.5f * 1.5f * 1.5f;
		m_ghost->active = false;
	}

	//if (m_ghost->active)
	//{
	//	double x, y;
	//	Application::GetCursorPos(&x, &y);
	//	int w = Application::GetWindowWidth();
	//	int h = Application::GetWindowHeight();
	//	float worldX = x * m_worldWidth / w;
	//	float worldY = (h - y) * m_worldHeight / h;
	//	Vector3 size = Vector3(worldX, worldY, 0) - m_ghost->pos;
	//	float radius = Math::Clamp(size.Length(), 2.f, 10.f);
	//	m_ghost->scale.Set(radius, radius, 1.0f);
	//}

	//Physics Simulation Section

	for (std::vector<GameObject *>::iterator it = m_goList.begin(); it != m_goList.end(); ++it)
	{
		GameObject *go = (GameObject *)*it;
		if (go->active)
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

				//Exercise 8a: handle collision between GO_BALL and GO_BALL using velocity swap

				for (std::vector<GameObject *>::iterator it2 = it + 1; it2 != m_goList.end(); ++it2)
				{
					GameObject *other = (GameObject*)*it2;
					//if (other->type != GameObject::GO_BALL)
					//	continue;
					if (!other->active)
						continue;

					GameObject *goA = go, *goB = other;
					if (go->type != GameObject::GO_BALL)
					{
						if (other->type != GameObject::GO_BALL)
							continue;
						goA = other;
						goB = go;
					}

					if (CheckCollision3(goA, goB, dt))
					{
						//Exercise 8b: store values in auditing variables

						CollisionResponse(goA, goB);

						break;
					}
				}

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
	case GameObject::GO_BALL:
		//Exercise 4: render a sphere using scale and pos
		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_BALL], false);
		modelStack.PopMatrix();

		//Exercise 11: think of a way to give balls different colors
		break;

	case GameObject::GO_WALL:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		//	float angle = Math::RadianToDegree(atan2(go->normal.y, go->normal.x));
		modelStack.Rotate(Math::RadianToDegree(atan2(go->normal.y, go->normal.x)), 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(meshList[GEO_CUBE], false);
		modelStack.PopMatrix();

		break;

	case GameObject::GO_POINT:

		modelStack.PushMatrix();
		modelStack.Translate(go->pos.x, go->pos.y, go->pos.z);
		//		float angle = Math::RadianToDegree(atan2(go->normal.y, go->normal.x));
		//		modelStack.Rotate(angle, 0, 0, 1);
		modelStack.Scale(go->scale.x, go->scale.y, go->scale.z);
		RenderMesh(go->mesh, true);
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

	//if (m_ghost->active)
	//{
	//	modelStack.PushMatrix();
	//	modelStack.Translate(m_ghost->pos.x, m_ghost->pos.y, m_ghost->pos.z);
	//	modelStack.Scale(m_ghost->scale.x, m_ghost->scale.y, m_ghost->scale.z);
	//	RenderMesh(meshList[GEO_BALL], false);
	//	modelStack.PopMatrix();
	//}

	//On screen text

	//Exercise 5: Render m_objectCount
	std::ostringstream ss3;
	ss3.precision(5);
	ss3 << "m_objectCount: " << m_objectCount;
	RenderTextOnScreen(meshList[GEO_TEXT], ss3.str(), Color(0, 1, 0), 3, 0, 9);

	//Exercise 8c: Render initial and final momentum
	std::ostringstream ss2;
	ss2.precision(5);
	ss2 << "Estimated Time: " << m_estimatedTime;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 15);


	ss2.precision(5);
	ss2 << "Initial Energy: " << 0.5f * m1 * u1.LengthSquared() + 0.5f * m2 * u2.LengthSquared();
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 15);

	ss2.str("");
	ss2.precision(5);
	ss2 << "Final energy: " << 0.5f * m1 * v1.LengthSquared() + 0.5f * m2 * v2.LengthSquared();
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 12);

	ss2.str("");
	ss2.precision(3);
	ss2 << "Speed: " << m_speed;
	RenderTextOnScreen(meshList[GEO_TEXT], ss2.str(), Color(0, 1, 0), 3, 0, 6);

	std::ostringstream ss;
	ss.precision(5);
	ss << "FPS: " << fps;
	RenderTextOnScreen(meshList[GEO_TEXT], ss.str(), Color(0, 1, 0), 3, 0, 3);

	RenderTextOnScreen(meshList[GEO_TEXT], "Collision", Color(0, 1, 0), 3, 0, 0);
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
