#include "stdafx.h"
#include "MonsterSpawner.h"
#include "Player.h"
#include "Scene.h"

SpawnWave::SpawnWave(float time, MonsterType type, int count, float interval,
    const std::string& message, bool repeating)
    : triggerTime(time), monsterType(type), spawnCount(count), spawnInterval(interval),
    waveMessage(message), isRepeating(repeating)
{
}

MonsterSpawner::MonsterSpawner(const std::string& name) : GameObject(name)
{
}

MonsterSpawner::~MonsterSpawner()
{
    KillAllMonsters();
}

void MonsterSpawner::Init()
{
    InitializeMonsterDefinitions();
    InitializeDefaultWaves();

    // LMJ: Initialize wave tracking
    waveTriggered.resize(spawnWaves.size(), false);
}

void MonsterSpawner::Release()
{
    KillAllMonsters();
    monsterDefinitions.clear();
    spawnWaves.clear();
    waveTriggered.clear();
}

void MonsterSpawner::Reset()
{
    sortingLayer = SortingLayers::Default;
    sortingOrder = 0;

    KillAllMonsters();

    // LMJ: Reset spawner state
    gameTime = 0.0f;
    currentSpawnTimer = 0.0f;
    difficultyMultiplier = 1.0f;
    spawnRateMultiplier = 1.0f;
    lastWaveCheckTime = 0.0f;
    monsterUpdateTimer = 0.0f;

    // LMJ: Reset wave triggers
    std::fill(waveTriggered.begin(), waveTriggered.end(), false);

    std::cout << "MonsterSpawner: Reset complete" << std::endl;
}

void MonsterSpawner::Update(float dt)
{
    if (!active || !targetPlayer || !currentScene) return;

    gameTime += dt;
    currentSpawnTimer += dt;
    monsterUpdateTimer += dt;

    // LMJ: Update difficulty scaling
    UpdateDifficulty(gameTime);

    // LMJ: Check for wave events
    CheckAndTriggerWaves(gameTime);

    // LMJ: Regular monster spawning
    float adjustedSpawnInterval = baseSpawnInterval / spawnRateMultiplier;
    if (currentSpawnTimer >= adjustedSpawnInterval)
    {
        if (GetActiveMonsterCount() < maxMonstersOnScreen)
        {
            SpawnRandomMonster();
        }
        currentSpawnTimer = 0.0f;
    }

    // LMJ: Periodic cleanup (optimization)
    if (monsterUpdateTimer >= monsterUpdateInterval)
    {
        RemoveDeadMonsters();
        RemoveDistantMonsters();
        monsterUpdateTimer = 0.0f;
    }
}

void MonsterSpawner::Draw(sf::RenderWindow& window)
{
    // LMJ: MonsterSpawner doesn't draw anything itself
    // Monsters are drawn by the scene
}

