#pragma once
#include "GameObject.h"
#include "HitBox.h"

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

    EnemyHitBox* hitBox = nullptr;
    float hitBoxRadius = 20.f;

public:
    Enemy(const std::string& name);
    ~Enemy();

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

    virtual void TakeDamage(int damage);
    void FollowPlayer(Player* player);

    EnemyHitBox* GetHitBox() const { return hitBox; }
    void SetHitBoxRadius(float radius);

    // LMJ: Get methods
    sf::Vector2f GetPosition() const;
    sf::Vector2f GetVelocity() const;
    int GetEnemyHp() const;
    int GetEnemyDamage() const;
    int GetExpValue() const;
    float GetSpeed() const { return speed; }

    // LMJ: Set methods
    void SetSpeed(float newSpeed) { speed = newSpeed; }
    void SetHp(int newHp) { hp = newHp; }
    void SetDamage(int newDamage) { damage = newDamage; }
    void SetExpValue(int newExpValue) { expValue = newExpValue; }

    // LMJ: Animation methods
    bool IsDead() const { return isDead; }
    bool IsDeathAnimationFinished() const { return deathAnimationFinished; }

    // LMJ: Monster Spawner implement
    void LoadAnimations(const std::string& runAnimPath, const std::string& deathAnimPath, const std::string& runTexPath, const std::string& deathTexPath);
    void SetTarget(Player* player) { target = player; }
    Player* GetTarget() const { return target; }


};

