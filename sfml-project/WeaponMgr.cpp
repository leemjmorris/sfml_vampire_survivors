#include "stdafx.h"
#include "WeaponMgr.h"
#include "Player.h"
#include "Enemy.h"
#include "Scene.h"

// Static member initialization
std::vector<WeaponInfo> WeaponMgr::weaponDefinitions;
bool WeaponMgr::definitionsInitialized = false;

Projectile::Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, int dmg,
    const std::string& textureId, WeaponType type, int pierceCount,
    float kb, float lifetime)
    : velocity(vel), startPosition(pos), baseDamage(dmg), actualDamage(dmg),
    maxPierce(pierceCount), knockback(kb), lifeTime(lifetime), weaponType(type)
{
    currentLife = 0.0f;
    currentPierce = 0;
    active = true;
    isCritical = false;
    lastHitTime = 0.0f;

    if (TEXTURE_MGR.Exists(textureId))
    {
        sprite.setTexture(TEXTURE_MGR.Get(textureId));
        Utils::SetOrigin(sprite, Origins::MC);
    }
    sprite.setPosition(pos);

    if (vel.x != 0.0f || vel.y != 0.0f)
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

    if (lifeTime > 0.0f)
    {
        float lifeRatio = currentLife / lifeTime;
        if (lifeRatio > 0.7f)
        {
            float fadeStart = 0.7f;
            float fadeRatio = (lifeRatio - fadeStart) / (1.0f - fadeStart);
            float alpha = Utils::Lerp(255.0f, 0.0f, fadeRatio, true);

            sf::Color currentColor = sprite.getColor();
            currentColor.a = static_cast<sf::Uint8>(alpha);
            sprite.setColor(currentColor);
        }
    }

    if (currentLife >= lifeTime || currentPierce >= maxPierce)
    {
        active = false;
    }
}

void Projectile::Draw(sf::RenderWindow& window)
{
    if (active)
    {
        window.draw(sprite);
    }
}

bool Projectile::CanHitEnemy(void* enemy) const
{
    if (!active || lastHitTime < hitboxDelay) return false;
    return hitEnemies.find(enemy) == hitEnemies.end();
}

void Projectile::OnHitEnemy(void* enemy)
{
    hitEnemies.insert(enemy);
    currentPierce++;
    lastHitTime = 0.0f;
}

void Projectile::ApplyAreaScaling(float areaMultiplier)
{
    area = areaMultiplier;
    sprite.setScale(sf::Vector2f(areaMultiplier, areaMultiplier));
}

WeaponInfo::WeaponInfo(WeaponType t, const std::string& n, const std::string& tex, WeaponRarity r)
    : type(t), name(n), textureId(tex), rarity(r)
{
    currentCooldown = 0.0f;
    lastAttackTime = 0.0f;
}

void WeaponInfo::CalculateFinalStats(const PlayerStats& playerStats)
{
    finalDamage = static_cast<int>(baseDamage * playerStats.mightMultiplier);
    finalArea = area * playerStats.areaMultiplier;
    finalSpeed = speed * playerStats.speedMultiplier;
    finalAmount = amount + playerStats.amountBonus;
    finalDuration = duration * playerStats.durationMultiplier;
    finalPierce = pierce;
    finalCooldown = cooldown * playerStats.cooldownMultiplier;
    finalKnockback = knockback;
    finalChance = Utils::Clamp01(chance + playerStats.luckBonus);
}

void WeaponInfo::LevelUp()
{
    if (level >= maxLevel) return;

    level++;

    switch (type)
    {
    case WeaponType::Knife:
        baseDamage += 5;
        if (level % 2 == 0)
            amount += 1;
        if (level >= 4)
            pierce += 1;
        cooldown *= 0.95f;
        break;
    }
}

WeaponMgr::WeaponMgr(const std::string& name) : GameObject(name)
{
    if (!definitionsInitialized)
    {
        InitializeWeaponDefinitions();
    }
}

