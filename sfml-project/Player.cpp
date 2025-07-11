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
	experienceToNextLevel = 100;
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
	if (invincibleTime > 0.f)
	{
		invincibleTime -= dt;

		float alpha = (sin(invincibleTime * 20.f) + 1.f) * 0.5f;
		sprite.setColor(sf::Color(255, 255, 255, (alpha * 255)));
	}
	else
	{
		sprite.setColor(sf::Color::White);
	}

	HandleInput(dt);

	position += velocity * dt;
	SetPosition(position);

	sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();
	sf::FloatRect bounds = GetGlobalBounds();

	if (position.x - bounds.width * 0.5f < 0) position.x = bounds.width * 0.5f;
	if (position.x + bounds.width * 0.5f > windowSize.x) position.x = windowSize.x - bounds.width * 0.5f;
	if (position.y - bounds.height * 0.5f < 0) position.y = bounds.height * 0.5f;
	if (position.y + bounds.height * 0.5f > windowSize.y) position.y = windowSize.y - bounds.height * 0.5f;

	SetPosition(position);

	UpdateAnimation();
}

void Player::HandleInput(float dir)
{
	direction = sf::Vector2f(0.f, 0.f);

	// LMJ: Moving Sides
	if (InputMgr::GetKey(sf::Keyboard::A) || InputMgr::GetKey(sf::Keyboard::Left))
	{
		direction.x -= 1.f;
		facingRight = false;
	}
	if (InputMgr::GetKey(sf::Keyboard::D) || InputMgr::GetKey(sf::Keyboard::Right))
	{
		direction.x += 1.f;
		facingRight = true;
	}

	// LMJ: Moving Up
	if (InputMgr::GetKey(sf::Keyboard::W) || InputMgr::GetKey(sf::Keyboard::Up))
	{
		direction.y -= 1.f;
	}
	if (InputMgr::GetKey(sf::Keyboard::S) || InputMgr::GetKey(sf::Keyboard::Down))
	{
		direction.y += 1.f;
	}

	if (direction.x != 0.f && direction.y != 0.f)
	{
		Utils::Normalize(direction);
	}

	velocity = direction * speed;
}

void Player::UpdateAnimation()
{
	std::string newAnimation;

	if (direction.x != 0.f || direction.y != 0.f)
	{
		newAnimation = "run";
	}
	else
	{
		newAnimation = "idle";
	}

	if (currentAnimation != newAnimation)
	{
		currentAnimation = newAnimation;
		if (newAnimation == "run")
		{
			animator.Play("animaitons/run.csv");
		}
		else
		{
			animator.Play("animations/idle.csv");
		}
	}
	
	if (facingRight)
	{
		SetScale(sf::Vector2f(1.f, 1.f));
	}
	else
	{
		SetScale(sf::Vector2f(-1.f, 1.f));
	}
}

void Player::TakeDamage(int damage)
{
	if (invincibleTime > 0.f) return;

	currentHp -= damage;
	if (currentHp < 0) currentHp = 0;
	
	invincibleTime = invincibleDuration;
	sprite.setColor(sf::Color::Red);

	// LMJ: Put Game Over Function HERE!!!!
	if (currentHp <= 0)
	{
		std::cout << "Player Dead!" << std::endl;
	}
}

void Player::GainExperience(int exp)
{
	experience += exp;

	while (experience >= experienceToNextLevel) // LMJ: Level UP Method needed
	{
		experience -= experienceToNextLevel;
		LevelUp();
	}
}

void Player::LevelUp()
{

}

void Player::Draw(sf::RenderWindow& window)
{

}