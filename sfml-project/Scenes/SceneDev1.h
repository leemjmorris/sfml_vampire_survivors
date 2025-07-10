#pragma once
#include "Scene.h"

class TextGo;

class SceneDev1 : public Scene
{
protected:
	enum class CollisionMode
	{
		Rectangle,
		Circle
	};

	sf::RectangleShape* rect1;
	sf::RectangleShape* rect2;
	sf::CircleShape* circle1;
	sf::CircleShape* circle2;
	TextGo* collisionText;
	TextGo* modeText;
	
	CollisionMode currentMode = CollisionMode::Rectangle;
	bool isColliding = false;

public:
	SceneDev1();
	~SceneDev1() override;

	void Init() override;
	void Enter() override;
	void Update(float dt) override;
	void Draw(sf::RenderWindow& window) override;
};