void WeaponMgr::Init()
{
    if (!definitionsInitialized)
    {
        InitializeWeaponDefinitions();
    }
}

void WeaponMgr::Release()
{
    weapons.clear();
    projectiles.clear();
}

void WeaponMgr::Reset()
{
    sortingLayer = SortingLayers::Foreground;
    sortingOrder = 5;

    weapons.clear();
    projectiles.clear();
    globalAttackTimer = 0.0f;

    playerStats = PlayerStats();
}

void WeaponMgr::Update(float dt)
{
    if (!active) return;

    globalAttackTimer += dt;

    UpdateWeaponCooldowns(dt);
    TryAttackWithAllWeapons(dt);
    UpdateProjectiles(dt);
    CheckProjectileCollisions();
    RemoveDeadProjectiles();
}

void WeaponMgr::Draw(sf::RenderWindow& window)
{
    if (!active) return;

    for (auto& projectile : projectiles)
    {
        projectile.Draw(window);
    }
}

void WeaponMgr::InitializeWeaponDefinitions()
{
    weaponDefinitions.clear();

    WeaponInfo knife(WeaponType::Knife, "Knife", "graphics/Knife.png", WeaponRarity::Common);
    knife.baseDamage = 15;
    knife.area = 1.0f;
    knife.speed = 300.0f;
    knife.amount = 1;
    knife.duration = 0.0f;
    knife.pierce = 1;
    knife.cooldown = 0.8f;
    knife.projectileInterval = 0.0f;
    knife.hitboxDelay = 0.1f;
    knife.knockback = 1.0f;
    knife.poolLimit = 30;
    knife.chance = 1.0f;
    knife.critMulti = 2.0f;
    knife.blockByWalls = true;
    knife.maxLevel = 8;

    weaponDefinitions.push_back(knife);

    definitionsInitialized = true;
}

void WeaponMgr::UpdatePlayerStats(const PlayerStats& stats)
{
    playerStats = stats;

    for (auto& weapon : weapons)
    {
        weapon.CalculateFinalStats(playerStats);
    }
}

void WeaponMgr::AddWeapon(WeaponType type)
{
    if (HasWeapon(type)) return;

    for (const auto& def : weaponDefinitions)
    {
        if (def.type == type)
        {
            WeaponInfo newWeapon = def;
            newWeapon.CalculateFinalStats(playerStats);
            weapons.push_back(newWeapon);
            std::cout << "Added weapon: " << newWeapon.name << std::endl;
            return;
        }
    }
}

void WeaponMgr::UpgradeWeapon(WeaponType type)
{
    WeaponInfo* weapon = FindWeapon(type);
    if (weapon && weapon->level < weapon->maxLevel)
    {
        weapon->LevelUp();
        weapon->CalculateFinalStats(playerStats);
        std::cout << "Upgraded " << weapon->name << " to level " << weapon->level << std::endl;
    }
}

void WeaponMgr::UpgradeRandomWeapon()
{
    if (weapons.empty()) return;

    std::vector<WeaponInfo*> upgradeable;
    for (auto& weapon : weapons)
    {
        if (weapon.level < weapon.maxLevel)
        {
            upgradeable.push_back(&weapon);
        }
    }

    if (!upgradeable.empty())
    {
        int randomIndex = Utils::RandomRange(0, static_cast<int>(upgradeable.size()));
        upgradeable[randomIndex]->LevelUp();
        upgradeable[randomIndex]->CalculateFinalStats(playerStats);
    }
}

bool WeaponMgr::HasWeapon(WeaponType type) const
{
    return GetWeapon(type) != nullptr;
}

void WeaponMgr::UpdateWeaponCooldowns(float dt)
{
    for (auto& weapon : weapons)
    {
        if (weapon.currentCooldown > 0.0f)
        {
            weapon.currentCooldown -= dt;
            if (weapon.currentCooldown < 0.0f)
            {
                weapon.currentCooldown = 0.0f;
            }
        }
    }
}

