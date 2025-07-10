#include "stdafx.h"
#include "TextGo.h"

TextGo::TextGo(const std::string& fontId, const std::string& name)
	: GameObject(name), fontId(fontId)
{
}

void TextGo::SetString(const std::string& str)
{
	text.setString(str);
	Utils::SetOrigin(text, originPreset);
}

void TextGo::SetCharacterSize(unsigned int size)
{
	text.setCharacterSize(size);
	Utils::SetOrigin(text, originPreset);
}

void TextGo::SetFillColor(const sf::Color& color)
{
	text.setFillColor(color);
}

std::string TextGo::GetString() const
{
	return text.getString();
}

void TextGo::SetPosition(const sf::Vector2f& pos)
{
	position = pos;
	text.setPosition(pos);
}

void TextGo::SetRotation(float rot)
{
	rotation = rot;
	text.setRotation(rotation);
}

void TextGo::SetScale(const sf::Vector2f& s)
{
	scale = s;
	text.setScale(scale);
}

void TextGo::SetOrigin(const sf::Vector2f& o)
{
	originPreset = Origins::Custom;
	origin = o;
	text.setOrigin(o);
}

void TextGo::SetOrigin(Origins preset)
{
	originPreset = preset;
	if (originPreset != Origins::Custom)
	{
		origin = Utils::SetOrigin(text, originPreset);
	}
}

void TextGo::Init()
{
}

void TextGo::Release()
{
}

void TextGo::Reset()
{
	text.setFont(FONT_MGR.Get(fontId));
	Utils::SetOrigin(text, originPreset);
}

void TextGo::Update(float dt)
{
}

void TextGo::Draw(sf::RenderWindow& window)
{
	window.draw(text);
}