void MonsterSpawner::InitializeMonsterDefinitions()
{
    monsterDefinitions.clear();

    // LMJ: Define monsters based on available CSV files
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Bat1, "Bat1", "animations/bat1_run.csv", "animations/bat1_death.csv", "graphics/sprite_bat1_run.png", "graphics/sprite_bat1_death.png", 1, 140.f, 5));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Ghoul1, "Ghoul1", "animations/ghoul1_run.csv", "animations/ghoul1_death.csv", "graphics/ghoul1_run.png", "graphics/ghoul1_death.png", 10, 100.f, 10));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Ghoul2, "Ghoul2", "animations/ghoul2_run.csv", "animations/ghoul2_death.csv", "graphics/ghoul2_run.png", "graphics/ghoul2_death.png", 10, 100.f, 10));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Ghoul3, "Ghoul3", "animations/ghoul3_run.csv", "animations/ghoul3_death.csv", "graphics/ghoul3_run.png", "graphics/ghoul3_death.png", 10, 100.f, 10));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Skeleton1, "Skeleton1", "animations/skeleton1_run.csv", "animations/skeleton1_death.csv", "graphics/skeleton1_run.png", "graphics/skeleton1_death.png", 30, 120.f, 8));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Skeleton2, "Skeleton2", "animations/skeleton2_run.csv", "animations/skeleton2_death.csv", "graphics/skeleton2_run.png", "graphics/skeleton2_death.png", 30, 120.f, 8));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Skeleton3, "Skeleton3", "animations/skeleton3_run.csv", "animations/skeleton3_death.csv", "graphics/skeleton3_run.png", "graphics/skeleton3_death.png", 30, 120.f, 8));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Skeleton4, "Skeleton4", "animations/skeleton4_run.csv", "animations/skeleton4_death.csv", "graphics/skeleton4_run.png", "graphics/skeleton4_death.png", 30, 120.f, 8));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Skeleton5, "Skeleton5", "animations/skeleton5_run.csv", "animations/skeleton5_death.csv", "graphics/skeleton5_run.png", "graphics/skeleton5_death.png", 30, 120.f, 8));
    monsterDefinitions.push_back(MonsterInfo(MonsterType::Skeleton6, "Skeleton6", "animations/skeleton6_run.csv", "animations/skeleton6_death.csv", "graphics/skeleton6_run.png", "graphics/skeleton6_death.png", 30, 120.f, 8));

    // LMJ: Set spawn weights and timing
    for (auto& monster : monsterDefinitions)
    {
        switch (monster.type)
        {
        case MonsterType::Bat1:
        case MonsterType::Ghoul1:
        case MonsterType::Ghoul2:
            monster.spawnWeight = 5.0f;  // Common early monsters
            monster.minGameTime = 0;
            monster.maxSimultaneous = 50;
            break;

        case MonsterType::Ghoul3:
        case MonsterType::Skeleton1:
        case MonsterType::Skeleton2:
            monster.spawnWeight = 3.0f;  // Mid-game monsters
            monster.minGameTime = 60;    // After 1 minute
            monster.maxSimultaneous = 30;
            break;

        case MonsterType::Skeleton3:
        case MonsterType::Skeleton4:
            monster.spawnWeight = 1.5f;  // Late-game monsters
            monster.minGameTime = 120;   // After 2 minutes
            monster.maxSimultaneous = 15;
            break;

        case MonsterType::Skeleton5:
        case MonsterType::Skeleton6:
            monster.spawnWeight = 1.0f;  // Elite monsters
            monster.minGameTime = 180;   // After 3 minutes
            monster.maxSimultaneous = 10;
            break;
        }
    }
    std::cout << "MonsterSpawner: Initialized " << monsterDefinitions.size() << " monster types" << std::endl;
}

void MonsterSpawner::InitializeDefaultWaves()
{
    spawnWaves.clear();

    // LMJ: Early game waves
    spawnWaves.push_back(SpawnWave(30.0f, MonsterType::Bat1, 50, 0.5f, "Bat Swarm Incoming!", false));
    spawnWaves.push_back(SpawnWave(90.0f, MonsterType::Ghoul1, 50, 0.3f, "Ghoul Pack Attack!", false));

    // LMJ: Mid game waves  
    spawnWaves.push_back(SpawnWave(150.0f, MonsterType::Skeleton1, 6, 0.8f, "Skeleton Warriors Rise!", false));
    spawnWaves.push_back(SpawnWave(210.0f, MonsterType::Ghoul2, 10, 0.2f, "Ghoul Horde!", false));

    // LMJ: Late game waves
    spawnWaves.push_back(SpawnWave(270.0f, MonsterType::Skeleton3, 4, 1.0f, "Elite Skeletons Awaken!", false));

    // LMJ: Repeating waves for sustained difficulty
    spawnWaves.push_back(SpawnWave(60.0f, MonsterType::Bat1, 3, 0.3f, "", true));  // Every minute
    spawnWaves.push_back(SpawnWave(120.0f, MonsterType::Ghoul1, 5, 0.4f, "", true)); // Every 2 minutes

    std::cout << "MonsterSpawner: Initialized " << spawnWaves.size() << " spawn waves" << std::endl;
}

