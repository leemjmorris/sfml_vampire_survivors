#pragma once
#include "GameObject.h"
#include <unordered_map>

class Player;

class TiledMap : public GameObject
{
protected:
    sf::VertexArray vertices; // LMJ: Use SFML's VertexArray for better performance
    sf::Vector2f tileSize; // LMJ: Size of each tile in pixels

    // LMJ: Infinite map system
    static const int CHUNK_SIZE = 32; // LMJ: 32x32 tiles per chunk
    static const int VIEW_DISTANCE = 2; // LMJ: How many chunks to keep around player in each direction

    struct Chunk
    {
        sf::VertexArray vertices;
        sf::Vector2i chunkCoord; // LMJ: Chunk coordinates (not world coordinates)
        bool isBuilt;

        Chunk() : vertices(sf::Quads), isBuilt(false) {}
        Chunk(sf::Vector2i coord) : vertices(sf::Quads), chunkCoord(coord), isBuilt(false) {}
    };

    // LMJ: Use hash map for efficient chunk lookup
    std::unordered_map<long long, Chunk> chunks; // LMJ: Key = hash of chunk coordinates

    // LMJ: Player tracking for infinite map
    Player* player = nullptr;
    sf::Vector2i currentPlayerChunk; // LMJ: Which chunk the player is currently in
    sf::Vector2i lastPlayerChunk; // LMJ: Previous chunk position to detect movement

    // LMJ: Window bounds for player boundary detection
    sf::Vector2f windowSize;
    float boundaryThreshold = 200.0f; // LMJ: Distance from edge to trigger map expansion

public:
    TiledMap(const std::string& textureId = "", const std::string& name = "TiledMap");
    ~TiledMap() override = default;

    void SetTextureId(const std::string& textureId);
    void SetTileSize(const sf::Vector2f& size);
    void SetPlayer(Player* playerRef) { player = playerRef; }
    void SetBoundaryThreshold(float threshold) { boundaryThreshold = threshold; }

    // LMJ: Infinite map methods
    void UpdateInfiniteMap();
    bool IsPlayerNearBoundary() const;
    sf::Vector2i GetChunkCoordinates(const sf::Vector2f& worldPos) const;
    sf::Vector2f GetChunkWorldPosition(const sf::Vector2i& chunkCoord) const;

    // LMJ: Chunk management
    void GenerateChunksAroundPlayer();
    void RemoveDistantChunks();
    void BuildChunk(const sf::Vector2i& chunkCoord);
    bool ChunkExists(const sf::Vector2i& chunkCoord) const;
    void RemoveChunk(const sf::Vector2i& chunkCoord);

    // LMJ: Utility methods
    long long HashChunkCoord(const sf::Vector2i& coord) const;
    std::vector<sf::Vector2i> GetChunksInRange(const sf::Vector2i& center, int range) const;

    // LMJ: Legacy compatibility methods (for infinite map, these return dynamic bounds)
    sf::FloatRect GetMapBounds() const;
    sf::Vector2f ClampToMapBounds(const sf::Vector2f& position, float objectRadius = 0.0f) const;

    void Init() override;
    void Release() override;
    void Reset() override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

protected:
    std::string textureId;
    void UpdateMapBounds();
};