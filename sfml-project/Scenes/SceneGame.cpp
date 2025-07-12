#include "stdafx.h"
#include "SceneGame.h"
#include "Player.h"
#include "TextGo.h"

SceneGame::SceneGame() : Scene(SceneIds::Game) // LMJ: Need to Change l8er. Need to Add SceneIds->SceneGame.
{
}

void SceneGame::Init()
{
	// LMJ: Need To Change Resource, This is Just For Test Drive Only.
	texIds.push_back("graphics/character_sheet_1.png");
	fontIds.push_back("fonts/DS-DIGIT.ttf"); 

	// LMJ: ONLY FOR TESTING PURPOSE. NEED TO CHANGE WHEN SPRITE AND RESOURCES ARE ADDED!!!!!!!!!
	textHp = new TextGo("fonts/DS-DIGIT.ttf");
	textHp->SetString("HP: 100/100");
	textHp->SetCharacterSize(24);
	textHp->SetFillColor(sf::Color::Red);
	textHp->SetPosition(sf::Vector2f(20.f, 20.f));
	textHp->sortingLayer = SortingLayers::UI;
	textHp->sortingOrder = 10;
	AddGameObject(textHp);

	textLevel = new TextGo("fonts/DS-DIGIT.ttf");
	textLevel->SetString("Level: 1");
	textLevel->SetCharacterSize(24);
	textLevel->SetFillColor(sf::Color::Cyan);
	textLevel->SetPosition(sf::Vector2f(20.f, 50.f));
	textLevel->sortingLayer = SortingLayers::UI;
	textLevel->sortingOrder = 10;
	AddGameObject(textLevel);

	textExp = new TextGo("fonts/DS-DIGIT.ttf");
	textExp->SetString("EXP: 0/100");
	textExp->SetCharacterSize(24);
	textExp->SetFillColor(sf::Color::Yellow);
	textExp->SetPosition(sf::Vector2f(20.f, 80.f));
	textExp->sortingLayer = SortingLayers::UI;
	textExp->sortingOrder = 10;
	AddGameObject(textExp);

	textTimer = new TextGo("fonts/DS-DIGIT.ttf");
	textTimer->SetString("Time: 00:00");
	textTimer->SetCharacterSize(24);
	textTimer->SetFillColor(sf::Color::White);
	textTimer->SetOrigin(Origins::TC);
	textTimer->SetPosition(sf::Vector2f(FRAMEWORK.GetWindowSizeF().x * 0.5f, 20.0f));
	textTimer->sortingLayer = SortingLayers::UI;
	textTimer->sortingOrder = 10;
	AddGameObject(textTimer);
	
	player = new Player("GamePlayer");
	AddGameObject(player);

	TextGo* instructionText = new TextGo("fonts/DS-DIGIT.ttf");
	instructionText->SetString("WASD: Move | T: Take Damage | G: Gain EXP | H: Heal | ESC: Exit");
	instructionText->SetCharacterSize(20);
	instructionText->SetFillColor(sf::Color(128, 128, 128)); // LMJ: Gray setting.
	instructionText->SetPosition(sf::Vector2f(20.f, FRAMEWORK.GetWindowSizeF().y - 30.f));
	instructionText->sortingLayer = SortingLayers::UI;
	instructionText->sortingOrder = 5;
	AddGameObject(instructionText);

	Scene::Init();
}

void SceneGame::Enter()
{
	auto size = FRAMEWORK.GetWindowSizeF();
	sf::Vector2f center{ size.x * 0.5f, size.y * 0.5f };

	uiView.setSize(size);
	uiView.setCenter(center);
	worldView.setSize(size);
	worldView.setCenter(center);
	
	//auto windowSize = FRAMEWORK.GetWindowSizeF();
	//worldView.setSize(windowSize);
	//worldView.setCenter(windowSize * 0.5f);

	gameTimer = 300.f;
	isGameRunning = true;

	Scene::Enter();
}

void SceneGame::Exit()
{
	Scene::Exit();
}

