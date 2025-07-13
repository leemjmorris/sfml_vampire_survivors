#include "stdafx.h"
#include "TiledMap.h"

TiledMap::TiledMap(const std::string& textureId, const std::string& name)
    : GameObject(name), textureId(textureId), vertices(sf::Quads)
{
    // LMJ: Default map size - will be adjusted in Reset
    mapSizeInTiles = sf::Vector2i(4, 4);
    tileSize = sf::Vector2f(256.0f, 256.0f);
}

void TiledMap::SetTextureId(const std::string& textureId)
{
    this->textureId = textureId;
}

void TiledMap::SetMapSize(const sf::Vector2i& sizeInTiles)
{
    mapSizeInTiles = sizeInTiles;
    UpdateMapBounds();
}

void TiledMap::SetTileSize(const sf::Vector2f& size)
{
    tileSize = size;
    UpdateMapBounds();
}

void TiledMap::UpdateMapBounds()
{
    // LMJ: Calculate total map size in pixels
    mapSizeInPixels.x = mapSizeInTiles.x * tileSize.x;
    mapSizeInPixels.y = mapSizeInTiles.y * tileSize.y;

    // LMJ: Set map bounds (assuming map starts at origin)
    mapBounds = sf::FloatRect(0.0f, 0.0f, mapSizeInPixels.x, mapSizeInPixels.y);
}

bool TiledMap::IsPositionInBounds(const sf::Vector2f& position, float margin) const
{
    // LMJ: Check if position is within map bounds with optional margin
    return (position.x >= mapBounds.left + margin &&
        position.x <= mapBounds.left + mapBounds.width - margin &&
        position.y >= mapBounds.top + margin &&
        position.y <= mapBounds.top + mapBounds.height - margin);
}

sf::Vector2f TiledMap::ClampToMapBounds(const sf::Vector2f& position, float objectRadius) const
{
    sf::Vector2f clampedPos = position;

    // LMJ: Clamp X position
    if (clampedPos.x < mapBounds.left + objectRadius)
        clampedPos.x = mapBounds.left + objectRadius;
    else if (clampedPos.x > mapBounds.left + mapBounds.width - objectRadius)
        clampedPos.x = mapBounds.left + mapBounds.width - objectRadius;

    // LMJ: Clamp Y position
    if (clampedPos.y < mapBounds.top + objectRadius)
        clampedPos.y = mapBounds.top + objectRadius;
    else if (clampedPos.y > mapBounds.top + mapBounds.height - objectRadius)
        clampedPos.y = mapBounds.top + mapBounds.height - objectRadius;

    return clampedPos;
}

void TiledMap::Init()
{
    // LMJ: Texture will be loaded in Reset()
}

void TiledMap::Release()
{
    // LMJ: Clean up chunks
    chunks.clear();
}

void TiledMap::Reset()
{
    sortingLayer = SortingLayers::Background;
    sortingOrder = 0;

    // LMJ: Load texture and setup tiles
    if (!textureId.empty() && TEXTURE_MGR.Exists(textureId))
    {
        // LMJ: Get texture size to determine tile size
        sf::Vector2u textureSize = TEXTURE_MGR.Get(textureId).getSize();
        tileSize = sf::Vector2f(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));

        // LMJ: Calculate smaller map size for better performance (2.5x instead of 4x)
        sf::Vector2f windowSize = FRAMEWORK.GetWindowSizeF();
        float targetMapWidth = windowSize.x * 2.5f;
        float targetMapHeight = windowSize.y * 2.5f;

        int tilesX = static_cast<int>(std::ceil(targetMapWidth / tileSize.x));
        int tilesY = static_cast<int>(std::ceil(targetMapHeight / tileSize.y));

        mapSizeInTiles = sf::Vector2i(tilesX, tilesY);
        UpdateMapBounds();

        // LMJ: Calculate chunk count
        chunkCount.x = (mapSizeInTiles.x + CHUNK_SIZE - 1) / CHUNK_SIZE;
        chunkCount.y = (mapSizeInTiles.y + CHUNK_SIZE - 1) / CHUNK_SIZE;

        std::cout << "=== OPTIMIZED MAP INFO ===" << std::endl;
        std::cout << "Window size: " << windowSize.x << "x" << windowSize.y << std::endl;
        std::cout << "Map size: " << mapSizeInPixels.x << "x" << mapSizeInPixels.y << std::endl;
        std::cout << "Tiles: " << mapSizeInTiles.x << "x" << mapSizeInTiles.y << std::endl;
        std::cout << "Chunks: " << chunkCount.x << "x" << chunkCount.y << std::endl;
        std::cout << "=========================" << std::endl;

        // LMJ: Build chunks
        BuildChunks();
    }
}

