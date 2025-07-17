#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "Animator.h"
#include "AnimationClip.h"
#include "MonsterSpawner.h"

Enemy::Enemy(const std::string& name) : GameObject(name)
{
	hitBox = new EnemyHitBox(this);
}

Enemy::~Enemy()
{
	if (hitBox)
	{
		CollisionManager::UnregisterHitBox(hitBox);
		delete hitBox;
		hitBox = nullptr;
	}
}

void Enemy::Init()
{
	animator.SetTarget(&sprite);

	if (hitBox)
	{
		hitBox->SetCircle(hitBoxRadius);
		hitBox->SetActive(true);
	}
}

void Enemy::Release()
{
}

void Enemy::Reset()
{
	sortingLayer = SortingLayers::Foreground;
	sortingOrder = 15;

	// LMJ: Will this affect all the other monsters? I think so... Test this part when we add another monsters.
	// LMJ: Yes this affects the base stats. Changed reset baseStats only if they are in "IF" conditions.
	if (speed <= 0) speed = 100.f;
	if (hp <= 0) hp = 50;
	if (damage <= 0) damage = 10;
	if (expValue <= 0) expValue = 10;

	SetOrigin(Origins::MC);

	// LMJ: Temporary Spawn Location.
	// LMJ: Now only spawned this location when "if" happens
	if (position.x == 0 && position.y == 0)
	{
		sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();
		SetPosition(sf::Vector2f(windowSize.x * 0.3f, windowSize.y * 0.3f));
	}
}

void Enemy::Update(float dt)
{
	if (isDead)
	{
		animator.Update(dt);

		if (hitBox) hitBox->SetActive(false);
		return;
	}

	if (hp <= 0)
	{
		isDead = true;
		if (hitBox) hitBox->SetActive(false);

		std::string currentClip = animator.GetCurrentClipId();
		std::string deathClip = currentClip;

		size_t runPos = deathClip.find("_run");
		if (runPos != std::string::npos)
		{
			deathClip.replace(runPos, 4, "_death");
		}
		else { return; }

		// LMJ: Set death animation event
		animator.AddEvent(deathClip, 14, [this]()
			{
				OnDeathAnimationComplete();
			});

		animator.Play(deathClip);
	}

	if (target != nullptr && !isDead)
	{
		FollowPlayer(target);
	}

	if (!isDead)
	{
		position += velocity * dt;
		SetPosition(position);
	}

	if (hitBox && !isDead)
	{
		hitBox->UpdateTransform(sprite.getPosition());
	}

	UpdateAnimation();
	animator.Update(dt);
}

void Enemy::UpdateAnimation()
{
	if (isDead) return;

	if (velocity.x != 0.f || velocity.y != 0.f)
	{
		animator.SetSpeed(1.f);

		if (velocity.x > 0.f && !facingRight)
		{
			facingRight = true;
			SetScale(sf::Vector2f(-1.f, 1.f));
		}

		else if (velocity.x < 0.f && facingRight)
		{
			facingRight = false;
			SetScale(sf::Vector2f(1.f, 1.f));
		}
	}
	else
	{
		animator.SetSpeed(0.5f);
	}

	sf::Vector2f currentPos = GetPosition();
	Utils::SetOrigin(sprite, Origins::MC);
	sprite.setPosition(currentPos);
}

void Enemy::Draw(sf::RenderWindow& window)
{
	if (active)
	{
		window.draw(sprite);
	}
}

void Enemy::TakeDamage(int damage)
{
	if (isDead) return;

	hp -= damage;
	if (hp < 0) hp = 0;

	// LMJ: Visual feedback for taking damage
	sprite.setColor(sf::Color::Red);
	// Reset color immediately (you might want to add a timer for this)
	sprite.setColor(sf::Color::White);

	if (hp <= 0)
	{
		isDead = true;
		sprite.setColor(sf::Color::White);

		if (hitBox) hitBox->SetActive(false);

		// LMJ: Give experience to player when enemy dies
		if (target)
		{
			target->GainExperience(expValue);
		}

		std::string deathClip = "";
		std::string enemyName = GetName();

		if (enemyName == "Bat1") deathClip = "animations/bat1_death.csv";
		else if (enemyName == "Ghoul1") deathClip = "animations/ghoul1_death.csv";
		else if (enemyName == "Ghoul2") deathClip = "animations/ghoul2_death.csv";
		else if (enemyName == "Ghoul3") deathClip = "animations/ghoul3_death.csv";
		else if (enemyName == "Skeleton1") deathClip = "animations/skeleton1_death.csv";
		else if (enemyName == "Skeleton2") deathClip = "animations/skeleton2_death.csv";
		else if (enemyName == "Skeleton3") deathClip = "animations/skeleton3_death.csv";
		else if (enemyName == "Skeleton4") deathClip = "animations/skeleton4_death.csv";
		else if (enemyName == "Skeleton5") deathClip = "animations/skeleton5_death.csv";
		else if (enemyName == "Skeleton6") deathClip = "animations/skeleton6_death.csv";
		else deathClip = "animations/bat1_death.csv";

		if (ANI_CLIP_MGR.Exists(deathClip))
		{
			std::cout << "Playing death animation: " << deathClip << " for " << enemyName << std::endl;

			// LMJ: Clear events and add death event
			animator.ClearEvent();

			animator.AddEvent(deathClip, 14, [this]() {
				OnDeathAnimationComplete();
				SetActive(false);
				});

			animator.Play(deathClip);
		}
		else
		{
			std::cout << "Death animation not found: " << deathClip << " for " << enemyName << std::endl;
			OnDeathAnimationComplete();
			SetActive(false);
		}

		std::cout << "Enemy dead, exp given: " << expValue << std::endl;
	}
}

