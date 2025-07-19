#pragma once
#include "GameObject.h"
#include "HitBox.h"

class WeaponMgr;
class TiledMap;

// LMJ: Player stats structure for weapon scaling
struct PlayerStats
{
    // LMJ: Weapon stats
    float mightMultiplier = 1.0f;      // Damage multiplier
    float areaMultiplier = 1.0f;       // Area/size multiplier  
    float speedMultiplier = 1.0f;      // Projectile speed multiplier
    int amountBonus = 0;               // Additional projectiles
    float durationMultiplier = 1.0f;   // Duration multiplier
    float cooldownMultiplier = 1.0f;   // Cooldown multiplier (lower = faster)
    float luckBonus = 0.0f;            // Luck bonus (0.01 = 1%)
    float critChance = 0.05f;          // Critical hit chance (0.05 = 5%)
    float critMultiplier = 2.0f;       // Critical damage multiplier

    // LMJ: Player stats
    float moveSpeedMultiplier = 1.0f;  // Player movement speed
    float healthMultiplier = 1.0f;     // Max health multiplier
    float expMultiplier = 1.0f;        // Experience gain multiplier
    float recoveryBonus = 0.0f;        // Health recovery per second
    float armorValue = 0.0f;           // Damage reduction
    float invincibilityBonus = 0.0f;   // Additional invincibility time

    PlayerStats() = default;
};

class Player : public GameObject
{
private:
    void UpdateAnimation();
    void OnDeathAnimationComplete() { deathAnimationFinished = true; SetActive(false); }
    void CheckMapBoundaries(sf::Vector2f& newPosition);

protected:
    sf::Sprite sprite;
    Animator animator;
    sf::Vector2f velocity{};
    sf::Vector2f direction{};

    // LMJ: Base stats
    float baseSpeed = 200.0f;
    int maxHp = 100;
    int currentHp = 100;

    // LMJ: Level system
    int level = 1;
    int experience = 0;
    int experienceToNextLevel = 100;

    // LMJ: Combat stats
    float invincibleTime = 0.f;
    bool facingRight = true;
    bool isDead = false;
    bool deathAnimationFinished = false;

    // LMJ: Player stats for weapon scaling
    PlayerStats playerStats;

    // LMJ: HitBox system
    PlayerHitBox* hitBox = nullptr;
    float hitBoxRadius = 20.f;
    float playerRadius = 16.f; // For map boundary checking

    // LMJ: Map reference for boundary checking
    TiledMap* currentMap = nullptr;

    // LMJ: Weapon manager reference
    WeaponMgr* weaponMgr = nullptr;

public:
    Player(const std::string& name);
    ~Player();

    void SetPosition(const sf::Vector2f& pos) override;
    void SetRotation(float angle) override;
    void SetScale(const sf::Vector2f& s) override;
    void SetOrigin(Origins preset) override;
    void SetOrigin(const sf::Vector2f& newOrigin) override;
    sf::FloatRect GetLocalBounds() const override;
    sf::FloatRect GetGlobalBounds() const override;

    void Init() override;
    void Release() override;
    void Reset() override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    void TakeDamage(int damage) override;
    void HandleInput(float dt = 0.f);

    // LMJ: HitBox management
    PlayerHitBox* GetHitBox() const { return hitBox; }
    void SetHitBoxRadius(float radius);

    // LMJ: Level system
    void GainExperience(int exp);
    void LevelUp();
    void UpdateStats();
    void ApplyStatsToAttributes();
    void ModifyStats(const PlayerStats& modification);
    void ResetStatsToBase();

    // LMJ: Health system
    void Heal(int amount);

    // LMJ: Getters
    sf::Vector2f GetPosition() const { return position; }
    sf::Vector2f GetVelocity() const { return velocity; }
    int GetCurrentHp() const { return currentHp; }
    int GetLevel() const { return level; }
    int GetExperience() const { return experience; }
    int GetExperienceToNextLevel() const { return experienceToNextLevel; }
    float GetSpeed() const { return baseSpeed; }
    bool IsDead() const { return isDead; }
    bool IsDeathAnimationFinished() const { return deathAnimationFinished; }

    // LMJ: Calculated final stats
    int GetFinalMaxHP() const { return static_cast<int>(maxHp * playerStats.healthMultiplier); }
    float GetFinalMoveSpeed() const { return baseSpeed * playerStats.moveSpeedMultiplier; }
    float GetFinalInvincibilityDuration() const { return 1.0f + playerStats.invincibilityBonus; }

    // LMJ: Stats access
    const PlayerStats& GetPlayerStats() const { return playerStats; }

    // LMJ: Map reference
    void SetCurrentMap(TiledMap* map) { currentMap = map; }
    TiledMap* GetCurrentMap() const { return currentMap; }

    // LMJ: Weapon manager reference
    void SetWeaponManager(WeaponMgr* mgr) { weaponMgr = mgr; }
    WeaponMgr* GetWeaponManager() const { return weaponMgr; }
};