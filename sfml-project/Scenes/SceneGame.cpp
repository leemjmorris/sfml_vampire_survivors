#include "stdafx.h"
#include "SceneGame.h"
#include "Player.h"
#include "Enemy.h"
#include "MonsterSpawner.h"
#include "TextGo.h"
#include "TiledMap.h"
#include "WeaponMgr.h"

SceneGame::SceneGame() : Scene(SceneIds::Game)
{
}

void SceneGame::Init()
{
    // LMJ: Load resources
    texIds.push_back("graphics/sprite_run.png");
    texIds.push_back("graphics/sprite_death.png");
    texIds.push_back("graphics/sprite_bat1_run.png");
    texIds.push_back("graphics/sprite_bat1_death.png");
    texIds.push_back("graphics/ghoul1_run.png");
    texIds.push_back("graphics/ghoul1_death.png");
    texIds.push_back("graphics/ghoul2_run.png");
    texIds.push_back("graphics/ghoul2_death.png");
    texIds.push_back("graphics/ghoul3_run.png");
    texIds.push_back("graphics/ghoul3_death.png");
    texIds.push_back("graphics/skeleton1_run.png");
    texIds.push_back("graphics/skeleton1_death.png");
    texIds.push_back("graphics/skeleton2_run.png");
    texIds.push_back("graphics/skeleton2_death.png");
    texIds.push_back("graphics/skeleton3_run.png");
    texIds.push_back("graphics/skeleton3_death.png");
    texIds.push_back("graphics/skeleton4_run.png");
    texIds.push_back("graphics/skeleton4_death.png");
    texIds.push_back("graphics/skeleton5_run.png");
    texIds.push_back("graphics/skeleton5_death.png");
    texIds.push_back("graphics/skeleton6_run.png");
    texIds.push_back("graphics/skeleton6_death.png");
    texIds.push_back("graphics/background_forest.png");
    texIds.push_back("graphics/Knife.png");

    fontIds.push_back("fonts/DS-DIGIT.ttf");

    // LMJ: Load all animation clips
    ANI_CLIP_MGR.Load("animations/run.csv");
    ANI_CLIP_MGR.Load("animations/death.csv");
    ANI_CLIP_MGR.Load("animations/bat1_run.csv");
    ANI_CLIP_MGR.Load("animations/bat1_death.csv");
    ANI_CLIP_MGR.Load("animations/ghoul1_run.csv");
    ANI_CLIP_MGR.Load("animations/ghoul1_death.csv");
    ANI_CLIP_MGR.Load("animations/ghoul2_run.csv");
    ANI_CLIP_MGR.Load("animations/ghoul2_death.csv");
    ANI_CLIP_MGR.Load("animations/ghoul3_run.csv");
    ANI_CLIP_MGR.Load("animations/ghoul3_death.csv");
    ANI_CLIP_MGR.Load("animations/skeleton1_run.csv");
    ANI_CLIP_MGR.Load("animations/skeleton1_death.csv");
    ANI_CLIP_MGR.Load("animations/skeleton2_run.csv");
    ANI_CLIP_MGR.Load("animations/skeleton2_death.csv");
    ANI_CLIP_MGR.Load("animations/skeleton3_run.csv");
    ANI_CLIP_MGR.Load("animations/skeleton3_death.csv");
    ANI_CLIP_MGR.Load("animations/skeleton4_run.csv");
    ANI_CLIP_MGR.Load("animations/skeleton4_death.csv");
    ANI_CLIP_MGR.Load("animations/skeleton5_run.csv");
    ANI_CLIP_MGR.Load("animations/skeleton5_death.csv");
    ANI_CLIP_MGR.Load("animations/skeleton6_run.csv");
    ANI_CLIP_MGR.Load("animations/skeleton6_death.csv");

    // LMJ: Create player
    player = new Player("Player");
    AddGameObject(player);

    // LMJ: Create tiled map background
    tiledMap = new TiledMap("graphics/background_forest.png", "ForestMap");
    AddGameObject(tiledMap);

    // LMJ: Create weapon manager
    weaponManager = new WeaponMgr("WeaponManager");
    weaponManager->SetOwner(player);
    weaponManager->SetScene(this);
    AddGameObject(weaponManager);

    // LMJ: Create monster spawner
    monsterSpawner = new MonsterSpawner("MonsterSpawner");
    monsterSpawner->SetTargetPlayer(player);
    monsterSpawner->SetScene(this);
    AddGameObject(monsterSpawner);

    // LMJ: Create UI elements
    textHp = new TextGo("fonts/DS-DIGIT.ttf", "TextHp");
    textHp->sortingLayer = SortingLayers::UI;
    textHp->sortingOrder = 100;
    AddGameObject(textHp);

    textLevel = new TextGo("fonts/DS-DIGIT.ttf", "TextLevel");
    textLevel->sortingLayer = SortingLayers::UI;
    textLevel->sortingOrder = 100;
    AddGameObject(textLevel);

    textExp = new TextGo("fonts/DS-DIGIT.ttf", "TextExp");
    textExp->sortingLayer = SortingLayers::UI;
    textExp->sortingOrder = 100;
    AddGameObject(textExp);

    textTimer = new TextGo("fonts/DS-DIGIT.ttf", "TextTimer");
    textTimer->sortingLayer = SortingLayers::UI;
    textTimer->sortingOrder = 100;
    AddGameObject(textTimer);

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

    // LMJ: Set map bounds for camera constraints
    if (tiledMap)
    {
        mapBounds = tiledMap->GetMapBounds();
    }

    // LMJ: Give player starting weapon
    if (weaponManager)
    {
        weaponManager->AddWeapon(WeaponType::Knife);
    }

    // LMJ: Configure monster spawner
    if (monsterSpawner)
    {
        monsterSpawner->SetMaxMonsters(30);
        monsterSpawner->SetSpawnDistance(600.0f);
        monsterSpawner->SetBaseSpawnInterval(1.5f);
    }

    // LMJ: Reset game state
    gameTimer = 300.0f; // 5 minutes
    isGameRunning = true;

    Scene::Enter();
}

