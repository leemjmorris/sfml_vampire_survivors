#pragma once
#include "Singleton.h"
class Framework : public Singleton<Framework>
{
	friend Singleton<Framework>;

protected:
	Framework() = default;
	~Framework() override = default;

	sf::RenderWindow window;

	sf::Clock clock;
	float timeScale = 1.f;

	float time = 0.f;
	float deltaTime = 0.f;

	float realTime = 0.f;
	float realDeltaTime = 0.f;

	std::vector<std::string> texIds;
	std::vector<std::string> fontIds;
	std::vector<std::string> soundIds;

public:
	sf::RenderWindow& GetWindow() { return window; }

	sf::Vector2u GetWindowSize() const { return window.getSize(); }
	sf::Vector2f GetWindowSizeF() const { return (sf::Vector2f)window.getSize(); }
	sf::FloatRect GetWindowBounds() const
	{
		sf::Vector2f size = GetWindowSizeF();
		return sf::FloatRect(0.f, 0.f, size.x, size.y);
	}

	float GetTimeScale() const { return timeScale; }
	void SetTimeScale(float scale) { timeScale = scale; }

	float GetTime() const { return time; }
	float GetDeltaTime() const { return deltaTime; }
	float GetRealTime() const { return realTime; }
	float GetRealDeltaTime() const { return realDeltaTime; }

	virtual void Init(int w, int h, const std::string& t);
	virtual void Do();
	virtual void Release();
};

#define FRAMEWORK (Framework::Instance())

