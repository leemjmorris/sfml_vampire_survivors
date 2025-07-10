#include "stdafx.h"
#include "Framework.h"

void Framework::Init(int w, int h, const std::string& t)
{
	window.create(sf::VideoMode(w, h), t);

	TEXTURE_MGR.Load(texIds);
	FONT_MGR.Load(fontIds);
	SOUNDBUFFER_MGR.Load(soundIds);

    Utils::Init();
	InputMgr::Init();
    SOUND_MGR.Init();
	SCENE_MGR.Init();
}

void Framework::Do()
{
    while (window.isOpen())
    {
        sf::Time dt = clock.restart();
        realDeltaTime = deltaTime = dt.asSeconds();
        deltaTime *= timeScale;
        time += deltaTime;
        realTime = realDeltaTime;

        InputMgr::Clear();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            InputMgr::UpdateEvent(event);
        }

        InputMgr::Update(deltaTime);

        // Update
        SOUND_MGR.Update(deltaTime);

#ifdef DEF_DEV
        if (InputMgr::GetKeyDown(sf::Keyboard::F10))
        {
            Variables::isDrawHitBox = !Variables::isDrawHitBox;
        }
#endif


        SCENE_MGR.Update(deltaTime);

        // Draw
        window.clear();
        SCENE_MGR.Draw(window);
        window.display();
    }
}

void Framework::Release()
{
    SOUND_MGR.Release();
	SCENE_MGR.Release();

	SOUNDBUFFER_MGR.Unload(soundIds);
	FONT_MGR.Unload(fontIds);
	TEXTURE_MGR.Unload(texIds);
}
