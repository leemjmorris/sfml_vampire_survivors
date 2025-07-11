#pragma once
#include "GameObject.h"
#include "Animator.h"

class Player :  public GameObject
{
protected:
    sf::Sprite sprite;
    Animator animator;

    sf::Vector2f velocity;
    sf::Vector2f direction;
    float speed = 200.0f;
    
    int maxHp = 100;
    int currentHp = 100;
    int level = 1;
    int experience = 0;
    int experienceToNextLevel = 100;

    std::string currentAnimation = "idle";
    bool facingRight = true;

    float invincibleTime = 0.f;
    float invincibleDuration = 0.f;
public:
    Player(const std::string& name = "Player");
    ~Player() = default;

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

    sf::Vector2f GetVelocity() const { return velocity; }
    int GetCurrentHp() const { return currentHp; }
    int GetMaxHp() const { return maxHp; }
    int GetLevel() const { return level; }
    int GetExperience() const { return experience; }
    int GetExperienceToNext() const { return experienceToNextLevel; }
    bool IsInvincible() const { return invincibleTime > 0.f; }

    void SetSpeed(float newSpeed) { speed = newSpeed; }
    void SetMaxHp(int newMaxHp) { maxHp = newMaxHp; }
};