#pragma once
#include "Scene.h"



class SceneTitle : public Scene
{
private:
    // UI Elements
    sf::Sprite backgroundSprite;
    sf::Sprite backgroundCharacter1;
    sf::Sprite backgroundCharacter2;
    sf::Sprite backgroundCharacter3;

    sf::Text textTitle;
    sf::Text textIntro;
    sf::RectangleShape fadeRect;

    // Font Management
    std::string fontId;
    bool fontInit;

    // Visibility States
    bool isShowTitleMessage;
    bool isShowIntroMessage;
    bool Visible;

    // Fade Effect
    float fadeAlpha;
    bool fadeIn;
    bool fadeOut;
    float fadeDuration;
    float fadeTimer;

    // Blink Effect
    float blinkTimer;
    float blinkTime;

    // === Private Methods ===

    // Initialization Methods
    void LoadResources();
    void InitializeUI();
    void SetupTitleText(const sf::Vector2f& windowCenter);
    void SetupIntroText(const sf::Vector2f& windowCenter);
    void SetupViews();
    void SetupBackground();
    void SetupFadeEffect();

    // Reset Methods
    void InitializeFonts();
    void ResetVisibilityFlags();
    void SetupTextContent();
    void PositionUIElements();

    // Text Utility Methods
    void CenterTextOrigin(sf::Text& text);

    // Update Methods
    void UpdateFadeEffect(float dt);
    void ProcessFadeIn(float dt);
    void ProcessFadeOut(float dt);
    void UpdateFadeColor();
    void UpdateBlinkEffect(float dt);
    void HandleInput();

public:
    SceneTitle();
    virtual ~SceneTitle();

    // === Public Interface ===
    virtual void Init() override;
    virtual void Enter() override;
    virtual void Exit() override;
    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderWindow& window) override;

    void Reset();
    void SetTitleMessage(const std::string& msg);
    void SetIntroMessage(const std::string& msg);

};

