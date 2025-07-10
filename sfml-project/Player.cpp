#include "stdafx.h"
#include "Player.h"
#include "Enemy.h"

Player::Player(const std::string& name)
{
}

void Player::SetPosition(const sf::Vector2f& pos)
{
	position = pos;
	sprite.setPosition(position);
}

void Player::SetRotation(float angle)
{
	rotation = angle;
	sprite.setRotation(rotation);
}

void Player::SetScale(const sf::Vector2f& s)
{
	scale = s;
	sprite.setScale(scale);
}

void Player::SetOrigin(Origins preset)
{
	originPreset = preset;
	if (originPreset != Origins::Custom)
	{
		Utils::SetOrigin(sprite, originPreset);
	}
}

void Player::SetOrigin(const sf::Vector2f& newOrigin)
{
	originPreset = Origins::Custom;
	origin = newOrigin;
	sprite.setOrigin(origin);
}

sf::FloatRect Player::GetLocalBounds() const
{
	return sprite.getLocalBounds();
}

sf::FloatRect Player::GetGlobalBounds() const
{
	return sprite.getGlobalBounds();
}

void Player::Init()
{
	animator.SetTarget(&sprite);

	ANI_CLIP_MGR.Load("animations/idle.csv");
	ANI_CLIP_MGR.Load("animations/run.csv");
}

void Player::Release()
{
	//need this space to clean things up chooochoooo~
}

void Player::Reset()
{
	sortingLayer = SortingLayers::Foreground;
	sortingOrder = 10;

	currentHp = maxHp;
	level = 1;
	experience = 0;
	experienceToNext = 100;
	velocity = sf::Vector2f(0.f, 0.f);
	direction = sf::Vector2f(0.f, 0.f);
	invincibleTime = 0.f;
	facingRight = true;

	animator.Play("animations/idle.csv");
	currentAnimation = "idle";
	SetOrigin(Origins::MC);
}

void Player::Update(float dt)
{
}

void Player::Draw(sf::RenderWindow& window)
{
}

void Player::HandleInput(float dir)
{
}

void Player::UpdateAnimaiton()
{
}
