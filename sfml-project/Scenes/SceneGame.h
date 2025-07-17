#pragma once
#include "Scene.h"
#include "HitBox.h"

class Player;
class Enemy;
class TextGo;
class TiledMap;
class WeaponMgr;
class MonsterSpawner;

class SceneGame : public Scene
{
protected:
	Player* player = nullptr;
	TiledMap* tiledMap = nullptr;
	WeaponMgr* weaponManager = nullptr;
	MonsterSpawner* monsterSpawner = nullptr;

	TextGo* textHp = nullptr;
	TextGo* textLevel = nullptr;
	TextGo* textExp = nullptr;
	TextGo* textTimer = nullptr;

	sf::Vector2i mouse;
	float gameTimer = 300.f; // LMJ: 5 minutes game duration
	bool isGameRunning = true;

	// LMJ: Map boundaries for camera constraint
	sf::FloatRect mapBounds;

private:
	void UpdateGameTimer(float dt);
	void UpdateUI(float dt);
	void CheckGameOver();
	void UpdateCameraWithBounds(const sf::Vector2f& playerPos);

public:
	SceneGame();
	~SceneGame() override = default;

	void Init() override;
	void Enter() override;
	void Exit() override;
	void Update(float dt) override;
	void Draw(sf::RenderWindow& window) override;
};