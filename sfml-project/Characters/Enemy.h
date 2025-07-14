#pragma once
#include "GameObject.h"

class Player;

class Enemy :  public GameObject
{
private:
    void UpdateAnimation();
    void OnDeathAnimationComplete() { SetActive(false); }

protected:
    sf::Sprite sprite;
    Animator animator;
    sf::Vector2f velocity{};

    float speed = 100.0f;
    int hp = 50;
    int damage = 10;
    int expValue = 10;

    bool facingRight = true;
    bool isDead = false;
    bool deathAnimationFinished = false;

    Player* target = nullptr;

public:
    Enemy(const std::string& name = "Enemy") : GameObject(name) {}
    ~Enemy() = default;

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

    void TakeDamage(int damage);
    void FollowPlayer(Player* player);

    // LMJ: Get methods
    sf::Vector2f GetPosition() const;
    sf::Vector2f GetVelocity() const;
    int GetEnemyHp() const;
    int GetEnemyDamage() const;
    int GetExpValue() const;

    // LMJ: Set methods
    void SetSpeed(float newSpeed) { speed = newSpeed; }
    void SetHp(int newHp) { hp = newHp; }
    void SetDamage(int newDamage) { damage = newDamage; }
    void SetExpValue(int newExpValue) { expValue = newExpValue; }

    // LMJ: Map boundary check methods
    // LMJ: Animation methods
    bool IsDead() const { return isDead; }
    bool IsDeathAnimationFinished() const { return deathAnimationFinished; }
    // LMJ: After adding Object Class, EXP drop related methods needed
};

