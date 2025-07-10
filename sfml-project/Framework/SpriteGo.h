#pragma once
#include "GameObject.h"

class SpriteGo : public GameObject
{
protected:
	sf::Sprite sprite;
	std::string textureId;

public:
	SpriteGo(const std::string& texPlayerId = "", const std::string& name = "");
	~SpriteGo() override = default;

	const std::string& GetTextureId() const { return textureId; }
	void SetTextureId(const std::string& texPlayerId) { textureId = texPlayerId; }

	sf::Sprite& GetSprite() { return sprite; }
	const sf::Sprite& GetSprite() const { return sprite; }

	void SetPosition(const sf::Vector2f& pos) override;
	void SetRotation(float rot) override;
	void SetScale(const sf::Vector2f& s) override;
	void SetOrigin(const sf::Vector2f& o) override;
	void SetOrigin(Origins preset) override;

	void Init() override;
	void Release() override;
	void Reset() override;
	void Update(float dt) override;
	void Draw(sf::RenderWindow& window) override;
};

