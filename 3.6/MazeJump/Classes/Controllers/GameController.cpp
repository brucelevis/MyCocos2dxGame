//
//  GameController.cpp
//  MazeJump
//
//  Created by wang haibo on 15/7/4.
//
//

#include "GameController.h"
#include "LevelsManager.h"
#include "ui/CocosGUI.h"
#include "AlisaMethod.h"
#include "MainScene.h"
#include "UIManager.h"
#include "GameConst.h"
#include "MenuScene.h"
USING_NS_CC;

GameController* g_pGameControllerInstance = nullptr;
GameController* GameController::getInstance()
{
    if( g_pGameControllerInstance == nullptr )
        g_pGameControllerInstance = new GameController();
    
    return g_pGameControllerInstance;
}
GameController::GameController()
{
    m_pMainLayer        = nullptr;
    m_pSkyBox           = nullptr;
    m_pGroundLayer      = nullptr;
    m_pMainCamera       = nullptr;
    m_pWhiteLayer       = nullptr;
    m_currentLevel      = 0;
    m_difficultLevel    = 0 ;
    m_mazeMode = NORAML;
}
GameController::~GameController()
{
}
bool GameController::init(Layer* pMainLayer,int difficultLevel,MAZE_MODE mazeMode)
{
    if(pMainLayer == nullptr)
        return false;
    m_pMainLayer = pMainLayer;
    m_difficultLevel = difficultLevel;
    m_mazeMode = mazeMode;
    
    m_pWhiteLayer = LayerColor::create(Color4B::WHITE);
    if(!m_pWhiteLayer)
        return false;
    m_pMainLayer->addChild(m_pWhiteLayer,LAYER_WHITE);
    EaseExponentialOut* fadeOut = EaseExponentialOut::create(FadeOut::create(1.0f));
    m_pWhiteLayer->runAction(fadeOut);
    
    auto size = Director::getInstance()->getVisibleSize();
    m_pMainCamera = Camera::createPerspective(90, size.width/size.height, 1, 5000);
    if(!m_pMainCamera)
        return false;
    m_pMainLayer->addChild(m_pMainCamera);
    
    
    if (m_mazeMode==MAZE) {
         m_currentLevel = m_difficultLevel;
        if (m_currentLevel>24) {
            m_currentLevel = 24;
        }
    }else if (m_mazeMode==NORAML)
    {
        m_currentLevel = randomLevel();
    }
    
    UIManager::getInstance()->init(m_pMainLayer);
    UIManager::getInstance()->setGameUi(UIManager::UI_GROUND_GAME);

    CCLOG("LEVEL:%d",m_currentLevel);
    if (!createMap(false,m_currentLevel)) {
        return false;
    }
    
    ParticleSystemQuad* starfield= ParticleSystemQuad::create("starfield.plist");
    if(!starfield)
    {
        CCLOG("Load explosion particle system failed! file: starfield.plist");
        return false;
    }
    starfield->setStartSize(5.0f);
    starfield->setStartSizeVar(3.0f);
    starfield->setGravity(Vec2(-40.0f, 0));
    m_pMainLayer->addChild(starfield);
    
    
    AmbientLight* ambLight = AmbientLight::create(Color3B(150, 150, 150));
    m_pMainLayer->addChild(ambLight);
    DirectionLight* directionLight = DirectionLight::create(Vec3(-3, -4, -2), Color3B(158, 158, 158));
    m_pMainLayer->addChild(directionLight);

    

    return true;
}
int GameController::randomLevel()
{
    int patternIndex = 0;
    int patternBeginIndex = m_difficultLevel ;
    int patternEndIndex = m_difficultLevel + 4;
    if(patternEndIndex >= LevelsManager::getInstance()->getMaxLevels()-1)
    {
        patternBeginIndex = LevelsManager::getInstance()->getMaxLevels() - 6;
        patternEndIndex = LevelsManager::getInstance()->getMaxLevels() - 1;
    }
    float percent5 = 0.1;
    float percent4 = 0.1;
    float percent3 = 0.1;
    float percent2 = 0.2;
    float percent1 = 1.0 - percent2 - percent3 - percent4 - percent5 ;
    AlisaMethod* am = AlisaMethod::create(percent1,percent2,percent3,percent4,percent5,-1.0, NULL);
    if(am)
    {
        patternIndex = am->getRandomIndex() + patternBeginIndex;
        CCASSERT(patternIndex >=patternBeginIndex && patternIndex <= patternEndIndex, "pattern index must between patternBeginIndex and patternEndIndex");
    }
    return patternIndex;
}

