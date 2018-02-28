#ifndef SCENE_H
#define SCENE_H

class Scene
{
public:
	Scene() {}
	~Scene() {}

	virtual void Init() = 0;
	virtual void Update(const float dt) = 0;
	virtual void Render() = 0;
	virtual void Exit() = 0;
};

#endif