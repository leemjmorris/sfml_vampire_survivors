#pragma once
#include "Scene.h"

class Player;
class TextGo;

class SceneGame : public Scene
{
protected:
	Player* player = nullptr;

	TextGo* textHp = nullptr;
	TextGo* textLevel = nullptr;
	TextGo* textExp = nullptr;
	TextGo* textTimer = nullptr;

	float gameTimer = 300.f; // LMJ: [BEFORE] 30min -> [NOW] 5min.
	bool isGameRunning = true;

private:
	void UpdateGameTimer(float dt);
	void UpdateUI(float dt);
	void CheckGameOver();

public:
	SceneGame();
	~SceneGame() override = default;

	void Init() override;
	void Enter() override;
	void Exit() override;
	void Update(float dt) override;
	void Draw(sf::RenderWindow& window) override;
};

