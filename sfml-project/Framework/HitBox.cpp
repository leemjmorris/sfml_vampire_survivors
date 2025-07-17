#include "stdafx.h"
#include "HitBox.h"
#include "GameObject.h"
#include "Player.h"
#include <algorithm>

// Static member initialization
std::vector<HitBox*> CollisionManager::allHitBoxes;
bool CollisionManager::debugDraw = false;

// ============================================================================
// HitBox Implementation
// ============================================================================

HitBox::HitBox(GameObject* owner, HitBoxType type)
    : owner(owner), type(type), active(true), radius(0.0f), useCircularCollision(false),
    offset(0, 0), debugColor(sf::Color::White)
{
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineThickness(1.0f);
    rect.setOutlineColor(sf::Color::Green);

    // Register with collision manager
    CollisionManager::RegisterHitBox(this);
}

void HitBox::SetRectangle(const sf::Vector2f& size, const sf::Vector2f& offset)
{
    this->offset = offset;
    useCircularCollision = false;
    rect.setSize(size);
    rect.setOrigin(size.x * 0.5f, size.y * 0.5f); // Center origin
}

void HitBox::SetCircle(float radius, const sf::Vector2f& offset)
{
    this->offset = offset;
    this->radius = radius;
    useCircularCollision = true;

    // Set rectangle to represent circle bounds (for debug drawing)
    float diameter = radius * 2.0f;
    rect.setSize(sf::Vector2f(diameter, diameter));
    rect.setOrigin(radius, radius);
}

void HitBox::AddCollisionLayer(HitBoxType layer)
{
    auto it = std::find(collisionLayers.begin(), collisionLayers.end(), layer);
    if (it == collisionLayers.end())
    {
        collisionLayers.push_back(layer);
    }
}

void HitBox::RemoveCollisionLayer(HitBoxType layer)
{
    auto it = std::find(collisionLayers.begin(), collisionLayers.end(), layer);
    if (it != collisionLayers.end())
    {
        collisionLayers.erase(it);
    }
}

void HitBox::ClearCollisionLayers()
{
    collisionLayers.clear();
}

bool HitBox::CanCollideWith(HitBoxType otherType) const
{
    return std::find(collisionLayers.begin(), collisionLayers.end(), otherType) != collisionLayers.end();
}

void HitBox::UpdateTransform(const sf::Transformable& tr, const sf::FloatRect& localBounds)
{
    sf::Vector2f position = tr.getPosition() + offset;
    rect.setPosition(position);
    rect.setRotation(tr.getRotation());
    rect.setScale(tr.getScale());
}

void HitBox::UpdateTransform(const sf::Vector2f& position, float rotation, const sf::Vector2f& scale)
{
    sf::Vector2f finalPosition = position + offset;
    rect.setPosition(finalPosition);
    rect.setRotation(rotation);
    rect.setScale(scale);
}

bool HitBox::CheckCollision(const HitBox& other) const
{
    if (!active || !other.active) return false;
    if (!CanCollideWith(other.type)) return false;

    if (useCircularCollision && other.useCircularCollision)
    {
        return CheckCircleCollision(other);
    }
    else if (!useCircularCollision && !other.useCircularCollision)
    {
        return CheckRectangleCollision(other);
    }
    else
    {
        return CheckMixedCollision(other);
    }
}

bool HitBox::CheckCollision(const sf::Vector2f& point) const
{
    if (!active) return false;

    if (useCircularCollision)
    {
        sf::Vector2f center = GetCenter();
        float distance = Utils::Distance(center, point);
        return distance <= radius;
    }
    else
    {
        return rect.getGlobalBounds().contains(point);
    }
}

