#pragma once
#include "GameObject.h"
#include "HitBox.h"

class Player;

class ExpOrb : public GameObject
{
private:
    sf::Sprite sprite;
    int expValue = 10;
    float attractRadius = 50.0f;
    float attractSpeed = 250.0f;
    bool isBeingAttracted = false;
    Player* target = nullptr;

    // LMJ: HitBox for pickup detection
    HitBox* hitBox = nullptr;
    float hitBoxRadius = 1.0f;

    void UpdateAttraction(float dt);
    void CheckPlayerProximity();
public:
    ExpOrb(const std::string& name = "ExpOrb");
    ~ExpOrb();

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

    // LMJ: Setters
    void SetExpValue(int value) { expValue = value; }
    void SetTarget(Player* player) { target = player; }

    // LMJ: Getters
    int GetExpValue() const { return expValue; }
    HitBox* GetHitBox() const { return hitBox; }
    bool ShouldBeRemoved() const { return !active; }
};