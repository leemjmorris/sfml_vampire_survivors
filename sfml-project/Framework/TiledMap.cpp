#include "stdafx.h"
#include "TiledMap.h"
#include "Player.h"

TiledMap::TiledMap(const std::string& textureId, const std::string& name)
    : GameObject(name), textureId(textureId), vertices(sf::Quads)
{
    // LMJ: Default tile size
    tileSize = sf::Vector2f(256.0f, 256.0f);
    currentPlayerChunk = sf::Vector2i(0, 0);
    lastPlayerChunk = sf::Vector2i(0, 0);
}

void TiledMap::SetTextureId(const std::string& textureId)
{
    this->textureId = textureId;
}

void TiledMap::SetTileSize(const sf::Vector2f& size)
{
    tileSize = size;
}

void TiledMap::Init()
{
    // LMJ: Initialize window size
    windowSize = FRAMEWORK.GetWindowSizeF();
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

    // LMJ: Load texture and setup initial tiles
    if (!textureId.empty() && TEXTURE_MGR.Exists(textureId))
    {
        // LMJ: Get texture size to determine tile size
        sf::Vector2u textureSize = TEXTURE_MGR.Get(textureId).getSize();
        tileSize = sf::Vector2f(static_cast<float>(textureSize.x), static_cast<float>(textureSize.y));

        std::cout << "=== INFINITE MAP INITIALIZED ===" << std::endl;
        std::cout << "Tile size: " << tileSize.x << "x" << tileSize.y << std::endl;
        std::cout << "Chunk size: " << CHUNK_SIZE << "x" << CHUNK_SIZE << " tiles" << std::endl;
        std::cout << "View distance: " << VIEW_DISTANCE << " chunks" << std::endl;
        std::cout << "=================================" << std::endl;

        // LMJ: Generate initial chunks around origin
        currentPlayerChunk = sf::Vector2i(0, 0);
        lastPlayerChunk = sf::Vector2i(0, 0);
        GenerateChunksAroundPlayer();
    }
}

void TiledMap::Update(float dt)
{
    if (!player) return;

    // LMJ: Update infinite map based on player position
    UpdateInfiniteMap();
}

void TiledMap::UpdateInfiniteMap()
{
    if (!player) return;

    // LMJ: Get current player chunk
    sf::Vector2f playerPos = player->GetPosition();
    sf::Vector2i newPlayerChunk = GetChunkCoordinates(playerPos);

    // LMJ: Check if player moved to a different chunk
    if (newPlayerChunk != currentPlayerChunk)
    {
        lastPlayerChunk = currentPlayerChunk;
        currentPlayerChunk = newPlayerChunk;

        std::cout << "Player moved to chunk: (" << currentPlayerChunk.x << ", " << currentPlayerChunk.y << ")" << std::endl;

        // LMJ: Generate new chunks around player
        GenerateChunksAroundPlayer();

        // LMJ: Remove distant chunks
        RemoveDistantChunks();
    }
}

bool TiledMap::IsPlayerNearBoundary() const
{
    if (!player) return false;

    sf::Vector2f playerPos = player->GetPosition();
    sf::Vector2f windowCenter = windowSize * 0.5f;

    // LMJ: Check distance from player to window edges
    float distToLeft = playerPos.x - (windowCenter.x - windowSize.x * 0.5f);
    float distToRight = (windowCenter.x + windowSize.x * 0.5f) - playerPos.x;
    float distToTop = playerPos.y - (windowCenter.y - windowSize.y * 0.5f);
    float distToBottom = (windowCenter.y + windowSize.y * 0.5f) - playerPos.y;

    return (distToLeft < boundaryThreshold || distToRight < boundaryThreshold ||
        distToTop < boundaryThreshold || distToBottom < boundaryThreshold);
}

sf::Vector2i TiledMap::GetChunkCoordinates(const sf::Vector2f& worldPos) const
{
    float chunkWorldSize = CHUNK_SIZE * tileSize.x; // LMJ: Assuming square tiles

    int chunkX = static_cast<int>(std::floor(worldPos.x / chunkWorldSize));
    int chunkY = static_cast<int>(std::floor(worldPos.y / chunkWorldSize));

    return sf::Vector2i(chunkX, chunkY);
}

