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

	ANI_CLIP_MGR.Load("animations/run.csv");
	animator.Play("animations/run.csv");

	std::cout << "Player initialized with run animation. checking for debugs" << std::endl;

}

void Player::Release()
{
	// LMJ: need this space to clean things up chooochoooo~
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

	SetOrigin(Origins::MC);

	// LMJ: Player spawn in center of display.
	sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();
	sf::Vector2f centerPos = sf::Vector2f(windowSize.x * 0.5f, windowSize.y * 0.5f);
	SetPosition(centerPos);

	if (TEXTURE_MGR.Exists("graphics/character_sheet_1.png"))
	{
		animator.Play("animations/run.csv");
	}	
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

	animator.Update(dt);
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

void Player::UpdateAnimation() // LMJ: check if this part is only initialized once every time.
{
	std::string newAnimation;

	if (direction.x != 0.f || direction.y != 0.f)
	{
		animator.SetSpeed(1.0f);
	}
	else
	{
		animator.SetSpeed(0.f);
	}
	
	if (facingRight)
	{
		SetScale(sf::Vector2f(-1.f, 1.f));
	}
	else
	{
		SetScale(sf::Vector2f(1.f, 1.f));
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
	level++;

	maxHp += 10;
	currentHp = maxHp;
	speed += 5.f;

	// LMJ: Levelup EXP Auto Increase. Needed exp will increase rapidly when level gets high.
	experienceToNextLevel = static_cast<int>(10 * std::pow(level, 1.5f));

	std::cout << "Level Up! New Level: " << level << std::endl; // LMJ: For debug purpose.
}

void Player::Heal(int amount)
{
	currentHp += amount;
	if (currentHp > maxHp) currentHp = maxHp;
}

void Player::Draw(sf::RenderWindow& window)
{
	window.draw(sprite);
}