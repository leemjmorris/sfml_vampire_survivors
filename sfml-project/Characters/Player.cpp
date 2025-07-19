#include "stdafx.h"
#include "Player.h"
#include "TiledMap.h"

Player::Player(const std::string& name) : GameObject(name)
{
	hitBox = new PlayerHitBox(this);
}

Player::~Player()
{
	if (hitBox)
	{
		CollisionManager::UnregisterHitBox(hitBox);
		delete hitBox;
		hitBox = nullptr;
	}
}

void Player::SetPosition(const sf::Vector2f& pos)
{
	position = pos;
	sprite.setPosition(position);

	if (hitBox)
	{
		hitBox->UpdateTransform(pos);
	}
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
	ANI_CLIP_MGR.Load("animations/death.csv");

	if (hitBox)
	{
		hitBox->SetCircle(hitBoxRadius);
		hitBox->SetActive(true);
	}
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
	isDead = false;
	deathAnimationFinished = false;

	// Initialize stats to base values
	ResetStatsToBase();
	UpdateStats();
	ApplyStatsToAttributes();

	SetOrigin(Origins::MC);

	sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();
	sf::Vector2f centerPos = sf::Vector2f(windowSize.x * 0.5f, windowSize.y * 0.5f);
	SetPosition(centerPos);

	if (TEXTURE_MGR.Exists("graphics/sprite_run.png"))
	{
		animator.Play("animations/run.csv");
	}
}

void Player::Update(float dt)
{
	if (isDead && deathAnimationFinished) return;

	if (invincibleTime > 0.f)
	{
		invincibleTime -= dt;
		if (invincibleTime < 0.f) invincibleTime = 0.f;
	}

	if (!isDead)
	{
		sf::Vector2f newPosition = position + velocity * dt;

		CheckMapBoundaries(newPosition);

		SetPosition(newPosition);
	}

	UpdateAnimation();
	animator.Update(dt);

	if (hitBox && !isDead)
	{
		hitBox->UpdateTransform(position);
	}
}

void Player::CheckMapBoundaries(sf::Vector2f& newPosition)
{
	// LMJ: If map is set, check boundaries and clamp position
	if (!currentMap)
	{
		return;
	}
	newPosition = currentMap->ClampToMapBounds(newPosition, playerRadius);
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
}

void Player::UpdateAnimation()
{
	if (isDead)
	{
		if (!deathAnimationFinished)
		{
			animator.SetSpeed(1.0f);
		}
		// LMJ: trying to fix this part
		else if (deathAnimationFinished)
		{
			animator.SetSpeed(0.f);
		}
	}

	if (direction.x != 0.f || direction.y != 0.f)
	{
		animator.SetSpeed(1.f);
	}
	// LMJ: Trying to fix this part.
	//else if (direction.x == 0.f || direction.y == 0.f)
	//{
	//	if (isDead == false)
	//	{
	//		animator.SetSpeed(0.f);
	//	}
	//	else
	//	{
	//		animator.SetSpeed(1.f);
	//	}
	//}
	// // LMJ: have to fix this part. it iterrupts death animations.
	//else if (isDead == false)
	//{
	//	animator.SetSpeed(0.f);
	//}

	sf::Vector2f currentPos = GetPosition();

	if (facingRight)
	{
		SetScale(sf::Vector2f(-1.f, 1.f));
	}
	else
	{
		SetScale(sf::Vector2f(1.f, 1.f));
	}
	Utils::SetOrigin(sprite, Origins::MC);
	sprite.setPosition(currentPos);
}

void Player::TakeDamage(int damage)
{
	if (invincibleTime > 0.f || isDead) return;

	// Apply armor reduction
	float damageReduction = playerStats.armorValue / (playerStats.armorValue + 100.0f);
	int finalDamage = static_cast<int>(damage * (1.0f - damageReduction));
	finalDamage = std::max(1, finalDamage); // Minimum 1 damage

	currentHp -= finalDamage;
	if (currentHp < 0) currentHp = 0;

	invincibleTime = GetFinalInvincibilityDuration();
	sprite.setColor(sf::Color::Red);

	if (currentHp <= 0)
	{
		isDead = true;
		sprite.setColor(sf::Color::White);

		if (hitBox) hitBox->SetActive(false);

		animator.AddEvent("animations/death.csv", 14, [this]() {
			OnDeathAnimationComplete();
			SetActive(false);
			});

		animator.Play("animations/death.csv");
		std::cout << "Player Dead!" << std::endl;
	}
}

void Player::SetHitBoxRadius(float radius)
{
	hitBoxRadius = radius;
	if (hitBox)
	{
		hitBox->SetCircle(radius);
	}
}

void Player::GainExperience(int exp)
{
	int finalExp = static_cast<int>(exp * playerStats.expMultiplier);
	experience += finalExp;

	while (experience >= experienceToNextLevel)
	{
		experience -= experienceToNextLevel;
		LevelUp();
	}
}

