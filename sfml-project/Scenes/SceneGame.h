#pragma once
#include "Scene.h"

class Player;
class TextGo;
class TiledMap;

class SceneGame : public Scene
{
protected:
	Player* player = nullptr;
	TiledMap* tiledMap = nullptr; // LMJ: Tiled map for background

	TextGo* textHp = nullptr;
	TextGo* textLevel = nullptr;
	TextGo* textExp = nullptr;
	TextGo* textTimer = nullptr;

	sf::Vector2i mouse;
	float gameTimer = 300.f; // LMJ: [BEFORE] 30min -> [NOW] 5min.
	bool isGameRunning = true;

	// LMJ: Map boundaries for camera constraint
	sf::FloatRect mapBounds;

private:
	void UpdateGameTimer(float dt);
	void UpdateUI(float dt);
	void CheckGameOver();
	void UpdateCameraWithBounds(const sf::Vector2f& playerPos); // LMJ: Camera boundary checking

public:
	SceneGame();
	~SceneGame() override = default;

	void Init() override;
	void Enter() override;
	void Exit() override;
	void Update(float dt) override;
	void Draw(sf::RenderWindow& window) override;
};