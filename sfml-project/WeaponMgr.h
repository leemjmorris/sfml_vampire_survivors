#pragma once
#include "GameObject.h"
#include <vector>

class Player;
class Enemy;
class Scene;

enum class WeaponType
{
	Knife,
	// LMJ: Weapons to be added l8er on.
};

enum class WeaponRarity
{
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary
};

struct Projectile
{
	sf::Sprite sprite;
	sf::Vector2f velocity;

	// LMJ: projectile display time
	float lifeTime = 0.f;
	float currentLife = 0.f;
	bool active = true;

	// LMJ: damage
	int baseDamage = 0;
	int actualDamage = 0;
	bool isCritical = false;

	// LMJ: penetration
	int pierce = 0;
	int currentPierce = 0;
	std::vector<void*> hitEnemies;

	// LMJ: knockbacks
	float knockback;

	// LMJ: hitbox
	float hitboxDelay;
	float lastHitTime;

	Projectile() = default;
	Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, int dmg, const std::string& textureId, int pierceCount = 1, float kb = 1.f);

	void Update(float dt);
	void Draw(sf::RenderWindow& window);
	bool IsAlive() const { return active && currentLife < lifeTime && currentPierce < pierce; }
	bool CanHitEnemy(void* enemy) const;
	void OnHitEnemy(void* enemy);
};

struct WeaponInfo
{
	WeaponType type;
	std::string name;
	std::string textureId;
	WeaponRarity rarity;

	// LMJ: Level
	int level;
	int maxLevel;

	// LMJ: Major Stats
	int baseDamage;
	float area;
	float speed;
	int amount;
	float duration;
	int pierce;
	float cooldown;
	float projectileInterval;
	float hitboxDelay;
	float knockback;
	int poolLimit;
	float chance;
	float critMulti;
	bool blockByWalls;

	// LMJ: Actual stats that effct;
	int finalDamage;
	float finalArea;
	float finalSpeed;
	int finalAmount;
	float finalDuration;
	int finalPierce;
	float finalColldown;
	float finalKnockback;
	float finalChance;

	WeaponInfo(WeaponType t, const std::string& n, const std::string& tex, WeaponRarity r = WeaponRarity::Common);

	void CalculateFinalStats(const struct PlayerStats& playerStats);
	void LevelUp();
};

class WeaponMgr : public GameObject
{
private:
	Player* owner;
	Scene* currentScene;

	std::vector<WeaponInfo> weapons;
	std::vector<Projectile> projectiles;

	std::vector<float> weaponTimers;

public:
	WeaponMgr(const std::string& name = "WeaponManager");
	~WeaponMgr() = default;

	void Init() override;
	void Release() override;
	void Reset() override;
	void Update(float dt) override;
	void Draw(sf::RenderWindow& window) override;

	// LMJ: SETTER
	void SetOwner(Player* player) { owner = player; }
	void SetScene(Scene* scene) { currentScene = scene; }

	// LMJ: Weapon related
	void AddWeapon(WeaponType type);
	void UpgradeWeapon(WeaponType type);
	void UpgradeRandomWeapon();

	// LMJ: Player Stats
	void UpdateAllWeaponStats();

	// LMJ: Attack methods
	void UpdateWeaponCooldowns(float dt);
	void TryAttackWithWeapon(int weaponIndex, float dt);
	void AttackWithWeapon(const WeaponInfo& weapon);

	// LMJ: Projectile methods
	void CreateProjectile(const sf::Vector2f& pos, const sf::Vector2f& direction, const WeaponInfo& weapon, bool isCritical = false);
	void UpdateProjectiles(float dt);
	void CheckProjectileCollisions();
	void CheckProjectileWithEnemy(Projectile& projectile, Enemy* enemy);
	void RemoveDeadProjectiles();
	bool IsProjectilePoolFull(const WeaponInfo& weapon) const;

	// LMJ: Utils
	sf::Vector2f GetRandomDirection() const;
	sf::Vector2f GetRandomPositionAroundOwner(float radius) const;
	sf::Vector2f GetownerPosition() const;
	bool RollCritical(float critChance) const;
	bool RollChance(float chance) const;

	// LMJ: Visuals
	void ApplyProjectileVisualEffects(Projectile& projectile, float dt);
	void CreateMuzzleFlash(const sf::Vector2f& position);

	// LMJ: Collision check
	bool CheckCircularCollision(const sf::Vector2f& pos1, float radius1, const sf::Vector2f pos2, float radius2) const;
	bool CheckSpriteCollision(const sf::Sprite& sprite1, const sf::Sprite& sprite2) const;

	// LMJ: GETTER
	int GetWeaponCount() const { return weapons.size(); }
	int GetProjectileCount() const { return projectiles.size(); }
	const WeaponInfo* GetWeapon(WeaponType type) const;
};

