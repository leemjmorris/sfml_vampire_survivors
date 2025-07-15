#include "stdafx.h"
#include "Enemy.h"
#include "Player.h"
#include "Animator.h"
#include "AnimationClip.h"

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

	ANI_CLIP_MGR.Load("animations/bat1_run.csv");
	ANI_CLIP_MGR.Load("animations/bat1_death.csv");

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
	speed = 100.f;
	hp = 50;
	damage = 10;
	expValue = 10;

	SetOrigin(Origins::MC);

	// LMJ: Temporary Spawn Location.
	sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();
	SetPosition(sf::Vector2f(windowSize.x * 0.3f, windowSize.y * 0.3f));

	// LMJ: Runtime Error. Check This Part!!!!!
	if (TEXTURE_MGR.Exists("graphics/sprite_bat1_run.png"))
	{
		animator.Play("animations/bat1_run.csv");
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

		animator.AddEvent("animations/bat1_death.csv", 14, [this]()
			{
				OnDeathAnimationComplete();
			});

		animator.Play("animations/bat1_death.csv");
	}

	if (target != nullptr)
	{
		FollowPlayer(target);
	}

	position += velocity * dt;
	SetPosition(position);

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
	
	sprite.setColor(sf::Color::Red);
	sprite.setColor(sf::Color::White);

	if (hp <= 0)
	{
		isDead = true;
		sprite.setColor(sf::Color::White);

		if (hitBox) hitBox->SetActive(false);

		animator.AddEvent("animations/bat1_death.csv", 14, [this]() {
			OnDeathAnimationComplete();
			SetActive(false);
			});

		animator.Play("animations/bat1_death.csv");
		std::cout << "testEnemy dead" << std::endl;
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

	Utils::Normalize(direction);

	velocity = direction * speed;
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
