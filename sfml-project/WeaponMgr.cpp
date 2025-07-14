#include "stdafx.h"
#include "WeaponMgr.h"

Projectile::Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, int dmg, const std::string& textureId, int pierceCount, float kb)
{
    velocity = vel;
    baseDamage = dmg;
    actualDamage = dmg;
    pierce = pierceCount;
    knockback = kb;
    currentLife = 0.f;
    isCritical = false;
    lifeTime = 3.f;
    hitboxDelay = 0.1f;
    lastHitTime = 0.f;

    if (TEXTURE_MGR.Exists(textureId))
    {
        sprite.setTexture(TEXTURE_MGR.Get(textureId));
        Utils::SetOrigin(sprite, Origins::MC);
    }
    sprite.setPosition(pos);

    if (vel.x != 0.f || vel.y != 0.f)
    {
        float angle = Utils::Angle(vel);
        sprite.setRotation(angle);
    }
    hitEnemies.clear();
}

void Projectile::Update(float dt)
{
    if (!active) return;

    currentLife += dt;
    lastHitTime += dt;

    sf::Vector2f currentPos = sprite.getPosition();
    currentPos += velocity * dt;
    sprite.setPosition(currentPos);

    if (lifeTime > 0.f)
    {
        float lifeRatio = currentLife / lifeTime;
        if (lifeRatio > 0.7f)
        {
            float fadeStart = 0.7f;
            float fadeRatio = (lifeRatio - fadeStart) / (1.f - fadeStart);
            float alpha = Utils::Lerp(255.f, 0.f, fadeRatio, true);

            sf::Color currentColor = sprite.getColor();
            currentColor.a = static_cast<sf::Unit8>(alpha);
            sprite.setColor(currentColor);
        }
    }

    if (currentLife >= lifeTime || currnetPierce >= pierce)
    {
        active = false;
    }
}

void Projectile::Draw(sf::RenderWindow& window)
{

}

bool Projectile::CanHitEnemy(void* enemy) const
{
    return false;
}

void Projectile::OnHitEnemy(void* enemy)
{
}

WeaponMgr::WeaponMgr(const std::string& name)
{
}

void WeaponMgr::Init()
{
}

void WeaponMgr::Release()
{
}

void WeaponMgr::Reset()
{
}

void WeaponMgr::Update(float dt)
{
}

void WeaponMgr::Draw(sf::RenderWindow& window)
{
}

void WeaponMgr::AddWeapon(WeaponType type)
{
}

void WeaponMgr::UpgradeWeapon(WeaponType type)
{
}

void WeaponMgr::UpgradeRandomWeapon()
{
}

void WeaponMgr::UpdateAllWeaponStats()
{
}

void WeaponMgr::UpdateWeaponCooldowns(float dt)
{
}

void WeaponMgr::TryAttackWithWeapon(int weaponIndex, float dt)
{
}

void WeaponMgr::AttackWithWeapon(const WeaponInfo& weapon)
{
}

void WeaponMgr::CreateProjectile(const sf::Vector2f& pos, const sf::Vector2f& direction, const WeaponInfo& weapon, bool isCritical)
{
}

void WeaponMgr::UpdateProjectiles(float dt)
{
}

void WeaponMgr::CheckProjectileCollisions()
{
}

void WeaponMgr::CheckProjectileWithEnemy(Projectile& projectile, Enemy* enemy)
{
}

void WeaponMgr::RemoveDeadProjectiles()
{
}

bool WeaponMgr::IsProjectilePoolFull(const WeaponInfo& weapon) const
{
    return false;
}

sf::Vector2f WeaponMgr::GetRandomDirection() const
{
    return sf::Vector2f();
}

sf::Vector2f WeaponMgr::GetRandomPositionAroundOwner(float radius) const
{
    return sf::Vector2f();
}

sf::Vector2f WeaponMgr::GetownerPosition() const
{
    return sf::Vector2f();
}

bool WeaponMgr::RollCritical(float critChance) const
{
    return false;
}

bool WeaponMgr::RollChance(float chance) const
{
    return false;
}

void WeaponMgr::ApplyProjectileVisualEffects(Projectile& projectile, float dt)
{
}

void WeaponMgr::CreateMuzzleFlash(const sf::Vector2f& position)
{
}

bool WeaponMgr::CheckCircularCollision(const sf::Vector2f& pos1, float radius1, const sf::Vector2f pos2, float radius2) const
{
    return false;
}

bool WeaponMgr::CheckSpriteCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2) const
{
    return false;
}

const WeaponInfo* WeaponMgr::GetWeapon(WeaponType type) const
{
    return nullptr;
}