sf::Vector2f HitBox::GetCenter() const
{
    sf::FloatRect bounds = rect.getGlobalBounds();
    return sf::Vector2f(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
}

void HitBox::Draw(sf::RenderWindow& window)
{
    if (CollisionManager::IsDebugDrawEnabled() && active)
    {
        rect.setOutlineColor(debugColor);
        window.draw(rect);
    }
}

void HitBox::SetDebugColor(const sf::Color& color)
{
    debugColor = color;
}

bool HitBox::CheckRectangleCollision(const HitBox& other) const
{
    return rect.getGlobalBounds().intersects(other.rect.getGlobalBounds());
}

bool HitBox::CheckCircleCollision(const HitBox& other) const
{
    sf::Vector2f center1 = GetCenter();
    sf::Vector2f center2 = other.GetCenter();
    float distance = Utils::Distance(center1, center2);
    return distance <= (radius + other.radius);
}

bool HitBox::CheckMixedCollision(const HitBox& other) const
{
    const HitBox* circle = useCircularCollision ? this : &other;
    const HitBox* rectangle = useCircularCollision ? &other : this;

    sf::Vector2f circleCenter = circle->GetCenter();
    sf::FloatRect rectBounds = rectangle->rect.getGlobalBounds();

    // Find closest point on rectangle to circle center
    float closestX = Utils::Clamp(circleCenter.x, rectBounds.left, rectBounds.left + rectBounds.width);
    float closestY = Utils::Clamp(circleCenter.y, rectBounds.top, rectBounds.top + rectBounds.height);

    sf::Vector2f closestPoint(closestX, closestY);
    float distance = Utils::Distance(circleCenter, closestPoint);

    return distance <= circle->radius;
}

// ============================================================================
// CollisionManager Implementation
// ============================================================================

void CollisionManager::RegisterHitBox(HitBox* hitBox)
{
    if (hitBox && std::find(allHitBoxes.begin(), allHitBoxes.end(), hitBox) == allHitBoxes.end())
    {
        allHitBoxes.push_back(hitBox);
    }
}

void CollisionManager::UnregisterHitBox(HitBox* hitBox)
{
    auto it = std::find(allHitBoxes.begin(), allHitBoxes.end(), hitBox);
    if (it != allHitBoxes.end())
    {
        allHitBoxes.erase(it);
    }
}

void CollisionManager::ClearAll()
{
    allHitBoxes.clear();
}

std::vector<HitBox*> CollisionManager::CheckCollisions(HitBox* hitBox)
{
    std::vector<HitBox*> collisions;

    if (!hitBox || !hitBox->IsActive()) return collisions;

    for (HitBox* other : allHitBoxes)
    {
        if (other == hitBox || !other->IsActive()) continue;

        if (hitBox->CheckCollision(*other))
        {
            collisions.push_back(other);
        }
    }

    return collisions;
}

std::vector<HitBox*> CollisionManager::GetHitBoxesByType(HitBoxType type)
{
    std::vector<HitBox*> result;

    for (HitBox* hitBox : allHitBoxes)
    {
        if (hitBox->IsActive() && hitBox->GetType() == type)
        {
            result.push_back(hitBox);
        }
    }

    return result;
}

std::vector<HitBox*> CollisionManager::GetNearbyHitBoxes(const sf::Vector2f& position, float range, HitBoxType type)
{
    std::vector<HitBox*> result;

    for (HitBox* hitBox : allHitBoxes)
    {
        if (!hitBox->IsActive() || hitBox->GetType() != type) continue;

        sf::Vector2f hitBoxCenter = hitBox->GetCenter();
        float distance = Utils::Distance(position, hitBoxCenter);

        if (distance <= range)
        {
            result.push_back(hitBox);
        }
    }

    return result;
}

void CollisionManager::Update(float dt)
{
    // Check all collisions and handle them
    for (size_t i = 0; i < allHitBoxes.size(); ++i)
    {
        HitBox* hitBoxA = allHitBoxes[i];
        if (!hitBoxA->IsActive()) continue;

        for (size_t j = i + 1; j < allHitBoxes.size(); ++j)
        {
            HitBox* hitBoxB = allHitBoxes[j];
            if (!hitBoxB->IsActive()) continue;

            if (hitBoxA->CheckCollision(*hitBoxB))
            {
                HandleCollision(hitBoxA, hitBoxB);
            }
        }
    }
}

void CollisionManager::DrawAll(sf::RenderWindow& window)
{
    if (debugDraw)
    {
        for (HitBox* hitBox : allHitBoxes)
        {
            if (hitBox->IsActive())
            {
                hitBox->Draw(window);
            }
        }
    }
}

void CollisionManager::HandleCollision(HitBox* hitBoxA, HitBox* hitBoxB)
{
    // Handle Player vs Enemy collision
    if ((hitBoxA->GetType() == HitBoxType::Player && hitBoxB->GetType() == HitBoxType::Enemy) ||
        (hitBoxA->GetType() == HitBoxType::Enemy && hitBoxB->GetType() == HitBoxType::Player))
    {
        Player* player = nullptr;
        Enemy* enemy = nullptr;

        if (hitBoxA->GetType() == HitBoxType::Player)
        {
            player = dynamic_cast<Player*>(hitBoxA->GetOwner());
            enemy = dynamic_cast<Enemy*>(hitBoxB->GetOwner());
        }
        else
        {
            player = dynamic_cast<Player*>(hitBoxB->GetOwner());
            enemy = dynamic_cast<Enemy*>(hitBoxA->GetOwner());
        }

        if (player && enemy && !player->IsDead() && !enemy->IsDead())
        {
            // Player takes damage from enemy
            player->TakeDamage(enemy->GetEnemyDamage());
            std::cout << "Player hit by enemy! HP: " << player->GetCurrentHp() << std::endl;
        }
    }

    // Handle Projectile vs Enemy collision (if using hitbox system for projectiles)
    if ((hitBoxA->GetType() == HitBoxType::Projectile && hitBoxB->GetType() == HitBoxType::Enemy) ||
        (hitBoxA->GetType() == HitBoxType::Enemy && hitBoxB->GetType() == HitBoxType::Projectile))
    {
        // This would be handled by WeaponMgr if we integrate projectiles with hitbox system
        // For now, WeaponMgr handles its own collision detection
    }
}