void MonsterSpawner::SpawnMonster(MonsterType type, const sf::Vector2f& position)
{
    if (!currentScene || !targetPlayer) return;

    // LMJ: Check if we can spawn this monster type
    if (!CanSpawnMonsterType(type)) return;

    // LMJ: Create monster
    Enemy* monster = CreateMonsterFromType(type, position);
    if (!monster) return;

    // LMJ: Apply difficulty scaling
    ApplyDifficultyToMonster(monster);

    // LMJ: Add to scene and track
    currentScene->AddGameObject(monster);
    activeMonsters.push_back(monster);

    std::cout << "MonsterSpawner: Spawned " << GetMonsterInfo(type)->name
        << " at (" << position.x << ", " << position.y << ")" << std::endl;
}

void MonsterSpawner::SpawnRandomMonster()
{
    if (!targetPlayer) return;

    // LMJ: Select monster type
    MonsterType selectedType = SelectRandomMonsterType();
    if (selectedType == MonsterType::Count) return; // No valid type found

    // LMJ: Get spawn position
    sf::Vector2f spawnPos = GetRandomSpawnPosition();
    if (!IsValidSpawnPosition(spawnPos)) return;

    SpawnMonster(selectedType, spawnPos);
}

Enemy* MonsterSpawner::CreateMonsterFromType(MonsterType type, const sf::Vector2f& position)
{
    const MonsterInfo* info = GetMonsterInfo(type);
    if (!info) return nullptr;

    // LMJ: Create base enemy
    Enemy* monster = new Enemy(info->name);

    monster->Init();
    monster->SetPosition(position);

    monster->LoadAnimations(info->runAnimationPath, info->deathAnimationPath, info->runTexturePath, info->deathTexturePath);

    // LMJ: Apply monster-specific stats
    monster->SetHp(info->baseHp);
    monster->SetSpeed(info->baseSpeed);
    monster->SetDamage(info->baseDamage);
    monster->SetExpValue(info->expValue);
    monster->SetHitBoxRadius(info->hitBoxRadius);

    // LMJ: Target player
    monster->SetTarget(targetPlayer);

    // LMJ: avoidance system
    monster->SetSpawnerReference(this);
    monster->SetAvoidanceRadius(45.f);
    monster->SetAvoidanceForce(0.6f);

    return monster;
}

void MonsterSpawner::ApplyDifficultyToMonster(Enemy* monster)
{
    if (!monster) return;

    // LMJ: Scale stats based on difficulty
    int scaledHp = static_cast<int>(monster->GetEnemyHp() * difficultyMultiplier);
    float scaledSpeed = monster->GetSpeed() * (1.0f + (difficultyMultiplier - 1.0f) * 0.3f); // Speed scales slower
    int scaledDamage = static_cast<int>(monster->GetEnemyDamage() * difficultyMultiplier);
    int scaledExp = static_cast<int>(monster->GetExpValue() * difficultyMultiplier);

    monster->SetHp(scaledHp);
    monster->SetSpeed(scaledSpeed);
    monster->SetDamage(scaledDamage);
    monster->SetExpValue(scaledExp);
}