void WeaponMgr::TryAttackWithAllWeapons(float dt)
{
    for (auto& weapon : weapons)
    {
        if (weapon.CanAttack() && RollChance(weapon.finalChance))
        {
            AttackWithWeapon(weapon);
        }
    }
}

void WeaponMgr::AttackWithWeapon(WeaponInfo& weapon)
{
    if (IsProjectilePoolFull(weapon)) return;

    sf::Vector2f ownerPos = GetOwnerPosition();
    bool isCrit = RollCritical(playerStats.critChance);

    switch (weapon.type)
    {
    case WeaponType::Knife:
    {
        sf::Vector2f direction = GetDirectionToNearestEnemy();
        if (direction.x == 0.0f && direction.y == 0.0f)
        {
            direction = GetRandomDirection();
        }

        for (int i = 0; i < weapon.finalAmount; ++i)
        {
            sf::Vector2f adjustedDir = direction;

            if (weapon.finalAmount > 1)
            {
                float spreadAngle = 30.0f;
                float angleStep = spreadAngle / (weapon.finalAmount - 1);
                float currentAngle = -spreadAngle * 0.5f + angleStep * i;

                float radians = Utils::DegreeToRadian(currentAngle);
                float cos_a = std::cos(radians);
                float sin_a = std::sin(radians);

                sf::Vector2f rotated;
                rotated.x = adjustedDir.x * cos_a - adjustedDir.y * sin_a;
                rotated.y = adjustedDir.x * sin_a + adjustedDir.y * cos_a;
                adjustedDir = rotated;
            }

            CreateProjectile(ownerPos, adjustedDir, weapon, isCrit);
        }
    }
    break;
    }

    weapon.StartCooldown();
}

void WeaponMgr::CreateProjectile(const sf::Vector2f& pos, const sf::Vector2f& direction,
    const WeaponInfo& weapon, bool isCritical)
{
    sf::Vector2f velocity = direction * weapon.finalSpeed;

    Projectile projectile(pos, velocity, weapon.finalDamage, weapon.textureId,
        weapon.type, weapon.finalPierce, weapon.finalKnockback, 3.0f);

    projectile.isCritical = isCritical;
    if (isCritical)
    {
        projectile.actualDamage = static_cast<int>(weapon.finalDamage * weapon.critMulti);
    }

    projectile.ApplyAreaScaling(weapon.finalArea);
    projectile.hitboxDelay = weapon.hitboxDelay;

    projectiles.push_back(projectile);
}

void WeaponMgr::UpdateProjectiles(float dt)
{
    for (auto& projectile : projectiles)
    {
        projectile.Update(dt);
    }
}

void WeaponMgr::CheckProjectileCollisions()
{
    if (!currentScene) return;

    auto enemies = GetNearbyEnemies(2000.0f); // LMJ: need to adjust this part. too far away.

    for (auto& projectile : projectiles)
    {
        if (!projectile.IsAlive()) continue;

        for (Enemy* enemy : enemies)
        {
            if (!enemy || !enemy->GetActive() || enemy->IsDead()) continue;

            if (projectile.CanHitEnemy(enemy) &&
                CheckProjectileEnemyCollision(projectile, enemy))
            {
                DamageEnemy(enemy, projectile.actualDamage, projectile.knockback);
                projectile.OnHitEnemy(enemy);

                if (projectile.isCritical)
                {
                    std::cout << "Critical hit! Damage: " << projectile.actualDamage << std::endl;
                }

                break;
            }
        }
    }
}

void WeaponMgr::RemoveDeadProjectiles()
{
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const Projectile& p) { return !p.IsAlive(); }),
        projectiles.end());
}

bool WeaponMgr::IsProjectilePoolFull(const WeaponInfo& weapon) const
{
    int count = 0;
    for (const auto& projectile : projectiles)
    {
        if (projectile.weaponType == weapon.type)
        {
            count++;
        }
    }
    return count >= weapon.poolLimit;
}

