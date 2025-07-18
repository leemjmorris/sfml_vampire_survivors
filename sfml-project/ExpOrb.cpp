#include "stdafx.h"
#include "ExpOrb.h"
#include "Player.h"

ExpOrb::ExpOrb(const std::string& name) : GameObject(name)
{
    hitBox = new HitBox(this, HitBoxType::Pickup);
}

ExpOrb::~ExpOrb()
{
    if (hitBox)
    {
        CollisionManager::UnregisterHitBox(hitBox);
        delete hitBox;
        hitBox = nullptr;
    }
}

void ExpOrb::Init()
{
    if (hitBox)
    {
        hitBox->SetCircle(hitBoxRadius);
        hitBox->AddCollisionLayer(HitBoxType::Player);
        hitBox->SetActive(true);
        hitBox->SetDebugColor(sf::Color::Cyan);
    }
}

void ExpOrb::Release()
{
}

void ExpOrb::Reset()
{
    sortingLayer = SortingLayers::Foreground;
    sortingOrder = 10;

    std::string textureId = "graphics/Gem1.png";
    if (TEXTURE_MGR.Exists(textureId))
    {
        sprite.setTexture(TEXTURE_MGR.Get(textureId));
    }

    SetOrigin(Origins::MC);
    isBeingAttracted = false;
}

void ExpOrb::Update(float dt)
{
    if (!active) return;

    CheckPlayerProximity();

    if (isBeingAttracted && target)
    {
        UpdateAttraction(dt);
    }

    if (hitBox)
    {
        hitBox->UpdateTransform(position);
    }
}

void ExpOrb::Draw(sf::RenderWindow& window)
{
    if (active)
    {
        // LMJ: Temp. debug.
        //sf::CircleShape circle(15.f);
        //circle.setPosition(position.x - 15.0f, position.y - 15.0f);
        //circle.setFillColor(sf::Color::Yellow);
        //window.draw(circle);

        window.draw(sprite);
    }
}

void ExpOrb::UpdateAttraction(float dt)
{
    if (!target) return;

    sf::Vector2f playerPos = target->GetPosition();
    sf::Vector2f direction = playerPos - position;
    float distance = Utils::Magnitude(direction);

    if (distance < 20.0f) // LMJ: Distance check for exporb and player
    {
        target->GainExperience(expValue);
        SetActive(false);
        return;
    }

    Utils::Normalize(direction);
    sf::Vector2f movement = direction * attractSpeed * dt;
    SetPosition(position + movement);
}

void ExpOrb::CheckPlayerProximity()
{
    if (!target || isBeingAttracted) return;

    float distance = Utils::Distance(position, target->GetPosition());
    if (distance <= attractRadius)
    {
        isBeingAttracted = true;
    }
}

void ExpOrb::SetPosition(const sf::Vector2f& pos)
{
    position = pos;
    sprite.setPosition(position);
}

void ExpOrb::SetRotation(float angle)
{
    rotation = angle;
    sprite.setRotation(rotation);
}

void ExpOrb::SetScale(const sf::Vector2f& s)
{
    scale = s;
    sprite.setScale(scale);
}

void ExpOrb::SetOrigin(Origins preset)
{
    originPreset = preset;
    if (originPreset != Origins::Custom)
    {
        Utils::SetOrigin(sprite, originPreset);
    }
}

void ExpOrb::SetOrigin(const sf::Vector2f& newOrigin)
{
    originPreset = Origins::Custom;
    origin = newOrigin;
    sprite.setOrigin(origin);
}

sf::FloatRect ExpOrb::GetLocalBounds() const
{
    return sprite.getLocalBounds();
}

sf::FloatRect ExpOrb::GetGlobalBounds() const
{
    return sprite.getGlobalBounds();
}