sf::Vector2f TiledMap::GetChunkWorldPosition(const sf::Vector2i& chunkCoord) const
{
    float chunkWorldSize = CHUNK_SIZE * tileSize.x;

    return sf::Vector2f(
        chunkCoord.x * chunkWorldSize,
        chunkCoord.y * chunkWorldSize
    );
}

void TiledMap::GenerateChunksAroundPlayer()
{
    // LMJ: Generate chunks in a square around the player
    std::vector<sf::Vector2i> chunksToGenerate = GetChunksInRange(currentPlayerChunk, VIEW_DISTANCE);

    for (const sf::Vector2i& chunkCoord : chunksToGenerate)
    {
        if (!ChunkExists(chunkCoord))
        {
            BuildChunk(chunkCoord);
            std::cout << "Generated chunk: (" << chunkCoord.x << ", " << chunkCoord.y << ")" << std::endl;
        }
    }
}

void TiledMap::RemoveDistantChunks()
{
    std::vector<sf::Vector2i> chunksToRemove;

    // LMJ: Find chunks that are too far from player
    for (const auto& pair : chunks)
    {
        sf::Vector2i chunkCoord = pair.second.chunkCoord;

        // LMJ: Calculate distance from player chunk
        int deltaX = std::abs(chunkCoord.x - currentPlayerChunk.x);
        int deltaY = std::abs(chunkCoord.y - currentPlayerChunk.y);

        // LMJ: Remove if beyond view distance + 1 (buffer zone)
        if (deltaX > VIEW_DISTANCE + 1 || deltaY > VIEW_DISTANCE + 1)
        {
            chunksToRemove.push_back(chunkCoord);
        }
    }

    // LMJ: Remove distant chunks
    for (const sf::Vector2i& chunkCoord : chunksToRemove)
    {
        RemoveChunk(chunkCoord);
        std::cout << "Removed chunk: (" << chunkCoord.x << ", " << chunkCoord.y << ")" << std::endl;
    }
}

void TiledMap::BuildChunk(const sf::Vector2i& chunkCoord)
{
    // LMJ: Create new chunk
    Chunk newChunk(chunkCoord);

    // LMJ: Calculate chunk world position
    sf::Vector2f chunkWorldPos = GetChunkWorldPosition(chunkCoord);

    // LMJ: Get texture size for UV coordinates
    sf::Vector2u textureSize = TEXTURE_MGR.Get(textureId).getSize();
    float texWidth = static_cast<float>(textureSize.x);
    float texHeight = static_cast<float>(textureSize.y);

    // LMJ: Reserve space for vertices (CHUNK_SIZE x CHUNK_SIZE tiles, 4 vertices per tile)
    int tileCount = CHUNK_SIZE * CHUNK_SIZE;
    newChunk.vertices.resize(tileCount * 4);

    // LMJ: Build vertices for this chunk
    int vertexIndex = 0;
    for (int y = 0; y < CHUNK_SIZE; ++y)
    {
        for (int x = 0; x < CHUNK_SIZE; ++x)
        {
            float posX = chunkWorldPos.x + x * tileSize.x;
            float posY = chunkWorldPos.y + y * tileSize.y;

            // LMJ: Create quad for this tile
            newChunk.vertices[vertexIndex].position = sf::Vector2f(posX, posY);
            newChunk.vertices[vertexIndex].texCoords = sf::Vector2f(0, 0);
            newChunk.vertices[vertexIndex + 1].position = sf::Vector2f(posX + tileSize.x, posY);
            newChunk.vertices[vertexIndex + 1].texCoords = sf::Vector2f(texWidth, 0);
            newChunk.vertices[vertexIndex + 2].position = sf::Vector2f(posX + tileSize.x, posY + tileSize.y);
            newChunk.vertices[vertexIndex + 2].texCoords = sf::Vector2f(texWidth, texHeight);
            newChunk.vertices[vertexIndex + 3].position = sf::Vector2f(posX, posY + tileSize.y);
            newChunk.vertices[vertexIndex + 3].texCoords = sf::Vector2f(0, texHeight);

            vertexIndex += 4;
        }
    }

    newChunk.isBuilt = true;

    // LMJ: Add chunk to map
    long long hash = HashChunkCoord(chunkCoord);
    chunks[hash] = newChunk;
}

