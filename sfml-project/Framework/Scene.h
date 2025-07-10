#pragma once

class Scene
{
protected:
	std::list<GameObject*> gameObjects;

	std::list<GameObject*> objectsToAdd;
	std::list<GameObject*> objectsToRemove;

	std::vector<std::string> texIds;
	std::vector<std::string> fontIds;
	std::vector<std::string> soundIds;

	sf::View uiView;
	sf::View worldView;

	void ApplyPendingChanges();

public:
	const SceneIds Id;

	Scene(SceneIds id);
	virtual ~Scene() = default;

	virtual void Init();
	virtual void Release();

	virtual void Enter();
	virtual void Exit();

	virtual void Update(float dt);
	virtual void Draw(sf::RenderWindow& window);

	GameObject* AddGameObject(GameObject* go);
	void RemoveGameObject(GameObject* go);

	GameObject* FindGameObject(const std::string& name);
	std::vector<GameObject*> FindGameObjects(const std::string& name);
	void FindGameObjects(const std::string& name, std::vector<GameObject*>& results);


	sf::Vector2f ScreenToWorld(sf::Vector2i screenPos);
	sf::Vector2i WorldToScreen(sf::Vector2f worldPos);
	sf::Vector2f ScreenToUi(sf::Vector2i screePos);
	sf::Vector2i UiToScreen(sf::Vector2f worldPos);
};

struct DrawOrderComparer
{
	bool operator()(const GameObject* a, const GameObject* b)
	{
		if (a->sortingLayer != b->sortingLayer)
		{
			return a->sortingLayer < b->sortingLayer;
		}
		return a->sortingOrder < b->sortingOrder;
	}
};