sf::Vector2f WeaponMgr::GetRandomDirection() const
{
    float angle = Utils::RandomRange(0.0f, 360.0f);
    float radians = Utils::DegreeToRadian(angle);
    return sf::Vector2f(std::cos(radians), std::sin(radians));
}

sf::Vector2f WeaponMgr::GetDirectionToNearestEnemy() const
{
    auto enemies = GetNearbyEnemies(500.0f);

    if (enemies.empty()) return sf::Vector2f(0.0f, 0.0f);

    sf::Vector2f ownerPos = GetOwnerPosition();
    float closestDistance = std::numeric_limits<float>::max();
    Enemy* closestEnemy = nullptr;

    for (Enemy* enemy : enemies)
    {
        if (!enemy || !enemy->GetActive() || enemy->IsDead()) continue;

        float distance = Utils::Distance(ownerPos, enemy->GetPosition());
        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestEnemy = enemy;
        }
    }

    if (closestEnemy)
    {
        sf::Vector2f direction = closestEnemy->GetPosition() - ownerPos;
        Utils::Normalize(direction);
        return direction;
    }

    return sf::Vector2f(0.0f, 0.0f);
}

sf::Vector2f WeaponMgr::GetOwnerPosition() const
{
    if (owner)
    {
        return owner->GetPosition();
    }
    return sf::Vector2f(0.0f, 0.0f);
}

bool WeaponMgr::RollCritical(float critChance) const
{
    return Utils::RandomValue() < critChance;
}

bool WeaponMgr::RollChance(float chance) const
{
    return Utils::RandomValue() < chance;
}

bool WeaponMgr::CheckProjectileEnemyCollision(const Projectile& projectile, const Enemy* enemy) const
{
    if (!enemy) return false;

    sf::FloatRect projectileBounds = projectile.sprite.getGlobalBounds();
    sf::FloatRect enemyBounds = enemy->GetGlobalBounds();

    return projectileBounds.intersects(enemyBounds);
}

const WeaponInfo* WeaponMgr::GetWeapon(WeaponType type) const
{
    for (const auto& weapon : weapons)
    {
        if (weapon.type == type)
        {
            return &weapon;
        }
    }
    return nullptr;
}

WeaponInfo* WeaponMgr::FindWeapon(WeaponType type)
{
    for (auto& weapon : weapons)
    {
        if (weapon.type == type)
        {
            return &weapon;
        }
    }
    return nullptr;
}

std::vector<Enemy*> WeaponMgr::GetNearbyEnemies(float range) const
{
    std::vector<Enemy*> enemies;

    if (!currentScene) return enemies;

    // LMJ: Find all enemy game objects in the scene
    auto allObjects = currentScene->FindGameObjects("Enemy");
    for (GameObject* obj : allObjects)
    {
        Enemy* enemy = dynamic_cast<Enemy*>(obj);
        if (enemy && enemy->GetActive() && !enemy->IsDead())
        {
            float distance = Utils::Distance(GetOwnerPosition(), enemy->GetPosition());
            if (distance <= range)
            {
                enemies.push_back(enemy);
            }
        }
    }

    // LMJ: Also check for TestEnemy
    auto testEnemies = currentScene->FindGameObjects("TestEnemy");
    for (GameObject* obj : testEnemies)
    {
        Enemy* enemy = dynamic_cast<Enemy*>(obj);
        if (enemy && enemy->GetActive() && !enemy->IsDead())
        {
            float distance = Utils::Distance(GetOwnerPosition(), enemy->GetPosition());
            if (distance <= range)
            {
                enemies.push_back(enemy);
            }
        }
    }

    return enemies;
}

void WeaponMgr::DamageEnemy(Enemy* enemy, int damage, float knockback)
{
    if (!enemy) return;

    enemy->TakeDamage(damage);

    // Apply knockback (you might want to implement this in Enemy class)
    // For now, just print debug info
    std::cout << "Enemy hit for " << damage << " damage" << std::endl;
}