MonsterType MonsterSpawner::SelectRandomMonsterType() const
{
    std::vector<const MonsterInfo*> availableMonsters;
    std::vector<float> weights;

    // LMJ: Build list of spawnable monsters
    for (const auto& monster : monsterDefinitions)
    {
        if (CanSpawnMonsterType(monster.type))
        {
            availableMonsters.push_back(&monster);
            weights.push_back(monster.spawnWeight);
        }
    }

    if (availableMonsters.empty()) return MonsterType::Count;

    // LMJ: Weighted random selection
    float totalWeight = 0.0f;
    for (float weight : weights)
    {
        totalWeight += weight;
    }

    float randomValue = Utils::RandomRange(0.0f, totalWeight);
    float currentWeight = 0.0f;

    for (size_t i = 0; i < availableMonsters.size(); ++i)
    {
        currentWeight += weights[i];
        if (randomValue <= currentWeight)
        {
            return availableMonsters[i]->type;
        }
    }

    // LMJ: Fallback to first available
    return availableMonsters[0]->type;
}

sf::Vector2f MonsterSpawner::GetRandomSpawnPosition() const
{
    if (!targetPlayer) return sf::Vector2f(0.0f, 0.0f);

    sf::Vector2f playerPos = targetPlayer->GetPosition();
    sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();

    float screenLeft = playerPos.x - windowSize.x * 0.5f;
    float screenRight = playerPos.x + windowSize.x * 0.5f;
    float screenTop = playerPos.y - windowSize.y * 0.5f;
    float screenBottom = playerPos.y - windowSize.y * 0.5f;

    float offscreenMargin = 100.f;

    int side = Utils::RandomRange(0, 4);
    sf::Vector2f spawnPos;

    switch (side)
    {
    case 0: // LMJ: top
        spawnPos.x = Utils::RandomRange(screenLeft - offscreenMargin, screenRight + offscreenMargin);
        spawnPos.y = screenTop - offscreenMargin - Utils::RandomRange(0.0f, 50.0f);
        break;

    case 1: // LMJ: bottom
        spawnPos.x = Utils::RandomRange(screenLeft - offscreenMargin, screenRight + offscreenMargin);
        spawnPos.y = screenBottom + offscreenMargin + Utils::RandomRange(0.0f, 50.0f);
        break;

    case 2: // LMJ: left
        spawnPos.x = screenLeft - offscreenMargin - Utils::RandomRange(0.0f, 50.0f);
        spawnPos.y = Utils::RandomRange(screenTop - offscreenMargin, screenBottom + offscreenMargin);
        break;

    case 3: // LMJ: right
        spawnPos.x = screenRight + offscreenMargin + Utils::RandomRange(0.0f, 50.0f);
        spawnPos.y = Utils::RandomRange(screenTop - offscreenMargin, screenBottom + offscreenMargin);
        break;
    }
    return spawnPos;
}

bool MonsterSpawner::IsValidSpawnPosition(const sf::Vector2f& pos) const
{
    // LMJ: Basic validation - you might want to add map bounds checking
    // For now, just check if it's not too close to player
    if (!targetPlayer) return false;

    float distanceToPlayer = Utils::Distance(pos, targetPlayer->GetPosition());
    return distanceToPlayer >= spawnDistance * 0.8f; // Allow some variance
}

