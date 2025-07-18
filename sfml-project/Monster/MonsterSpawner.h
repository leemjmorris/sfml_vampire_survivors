#pragma once
#include "GameObject.h"
#include "Enemy.h"

class Player;
class Scene;

enum class MonsterType
{
    Bat1,
    Ghoul1,
    Ghoul2,
    Ghoul3,
    Skeleton1,
    Skeleton2,
    Skeleton3,
    Skeleton4,
    Skeleton5,
    Skeleton6,
    Count
};

struct MonsterInfo
{
    MonsterType type;
    std::string name;
    std::string runAnimationPath;
    std::string deathAnimationPath;
    std::string runTexturePath;
    std::string deathTexturePath;

    // LMJ: Monster stats
    int baseHp = 50;
    float baseSpeed = 100.f;
    int baseDamage = 10;
    int expValue = 10;
    float hitBoxRadius = 20.f;

    // LMJ: Spawn properties
    float spawnWeight = 1.f;
    int minGameTime = 0;
    int maxSimultaneous = 10;

    MonsterInfo() = default;
    MonsterInfo(MonsterType t, const std::string& n, const std::string& runAnim,
        const std::string& deathAnim, const std::string& runTex,
        const std::string& deathTex, int hp, float speed, int damage)
        : type(t), name(n), runAnimationPath(runAnim), deathAnimationPath(deathAnim),
        runTexturePath(runTex), deathTexturePath(deathTex),
        baseHp(hp), baseSpeed(speed), baseDamage(damage) {
    }
};

struct SpawnWave
{
    float triggerTime;
    MonsterType monsterType;
    int spawnCount;
    float spawnInterval;
    bool isRepeating = false;
    std::string waveMessage;

    SpawnWave() = default;
    SpawnWave(float time, MonsterType type, int count, float interval, const std::string& message = "", bool repeating = false);
};

class MonsterSpawner : public GameObject
{
private:
    Player* targetPlayer = nullptr;
    Scene* currentScene = nullptr;

    std::vector<Enemy*> activeMonsters;
    std::vector<MonsterInfo> monsterDefinitions;
    std::vector<SpawnWave> spawnWaves;

    int maxMonstersOnScreen = 500;
    float baseSpawnInterval = 0.f;
    float currentSpawnTimer = 0.f;

    float gameTime = 0.f;
    float difficultyMultiplier = 1.f;
    float spawnRateMultiplier = 1.f;

    float spawnDistance = 800.f;
    float despawnDistance = 1200.f;

    std::vector<bool> waveTriggered;
    float lastWaveCheckTime = 0.f;

    float monsterUpdateTimer = 0.f;
    float monsterUpdateInterval = 1.0f;

    float wraparoundMargin = 150.f;
    float wraparoundCheckInterval = 0.5f;
    float lastWraparoundCheck = 0.f;

    // LMJ: Private helper methods
    void InitializeMonsterDefinitions();
    void InitializeDefaultWaves();
    Enemy* CreateMonsterFromType(MonsterType type, const sf::Vector2f& position);
    void ApplyDifficultyToMonster(Enemy* monster);
    MonsterType SelectRandomMonsterType() const;
    void ShowWaveMessage(const std::string& message);

    // LMJ: 이 함수들이 private에 있어야 합니다
    const MonsterInfo* GetMonsterInfo(MonsterType type) const;
    bool CanSpawnMonsterType(MonsterType type) const;

public:
    MonsterSpawner(const std::string& name = "MonsterSpawner");
    ~MonsterSpawner() override;

    void Init() override;
    void Release() override;
    void Reset() override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    // LMJ: Setup methods
    void SetTargetPlayer(Player* player) { targetPlayer = player; }
    void SetScene(Scene* scene) { currentScene = scene; }

    // LMJ: Configuration methods
    void SetMaxMonsters(int maxCount) { maxMonstersOnScreen = maxCount; }
    void SetSpawnDistance(float distance) { spawnDistance = distance; }
    void SetBaseSpawnInterval(float interval) { baseSpawnInterval = interval; }

    // LMJ: Monster management
    void SpawnMonster(MonsterType type, const sf::Vector2f& position);
    void SpawnRandomMonster();
    void RemoveDeadMonsters();
    void RemoveDistantMonsters();
    void KillAllMonsters();

    // LMJ: Wave system
    void AddSpawnWave(const SpawnWave& wave);
    void CheckAndTriggerWaves(float currentGameTime);
    void TriggerWave(const SpawnWave& wave);

    // LMJ: Difficulty scaling
    void UpdateDifficulty(float gameTime);
    float GetDifficultyMultiplier() const { return difficultyMultiplier; }
    float GetSpawnRateMultiplier() const { return spawnRateMultiplier; }

    // LMJ: Information getters
    int GetActiveMonsterCount() const { return static_cast<int>(activeMonsters.size()); }
    int GetMonsterCountByType(MonsterType type) const;
    std::vector<Enemy*> GetActiveMonsters() const { return activeMonsters; }

    // LMJ: Spawn positioning
    sf::Vector2f GetRandomSpawnPosition() const;
    bool IsValidSpawnPosition(const sf::Vector2f& pos) const;

    // LMJ: Re-placing monsters
    void CheckMonsterWraparound(float dt);
    void WrapAroundMonster(Enemy* monster, const sf::Vector2f& playerPos, const sf::Vector2f& windowSize);
    sf::Vector2f GetBetterWraparoundPosition(const sf::Vector2f& monsterPos, const sf::Vector2f& playerPos, const sf::Vector2f& windowSize);

    void SetCurrentScene(Scene* scene) { currentScene = scene; }
};