#pragma once
#include "GameObject.h"

class TiledMap : public GameObject
{
protected:
    sf::VertexArray vertices; // LMJ: Use SFML's VertexArray for better performance
    sf::Vector2i mapSizeInTiles; // LMJ: Map size in number of tiles
    sf::Vector2f tileSize; // LMJ: Size of each tile in pixels
    sf::Vector2f mapSizeInPixels; // LMJ: Total map size in pixels
    sf::FloatRect mapBounds; // LMJ: Map boundaries for collision detection

    // LMJ: Chunking system for better performance
    static const int CHUNK_SIZE = 64; // LMJ: 64x64 tiles per chunk
    struct Chunk
    {
        sf::VertexArray vertices;
        sf::FloatRect bounds;
        bool isBuilt;

        Chunk() : vertices(sf::Quads), isBuilt(false) {}
    };

    std::vector<std::vector<Chunk>> chunks; // LMJ: 2D array of chunks
    sf::Vector2i chunkCount; // LMJ: Number of chunks in X and Y

public:
    TiledMap(const std::string& textureId = "", const std::string& name = "TiledMap");
    ~TiledMap() override = default;

    void SetTextureId(const std::string& textureId);
    void SetMapSize(const sf::Vector2i& sizeInTiles);
    void SetTileSize(const sf::Vector2f& size);

    // LMJ: Get map boundaries
    sf::FloatRect GetMapBounds() const { return mapBounds; }
    sf::Vector2f GetMapSize() const { return mapSizeInPixels; }

    // LMJ: Check if position is within map bounds
    bool IsPositionInBounds(const sf::Vector2f& position, float margin = 0.0f) const;

    // LMJ: Clamp position to stay within map bounds
    sf::Vector2f ClampToMapBounds(const sf::Vector2f& position, float objectRadius = 0.0f) const;

    void Init() override;
    void Release() override;
    void Reset() override;
    void Update(float dt) override;
    void Draw(sf::RenderWindow& window) override;

protected:
    std::string textureId;
    void UpdateMapBounds();
    void BuildChunks(); // LMJ: Build all chunks at initialization
    void BuildChunk(int chunkX, int chunkY); // LMJ: Build specific chunk
};