void Player::LevelUp()
{
	level++;

	// Base stat increases per level
	maxHp += 10;
	baseSpeed += 5.f;

	experienceToNextLevel = static_cast<int>(100 * std::pow(level, 1.5f));

	// Update player stats based on new level
	UpdateStats();
	ApplyStatsToAttributes();

	// Heal to new max HP
	currentHp = GetFinalMaxHP();

	std::cout << "Level Up! New Level: " << level << std::endl;
	std::cout << "New Max HP: " << GetFinalMaxHP() << std::endl;
	std::cout << "New Move Speed: " << GetFinalMoveSpeed() << std::endl;
}

void Player::UpdateStats()
{
	// Reset to base and calculate level-based bonuses
	ResetStatsToBase();

	// Level-based stat scaling
	float levelMultiplier = 1.0f + (level - 1) * 0.1f; // 10% increase per level

	playerStats.mightMultiplier = levelMultiplier;
	playerStats.areaMultiplier = 1.0f + (level - 1) * 0.05f; // 5% area increase per level
	playerStats.speedMultiplier = 1.0f + (level - 1) * 0.03f; // 3% speed increase per level
	playerStats.amountBonus = (level - 1) / 3; // +1 projectile every 3 levels
	playerStats.durationMultiplier = 1.0f;
	playerStats.cooldownMultiplier = std::max(0.3f, 1.0f - (level - 1) * 0.05f); // Faster cooldown, min 30%
	playerStats.luckBonus = (level - 1) * 0.01f; // +1% luck per level
	playerStats.critChance = 0.05f + (level - 1) * 0.01f; // +1% crit per level
	playerStats.critMultiplier = 2.0f + (level - 1) * 0.1f; // +0.1x crit multiplier per level

	// Survival stats
	playerStats.moveSpeedMultiplier = 1.0f + (level - 1) * 0.02f; // 2% move speed per level
	playerStats.healthMultiplier = 1.0f + (level - 1) * 0.05f; // 5% HP per level
	playerStats.expMultiplier = 1.0f;
	playerStats.recoveryBonus = (level - 1) * 0.5f; // 0.5 HP/sec per level
	playerStats.armorValue = (level - 1) * 2.0f; // 2 armor per level
	playerStats.invincibilityBonus = (level - 1) * 0.1f; // +0.1 sec invincibility per level
}

void Player::ApplyStatsToAttributes()
{
	// Update max HP based on health multiplier
	int newMaxHp = GetFinalMaxHP();
	if (newMaxHp != maxHp)
	{
		float hpRatio = static_cast<float>(currentHp) / maxHp;
		maxHp = newMaxHp;
		currentHp = static_cast<int>(maxHp * hpRatio); // Maintain HP ratio
	}
}

void Player::ModifyStats(const PlayerStats& modification)
{
	// Add modification values to current stats
	playerStats.mightMultiplier += modification.mightMultiplier - 1.0f;
	playerStats.areaMultiplier += modification.areaMultiplier - 1.0f;
	playerStats.speedMultiplier += modification.speedMultiplier - 1.0f;
	playerStats.amountBonus += modification.amountBonus;
	playerStats.durationMultiplier += modification.durationMultiplier - 1.0f;
	playerStats.cooldownMultiplier = std::min(playerStats.cooldownMultiplier, modification.cooldownMultiplier);
	playerStats.luckBonus += modification.luckBonus;
	playerStats.critChance += modification.critChance - 0.05f;
	playerStats.critMultiplier += modification.critMultiplier - 2.0f;

	playerStats.moveSpeedMultiplier += modification.moveSpeedMultiplier - 1.0f;
	playerStats.healthMultiplier += modification.healthMultiplier - 1.0f;
	playerStats.expMultiplier += modification.expMultiplier - 1.0f;
	playerStats.recoveryBonus += modification.recoveryBonus;
	playerStats.armorValue += modification.armorValue;
	playerStats.invincibilityBonus += modification.invincibilityBonus;

	// Ensure minimum values
	playerStats.mightMultiplier = std::max(0.1f, playerStats.mightMultiplier);
	playerStats.areaMultiplier = std::max(0.1f, playerStats.areaMultiplier);
	playerStats.speedMultiplier = std::max(0.1f, playerStats.speedMultiplier);
	playerStats.cooldownMultiplier = std::max(0.1f, playerStats.cooldownMultiplier);
	playerStats.moveSpeedMultiplier = std::max(0.1f, playerStats.moveSpeedMultiplier);
	playerStats.healthMultiplier = std::max(0.1f, playerStats.healthMultiplier);
	playerStats.expMultiplier = std::max(0.1f, playerStats.expMultiplier);

	ApplyStatsToAttributes();
}

void Player::ResetStatsToBase()
{
	playerStats = PlayerStats(); // Reset to default values
}

void Player::Heal(int amount)
{
	currentHp += amount;
	if (currentHp > GetFinalMaxHP()) currentHp = GetFinalMaxHP();
}

void Player::Draw(sf::RenderWindow& window)
{
	window.draw(sprite);
}