bool TiledMap::ChunkExists(const sf::Vector2i& chunkCoord) const
{
    long long hash = HashChunkCoord(chunkCoord);
    return chunks.find(hash) != chunks.end();
}

void TiledMap::RemoveChunk(const sf::Vector2i& chunkCoord)
{
    long long hash = HashChunkCoord(chunkCoord);
    auto it = chunks.find(hash);
    if (it != chunks.end())
    {
        chunks.erase(it);
    }
}

long long TiledMap::HashChunkCoord(const sf::Vector2i& coord) const
{
    // LMJ: Simple hash function for chunk coordinates
    // LMJ: Combine x and y coordinates into a single hash
    return (static_cast<long long>(coord.x) << 32) | static_cast<long long>(coord.y);
}

std::vector<sf::Vector2i> TiledMap::GetChunksInRange(const sf::Vector2i& center, int range) const
{
    std::vector<sf::Vector2i> chunks;

    for (int x = center.x - range; x <= center.x + range; ++x)
    {
        for (int y = center.y - range; y <= center.y + range; ++y)
        {
            chunks.push_back(sf::Vector2i(x, y));
        }
    }

    return chunks;
}

void TiledMap::Draw(sf::RenderWindow& window)
{
    if (!active || textureId.empty() || chunks.empty())
        return;

    // LMJ: Get current view for frustum culling
    sf::View currentView = window.getView();
    sf::FloatRect viewBounds = sf::FloatRect(
        currentView.getCenter().x - currentView.getSize().x * 0.6f,
        currentView.getCenter().y - currentView.getSize().y * 0.6f,
        currentView.getSize().x * 1.2f,
        currentView.getSize().y * 1.2f
    );

    // LMJ: Draw all visible chunks
    sf::RenderStates states;
    states.texture = &TEXTURE_MGR.Get(textureId);

    for (const auto& pair : chunks)
    {
        const Chunk& chunk = pair.second;

        if (!chunk.isBuilt) continue;

        // LMJ: Calculate chunk bounds for frustum culling
        sf::Vector2f chunkWorldPos = GetChunkWorldPosition(chunk.chunkCoord);
        float chunkWorldSize = CHUNK_SIZE * tileSize.x;
        sf::FloatRect chunkBounds(chunkWorldPos.x, chunkWorldPos.y, chunkWorldSize, chunkWorldSize);

        // LMJ: Only draw chunks that are visible
        if (viewBounds.intersects(chunkBounds))
        {
            window.draw(chunk.vertices, states);
        }
    }
}

void TiledMap::UpdateMapBounds()
{
    // LMJ: For infinite map, bounds are dynamic and not needed for boundary checking
    // LMJ: This method can be used for other purposes if needed
}

sf::FloatRect TiledMap::GetMapBounds() const
{
    // LMJ: For infinite map, return a large bounds around the player
    if (!player)
    {
        return sf::FloatRect(-10000.0f, -10000.0f, 20000.0f, 20000.0f);
    }

    sf::Vector2f playerPos = player->GetPosition();
    float mapSize = (VIEW_DISTANCE * 2 + 1) * CHUNK_SIZE * tileSize.x;

    return sf::FloatRect(
        playerPos.x - mapSize * 0.5f,
        playerPos.y - mapSize * 0.5f,
        mapSize,
        mapSize
    );
}

sf::Vector2f TiledMap::ClampToMapBounds(const sf::Vector2f& position, float objectRadius) const
{
    // LMJ: For infinite map, no clamping is needed - return position as is
    // LMJ: The infinite map system handles boundaries differently
    return position;
}