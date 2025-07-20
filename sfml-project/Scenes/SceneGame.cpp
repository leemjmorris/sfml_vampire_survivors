#include "stdafx.h"
#include "SceneGame.h"
#include "Player.h"
#include "TextGo.h"
#include "TiledMap.h"

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
    texIds.push_back("graphics/Gem1.png");

    fontIds.push_back("fonts/perfect.ttf");

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

    // LMJ: Create tiled map background
    tiledMap = (TiledMap*)AddGameObject(new TiledMap("graphics/background_forest.png", "TiledMap"));
    AddGameObject(tiledMap);

    // LMJ: Create player
    player = (Player*)AddGameObject(new Player("Player"));
    tiledMap->SetPlayer(player);

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
    textHp = new TextGo("fonts/perfect.ttf", "TextHp");
    textHp->sortingLayer = SortingLayers::UI;
    textHp->sortingOrder = 100;
    AddGameObject(textHp);

    textLevel = new TextGo("fonts/perfect.ttf", "TextLevel");
    textLevel->sortingLayer = SortingLayers::UI;
    textLevel->sortingOrder = 100;
    AddGameObject(textLevel);

    textExp = new TextGo("fonts/perfect.ttf", "TextExp");
    textExp->sortingLayer = SortingLayers::UI;
    textExp->sortingOrder = 100;
    AddGameObject(textExp);

    textTimer = new TextGo("fonts/perfect.ttf", "TextTimer");
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
        // LMJ: 플레이어에게 맵 정보 전달
        if (player)
        {
            player->SetCurrentMap(tiledMap);
        }
    }

    // LMJ: Player set position
    if (player)
    {
        player->SetPosition(sf::Vector2f(0.0f, 0.0f)); // LMJ: 월드 원점에서 시작
    }

    // LMJ: Give player starting weapon
    if (weaponManager)
    {
        weaponManager->AddWeapon(WeaponType::Knife);
    }

    // LMJ: Configure monster spawner
    if (monsterSpawner)
    {
        monsterSpawner->SetMaxMonsters(500);
        monsterSpawner->SetSpawnDistance(600.0f);
        monsterSpawner->SetBaseSpawnInterval(0.f);
        monsterSpawner->SetCurrentScene(this);
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

    // LMJ: Debug input for testing
    if (InputMgr::GetKeyDown(sf::Keyboard::F1))
    {
        if (monsterSpawner)
        {
            monsterSpawner->SpawnRandomMonster();
            std::cout << "Spawned random monster (F1)" << std::endl;
        }
    }

    if (InputMgr::GetKeyDown(sf::Keyboard::F2))
    {
        if (weaponManager)
        {
            weaponManager->UpgradeRandomWeapon();
            std::cout << "Upgraded random weapon (F2)" << std::endl;
        }
    }

    if (InputMgr::GetKeyDown(sf::Keyboard::F3))
    {
        if (monsterSpawner)
        {
            monsterSpawner->KillAllMonsters();
            std::cout << "Killed all monsters (F3)" << std::endl;
        }
    }

    // LMJ: 추가 디버그 키들
    if (InputMgr::GetKeyDown(sf::Keyboard::I))
    {
        if (weaponManager && !weaponManager->HasWeapon(WeaponType::Knife))
        {
            weaponManager->AddWeapon(WeaponType::Knife);
            std::cout << "Added Knife weapon (I)" << std::endl;
        }
    }

    if (InputMgr::GetKeyDown(sf::Keyboard::U))
    {
        if (weaponManager)
        {
            weaponManager->UpgradeWeapon(WeaponType::Knife);
            std::cout << "Upgraded Knife weapon (U)" << std::endl;
        }
    }

    if (InputMgr::GetKeyDown(sf::Keyboard::O))
    {
        if (player)
        {
            player->GainExperience(50);
            std::cout << "Gained 50 experience (O)" << std::endl;
        }
    }

    if (InputMgr::GetKeyDown(sf::Keyboard::P))
    {
        if (player)
        {
            player->TakeDamage(10);
            std::cout << "Player took 10 damage (P)" << std::endl;
        }
    }
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
            monsterCountText = new TextGo("fonts/perfect.ttf", "MonsterCount");
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

    // LMJ: Weapon info display
    if (weaponManager)
    {
        static TextGo* weaponInfoText = nullptr;
        if (!weaponInfoText)
        {
            weaponInfoText = new TextGo("fonts/perfect.ttf", "WeaponInfo");
            weaponInfoText->sortingLayer = SortingLayers::UI;
            weaponInfoText->sortingOrder = 100;
            AddGameObject(weaponInfoText);
        }

        int weaponCount = weaponManager->GetWeaponCount();
        int projectileCount = weaponManager->GetProjectileCount();
        weaponInfoText->SetString("Weapons: " + std::to_string(weaponCount) +
            " | Projectiles: " + std::to_string(projectileCount));
        weaponInfoText->SetPosition(sf::Vector2f(10.0f, 160.0f));
        weaponInfoText->SetFillColor(sf::Color::Magenta);
        weaponInfoText->SetCharacterSize(20);
    }

    // LMJ: Debug keys info
    static TextGo* debugKeysText = nullptr;
    if (!debugKeysText)
    {
        debugKeysText = new TextGo("fonts/perfect.ttf", "DebugKeys");
        debugKeysText->sortingLayer = SortingLayers::UI;
        debugKeysText->sortingOrder = 100;
        AddGameObject(debugKeysText);
    }

    debugKeysText->SetString("Debug: F1-Spawn F2-Upgrade F3-Kill | I-AddWeapon U-UpgradeWeapon O-Exp P-Damage");
    debugKeysText->SetPosition(sf::Vector2f(10.0f, FRAMEWORK.GetWindowSizeF().y - 30.0f));
    debugKeysText->SetFillColor(sf::Color(128, 128, 128));
    debugKeysText->SetCharacterSize(16);
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
    sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();

    worldView.setCenter(playerPos);
    worldView.setSize(windowSize);
}