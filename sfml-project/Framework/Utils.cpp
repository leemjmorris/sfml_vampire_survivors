#include "stdafx.h"
#include "Utils.h"

std::random_device Utils::rd;
std::mt19937 Utils::gen;
const float Utils::PI = acosf(-1.f);

void Utils::Init()
{
    gen.seed(rd());
}

float Utils::RandomValue()
{
    std::uniform_real_distribution<float> dist(0.f, 1.f);
    return dist(gen);
}

int Utils::RandomRange(int min, int maxExclude)
{
    if (min >= maxExclude)
    {
        return min;
    }
    std::uniform_int_distribution<int> dist(min, maxExclude - 1);
	return dist(gen);
}

float Utils::RandomRange(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

sf::Vector2f Utils::RandomOnUnitCircle()
{
    sf::Vector2f point;
    do
    {
        point = RandomInUnitCircle();
    } while (SqrMagnitude(point) < std::numeric_limits<float>::epsilon());
    
    return GetNormal(point);
}

sf::Vector2f Utils::RandomInUnitCircle()
{
    sf::Vector2f point;
    do
    {
        point = RandomPointInRect(sf::FloatRect(-1.f, -1.f, 2.f, 2.f));
    } while (SqrMagnitude(point) > 1.f);
    
    return point;
}

sf::Color Utils::RandomColor(bool opaque)
{
    sf::Uint8 r = (sf::Uint8)RandomRange(0, 256);
    sf::Uint8 g = (sf::Uint8)RandomRange(0, 256);
    sf::Uint8 b = (sf::Uint8)RandomRange(0, 256);
    sf::Uint8 a = opaque ? 255 : (sf::Uint8)RandomRange(0, 256);
    return sf::Color(r, g, b, a);
}

sf::Vector2f Utils::RandomPointInRect(const sf::FloatRect& rect)
{
    return sf::Vector2f(
        RandomRange(rect.left, rect.left + rect.width),
        RandomRange(rect.top, rect.top + rect.height));
}


sf::Vector2f Utils::SetOrigin(sf::Transformable& obj, Origins preset, const sf::FloatRect rect)
{
    sf::Vector2f newOrigin(rect.width, rect.height);
    newOrigin.x *= ((int)preset % 3) * 0.5f;
    newOrigin.y *= ((int)preset / 3) * 0.5f;
    obj.setOrigin(newOrigin);
    return newOrigin;
}

sf::Vector2f Utils::SetOrigin(sf::Shape& obj, Origins preset)
{
    return SetOrigin(obj, preset, obj.getLocalBounds());
}

sf::Vector2f Utils::SetOrigin(sf::Text& obj, Origins preset)
{
    return SetOrigin(obj, preset, obj.getLocalBounds());
}

sf::Vector2f Utils::SetOrigin(sf::Sprite& obj, Origins preset)
{
    return SetOrigin(obj, preset, obj.getLocalBounds());
}

float Utils::Clamp(float value, float min, float max)
{
    if (value < min)
        return min;
    else if (value > max)
        return max;
    return value;
}

float Utils::Clamp01(float value)
{
    return Clamp(value, 0.f, 1.f);
}

float Utils::Magnitude(const sf::Vector2f& vec)
{
    return std::sqrtf(SqrMagnitude(vec));
}

float Utils::SqrMagnitude(const sf::Vector2f& vec)
{
    return vec.x * vec.x + vec.y * vec.y;
}

sf::Vector2f Utils::GetNormal(const sf::Vector2f& vec)
{
    float mag = Utils::Magnitude(vec);
    if (mag < std::numeric_limits<float>::epsilon())
    {
        return { 0.f, 0.f };
    }
    return vec / mag;
}

void Utils::Normalize(sf::Vector2f& vec)
{
    float mag = Utils::Magnitude(vec);
    if (mag > std::numeric_limits<float>::epsilon())
    {
        vec /= mag;
    }
}

float Utils::Distance(const sf::Vector2f& p1, const sf::Vector2f& p2)
{
    return Utils::Magnitude(p1 - p2);
}

float Utils::Lerp(float min, float max, float t, bool clamp)
{
    if (clamp)
    {
        t = Utils::Clamp01(t);
    }
    return min + (max - min) * t;
}

sf::Vector2f Utils::Lerp(const sf::Vector2f& min, const sf::Vector2f& max, float t, bool clamp)
{
    if (clamp)
    {
        t = Utils::Clamp01(t);
    }
    return min + (max - min) * t;
}

sf::Color Utils::Lerp(const sf::Color& min, const sf::Color& max, float t, bool clamp)
{
    if (clamp)
    {
        t = Utils::Clamp01(t);
    }

    return sf::Color(
        (sf::Uint8)std::round(Clamp(Lerp((float)min.r, (float)max.r, t, clamp), 0.f, 255.f)),
        (sf::Uint8)std::round(Clamp(Lerp((float)min.g, (float)max.g, t, clamp), 0.f, 255.f)),
        (sf::Uint8)std::round(Clamp(Lerp((float)min.b, (float)max.b, t, clamp), 0.f, 255.f)),
        (sf::Uint8)std::round(Clamp(Lerp((float)min.a, (float)max.a, t, clamp), 0.f, 255.f))
    );
}

float Utils::RadianToDegree(float radian)
{
    return radian * (180.f / PI);
}

float Utils::DegreeToRadian(float degree)
{
    return degree * (PI / 180.f);
}

float Utils::AngleRadian(const sf::Vector2f& vec)
{
    return std::atan2(vec.y, vec.x);
}

float Utils::Angle(const sf::Vector2f& vec)
{
    return RadianToDegree(AngleRadian(vec));
}

float Utils::Dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

bool Utils::CheckCollision(const sf::Sprite& shapeA, const sf::Sprite& shapeB)
{
    if (!shapeA.getGlobalBounds().intersects(shapeB.getGlobalBounds()))
        return false;
    
    auto pointsA = GetShapePoints(shapeA);
    auto pointsB = GetShapePoints(shapeB);
    return PolygonsIntersect(pointsA, shapeA.getTransform(), pointsB, shapeB.getTransform());
}


bool Utils::CheckCollision(const sf::RectangleShape& shapeA, const sf::RectangleShape& shapeB)
{
    if (!shapeA.getGlobalBounds().intersects(shapeB.getGlobalBounds()))
        return false;
    
    auto pointsA = GetShapePoints(shapeA);
    auto pointsB = GetShapePoints(shapeB);
    return PolygonsIntersect(pointsA, shapeA.getTransform(), pointsB, shapeB.getTransform());
}

bool Utils::CheckCircleCollision(const sf::Vector2f& centerA, float radiusA, const sf::Vector2f& centerB, float radiusB)
{
    float distanceSqr = SqrMagnitude(centerA - centerB);
    float radiusSum = radiusA + radiusB;
    return distanceSqr <= radiusSum * radiusSum;
}

bool Utils::PointInTransformBounds(const sf::Transformable& transformable, const sf::FloatRect& localBounds, const sf::Vector2f& point)
{
    sf::Transform inverse = transformable.getInverseTransform();
    sf::Vector2f localPoint = inverse.transformPoint(point);
    return localBounds.contains(localPoint);
}

std::vector<sf::Vector2f> Utils::GetShapePoints(const sf::RectangleShape& shape)
{
    sf::FloatRect localBounds = shape.getLocalBounds();
    return GetRectanglePointsFromBounds(localBounds);
}

std::vector<sf::Vector2f> Utils::GetShapePoints(const sf::Sprite& shape)
{
    sf::FloatRect localBounds = shape.getLocalBounds();
    return GetRectanglePointsFromBounds(localBounds);
}

std::vector<sf::Vector2f> Utils::GetRectanglePointsFromBounds(const sf::FloatRect& localBounds)
{
    std::vector<sf::Vector2f> points(4);
    points[0] = sf::Vector2f(localBounds.left, localBounds.top);
    points[1] = sf::Vector2f(localBounds.left + localBounds.width, localBounds.top);
    points[2] = sf::Vector2f(localBounds.left + localBounds.width, localBounds.top + localBounds.height);
    points[3] = sf::Vector2f(localBounds.left, localBounds.top + localBounds.height);
    return points;
}

bool Utils::PolygonsIntersect(const std::vector<sf::Vector2f>& polygonA, const sf::Transform& transformA, const std::vector<sf::Vector2f>& polygonB, const sf::Transform& transformB)
{
    std::vector<sf::Vector2f> axes;
    axes.reserve(polygonA.size() + polygonB.size());
    
    int countA = polygonA.size();
    for (int i = 0; i < countA; ++i)
    {
        sf::Vector2f p1 = transformA.transformPoint(polygonA[i]);
        sf::Vector2f p2 = transformA.transformPoint(polygonA[(i + 1) % countA]);
        sf::Vector2f edge = p2 - p1;
        
        if (SqrMagnitude(edge) < std::numeric_limits<float>::epsilon())
            continue;
            
        sf::Vector2f normal(-edge.y, edge.x);
        
        bool isDuplicate = false;
        for (const auto& axis : axes)
        {
            float cross = normal.x * axis.y - normal.y * axis.x;
            if (std::abs(cross) < std::numeric_limits<float>::epsilon())
            {
                isDuplicate = true;
                break;
            }
        }
        
        if (!isDuplicate)
            axes.push_back(normal);
    }

    int countB = polygonB.size();
    for (int i = 0; i < countB; ++i)
    {
        sf::Vector2f p1 = transformB.transformPoint(polygonB[i]);
        sf::Vector2f p2 = transformB.transformPoint(polygonB[(i + 1) % countB]);
        sf::Vector2f edge = p2 - p1;
        
        if (SqrMagnitude(edge) < std::numeric_limits<float>::epsilon())
            continue;
            
        sf::Vector2f normal(-edge.y, edge.x);
        
        bool isDuplicate = false;
        for (const auto& axis : axes)
        {
            float cross = normal.x * axis.y - normal.y * axis.x;
            if (std::abs(cross) < std::numeric_limits<float>::epsilon())
            {
                isDuplicate = true;
                break;
            }
        }
        
        if (!isDuplicate)
            axes.push_back(normal);
    }

    for (const auto& axis : axes)
    {
        float minA = std::numeric_limits<float>::max();
        float maxA = std::numeric_limits<float>::lowest();
        for (const auto& point : polygonA)
        {
            sf::Vector2f transformedPoint = transformA.transformPoint(point);
            float projection = Dot(axis, transformedPoint);
            minA = std::min(minA, projection);
            maxA = std::max(maxA, projection);
        }

        float minB = std::numeric_limits<float>::max();
        float maxB = std::numeric_limits<float>::lowest();
        for (const auto& point : polygonB)
        {
            sf::Vector2f transformedPoint = transformB.transformPoint(point);
            float projection = Dot(axis, transformedPoint);
            minB = std::min(minB, projection);
            maxB = std::max(maxB, projection);
        }

        if (maxA < minB || maxB < minA)
        {
            return false;
        }
    }
    return true;
}