void Enemy::FollowPlayer(Player* player)
{
	if (player == nullptr || isDead)
	{
		velocity = sf::Vector2f(0.f, 0.f);
		return;
	}

	target = player;
	sf::Vector2f playerPos = player->GetPosition();
	sf::Vector2f direction = playerPos - position;

	float distanceToPlayer = Utils::Magnitude(direction);
	if (distanceToPlayer < 30.0f)
	{
		velocity = sf::Vector2f(0.f, 0.f);
		return;
	}

	Utils::Normalize(direction);

	sf::Vector2f avoidanceForce = CalculateAvoidanceForce();

	// LMJ: follow target and avoid.
	sf::Vector2f finalDirection = direction + avoidanceForce;
	Utils::Normalize(finalDirection);

	velocity = finalDirection * speed;

	// std::cout << "Enemy moving: vel(" << velocity.x << ", " << velocity.y << "), speed: " << speed << std::endl;
}

sf::Vector2f Enemy::CalculateAvoidanceForce()
{
	sf::Vector2f avoidanceForce(0.f, 0.f);

	// LMJ: monster info from monsterspawner.
	if (spawnerRef)
	{
		auto otherMonsters = spawnerRef->GetActiveMonsters();

		for (const Enemy* other : otherMonsters)
		{
			if (other == this || !other || !other->GetActive() || other->IsDead())
				continue;

			sf::Vector2f toOther = other->GetPosition() - position;
			float distance = Utils::Magnitude(toOther);

			// LMJ: distance check to avoid collision
			if (distance < avoidanceRadius && distance > 0.1f)
			{
				// LMJ: closer stronger
				float avoidanceStrength = (avoidanceRadius - distance) / avoidanceRadius;
				avoidanceStrength *= avoidanceForceMultiplier;

				// LMJ: to oposite direction
				sf::Vector2f awayDirection = -toOther;
				Utils::Normalize(awayDirection);

				avoidanceForce += awayDirection * avoidanceStrength;
			}
		}
	}

	// LMJ: max limit of force
	float maxAvoidanceForce = 1.0f;
	if (Utils::Magnitude(avoidanceForce) > maxAvoidanceForce)
	{
		Utils::Normalize(avoidanceForce);
		avoidanceForce *= maxAvoidanceForce;
	}

	return avoidanceForce;
}

void Enemy::SetPosition(const sf::Vector2f& pos)
{
	position = pos;
	sprite.setPosition(position);

	if (hitBox)
	{
		hitBox->UpdateTransform(pos);
	}
}

void Enemy::SetRotation(float angle)
{
	rotation = angle;
	sprite.setRotation(rotation);
}

void Enemy::SetScale(const sf::Vector2f& s)
{
	scale = s;
	sprite.setScale(scale);
}

void Enemy::SetOrigin(Origins preset)
{
	originPreset = preset;
	if (originPreset != Origins::Custom)
	{
		Utils::SetOrigin(sprite, originPreset);
	}
}

void Enemy::SetOrigin(const sf::Vector2f& newOrigin)
{
	originPreset = Origins::Custom;
	origin = newOrigin;
	sprite.setOrigin(origin);
}

void Enemy::SetHitBoxRadius(float radius)
{
	hitBoxRadius = radius;
	if (hitBox)
	{
		hitBox->SetCircle(radius);
	}
}

int Enemy::GetEnemyHp() const
{
	return hp;
}

int Enemy::GetEnemyDamage() const
{
	return damage;
}

int Enemy::GetExpValue() const
{
	return expValue;
}

void Enemy::LoadAnimations(const std::string& runAnimPath, const std::string& deathAnimPath, const std::string& runTexPath, const std::string& deathTexPath)
{
	// LMJ: animation load for each every monsters
	ANI_CLIP_MGR.Load(runAnimPath);
	ANI_CLIP_MGR.Load(deathAnimPath);

	std::string runTexturePath = runAnimPath;
	std::string deathTexturePath = deathAnimPath;

	// LMJ: texture setting
	if (TEXTURE_MGR.Exists(runTexPath))
	{
		sprite.setTexture(TEXTURE_MGR.Get(runTexPath));
	}

	// LMJ: Run animation play
	if (ANI_CLIP_MGR.Exists(runAnimPath))
	{
		animator.Play(runAnimPath);
		std::cout << "Playing run animation: " << runAnimPath << " for " << GetName() << std::endl;
	}
	else
	{
		std::cout << "Run animation not found: " << runAnimPath << " for " << GetName() << std::endl;
	}
}

sf::FloatRect Enemy::GetLocalBounds() const
{
	return sprite.getLocalBounds();
}

sf::FloatRect Enemy::GetGlobalBounds() const
{
	return sprite.getGlobalBounds();
}

sf::Vector2f Enemy::GetPosition() const
{
	return position;
}

sf::Vector2f Enemy::GetVelocity() const
{
	return velocity;
}