void GameController::update(float delta)
{

}
void GameController::destroy()
{
    m_pMainLayer->removeAllChildren();
    m_pMainLayer = nullptr;
    UIManager::getInstance()->destory();
}

bool GameController::createMap(bool _playing,int level)
{
    if (m_pSkyBox) {
         m_pMainLayer->removeChild(m_pSkyBox);
        m_pSkyBox = nullptr;
    }
    if (m_pGroundLayer) {
        m_pMainLayer->removeChild(m_pGroundLayer);
        m_pGroundLayer = nullptr;
    }
    
    std::string skyTexName = LevelsManager::getInstance()->getLevelSkyTextureName(level);
    std::string skyTexTop = skyTexName + "_t.png";
    std::string skyTexBottom = skyTexName + "_b.png";
    std::string skyTex = skyTexName + ".png";
    m_pSkyBox = Skybox::create(skyTex, skyTex, skyTex, skyTex, skyTex, skyTex);
    if(!m_pSkyBox)
        return false;
    m_pSkyBox->setScale(1000);
    m_pSkyBox->setCameraMask((unsigned short)CameraFlag::USER1);
    m_pMainLayer->addChild(m_pSkyBox);
    
    
    m_pGroundLayer = GroundLayer::create(level,_playing);
    if(!m_pGroundLayer)
        return false;
    m_pGroundLayer->setCameraMask((unsigned short)CameraFlag::USER1);
    ///focus
    m_pGroundLayer->setAnchorPoint(Vec2::ZERO);
    m_pMainLayer->addChild(m_pGroundLayer);
    
   
    m_pMainCamera->setPosition3D(Vec3(0,-m_pGroundLayer->getGroundRadius()*2.5f*cosf(M_PI),m_pGroundLayer->getGroundRadius()*2.5f*sinf(M_PI/4)) + m_pGroundLayer->getOffset());
    if (m_mazeMode==MAZE) {
         m_pMainCamera->lookAt(m_pGroundLayer->getPosition3D() + m_pGroundLayer->getOffset());
    }else if (m_mazeMode == NORAML)
    {
         m_pMainCamera->lookAt(m_pGroundLayer->getPosition3D() + m_pGroundLayer->getOffset()+Vec3(0,0,15));
    }
   

    m_pMainCamera->setCameraFlag(CameraFlag::USER1);
    m_pGroundLayer->setCamera(m_pMainCamera);
    
    
    return true;
}
void GameController::switchToRainbowRun()
{
    if(m_pWhiteLayer)
    {
        EaseExponentialOut* fadeIn = EaseExponentialOut::create(FadeIn::create(1.0f));
        CallFunc* callFunc = CallFunc::create(CC_CALLBACK_0(GameController::switchToMainScene, this));
        Sequence* sequence = Sequence::create( fadeIn, callFunc, NULL);
        m_pWhiteLayer->runAction(sequence);
    }
}
void GameController::switchToMainScene()
{
    Director::getInstance()->popScene();
}
void GameController::switchToMenu()
{
    if(m_pWhiteLayer)
    {
        EaseExponentialOut* fadeIn = EaseExponentialOut::create(FadeIn::create(1.0f));
        CallFunc* callFunc = CallFunc::create(CC_CALLBACK_0(GameController::switchToMenuScene, this));
        Sequence* sequence = Sequence::create( fadeIn, callFunc, NULL);
        m_pWhiteLayer->runAction(sequence);
    }
}
void GameController::switchToMenuScene()
{
    auto menuScene = MenuScene::createScene();
    Director::getInstance()->replaceScene(menuScene);
}
int GameController::getCurrentGoldReward()
{
    
    int a = m_currentLevel*m_currentLevel*0.3;
    CCLOG("goldreward:%d",a - a%10 +m_currentLevel*4+10);
    return a - a%10 +m_currentLevel*4+10;
}
int GameController::getCurrentHeartReward()
{
    return  1;
}