void TiledMap::BuildChunks()
{
    // LMJ: Initialize chunk array
    chunks.clear();
    chunks.resize(chunkCount.y);
    for (int y = 0; y < chunkCount.y; ++y)
    {
        chunks[y].resize(chunkCount.x);
    }

    // LMJ: Get texture size for UV coordinates
    sf::Vector2u textureSize = TEXTURE_MGR.Get(textureId).getSize();
    float texWidth = static_cast<float>(textureSize.x);
    float texHeight = static_cast<float>(textureSize.y);

    // LMJ: Build each chunk
    for (int chunkY = 0; chunkY < chunkCount.y; ++chunkY)
    {
        for (int chunkX = 0; chunkX < chunkCount.x; ++chunkX)
        {
            Chunk& chunk = chunks[chunkY][chunkX];

            // LMJ: Calculate tile range for this chunk
            int startTileX = chunkX * CHUNK_SIZE;
            int endTileX = std::min(startTileX + CHUNK_SIZE, mapSizeInTiles.x);
            int startTileY = chunkY * CHUNK_SIZE;
            int endTileY = std::min(startTileY + CHUNK_SIZE, mapSizeInTiles.y);

            // LMJ: Set chunk bounds
            chunk.bounds = sf::FloatRect(
                startTileX * tileSize.x,
                startTileY * tileSize.y,
                (endTileX - startTileX) * tileSize.x,
                (endTileY - startTileY) * tileSize.y
            );

            // LMJ: Reserve space for vertices
            int tileCount = (endTileX - startTileX) * (endTileY - startTileY);
            chunk.vertices.resize(tileCount * 4);

            // LMJ: Build vertices for this chunk
            int vertexIndex = 0;
            for (int y = startTileY; y < endTileY; ++y)
            {
                for (int x = startTileX; x < endTileX; ++x)
                {
                    float posX = x * tileSize.x;
                    float posY = y * tileSize.y;

                    // LMJ: Create quad for this tile
                    chunk.vertices[vertexIndex].position = sf::Vector2f(posX, posY);
                    chunk.vertices[vertexIndex].texCoords = sf::Vector2f(0, 0);
                    chunk.vertices[vertexIndex + 1].position = sf::Vector2f(posX + tileSize.x, posY);
                    chunk.vertices[vertexIndex + 1].texCoords = sf::Vector2f(texWidth, 0);
                    chunk.vertices[vertexIndex + 2].position = sf::Vector2f(posX + tileSize.x, posY + tileSize.y);
                    chunk.vertices[vertexIndex + 2].texCoords = sf::Vector2f(texWidth, texHeight);
                    chunk.vertices[vertexIndex + 3].position = sf::Vector2f(posX, posY + tileSize.y);
                    chunk.vertices[vertexIndex + 3].texCoords = sf::Vector2f(0, texHeight);

                    vertexIndex += 4;
                }
            }

            chunk.isBuilt = true;
        }
    }
}

void TiledMap::BuildChunk(int chunkX, int chunkY)
{
    // LMJ: This method is for future use if we want to build chunks on demand
    if (chunkY >= 0 && chunkY < chunkCount.y && chunkX >= 0 && chunkX < chunkCount.x)
    {
        // Implementation would be similar to the chunk building in BuildChunks()
    }
}

void TiledMap::Update(float dt)
{
    // LMJ: Map doesn't need regular updates
}

void TiledMap::Draw(sf::RenderWindow& window)
{
    if (!active || textureId.empty() || chunks.empty())
        return;

    // LMJ: Get current view bounds
    sf::View currentView = window.getView();
    sf::FloatRect viewBounds = sf::FloatRect(
        currentView.getCenter().x - currentView.getSize().x * 0.6f,
        currentView.getCenter().y - currentView.getSize().y * 0.6f,
        currentView.getSize().x * 1.2f,
        currentView.getSize().y * 1.2f
    );

    // LMJ: Draw only visible chunks
    sf::RenderStates states;
    states.texture = &TEXTURE_MGR.Get(textureId);

    for (int chunkY = 0; chunkY < chunkCount.y; ++chunkY)
    {
        for (int chunkX = 0; chunkX < chunkCount.x; ++chunkX)
        {
            const Chunk& chunk = chunks[chunkY][chunkX];

            // LMJ: Check if chunk is visible
            if (chunk.isBuilt && viewBounds.intersects(chunk.bounds))
            {
                window.draw(chunk.vertices, states);
            }
        }
    }
}