void SceneGame::Exit()
{
    Scene::Exit();
}

void SceneGame::Update(float dt)
{
    if (!isGameRunning) return;

    Scene::Update(dt);

    // LMJ: Update game timer
    UpdateGameTimer(dt);

    // LMJ: Update UI
    UpdateUI(dt);

    // LMJ: Update camera to follow player
    if (player)
    {
        UpdateCameraWithBounds(player->GetPosition());
    }

    // LMJ: Check game over conditions
    CheckGameOver();

    // LMJ: Update collision manager
    CollisionManager::Update(dt);

#ifdef DEF_DEV
    // LMJ: Debug input for testing
    if (InputMgr::GetKeyDown(sf::Keyboard::F1))
    {
        if (monsterSpawner)
        {
            monsterSpawner->SpawnRandomMonster();
        }
    }

    if (InputMgr::GetKeyDown(sf::Keyboard::F2))
    {
        if (weaponManager)
        {
            weaponManager->UpgradeRandomWeapon();
        }
    }

    if (InputMgr::GetKeyDown(sf::Keyboard::F3))
    {
        if (monsterSpawner)
        {
            monsterSpawner->KillAllMonsters();
        }
    }
#endif
}

void SceneGame::Draw(sf::RenderWindow& window)
{
    Scene::Draw(window);

#ifdef DEF_DEV
    // LMJ: Draw collision debug if enabled
    if (Variables::isDrawHitBox)
    {
        CollisionManager::DrawAll(window);
    }
#endif
}

void SceneGame::UpdateGameTimer(float dt)
{
    if (gameTimer > 0.0f)
    {
        gameTimer -= dt;
        if (gameTimer <= 0.0f)
        {
            gameTimer = 0.0f;
            isGameRunning = false;
            std::cout << "Game Completed! Player survived!" << std::endl;
        }
    }
}

