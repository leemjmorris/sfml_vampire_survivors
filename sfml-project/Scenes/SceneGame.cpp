#include "stdafx.h"
#include "SceneGame.h"
#include "Player.h"
#include "TextGo.h"
#include "TiledMap.h"
#include "WeaponMgr.h"

SceneGame::SceneGame() : Scene(SceneIds::Game) // LMJ: Need to Change l8er. Need to Add SceneIds->Game.
{
}

void SceneGame::Init()
{
	texIds.push_back("graphics/background_forest.png");
	// LMJ: Player sprite section
	texIds.push_back("graphics/sprite_run.png");
	texIds.push_back("graphics/sprite_death.png");
	texIds.push_back("graphics/Knife.png");

	// LMJ: Enemy sprite section
	texIds.push_back("graphics/sprite_bat1_run.png");
	texIds.push_back("graphics/sprite_bat1_death.png");

	// LMJ: Font section
	fontIds.push_back("fonts/DS-DIGIT.ttf");

	CollisionManager::SetDebugDraw(true);

	// LMJ: Create tiled map
	tiledMap = new TiledMap("graphics/background_forest.png", "ForestMap");
	AddGameObject(tiledMap);

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

	// LMJ: Weapon set
	weaponManager = new WeaponMgr("WeaponManager");
	weaponManager->SetOwner(player);
	weaponManager->SetScene(this);
	AddGameObject(weaponManager);

	player->SetWeaponManager(weaponManager);

	// LMJ: Add Weapon
	weaponManager->AddWeapon(WeaponType::Knife);
	std::cout << "=== WEAPON DEBUG ===" << std::endl;
	std::cout << "Weapon count: " << weaponManager->GetWeaponCount() << std::endl;
	std::cout << "Has knife: " << weaponManager->HasWeapon(WeaponType::Knife) << std::endl;

	// LMJ: Enemy for TESTING.
	testEnemy = new Enemy("TestEnemy");
	AddGameObject(testEnemy);


	TextGo* instructionText = new TextGo("fonts/DS-DIGIT.ttf");
	instructionText->SetString("WASD: Move | T: Damage | G: +EXP | H: Heal | U: Upgrade Knife | I: Add Knife | P: Show Stats | K: Damage Enemy | ESC: Exit");
	instructionText->SetCharacterSize(16); // 더 작게 만들어서 모든 텍스트가 보이도록
	instructionText->SetFillColor(sf::Color(128, 128, 128));
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

	gameTimer = 300.f;
	isGameRunning = true;

	// LMJ: Set up player-map relationship after both are initialized
	if (player != nullptr && tiledMap != nullptr)
	{
		player->SetCurrentMap(tiledMap);

		// LMJ: Get map size for camera boundary calculations
		mapBounds = tiledMap->GetMapBounds();

		std::cout << "Map bounds set: " << mapBounds.left << ", " << mapBounds.top	<< ", " << mapBounds.width << ", " << mapBounds.height << std::endl;
	}

	if (testEnemy != nullptr && player != nullptr)
	{
		testEnemy->FollowPlayer(player);
	}

	Scene::Enter();
}

