#pragma once
#include "GameObject.h"
#include "Animator.h"
#include "HitBox.h"

class TiledMap;
class WeaponMgr;

// Player stats structure moved here from WeaponMgr.h
struct PlayerStats
{
    float mightMultiplier = 1.0f;    // Damage multiplier
    float areaMultiplier = 1.0f;     // Area multiplier
    float speedMultiplier = 1.0f;    // Projectile speed multiplier
    int amountBonus = 0;             // Additional projectiles
    float durationMultiplier = 1.0f; // Duration multiplier
    float cooldownMultiplier = 1.0f; // Cooldown multiplier (lower is better)
    float luckBonus = 0.0f;          // Luck bonus for chance-based effects
    float critChance = 0.05f;        // Base 5% crit chance
    float critMultiplier = 2.0f;     // 2x damage on crit

    // Movement and survival stats
    float moveSpeedMultiplier = 1.0f;
    float healthMultiplier = 1.0f;
    float expMultiplier = 1.0f;
    float recoveryBonus = 0.0f;      // Health recovery per second
    float armorValue = 0.0f;         // Damage reduction
    float invincibilityBonus = 0.0f; // Additional invincibility time
};

class Player : public GameObject
{
protected:
    sf::Sprite sprite;
    Animator animator;

    sf::Vector2f velocity;
    sf::Vector2f direction;
    float baseSpeed = 200.0f;

    int maxHp = 100;
    int currentHp = 100;
    int level = 1;
    int experience = 0;
    int experienceToNextLevel = 100;

    bool facingRight = true;
    bool isDead = false;
    bool deathAnimationFinished = false;

    float invincibleTime = 0.f;
    float baseInvincibleDuration = 1.0f;

    TiledMap* currentMap = nullptr;
    float playerRadius = 32.0f;

    // LMJ: Stats system
    PlayerStats playerStats;
    WeaponMgr* weaponMgr = nullptr;

    // LMJ: HitBox system
    PlayerHitBox* hitBox = nullptr;
    float hitBoxRadius = 16.f;

private:
    void UpdateStats();
    void ApplyStatsToAttributes();

public:
    Player(const std::string& name = "Player");
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

    void HandleInput(float dir);
    void UpdateAnimation();
    void TakeDamage(int damage);
    void GainExperience(int exp);
    void LevelUp();
    void Heal(int amount);

    // Map boundary methods
    void SetCurrentMap(TiledMap* map) { currentMap = map; }
    void CheckMapBoundaries();

    // Weapon system
    void SetWeaponManager(WeaponMgr* mgr) { weaponMgr = mgr; }
    WeaponMgr* GetWeaponManager() const { return weaponMgr; }

    // Stats system
    const PlayerStats& GetPlayerStats() const { return playerStats; }
    void ModifyStats(const PlayerStats& modification); // 스탯 수정 (아이템, 버프 등)
    void ResetStatsToBase(); // 기본 스탯으로 리셋

    // Stat getters for individual values
    float GetFinalMoveSpeed() const { return baseSpeed * playerStats.moveSpeedMultiplier; }
    float GetFinalInvincibilityDuration() const { return baseInvincibleDuration + playerStats.invincibilityBonus; }
    int GetFinalMaxHP() const { return static_cast<int>(maxHp * playerStats.healthMultiplier); }

    PlayerHitBox* GetHitBox() const { return hitBox; }
    void SetHitBoxRadius(float radius);

    // Existing getters
    sf::Vector2f GetVelocity() const { return velocity; }
    int GetCurrentHp() const { return currentHp; }
    int GetMaxHp() const { return maxHp; }
    int GetLevel() const { return level; }
    int GetExperience() const { return experience; }
    int GetExperienceToNext() const { return experienceToNextLevel; }

    bool IsInvincible() const { return invincibleTime > 0.f; }
    bool IsDead() const { return isDead; }
    bool IsDeathAnimationFinished() const { return deathAnimationFinished; }

    void OnDeathAnimationComplete() { deathAnimationFinished = true; }
    void SetSpeed(float newSpeed) { baseSpeed = newSpeed; }
    void SetMaxHp(int newMaxHp) { maxHp = newMaxHp; }
};