void SceneGame::UpdateUI(float dt)
{
    if (!player) return;

    // LMJ: Update HP display
    if (textHp)
    {
        textHp->SetString("HP: " + std::to_string(player->GetCurrentHp()) +
            "/" + std::to_string(player->GetFinalMaxHP()));
        textHp->SetPosition(sf::Vector2f(10.0f, 10.0f));
        textHp->SetFillColor(sf::Color::Red);
        textHp->SetCharacterSize(24);
    }

    // LMJ: Update Level display
    if (textLevel)
    {
        textLevel->SetString("Level: " + std::to_string(player->GetLevel()));
        textLevel->SetPosition(sf::Vector2f(10.0f, 40.0f));
        textLevel->SetFillColor(sf::Color::Yellow);
        textLevel->SetCharacterSize(24);
    }

    // LMJ: Update Experience display
    if (textExp)
    {
        int currentExp = player->GetExperience();
        int expToNext = player->GetExperienceToNextLevel();
        textExp->SetString("EXP: " + std::to_string(currentExp) + "/" + std::to_string(expToNext));
        textExp->SetPosition(sf::Vector2f(10.0f, 70.0f));
        textExp->SetFillColor(sf::Color::Cyan);
        textExp->SetCharacterSize(24);
    }

    // LMJ: Update Timer display
    if (textTimer)
    {
        int minutes = static_cast<int>(gameTimer) / 60;
        int seconds = static_cast<int>(gameTimer) % 60;
        textTimer->SetString("Time: " + std::to_string(minutes) + ":" +
            (seconds < 10 ? "0" : "") + std::to_string(seconds));
        textTimer->SetPosition(sf::Vector2f(10.0f, 100.0f));
        textTimer->SetFillColor(sf::Color::White);
        textTimer->SetCharacterSize(24);
    }

    // LMJ: Monster count display (debug info)
    if (monsterSpawner)
    {
        static TextGo* monsterCountText = nullptr;
        if (!monsterCountText)
        {
            monsterCountText = new TextGo("fonts/DS-DIGIT.ttf", "MonsterCount");
            monsterCountText->sortingLayer = SortingLayers::UI;
            monsterCountText->sortingOrder = 100;
            AddGameObject(monsterCountText);
        }

        int monsterCount = monsterSpawner->GetActiveMonsterCount();
        float difficulty = monsterSpawner->GetDifficultyMultiplier();
        monsterCountText->SetString("Monsters: " + std::to_string(monsterCount) +
            " | Difficulty: " + std::to_string(static_cast<int>(difficulty * 100)) + "%");
        monsterCountText->SetPosition(sf::Vector2f(10.0f, 130.0f));
        monsterCountText->SetFillColor(sf::Color::Green);
        monsterCountText->SetCharacterSize(20);
    }
}

void SceneGame::CheckGameOver()
{
    if (!player) return;

    if (player->IsDead())
    {
        isGameRunning = false;
        std::cout << "Game Over! Player defeated!" << std::endl;

        // LMJ: Clean up monsters
        if (monsterSpawner)
        {
            monsterSpawner->KillAllMonsters();
        }
    }
}

void SceneGame::UpdateCameraWithBounds(const sf::Vector2f& playerPos)
{
    sf::Vector2f cameraPos = playerPos;
    sf::Vector2f viewSize = worldView.getSize();

    // LMJ: Constrain camera to map bounds if available
    if (mapBounds.width > 0 && mapBounds.height > 0)
    {
        float halfViewWidth = viewSize.x * 0.5f;
        float halfViewHeight = viewSize.y * 0.5f;

        // LMJ: Clamp camera position
        if (cameraPos.x - halfViewWidth < mapBounds.left)
            cameraPos.x = mapBounds.left + halfViewWidth;
        else if (cameraPos.x + halfViewWidth > mapBounds.left + mapBounds.width)
            cameraPos.x = mapBounds.left + mapBounds.width - halfViewWidth;

        if (cameraPos.y - halfViewHeight < mapBounds.top)
            cameraPos.y = mapBounds.top + halfViewHeight;
        else if (cameraPos.y + halfViewHeight > mapBounds.top + mapBounds.height)
            cameraPos.y = mapBounds.top + mapBounds.height - halfViewHeight;
    }

    worldView.setCenter(cameraPos);
}
