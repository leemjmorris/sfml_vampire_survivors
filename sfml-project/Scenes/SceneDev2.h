#pragma once
#include "Scene.h"
class SceneDev2 : public Scene
{
protected:

public:
	SceneDev2();

	void Init() override;
	void Enter() override;
	void Update(float dt) override;

};