void SceneGame::Update(float dt)
{
	// LMJ: For checking pos with mouse. needed for debug.
	if (InputMgr::GetMouseButtonDown(sf::Mouse::Left))
	{
		mouse = InputMgr::GetMousePosition();
		std::cout << mouse.x << "," << mouse.y << std::endl;
	}


	if (!isGameRunning)
		return;

	Scene::Update(dt);

	if (player != nullptr)
	{
		sf::Vector2f playerPos = player->GetPosition();
		worldView.setCenter(playerPos);

		if (InputMgr::GetKeyDown(sf::Keyboard::T))
		{
			int currentHp = player->GetCurrentHp();
			player->TakeDamage(20);
			int newHp = player->GetCurrentHp();
			std::cout << "Take Damage! HP: " << currentHp << " -> " << newHp << std::endl;
		}
		if (InputMgr::GetKeyDown(sf::Keyboard::G))
		{
			player->GainExperience(50);
		}
		if (InputMgr::GetKeyDown(sf::Keyboard::H))
		{
			int currentHp = player->GetCurrentHp();
			player->Heal(25);
			int newHp = player->GetCurrentHp();
			std::cout << "HP healed!!! HP: " << currentHp << " -> " << newHp << std::endl;
		}
	}

	UpdateGameTimer(dt);
	UpdateUI(dt);

	if (InputMgr::GetKeyDown(sf::Keyboard::Escape))
	{
		SCENE_MGR.ChangeScene(SceneIds::Game);
	}

	CheckGameOver();
}

void SceneGame::UpdateGameTimer(float dt)
{
	if (!isGameRunning) return;

	gameTimer -= dt;

	if (gameTimer < 0.f) gameTimer = 0.f;

	if (textTimer != nullptr)
	{
		int minutes = (int)(gameTimer / 60.f);
		int seconds = (int)(gameTimer) % 60;

		std::string timeString = "Time: ";
		if (minutes < 10) timeString += "0";
		timeString += std::to_string(minutes) + ":";
		if (seconds < 10) timeString += "0";
		timeString += std::to_string(seconds);
		
		textTimer->SetString(timeString);

		if (gameTimer < 60.f) textTimer->SetFillColor(sf::Color::Red);
		else if (gameTimer < 150.f) textTimer->SetFillColor(sf::Color::Yellow);
		else textTimer->SetFillColor(sf::Color::White);
	}
}

void SceneGame::CheckGameOver()
{
	if (!isGameRunning) return;

	// LMJ: Player Dead by low hp
	if (player != nullptr && player->GetCurrentHp() <= 0)
	{
		isGameRunning = false;
		std::cout << "GAME OVER" << std::endl;
		std::cout << "PLAYER DEAD" << std::endl;
		std::cout << "FINAL LEVEL: " << player->GetLevel() << std::endl;
	}

	// LMJ: Player Survive for 5min.
	if (gameTimer <= 0.f)
	{
		isGameRunning = false;
		std::cout << "WINNER WINNER CHICKEN DINNER" << std::endl;
		std::cout << "You survived 05 minutes!" << std::endl;
		if (player != nullptr)
		{
			std::cout << "FINAL LEVEL: " << player->GetLevel() << std::endl;
			std::cout << "FINAL HP: " << player->GetCurrentHp() << "/" << player->GetMaxHp() << std::endl;
		}
		else
		{
			std::cout << "Error, Player Data not found (nullptr, See SceneGame.cpp" << std::endl;
		}
		// LMJ: Need to make victory scene to change the scene when player wins. Or, can just use UI Mgr to let a certain UI pop-up when player wins.
	}
}

void SceneGame::UpdateUI(float dt)
{
	if (player == nullptr) return;
	if (textHp != nullptr)
	{
		std::string hpString = "HP: " + std::to_string(player->GetCurrentHp()) + "/" + std::to_string(player->GetMaxHp());
		textHp->SetString(hpString);

		float hpRatio = (float)player->GetCurrentHp() / (float)player->GetMaxHp();
		if (hpRatio > 0.5f) textHp->SetFillColor(sf::Color::Green); // LMJ: 50/100 = 0.5f
		else if (hpRatio > 0.25f) textHp->SetFillColor(sf::Color::Yellow);
		else textHp->SetFillColor(sf::Color::Red);
	}

	if (textLevel != nullptr)
	{
		std::string levelString = "Level: " + std::to_string(player->GetLevel());
		textLevel->SetString(levelString);
	}

	if (textExp != nullptr)
	{
		std::string expString = "EXP: " + std::to_string(player->GetExperience()) + "/" + std::to_string(player->GetExperienceToNext());
		textExp->SetString(expString);
	}
}

void SceneGame::Draw(sf::RenderWindow& window)
{
	Scene::Draw(window);
}