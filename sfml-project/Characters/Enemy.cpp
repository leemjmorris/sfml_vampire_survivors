#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "Animator.h"
#include "AnimationClip.h"

void Enemy::Init()
{
	// LMJ: why? define? i added all .h. need to check after lunch. ASAP >> animator.SetTarget(&sprite);
}

void Enemy::Release()
{
}

void Enemy::Reset()
{
	sortingLayer = SortingLayers::Foreground;
	sortingOrder = 5;

	// LMJ: Will this affect all the other monsters? I think so... Test this part when we add another monsters.
	speed = 100.f;
	hp = 50;
	damage = 10;
	expValue = 10;

	SetOrigin(Origins::MC);

	// LMJ: Temporary Spawn Location.
	sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();
	SetPosition(sf::Vector2f(windowSize.x * 0.3f, windowSize.y * 0.3f));

	if (TEXTURE_MGR.Exists("graphics/sprite_bat1_run.png"))
	{
		animator.Play("animations/bat1_run.csv");
	}
}

void Enemy::Update(float dt)
{
}

void Enemy::Draw(sf::RenderWindow& window)
{
}

void Enemy::TakeDamage(int damage)
{
}

void Enemy::FollowPlayer(Player* player)
{
}

void Enemy::SetPosition(const sf::Vector2f& pos)
{
}

void Enemy::SetRotation(float angle)
{
}

void Enemy::SetScale(const sf::Vector2f& s)
{
}

void Enemy::SetOrigin(Origins preset)
{
}

void Enemy::SetOrigin(const sf::Vector2f& newOrigin)
{
}


int Enemy::GetEnemyHp() const
{
	return 0;
}

int Enemy::GetEnemyDamage() const
{
	return 0;
}

int Enemy::GetExpValue() const
{
	return 0;
}

sf::FloatRect Enemy::GetLocalBounds() const
{
	return sf::FloatRect();
}

sf::FloatRect Enemy::GetGlobalBounds() const
{
	return sf::FloatRect();
}
