#pragma once
#include "GameObject.h"

class Player;

class Enemy :  public GameObject
{
protected:
    sf::Sprite sprite;
    float speed = 100.0f;
    int hp = 50;
    int damage = 10;
    int expValue = 10;
    Player* target = nullptr;

public:
    void Init() override;
    void Release() override;
    void Reset() override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

    void TakeDamage(int damage);
    void FollowPlayer(Player* player);
};