void SceneGame::Exit()
{
	CollisionManager::ClearAll();
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

	CollisionManager::Update(dt);
	Scene::Update(dt);

	if (player != nullptr)
	{
		sf::Vector2f playerPos = player->GetPosition();
		UpdateCameraWithBounds(playerPos);

		// LMJ: test keys
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

		// LMJ: weapons test key
		if (InputMgr::GetKeyDown(sf::Keyboard::U))
		{
			if (weaponManager != nullptr)
			{
				weaponManager->UpgradeWeapon(WeaponType::Knife);
				std::cout << "Knife upgraded!" << std::endl;
			}
		}

		if (InputMgr::GetKeyDown(sf::Keyboard::I))
		{
			if (weaponManager != nullptr && !weaponManager->HasWeapon(WeaponType::Knife))
			{
				weaponManager->AddWeapon(WeaponType::Knife);
				std::cout << "Knife added!" << std::endl;
			}
		}

		// LMJ: Stat debug test run
		if (InputMgr::GetKeyDown(sf::Keyboard::P))
		{
			const PlayerStats& stats = player->GetPlayerStats();
			std::cout << "=== Player Stats ===" << std::endl;
			std::cout << "Level: " << player->GetLevel() << std::endl;
			std::cout << "Might: " << stats.mightMultiplier << "x" << std::endl;
			std::cout << "Area: " << stats.areaMultiplier << "x" << std::endl;
			std::cout << "Speed: " << stats.speedMultiplier << "x" << std::endl;
			std::cout << "Amount: +" << stats.amountBonus << std::endl;
			std::cout << "Cooldown: " << stats.cooldownMultiplier << "x" << std::endl;
			std::cout << "Crit Chance: " << (stats.critChance * 100) << "%" << std::endl;
			std::cout << "Move Speed: " << player->GetFinalMoveSpeed() << std::endl;
			std::cout << "Max HP: " << player->GetFinalMaxHP() << std::endl;
			std::cout << "Armor: " << stats.armorValue << std::endl;
			std::cout << "Recovery: " << stats.recoveryBonus << " HP/sec" << std::endl;
			std::cout << "===================" << std::endl;
		}

		if (InputMgr::GetKeyDown(sf::Keyboard::Num1))
		{
			player->SetHitBoxRadius(player->GetHitBox()->GetRadius() - 2.f);
			std::cout << "Player hitbox radius: " << player->GetHitBox()->GetRadius() << std::endl;
		}
		if (InputMgr::GetKeyDown(sf::Keyboard::Num2))
		{
			player->SetHitBoxRadius(player->GetHitBox()->GetRadius() + 2.0f);
			std::cout << "Player hitbox radius: " << player->GetHitBox()->GetRadius() << std::endl;
		}

		// LMJ: Damage Test
		if (InputMgr::GetKeyDown(sf::Keyboard::K))
		{
			if (testEnemy != nullptr && testEnemy->GetActive() && !testEnemy->IsDead())
			{
				testEnemy->TakeDamage(25);
				std::cout << "enemy damage taken! HP: " << testEnemy->GetEnemyHp() << std::endl;
				if (testEnemy->GetEnemyHp() <= 0)
				{
					player->GainExperience(testEnemy->GetExpValue());
					std::cout << "enemy dead" << std::endl;
					std::cout << "player gain exp: " << player->GetExperience() << std::endl;
				}
			}
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

void SceneGame::UpdateCameraWithBounds(const sf::Vector2f& playerPos)
{
	// LMJ: Get current view size
	sf::Vector2f viewSize = worldView.getSize();
	sf::Vector2f halfViewSize = viewSize * 0.5f;

	// LMJ: Calculate desired camera center (player position)
	sf::Vector2f desiredCenter = playerPos;

	// LMJ: Clamp camera center to keep view within map bounds
	if (mapBounds.width > 0 && mapBounds.height > 0)
	{
		// LMJ: Clamp X coordinate
		if (desiredCenter.x - halfViewSize.x < mapBounds.left)
			desiredCenter.x = mapBounds.left + halfViewSize.x;
		else if (desiredCenter.x + halfViewSize.x > mapBounds.left + mapBounds.width)
			desiredCenter.x = mapBounds.left + mapBounds.width - halfViewSize.x;

		// LMJ: Clamp Y coordinate
		if (desiredCenter.y - halfViewSize.y < mapBounds.top)
			desiredCenter.y = mapBounds.top + halfViewSize.y;
		else if (desiredCenter.y + halfViewSize.y > mapBounds.top + mapBounds.height)
			desiredCenter.y = mapBounds.top + mapBounds.height - halfViewSize.y;
	}

	// LMJ: Set the clamped camera center
	worldView.setCenter(desiredCenter);
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
	if (player != nullptr && player->GetCurrentHp() <= 0 && player->IsDeathAnimationFinished())
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
		// 최종 HP 사용
		int finalMaxHp = player->GetFinalMaxHP();
		std::string hpString = "HP: " + std::to_string(player->GetCurrentHp()) + "/" + std::to_string(finalMaxHp);
		textHp->SetString(hpString);

		float hpRatio = static_cast<float>(player->GetCurrentHp()) / finalMaxHp;
		if (hpRatio > 0.5f) textHp->SetFillColor(sf::Color::Green);
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
	CollisionManager::DrawAll(window);
}