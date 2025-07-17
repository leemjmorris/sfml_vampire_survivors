#pragma once

// LMJ: Forward declarations
class GameObject;

enum class HitBoxType
{
    Player,
    Enemy,
    Projectile,
    Pickup
};

class HitBox
{
protected:
    sf::RectangleShape rect;
    GameObject* owner;
    HitBoxType type;
    bool active;
    float radius; // LMJ: For circular collision detection
    bool useCircularCollision;

    // LMJ: Collision layers - what this hitbox can collide with
    std::vector<HitBoxType> collisionLayers;

public:
    HitBox(GameObject* owner = nullptr, HitBoxType type = HitBoxType::Player);
    virtual ~HitBox() = default;

    // Setup methods
    void SetOwner(GameObject* obj) { owner = obj; }
    void SetType(HitBoxType hitBoxType) { type = hitBoxType; }
    void SetActive(bool isActive) { active = isActive; }

    // Collision shape setup
    void SetRectangle(const sf::Vector2f& size, const sf::Vector2f& offset = sf::Vector2f(0, 0));
    void SetCircle(float radius, const sf::Vector2f& offset = sf::Vector2f(0, 0));

    // Collision layer management
    void AddCollisionLayer(HitBoxType layer);
    void RemoveCollisionLayer(HitBoxType layer);
    void ClearCollisionLayers();
    bool CanCollideWith(HitBoxType otherType) const;

    // Transform update
    void UpdateTransform(const sf::Transformable& tr, const sf::FloatRect& localBounds);
    void UpdateTransform(const sf::Vector2f& position, float rotation = 0.0f, const sf::Vector2f& scale = sf::Vector2f(1, 1));

    // Collision detection
    bool CheckCollision(const HitBox& other) const;
    bool CheckCollision(const sf::Vector2f& point) const;

    // Getters
    GameObject* GetOwner() const { return owner; }
    HitBoxType GetType() const { return type; }
    bool IsActive() const { return active; }
    sf::FloatRect GetBounds() const { return rect.getGlobalBounds(); }
    sf::Vector2f GetCenter() const;
    float GetRadius() const { return radius; }
    bool IsUsingCircularCollision() const { return useCircularCollision; }

    // Debug drawing
    void Draw(sf::RenderWindow& window);
    void SetDebugColor(const sf::Color& color);



private:
    sf::Vector2f offset; // Offset from owner's position
    sf::Color debugColor;

    // Helper methods
    bool CheckRectangleCollision(const HitBox& other) const;
    bool CheckCircleCollision(const HitBox& other) const;
    bool CheckMixedCollision(const HitBox& other) const; // Rectangle vs Circle
};

// Collision Manager for handling all collision detection
class CollisionManager
{
private:
    static std::vector<HitBox*> allHitBoxes;
    static bool debugDraw;

public:
    // HitBox registration
    static void RegisterHitBox(HitBox* hitBox);
    static void UnregisterHitBox(HitBox* hitBox);
    static void ClearAll();

    // Collision detection
    static std::vector<HitBox*> CheckCollisions(HitBox* hitBox);
    static std::vector<HitBox*> GetHitBoxesByType(HitBoxType type);
    static std::vector<HitBox*> GetNearbyHitBoxes(const sf::Vector2f& position, float range, HitBoxType type = HitBoxType::Player);

    // Update and draw
    static void Update(float dt);
    static void DrawAll(sf::RenderWindow& window);

    // Debug
    static void SetDebugDraw(bool enable) { debugDraw = enable; }
    static bool IsDebugDrawEnabled() { return debugDraw; }

    // Collision callbacks
    static void HandleCollision(HitBox* hitBoxA, HitBox* hitBoxB);
};

// Specialized HitBox classes
class PlayerHitBox : public HitBox
{
public:
    PlayerHitBox(GameObject* owner) : HitBox(owner, HitBoxType::Player)
    {
        // Players can collide with enemies and pickups
        AddCollisionLayer(HitBoxType::Enemy);
        AddCollisionLayer(HitBoxType::Pickup);
        SetDebugColor(sf::Color::Green);
    }
};

class EnemyHitBox : public HitBox
{
public:
    EnemyHitBox(GameObject* owner) : HitBox(owner, HitBoxType::Enemy)
    {
        // Enemies can collide with players and projectiles
        AddCollisionLayer(HitBoxType::Player);
        AddCollisionLayer(HitBoxType::Projectile);
        AddCollisionLayer(HitBoxType::Enemy);
        SetDebugColor(sf::Color::Red);
    }
};

class ProjectileHitBox : public HitBox
{
public:
    ProjectileHitBox(GameObject* owner) : HitBox(owner, HitBoxType::Projectile)
    {
        // Projectiles can collide with enemies
        AddCollisionLayer(HitBoxType::Enemy);
        SetDebugColor(sf::Color::Yellow);
    }
};