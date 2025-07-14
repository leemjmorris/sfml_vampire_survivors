#pragma once
#include "GameObject.h"
#include "Player.h"
#include <vector>
#include <unordered_set>

class Player;
class Enemy;
class Scene;

// LMJ: Forward declaration - PlayerStats is now defined in Player.h
struct PlayerStats;

enum class WeaponType
{
    Knife,
    // LMJ: Future weapons can be added here
    Count
};

enum class WeaponRarity
{
    Common,
    Uncommon,
    Rare,
    Epic,
    Legendary
};

struct Projectile
{
    sf::Sprite sprite;
    sf::Vector2f velocity;
    sf::Vector2f startPosition;

    // LMJ: Lifetime management
    float lifeTime = 3.0f;
    float currentLife = 0.0f;
    bool active = true;

    // LMJ: Damage properties
    int baseDamage = 0;
    int actualDamage = 0;
    bool isCritical = false;

    // LMJ: Penetration system
    int maxPierce = 1;
    int currentPierce = 0;
    std::unordered_set<void*> hitEnemies; // LMJ: Track hit enemies for pierce system

    // LMJ: Combat properties
    float knockback = 1.0f;
    bool blockByWalls = true;

    // LMJ: Hitbox delay system
    float hitboxDelay = 0.1f;
    float lastHitTime = 0.0f;

    // LMJ: Visual properties
    float area = 1.0f; // For scaling sprite size
    WeaponType weaponType;

    Projectile() = default;
    Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, int dmg,
        const std::string& textureId, WeaponType type, int pierceCount = 1,
        float kb = 1.0f, float lifetime = 3.0f);

    void Update(float dt);
    void Draw(sf::RenderWindow& window);
    bool IsAlive() const { return active && currentLife < lifeTime && currentPierce < maxPierce; }
    bool CanHitEnemy(void* enemy) const;
    void OnHitEnemy(void* enemy);
    void ApplyAreaScaling(float areaMultiplier);
};

struct WeaponInfo
{
    WeaponType type;
    std::string name;
    std::string textureId;
    WeaponRarity rarity;

    // LMJ: Level system
    int level = 1;
    int maxLevel = 8;

    // LMJ: Base stats (from weapon definition)
    int baseDamage = 10;
    float area = 1.0f;
    float speed = 200.0f;
    int amount = 1;
    float duration = 0.0f;          // LMJ: 0 = instant projectile
    int pierce = 1;
    float cooldown = 1.0f;
    float projectileInterval = 0.0f; // LMJ: Time between projectiles in same attack
    float hitboxDelay = 0.1f;
    float knockback = 1.0f;
    int poolLimit = 50;             // LMJ: Max projectiles on screen
    float chance = 1.0f;            // LMJ: 100% chance by default
    float critMulti = 2.0f;
    bool blockByWalls = true;

    // LMJ: Final calculated stats (after player stats)
    int finalDamage = 10;
    float finalArea = 1.0f;
    float finalSpeed = 200.0f;
    int finalAmount = 1;
    float finalDuration = 0.0f;
    int finalPierce = 1;
    float finalCooldown = 1.0f;
    float finalKnockback = 1.0f;
    float finalChance = 1.0f;

    // LMJ: Internal timing
    float currentCooldown = 0.0f;
    float lastAttackTime = 0.0f;

    WeaponInfo() = default;
    WeaponInfo(WeaponType t, const std::string& n, const std::string& tex, WeaponRarity r = WeaponRarity::Common);

    void CalculateFinalStats(const PlayerStats& playerStats);
    void LevelUp();
    bool CanAttack() const { return currentCooldown <= 0.0f; }
    void StartCooldown() { currentCooldown = finalCooldown; }
};

class WeaponMgr : public GameObject
{
private:
    Player* owner = nullptr;
    Scene* currentScene = nullptr;

    std::vector<WeaponInfo> weapons;
    std::vector<Projectile> projectiles;

    PlayerStats playerStats;

    // LMJ: Attack timing
    float globalAttackTimer = 0.0f;

    // LMJ: Weapon definitions (static data)
    static void InitializeWeaponDefinitions();
    static std::vector<WeaponInfo> weaponDefinitions;
    static bool definitionsInitialized;

public:
    WeaponMgr(const std::string& name = "WeaponManager");
    ~WeaponMgr() = default;

    void Init() override;
    void Release() override;
    void Reset() override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    // LMJ: Setup methods
    void SetOwner(Player* player) { owner = player; }
    void SetScene(Scene* scene) { currentScene = scene; }
    void UpdatePlayerStats(const PlayerStats& stats);

    // LMJ: Weapon management
    void AddWeapon(WeaponType type);
    void UpgradeWeapon(WeaponType type);
    void UpgradeRandomWeapon();
    bool HasWeapon(WeaponType type) const;

    // LMJ: Combat system
    void UpdateWeaponCooldowns(float dt);
    void TryAttackWithAllWeapons(float dt);
    void AttackWithWeapon(WeaponInfo& weapon);

    // LMJ: Projectile management
    void CreateProjectile(const sf::Vector2f& pos, const sf::Vector2f& direction,
        const WeaponInfo& weapon, bool isCritical = false);
    void UpdateProjectiles(float dt);
    void CheckProjectileCollisions();
    void RemoveDeadProjectiles();
    bool IsProjectilePoolFull(const WeaponInfo& weapon) const;

    // LMJ: Utility methods
    sf::Vector2f GetRandomDirection() const;
    sf::Vector2f GetDirectionToNearestEnemy() const;
    sf::Vector2f GetOwnerPosition() const;
    bool RollCritical(float critChance) const;
    bool RollChance(float chance) const;

    // LMJ: Collision detection
    bool CheckProjectileEnemyCollision(const Projectile& projectile, const Enemy* enemy) const;

    // LMJ: Getters
    int GetWeaponCount() const { return static_cast<int>(weapons.size()); }
    int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }
    const WeaponInfo* GetWeapon(WeaponType type) const;
    const std::vector<WeaponInfo>& GetAllWeapons() const { return weapons; }

private:
    // LMJ: Helper methods
    WeaponInfo* FindWeapon(WeaponType type);
    std::vector<Enemy*> GetNearbyEnemies(float range) const;
    void DamageEnemy(Enemy* enemy, int damage, float knockback);
};