void MonsterSpawner::RemoveDeadMonsters()
{
    auto it = activeMonsters.begin();
    while (it != activeMonsters.end())
    {
        Enemy* monster = *it;
        if (!monster || !monster->GetActive() || monster->IsDead())
        {
            it = activeMonsters.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void MonsterSpawner::RemoveDistantMonsters()
{
    if (!targetPlayer) return;

    sf::Vector2f playerPos = targetPlayer->GetPosition();

    auto it = activeMonsters.begin();
    while (it != activeMonsters.end())
    {
        Enemy* monster = *it;
        if (monster && monster->GetActive())
        {
            float distance = Utils::Distance(playerPos, monster->GetPosition());
            if (distance > despawnDistance)
            {
                // LMJ: Remove from scene and our tracking
                if (currentScene)
                {
                    currentScene->RemoveGameObject(monster);
                }
                it = activeMonsters.erase(it);
                continue;
            }
        }
        ++it;
    }
}

void MonsterSpawner::KillAllMonsters()
{
    if (currentScene)
    {
        for (Enemy* monster : activeMonsters)
        {
            if (monster)
            {
                currentScene->RemoveGameObject(monster);
            }
        }
    }
    activeMonsters.clear();
}

void MonsterSpawner::UpdateDifficulty(float gameTime)
{
    // LMJ: Difficulty scales gradually over time
    difficultyMultiplier = 1.0f + (gameTime / 60.0f) * 0.2f; // +20% per minute

    // LMJ: Spawn rate increases over time
    spawnRateMultiplier = 1.0f + (gameTime / 30.0f) * 0.1f; // +10% every 30 seconds

    // LMJ: Cap the scaling to prevent insane difficulty
    difficultyMultiplier = Utils::Clamp(difficultyMultiplier, 1.0f, 3.0f);
    spawnRateMultiplier = Utils::Clamp(spawnRateMultiplier, 1.0f, 5.0f);
}

void MonsterSpawner::CheckAndTriggerWaves(float currentGameTime)
{
    for (size_t i = 0; i < spawnWaves.size(); ++i)
    {
        const SpawnWave& wave = spawnWaves[i];

        bool shouldTrigger = false;

        if (wave.isRepeating)
        {
            // LMJ: For repeating waves, check if enough time has passed since last check
            if (currentGameTime >= wave.triggerTime &&
                (currentGameTime - lastWaveCheckTime) >= wave.triggerTime)
            {
                shouldTrigger = true;
            }
        }
        else
        {
            // LMJ: For one-time waves, check if not triggered and time reached
            if (!waveTriggered[i] && currentGameTime >= wave.triggerTime)
            {
                shouldTrigger = true;
                waveTriggered[i] = true;
            }
        }

        if (shouldTrigger)
        {
            TriggerWave(wave);
        }
    }

    lastWaveCheckTime = currentGameTime;
}

void MonsterSpawner::TriggerWave(const SpawnWave& wave)
{
    if (!wave.waveMessage.empty())
    {
        ShowWaveMessage(wave.waveMessage);
    }

    std::cout << "MonsterSpawner: Triggering wave - " << wave.waveMessage << std::endl;

    // LMJ: Spawn monsters over time
    for (int i = 0; i < wave.spawnCount; ++i)
    {
        // LMJ: For now, spawn immediately. You might want to implement delayed spawning
        sf::Vector2f spawnPos = GetRandomSpawnPosition();
        if (IsValidSpawnPosition(spawnPos))
        {
            SpawnMonster(wave.monsterType, spawnPos);
        }
    }
}

void MonsterSpawner::ShowWaveMessage(const std::string& message)
{
    // LMJ: For now, just print to console
    // You might want to implement a UI system to show this on screen
    std::cout << "WAVE: " << message << std::endl;
}

bool MonsterSpawner::CanSpawnMonsterType(MonsterType type) const
{
    const MonsterInfo* info = GetMonsterInfo(type);
    if (!info) return false;

    // LMJ: Check time requirement
    if (gameTime < info->minGameTime) return false;

    // LMJ: Check monster count limit
    int currentCount = GetMonsterCountByType(type);
    if (currentCount >= info->maxSimultaneous) return false;

    return true;
}

int MonsterSpawner::GetMonsterCountByType(MonsterType type) const
{
    const MonsterInfo* info = GetMonsterInfo(type);
    if (!info) return 0;

    int count = 0;
    for (const Enemy* monster : activeMonsters)
    {
        if (monster && monster->GetActive() && monster->GetName() == info->name)
        {
            count++;
        }
    }
    return count;
}

const MonsterInfo* MonsterSpawner::GetMonsterInfo(MonsterType type) const
{
    for (const auto& monster : monsterDefinitions)
    {
        if (monster.type == type)
        {
            return &monster;
        }
    }
    return nullptr;
}

void MonsterSpawner::AddSpawnWave(const SpawnWave& wave)
{
    spawnWaves.push_back(wave);
    waveTriggered.push_back(false);
}