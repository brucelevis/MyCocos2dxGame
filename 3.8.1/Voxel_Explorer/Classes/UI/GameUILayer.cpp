//
//  GameUILayer.cpp
//  Voxel_Explorer
//
//  Created by 创李 on 15/10/16.
//
//

#include "GameUILayer.h"
#include "UtilityHelper.h"
#include "PopupUILayerManager.h"
#include "PlayerProperty.hpp"
#include "GameFormula.hpp"
#include "VoxelExplorer.h"
#include "EventConst.h"
#include "ui/CocosGUI.h"
#include "RandomDungeon.hpp"
#include "StandardMonster.hpp"
#include "BaseDoor.hpp"
#include "PotionsProperty.hpp"
#include "ScrollProperty.hpp"
#include "RolePopupUI.h"
#include "LevelResourceManager.h"
#include "PromptLayer.hpp"
#include "GameInfoLayer.hpp"
#include "GameToolbarLayer.hpp"
#include "RolePropLayer.hpp"
#include "MonsterPropLayer.hpp"
#include "NpcPropLayer.hpp"
#include "ShopPopupUI.h"
#include "StatisticsManager.hpp"
#include "BossPropLayer.hpp"
#include "AlertPopupUI.hpp"
#include "InformationPopupUI.h"
#include "AchieveProperty.hpp"
#include "HudPromptLayer.hpp"
#include "AchieveItemPopupUI.hpp"
#include "DeadPopupUI.h"
USING_NS_CC;
GameUILayer::GameUILayer()
{
    m_pGameInfoLayer            = nullptr;
    m_pGameToolBarLayer         = nullptr;
    m_pRolePropLayer            = nullptr;
    m_pMonsterPropLayer         = nullptr;
    m_pRoleHudLayer             = nullptr;
    m_pMonsterHudLayer          = nullptr;
}
GameUILayer::~GameUILayer()
{
    
}
bool GameUILayer::initUi()
{
    if (!registerTouchEvent())
        return false;
    
    m_pGameInfoLayer = GameInfoLayer::create();
    m_pGameInfoLayer->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_TOP);
    m_pGameInfoLayer->setPosition(cocos2d::Vec2(m_pRootLayer->getContentSize().width*0.5,m_pRootLayer->getContentSize().height));
    m_pRootLayer->addChild(m_pGameInfoLayer);
    
    m_pGameToolBarLayer = GameToolbarLayer::create();
    m_pGameToolBarLayer->setAnchorPoint(cocos2d::Vec2::ANCHOR_MIDDLE_BOTTOM);
    m_pGameToolBarLayer->setPosition(cocos2d::Vec2(m_pRootLayer->getContentSize().width*0.5,0));
    m_pRootLayer->addChild(m_pGameToolBarLayer);
    
    m_pRolePropLayer = RolePropLayer::create();
    m_pRolePropLayer->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_LEFT);
    m_pRolePropLayer->setPosition(cocos2d::Vec2::ZERO);
    m_pGameInfoLayer->addChild(m_pRolePropLayer);
    
    m_pMonsterPropLayer = MonsterPropLayer::create();
    m_pMonsterPropLayer->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_RIGHT);
    m_pMonsterPropLayer->setPosition(cocos2d::Vec2(m_pGameInfoLayer->getContentSize().width,0));
    m_pGameInfoLayer->addChild(m_pMonsterPropLayer);
    
    m_pNpcPropLayer = NpcPropLayer::create();
    m_pNpcPropLayer->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_RIGHT);
    m_pNpcPropLayer->setPosition(cocos2d::Vec2(m_pGameInfoLayer->getContentSize().width,0));
    m_pGameInfoLayer->addChild(m_pNpcPropLayer);
    
    
    m_pBossPropLayer = BossPropLayer::create();
    m_pBossPropLayer->setAnchorPoint(cocos2d::Vec2::ANCHOR_TOP_RIGHT);
    m_pBossPropLayer->setPosition(cocos2d::Vec2(m_pGameInfoLayer->getContentSize().width,0));
    m_pGameInfoLayer->addChild(m_pBossPropLayer);
    
    m_pRoleHudLayer = HudPromptLayer::create();
    m_pRootLayer->addChild(m_pRoleHudLayer);
    
    m_pMonsterHudLayer = HudPromptLayer::create();
    m_pRootLayer->addChild(m_pMonsterHudLayer);
    
    
    setCharacterPropLayerVisible(false,false,false);
    
    refreshUIView();
    return true;
}


bool GameUILayer::registerTouchEvent()
{
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto touchListener = EventListenerTouchOneByOne::create();
    if(touchListener == nullptr)
        return false;
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(GameUILayer::onTouchBegan, this);
    touchListener->onTouchMoved = CC_CALLBACK_2(GameUILayer::onTouchMoved, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(GameUILayer::onTouchEnded, this);
    dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    
    return true;
}
bool GameUILayer::onTouchBegan(Touch *touch, Event *event)
{

    return m_pGameToolBarLayer->isOpenDist();
}
void GameUILayer::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event)
{
    return;
}
void GameUILayer::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event)
{
    if(!touch)
        return;
    if(m_pGameToolBarLayer->isOpenDist())
    {
        std::string iconRes;
        std::string desc = VoxelExplorer::getInstance()->getScreenPickDesc(touch->getLocation(), iconRes);
        InformationPopupUI* infoUi = static_cast<InformationPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupInformation));
        if(infoUi)
        {
            m_pGameToolBarLayer->onClickDistTipsFrame();
            infoUi->setDarkLayerVisble(false);
            infoUi->setInfoIcon(iconRes);
            infoUi->setInfoDesc(desc);
        }
        CCLOG("Pick Desc : %s, Icon Res: %s", desc.c_str(), iconRes.c_str());
    }
    return;
}
void GameUILayer::onEventRoleMoneyNotEnough(cocos2d::EventCustom *sender)
{
    CCLOG("onEventRoleMoneyNotEnough");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_MONEY_NOT_ENOUGH);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
    
}
void GameUILayer::onEventRoleNoMana(cocos2d::EventCustom *sender)
{
    CCLOG("onEventRoleNoMana");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_NO_MANA);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventRoleBagNoSpace(cocos2d::EventCustom *sender)
{
    CCLOG("onEventRoleBagNoSpace");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_BAG_NO_SPACE);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
    
//    CallFunc* func = CallFunc::create([]{
//        PopupUILayerManager::getInstance()->openPopup(ePopupRole);
//    });
//    this->runAction(Sequence::createWithTwoActions(DelayTime::create(3.0f), func));

    
}
void GameUILayer::onEventRoleNoCopperKey(cocos2d::EventCustom *sender)
{
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_NO_COPPER_KEY);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
    CCLOG("onEventRoleNoCopperKey");
}
void GameUILayer::onEventRoleNoSilverKey(cocos2d::EventCustom *sender)
{
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_NO_SILVER_KEY);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
     CCLOG("onEventRoleNoSilverKey");
}
void GameUILayer::onEventRoleNoGoldKey(cocos2d::EventCustom *sender)
{
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_NO_GOLD_KEY);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
     CCLOG("onEventRoleNoGoldKey");
}
void GameUILayer::onEventRoleNoRoomKey(cocos2d::EventCustom *sender)
{
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_NO_ROOM_KEY);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
    CCLOG("onEventRoleNoRoomKey");
}
void GameUILayer::onEventRoleNoBossKey(cocos2d::EventCustom *sender)
{
    
    std::string msg = cocos2d::StringUtils::format(UtilityHelper::getLocalStringForUi(EVENT_PLAYER_NO_BOSS_KEY).c_str(),RandomDungeon::getInstance()->getCurrentBossName().c_str());
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
    CCLOG("onEventRoleNoBossKey");
}
//武器店
void GameUILayer::onEventNpcKnightAnsWer(cocos2d::EventCustom *sender)
{
    CCLOG("onEventNpcKnightAnsWer");
    ShopPopupUI* popupUILayer = static_cast<ShopPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupWeaponShop));
    if (popupUILayer) {
        popupUILayer->setShopTitle(EVENT_NPC_KNIGHT_ANSWER);
        popupUILayer->setDarkLayerVisble(false);
    }
}
//任务
void GameUILayer::onEventNpcChildAnsWer(cocos2d::EventCustom *sender)
{
    CCLOG("onEventNpcChildAnsWer");
}
//魔法物品店
void GameUILayer::onEventNpcShopGirlAnsWer(cocos2d::EventCustom *sender)
{
     CCLOG("onEventNpcChildAnsWer");
    ShopPopupUI* popupUILayer = static_cast<ShopPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupMagicShop));
    if (popupUILayer) {
        popupUILayer->setShopTitle(EVENT_NPC_SHOPGIRL_ANSWER);
        popupUILayer->setDarkLayerVisble(false);
    }
}
//炼金店
void GameUILayer::onEventNpcOldLadyAnsWer(cocos2d::EventCustom *sender)
{
    CCLOG("onEventNpcOldLadyAnsWer");
    ShopPopupUI* popupUILayer = static_cast<ShopPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupAlchemyShop));
    if (popupUILayer) {
        popupUILayer->setShopTitle(EVENT_NPC_OLDLADY_ANSWER);
        popupUILayer->setDarkLayerVisble(false);
    }
}
//赌博店
void GameUILayer::onEventNpcWeiRdoAnsWer(cocos2d::EventCustom *sender)
{
    CCLOG("onEventNpcWeiRdoAnsWer");
    ShopPopupUI* popupUILayer = static_cast<ShopPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupGambleShop));
    if (popupUILayer) {
        popupUILayer->setShopTitle(EVENT_NPC_WEIRDO_ANSWER);
        popupUILayer->setDarkLayerVisble(false);
    }
}
//智者
void GameUILayer::onEventNpcOldManAnsWer(cocos2d::EventCustom *sender)
{
    CCLOG("onEventNpcOldManAnsWer");
}
//魔女随机事件
void GameUILayer::onEventNpcLittleWitchAnsWer(cocos2d::EventCustom *sender)
{
    CCLOG("onEventNpcLittleWitchAnsWer");
}
//护士站
void GameUILayer::onEventNpcNurseAnsWer(cocos2d::EventCustom *sender)
{
    CCLOG("onEventNpcNurseAnsWer");
}

void GameUILayer::onEventUseGoldChestKey(cocos2d::EventCustom *sender)
{
    CCLOG("onEventUseGoldChestKey");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_USE_GOLD_CHEST_KEY);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_POSITIVE, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventUseSilverChestKey(cocos2d::EventCustom *sender)
{
    CCLOG("onEventUseSilverChestKey");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_USE_SILVER_CHEST_KEY);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_POSITIVE, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventUseCopperChestKey(cocos2d::EventCustom *sender)
{
    CCLOG("onEventUseCopperChestKey");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_USE_COPPER_CHEST_KEY);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_POSITIVE, msg);
    m_pGameToolBarLayer->sendMessage(msg);

}
void GameUILayer::onEventUseRoomKey(cocos2d::EventCustom *sender)
{
    CCLOG("onEventUseRoomKey");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_PLAYER_USE_ROOM_KEY);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_POSITIVE, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventUseBossKey(cocos2d::EventCustom *sender)
{
    CCLOG("onEventUseBossKey");
   
    std::string msg = cocos2d::StringUtils::format(UtilityHelper::getLocalStringForUi(EVENT_PLAYER_USE_BOSS_KEY).c_str(),RandomDungeon::getInstance()->getCurrentBossName().c_str());
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}

void GameUILayer::onEventDoorMagicLocked(cocos2d::EventCustom *sender)
{
    CCLOG("onEventDoorMagicLocked");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_DOOR_MAGIC_LOCKED);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventDoorMagicClosed(cocos2d::EventCustom *sender)
{
    CCLOG("onEventDoorMagicClosed");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_DOOR_MAGIC_CLOSED);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderDoor(cocos2d::EventCustom *sender) //发现隐藏门
{
    CCLOG("onEventFoundHidderDoor");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_DOOR);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderTrapToxic(cocos2d::EventCustom *sender)//发现隐藏中毒机关
{
    CCLOG("onEventFoundHidderTrapToxic");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_TOXIC_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderTrapFire(cocos2d::EventCustom *sender)//发现隐藏火机关
{
    CCLOG("onEventFoundHidderTrapFire");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_FIRE_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderTrapParalyic(cocos2d::EventCustom *sender)//发现隐藏麻痹机关
{
    CCLOG("onEventFoundHidderTrapParalyic");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_PARALYTIC_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderTrapGripping(cocos2d::EventCustom *sender)//发现隐藏夹子机关
{
     CCLOG("onEventFoundHidderTrapGripping");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_GRIPPING_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderTrapSummoning(cocos2d::EventCustom *sender)//发现隐藏召唤机关
{
     CCLOG("onEventFoundHidderTrapSummoning");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_SUMMONING_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderTrapWeak(cocos2d::EventCustom *sender)//发现隐藏虚弱机关
{
    CCLOG("onEventFoundHidderTrapWeak");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_WEAK_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderMsg(cocos2d::EventCustom *sender)
{
    CCLOG("onEventFoundHidderMsg");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_MSG);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
void GameUILayer::onEventFoundHidderItem(cocos2d::EventCustom *sender)
{
    CCLOG("onEventFoundHidderItem");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FOUND_HIDDEN_ITEM);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg);
}
//上楼提示
void GameUILayer::onEventGoUpStairs(cocos2d::EventCustom *sender)
{
     CCLOG("onEventGoUpStairs");
     std::string msg = UtilityHelper::getLocalStringForUi(EVENT_GO_UPSTAIRS);
     PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
}
//禁止上楼提示
void GameUILayer::onEventGoUpStairsForbidden(cocos2d::EventCustom *sender)
{
     CCLOG("onEventGoUpStairsForbidden");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_GO_UPSTAIRS_FORBIDDEN);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
}
//下楼提示
void GameUILayer::onEventGoDownStairs(cocos2d::EventCustom *sender)
{
    CCLOG("onEventGoDownStairs");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_GO_DOWNSTAIRS);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
}
void GameUILayer::onEventGoBossRoom(cocos2d::EventCustom *sender)
{
    CCLOG("onEventGoBossRoom");
    AlertPopupUI* alertPopupUI = static_cast<AlertPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupAlert));
    if (alertPopupUI) {
        alertPopupUI->setMessage(UtilityHelper::getLocalStringForUi(EVENT_GO_BOSSROOM));
        alertPopupUI->setPositiveListerner([](Ref* ref)
                                           {
                                               CCLOG("onEventGoBossRoom");
                                               VoxelExplorer::getInstance()->handleDownstairs();
                                           });
        alertPopupUI->setNegativeListerner([](Ref* ref){});
    }
    
}
void GameUILayer::onEventGoChasm(cocos2d::EventCustom *sender)
{
    CCLOG("onEventGoChasm");
    AlertPopupUI* alertPopupUI = static_cast<AlertPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupAlert));
    if (alertPopupUI) {
        alertPopupUI->setMessage(UtilityHelper::getLocalStringForUi(EVENT_GO_CHASM));
        alertPopupUI->setPositiveListerner([](Ref* ref)
        {
            CCLOG("handleGoChasm");
            VoxelExplorer::getInstance()->handleGoChasm();
        });
        alertPopupUI->setNegativeListerner([](Ref* ref){});
    }
}
//跳崖传送
void GameUILayer::onEventFallDownStairs(cocos2d::EventCustom *sender)
{
    CCLOG("onEventFallDownStairs");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_FALL_DOWNSTAIRS);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);

}
void GameUILayer::onEventSelectLeftDungeonNode(cocos2d::EventCustom *sender)
{
    CCLOG("onEventSelectLeftDungeonNode");
    AlertPopupUI* popupUi = static_cast<AlertPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupAlert));
    if (popupUi) {
        popupUi->setMessage(UtilityHelper::getLocalStringForUi(EVENT_SELECT_LEFT_DUNGEON_NODE));
        popupUi->setPositiveListerner([](Ref* ref)
                                      {
                                          RandomDungeon::getInstance()->selectDungeonNode(true);
                                      });
        popupUi->setNegativeListerner([](Ref* ref){});

    }
}
void GameUILayer::onEventSelectRightDungeonNode(cocos2d::EventCustom *sender)
{
    CCLOG("onEventSelectRightDungeonNode");
    AlertPopupUI* popupUi = static_cast<AlertPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupAlert));
    if (popupUi) {
        popupUi->setMessage(UtilityHelper::getLocalStringForUi(EVENT_SELECT_RIGHT_DUNGEON_NODE));
        popupUi->setPositiveListerner([](Ref* ref)
                                      {
                                          RandomDungeon::getInstance()->selectDungeonNode(false);
                                      });
        popupUi->setNegativeListerner([](Ref* ref){});
        
    }
}
//小传送们
void GameUILayer::onEventUseSmallProtal(cocos2d::EventCustom *sender)
{
    CCLOG("onEventUseSmallProtal");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_USE_SMALL_PROTAL);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);

}
void GameUILayer::onEventStandardProtalNoEnergy(cocos2d::EventCustom *sender)
{
    CCLOG("onEventStandardProtalNoEnergy");
     AlertPopupUI* popupUi = static_cast<AlertPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupAlert));
    if (popupUi) {
        popupUi->setMessage(UtilityHelper::getLocalStringForUi(EVENT_STANDARD_PROTAL_NO_ENERGY));
        popupUi->setPositiveListerner([](Ref* ref){});
    }

}
void GameUILayer::onEventTriggerToxic(cocos2d::EventCustom *sender) //中毒机关
{
    CCLOG("onEventTriggerToxic");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_TRIGGER_TOXIC_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg,PopupUILayerManager::getInstance()->getTipsColor(TipTypes::TIP_NEGATIVE));
    
}
void GameUILayer::onEventTriggerFire(cocos2d::EventCustom *sender) //火机关
{
    CCLOG("onEventTriggerFire");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_TRIGGER_FIRE_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
     m_pGameToolBarLayer->sendMessage(msg,PopupUILayerManager::getInstance()->getTipsColor(TipTypes::TIP_NEGATIVE));
    
}
void GameUILayer::onEventTriggerParalyic(cocos2d::EventCustom *sender)//麻痹机关
{
    CCLOG("onEventTriggerParalyic");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_TRIGGER_PARALYTIC_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
     m_pGameToolBarLayer->sendMessage(msg,PopupUILayerManager::getInstance()->getTipsColor(TipTypes::TIP_NEGATIVE));
    
}
void GameUILayer::onEventTriggerGripping(cocos2d::EventCustom *sender)//夹子机关
{
    CCLOG("onEventTriggerGripping");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_TRIGGER_GRIPPING_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
     m_pGameToolBarLayer->sendMessage(msg,PopupUILayerManager::getInstance()->getTipsColor(TipTypes::TIP_NEGATIVE));
    
}
void GameUILayer::onEventTriggerSummoning(cocos2d::EventCustom *sender)//召唤机关
{
    CCLOG("onEventTriggerSummoning");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_TRIGGER_SUMMONING_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
     m_pGameToolBarLayer->sendMessage(msg,PopupUILayerManager::getInstance()->getTipsColor(TipTypes::TIP_NEGATIVE));
    
}
void GameUILayer::onEventTriggerWeak(cocos2d::EventCustom *sender) //虚弱机关
{
    CCLOG("onEventTriggerWeak");
    std::string msg = UtilityHelper::getLocalStringForUi(EVENT_TRIGGER_WEAK_TRAP);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
     m_pGameToolBarLayer->sendMessage(msg,PopupUILayerManager::getInstance()->getTipsColor(TipTypes::TIP_NEGATIVE));
   
}
void GameUILayer::onEventTriggerFrozen(cocos2d::EventCustom *sender) //冰冻，不一定是机关
{
    CCLOG("onEventTriggerFrozen");
    std::string msg = UtilityHelper::getLocalStringForUi("TRIGGER_MESSAGE_FROZEN_TRAP");
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    m_pGameToolBarLayer->sendMessage(msg,PopupUILayerManager::getInstance()->getTipsColor(TipTypes::TIP_NEGATIVE));
    
}
void GameUILayer::onEventRoleUserPotion(cocos2d::EventCustom *sender)
{
    CCLOG("onEvenetUserPotion");
    //关闭ItemPopup窗口
    PopupUILayerManager::getInstance()->closeCurrentPopup();
    
    //窗口都未关闭,回调后来判断是否关闭
    Vec2 pt = VoxelExplorer::getInstance()->getMainCamera()->projectGL(VoxelExplorer::getInstance()->getPlayer()->getPosition3D());
    pt = Vec2(pt.x, pt.y+TerrainTile::CONTENT_SCALE*2.5);
    PotionsProperty* potionsProperty = static_cast<PotionsProperty*>(sender->getUserData());
    PickableItem::PickableItemType itemType = potionsProperty->getPickableItemType();
    std::string tipStr;
    switch (itemType) {
        case PickableItem::PIT_POTION_MINORHEALTH:
        case PickableItem::PIT_POTION_LESSERHEALTH:
        case PickableItem::PIT_POTION_HEALTH:
            CCLOG("使用治疗药水 恢复HP");
            tipStr = StringUtils::format(UtilityHelper::getLocalStringForUi("USE_POTION_TREAT").c_str(),int(potionsProperty->getValue()));
            m_pGameToolBarLayer->sendMessage(tipStr,PopupUILayerManager::getInstance()->getTipsColor(TIP_POSITIVE));
            break;
        case PickableItem::PIT_POTION_MINORMANA:
        case PickableItem::PIT_POTION_LESSERMANA:
        case PickableItem::PIT_POTION_MANA:
            CCLOG("使用魔法药水 恢复MP");
             tipStr =  StringUtils::format(UtilityHelper::getLocalStringForUi("USE_POTION_MAGIC").c_str(),int(potionsProperty->getValue()));
             m_pGameToolBarLayer->sendMessage(tipStr,PopupUILayerManager::getInstance()->getTipsColor(TIP_POSITIVE));
            break;
        case PickableItem::PIT_POTION_MINORRECOVERY:
        case PickableItem::PIT_POTION_LESSERRECOVERY:
        case PickableItem::PIT_POTION_RECOVERY:
        {
            CCLOG("恢复药水 恢复HP+MP");
            int hp = PlayerProperty::getInstance()->getMaxHP() * potionsProperty->getValue().GetFloatValue();
            int mp = PlayerProperty::getInstance()->getMaxMP() * potionsProperty->getValue().GetFloatValue();
            
             tipStr =  StringUtils::format(UtilityHelper::getLocalStringForUi("USE_POTION_RECOVERY").c_str(),hp,mp);
            m_pGameToolBarLayer->sendMessage(tipStr,PopupUILayerManager::getInstance()->getTipsColor(TIP_POSITIVE));
                   }
            break;
        case PickableItem::PIT_POTION_DETOXIFICATION:
            tipStr =  UtilityHelper::getLocalStringForUi("USE_POTION_RELIEVE");
            m_pGameToolBarLayer->sendMessage(tipStr,PopupUILayerManager::getInstance()->getTipsColor(TIP_POSITIVE));
            CCLOG("解除中毒");
            break;
        case PickableItem::PIT_POTION_SPECIFIC:
            tipStr = UtilityHelper::getLocalStringForUi("USE_POTION_SPECIFIC");
            m_pGameToolBarLayer->sendMessage(tipStr,PopupUILayerManager::getInstance()->getTipsColor(TIP_POSITIVE));
            CCLOG("解除冰冻、麻痹、火");
            break;
        case PickableItem::PIT_POTION_HEALING:
            tipStr = UtilityHelper::getLocalStringForUi("USE_POTION_WEAK");
            m_pGameToolBarLayer->sendMessage(tipStr,PopupUILayerManager::getInstance()->getTipsColor(TIP_POSITIVE));
            CCLOG("解除虚弱");
            break;
        case PickableItem::PIT_POTION_UNIVERSAL:
            tipStr = UtilityHelper::getLocalStringForUi("USE_POTION_UNIVERSAL");
            m_pGameToolBarLayer->sendMessage(tipStr,PopupUILayerManager::getInstance()->getTipsColor(TIP_POSITIVE));
            CCLOG("万能药水,解除中毒，冰冻，麻痹，虚弱，着火");
            break;
        default:
            break;
    }
    if (!tipStr.empty())
    {
        PopupUILayerManager::getInstance()->showStatusImport(TIP_POSITIVE, tipStr);
    }
    
    //关闭角色对话框
    PopupUILayerManager::getInstance()->closeCurrentPopup();

}
void GameUILayer::onEventRoleUserScroll(cocos2d::EventCustom *sender)
{
    //关闭ItemPopup窗口
    PopupUILayerManager::getInstance()->closeCurrentPopup();
    //窗口都未关闭,回调后来判断是否关闭
    CCLOG("onEvenetUserScroll");
    ScrollProperty* scrollProperty = static_cast<ScrollProperty*>(sender->getUserData());
    std::string tipStr;
    if (scrollProperty->getPickableItemType() == PickableItem::PIT_SCROLL_INDENTIFY)
    {
        CCLOG("鉴定卷轴");
        PopupUILayer* popup = nullptr;
        RolePopupUI* rolePopup = nullptr;
        if(PopupUILayerManager::getInstance()->isOpenPopup(ePopupRole, popup)){
            rolePopup = static_cast<RolePopupUI*>(popup);
        }else{
            rolePopup = static_cast<RolePopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupRole));
        }
        if (rolePopup) {
            rolePopup->updateItems(true);
        }
        return;
    }else if(scrollProperty->getPickableItemType() == PickableItem::PIT_SCROLL_TELEPORT)
    {
        CCLOG("传送卷轴");
        tipStr = UtilityHelper::getLocalStringForUi("USE_SCROLL_TELEPORT");
    }else if(scrollProperty->getPickableItemType() == PickableItem::PIT_SCROLL_SPEED)
    {
        CCLOG("速度卷轴");
        tipStr = UtilityHelper::getLocalStringForUi("USE_SCROLL_SPEED");
    }else if(scrollProperty->getPickableItemType() == PickableItem::PIT_SCROLL_STEALTH)
    {
        CCLOG("隐身卷轴");
        tipStr = UtilityHelper::getLocalStringForUi("USE_SCROLL_STEALTH");
    }else if(scrollProperty->getPickableItemType() == PickableItem::PIT_SCROLL_DESTINY)
    {
        CCLOG("命运卷轴");
        tipStr = UtilityHelper::getLocalStringForUi("USE_SCROLL_DESTINY");
    }
    
    if (!tipStr.empty())
    {
        PopupUILayerManager::getInstance()->showStatusImport(TIP_POSITIVE, tipStr);
    }

    //关闭角色对话框
    PopupUILayerManager::getInstance()->closeCurrentPopup();
}
void GameUILayer::refreshUIView()
{
    if (m_pGameInfoLayer) {
        m_pGameInfoLayer->refreshUIView();
    }
    
    if (m_pGameToolBarLayer) {
        m_pGameToolBarLayer->refreshUIView();
    }
    
    if (m_pRolePropLayer) {
        m_pRolePropLayer->refreshUIView();
    }
    
    if (m_pMonsterPropLayer) {
        m_pMonsterPropLayer->refreshUIView();
    }
    
    if (m_pNpcPropLayer) {
        m_pNpcPropLayer->refreshUIView();
    }
    if (m_pBossPropLayer) {
        m_pBossPropLayer->refreshUIView();
    }
}
void GameUILayer::onEventRoleLevelUp(cocos2d::EventCustom *sender)
{
    CCLOG("onEventRoleLevelUp");
    refreshUIView();
    PopupUILayerManager::getInstance()->showStatusImport(TIP_DEFAULT, UtilityHelper::getLocalStringForUi("GAME_MESSAGE_LEVEL_UP"));
    m_pGameToolBarLayer->sendMessage(UtilityHelper::getLocalStringForUi("GAME_MESSAGE_LEVEL_UP"),PopupUILayerManager::getInstance()->getTipsColor(TIP_NEUTRAL));
}

void GameUILayer::onEventRoleUpdateProp(cocos2d::EventCustom *sender)
{
    CCLOG("onEventUpdateProp");
    refreshUIView();

}

void GameUILayer::onEventRoleDead(cocos2d::EventCustom *sender)
{
    CCLOG("onEventRoleDead");
    refreshUIView();
    CallFunc* func = CallFunc::create([]{
        DeadPopupUI* pausePopup = static_cast<DeadPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupDead));
        if (pausePopup) {
//            pausePopup->setDarkLayerVisble(false);
        }

    });
    this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0F),func ));
}
void GameUILayer::onEventRoleFallAndDie(cocos2d::EventCustom *sender)
{
    CCLOG("onEventRoleFallAndDie");
}

void GameUILayer::onEventRoleHud(cocos2d::EventCustom *sender)
{
       CCLOG("onEvenetRoleHud");
    HurtData* hurData = static_cast<HurtData*>(sender->getUserData());
    updateCharacterHud(hurData,TIP_DODGE,TIP_BOLOCK,TIP_ROLE_CRITICAL_STRIKE,TIP_NEGATIVE);
}
void GameUILayer::onEventMonsterDead(cocos2d::EventCustom *sender)
{
    CCLOG("onEvenetMonsterDead");
    BaseMonster* monster = static_cast<BaseMonster*>(sender->getUserData());
    if (!monster)
        return;
    Vec2 pt = VoxelExplorer::getInstance()->getMainCamera()->projectGL(monster->getPosition3D());
    pt = Vec2(pt.x, pt.y+TerrainTile::CONTENT_SCALE*2.5);
    if (monster->getState() == BaseMonster::MonsterState::MS_DEATH) {
        int roleLevel = PlayerProperty::getInstance()->getLevel();
        int monsterLevel = monster->getMonsterProperty()->getLevel();
        int exp = GameFormula::getKillNormalMonsterExp(roleLevel, monsterLevel);
        if (monster->getMonsterProperty()->isElite()) {
            exp = GameFormula::getKillEliteMonsterExp(roleLevel, monsterLevel);
        }
        
        if (m_pMonsterHudLayer) {
            m_pMonsterHudLayer->shwoPrompt(pt, TIP_POSITIVE, StringUtils::format(UtilityHelper::getLocalStringForUi("STATUS_TEXT_EXP").c_str(),exp));
        }
        
        if (m_pMonsterPropLayer) {
            m_pMonsterPropLayer->setMonster(monster);
            m_pMonsterPropLayer->setVisible(false);
        }

    }
}

void GameUILayer::onEventMonsterHud(cocos2d::EventCustom *sender)
{
        CCLOG("onEventMonsterHud");
    HurtData* hurData = static_cast<HurtData*>(sender->getUserData());
    updateCharacterHud(hurData,TIP_DODGE,TIP_BOLOCK,TIP_MONSTER_CRITICAL_STRIKE,TIP_NEUTRAL,false);

}
void GameUILayer::onEventMonsterUpdateProp(cocos2d::EventCustom *sender)
{
    CCLOG("onEventUpdateMonsterProp");
    BaseMonster* monster = static_cast<BaseMonster*>(sender->getUserData());
    if (m_pMonsterPropLayer)
    {
        setCharacterPropLayerVisible(true, false, false);
        m_pMonsterPropLayer->setMonster(monster);
    }
    
    refreshUIView();
}
void GameUILayer::onEventMonsterAlert(cocos2d::EventCustom *sender)
{
    CCLOG("onEventMonsterAlert");
    BaseMonster* monster = static_cast<BaseMonster*>(sender->getUserData());
    PromptLayer* promptLayer = PromptLayer::create(PromptLayer::PT_AWAKE);
    if (monster) {
       promptLayer->setActor(monster);
    }
    m_pRootLayer->addChild(promptLayer);

}
void GameUILayer::onEventMonsterConfusing(cocos2d::EventCustom *sender)
{
    CCLOG("onEventMonsterConfusing");
    BaseMonster* monster = static_cast<BaseMonster*>(sender->getUserData());
    PromptLayer* promptLayer = PromptLayer::create(PromptLayer::PT_CONFUSING);
    if (monster) {
        promptLayer->setActor(monster);
    }
    m_pRootLayer->addChild(promptLayer);
    if (m_pMonsterPropLayer) {
         m_pMonsterPropLayer->setVisible(false);
    }
   
}
void GameUILayer::onEventBossUpdateProp(cocos2d::EventCustom *sender)
{
    CCLOG("onEventBossUpdateProp");
    BaseBoss* monster = static_cast<BaseBoss*>(sender->getUserData());
    if (m_pBossPropLayer)
    {
        setCharacterPropLayerVisible(false, false, true);
        m_pBossPropLayer->setBoss(monster);
    }
    refreshUIView();

}
void GameUILayer::onEventBossDeath(cocos2d::EventCustom *sender)
{
     CCLOG("onEventBossDeath");
    BaseBoss* monster = static_cast<BaseBoss*>(sender->getUserData());
    if (!monster)
        return;
    Vec2 pt = VoxelExplorer::getInstance()->getMainCamera()->projectGL(monster->getPosition3D());
    pt = Vec2(pt.x, pt.y+TerrainTile::CONTENT_SCALE*2.5);
    

    if (monster->getState() == BaseBoss::BossState::BS_DEATH) {
        int roleLevel = PlayerProperty::getInstance()->getLevel();
        int exp  = GameFormula::getKillBossExp(roleLevel, roleLevel+5);
        
        if (m_pMonsterHudLayer) {
            m_pMonsterHudLayer->shwoPrompt(pt, TIP_POSITIVE, StringUtils::format(UtilityHelper::getLocalStringForUi("STATUS_TEXT_EXP").c_str(),exp));
        }
//        
//        PopupUILayerManager::getInstance()->showStatus(TIP_POSITIVE, StringUtils::format(UtilityHelper::getLocalStringForUi("STATUS_TEXT_EXP").c_str(),exp),pt);
        if (m_pBossPropLayer) {
            m_pBossPropLayer->setBoss(monster);
            m_pBossPropLayer->setVisible(false);
        }
    }
    
    CallFunc* func = CallFunc::create([monster]{
        std::string msg = monster->getBossDescByEvent(EVENT_BOSS_DEATH);
        PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
    });
    this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0f), func));

}
void GameUILayer::onEventBossHud(cocos2d::EventCustom *sender)
{
    CCLOG("onEventBossHud");
    HurtData* hurData = static_cast<HurtData*>(sender->getUserData());
    
    updateCharacterHud(hurData,TIP_DODGE,TIP_BOLOCK,TIP_MONSTER_CRITICAL_STRIKE,TIP_NEUTRAL,false);
}
void GameUILayer::onEventBossAlert(cocos2d::EventCustom *sender)
{
    CCLOG("onEventBossAlert");
    BaseBoss* monster = static_cast<BaseBoss*>(sender->getUserData());
    if (!monster)
        return;
    PromptLayer* promptLayer = PromptLayer::create(PromptLayer::PT_AWAKE);
    if (monster) {
        promptLayer->setActor(monster);
    }
    m_pRootLayer->addChild(promptLayer);
    
    CallFunc* func = CallFunc::create([monster]{
        std::string msg =monster->getBossDescByEvent(EVENT_BOSS_ALERT);
        PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);

    });
    this->runAction(Sequence::createWithTwoActions(DelayTime::create(2.0f), func));

}
void GameUILayer::onEventBossSkill1(cocos2d::EventCustom *sender)
{
    CCLOG("onEventBossSkill1");
    BaseBoss* monster = static_cast<BaseBoss*>(sender->getUserData());
    if (!monster)
        return;
    std::string msg = monster->getBossDescByEvent(EVENT_BOSS_SKILL1);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
}
void GameUILayer::onEventBossSkill2(cocos2d::EventCustom *sender)
{
    CCLOG("onEventBossSkill2");
    BaseBoss* monster = static_cast<BaseBoss*>(sender->getUserData());
    if (!monster)
        return;
    std::string msg = monster->getBossDescByEvent(EVENT_BOSS_SKILL2);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
}
void GameUILayer::onEventBossSkill3(cocos2d::EventCustom *sender)
{
    CCLOG("onEventBossSkill3");
    BaseBoss* monster = static_cast<BaseBoss*>(sender->getUserData());
    if (!monster)
        return;
    std::string msg =monster->getBossDescByEvent(EVENT_BOSS_SKILL3);
    PopupUILayerManager::getInstance()->showStatusImport(TIP_WARNING, msg);
}

void GameUILayer::onEvenetAchieveComplete(cocos2d::EventCustom *sender)
{
     CCLOG("onEvenetAchieveComplete");
    AchieveProperty* achieveProp = static_cast<AchieveProperty*>(sender->getUserData());
    if (achieveProp) {
        CCLOG("active:%s",achieveProp->getAchieveName().c_str());
        std::string icon = achieveProp->getAchieveIcon();
        std::string name = achieveProp->getAchieveName();
        std::string targetDesc = achieveProp->getTargetDesc();
        AchieveItemPopupUI* achieveItermPopup = static_cast<AchieveItemPopupUI*>(PopupUILayerManager::getInstance()->openPopup(ePopupAchieveItem));
        if (achieveItermPopup) {
            achieveItermPopup->setDarkLayerVisble(false);
            achieveItermPopup->setAchieveDatas(icon, name, targetDesc);
        }
    }
}

void GameUILayer::setCharacterPropLayerVisible(bool isMonster, bool isNpc, bool isBoss)
{
    if (m_pMonsterPropLayer) {
        m_pMonsterPropLayer->setVisible(isMonster);
    }
    
    if (m_pNpcPropLayer) {
        m_pNpcPropLayer->setVisible(isNpc);
    }
    if (m_pBossPropLayer) {
        m_pBossPropLayer->setVisible(isBoss);
    }
}
void GameUILayer::updateCharacterHud(HurtData* hurData,TipTypes tipDodge,TipTypes tipBolock,TipTypes tipCriticalStike,TipTypes tipNormal,bool isWho)
{
    Vec2 pt = VoxelExplorer::getInstance()->getMainCamera()->projectGL(hurData->m_vPos);
    pt = Vec2(pt.x, pt.y+TerrainTile::CONTENT_SCALE*2.5);
    std::string str;
    TipTypes type;
    if (hurData->m_bDodge) {
        str = StringUtils::format(UtilityHelper::getLocalStringForUi("STATUS_TEXT_DODGE").c_str(),hurData->m_nDamage);
        type = tipDodge;
        CCLOG("monster 闪避");
    }else {
        if((hurData->m_bBlocked && hurData->m_bCriticalStrike) || hurData->m_bBlocked)
        {
            str = StringUtils::format(UtilityHelper::getLocalStringForUi("STATUS_TEXT_BOLOCK").c_str(),hurData->m_nDamage);
            type = tipBolock;
            
            CCLOG("monster 格挡");
        }else if (hurData->m_bCriticalStrike && !hurData->m_bBlocked)
        {
            str = StringUtils::format(UtilityHelper::getLocalStringForUi("STATUS_TEXT_CRITICAL_STRIKE").c_str(),hurData->m_nDamage);
            type = tipCriticalStike;
            
            CCLOG("monster 暴击");
        }else
        {
            str =  Value(hurData->m_nDamage).asString();
            type = tipNormal;
        }
    }
    if (isWho && m_pRoleHudLayer)
    {
         m_pRoleHudLayer->shwoPrompt(pt, type, str);
    }
    if (!isWho && m_pMonsterHudLayer)
    {
       m_pMonsterHudLayer->shwoPrompt(pt, type, str);
    }

}
void GameUILayer::onEnter()
{
    WrapperUILayer::onEnter();
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_PROPERTY_DIRTY, CC_CALLBACK_1(GameUILayer::onEventRoleUpdateProp,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_DEATH, CC_CALLBACK_1(GameUILayer::onEventRoleDead,this));
   Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_FALL_AND_DIE, CC_CALLBACK_1(GameUILayer::onEventRoleFallAndDie,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_LEVEL_UP, CC_CALLBACK_1(GameUILayer::onEventRoleLevelUp,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_HURT, CC_CALLBACK_1(GameUILayer::onEventRoleHud,this));
    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_MONEY_NOT_ENOUGH, CC_CALLBACK_1(GameUILayer::onEventRoleMoneyNotEnough,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_NO_MANA, CC_CALLBACK_1(GameUILayer::onEventRoleNoMana,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_BAG_NO_SPACE, CC_CALLBACK_1(GameUILayer::onEventRoleBagNoSpace,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_NO_COPPER_KEY, CC_CALLBACK_1(GameUILayer::onEventRoleNoCopperKey,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_NO_SILVER_KEY, CC_CALLBACK_1(GameUILayer::onEventRoleNoSilverKey,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_NO_GOLD_KEY, CC_CALLBACK_1(GameUILayer::onEventRoleNoGoldKey,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_NO_ROOM_KEY, CC_CALLBACK_1(GameUILayer::onEventRoleNoRoomKey,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_NO_BOSS_KEY, CC_CALLBACK_1(GameUILayer::onEventRoleNoBossKey,this));
    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_USE_POTION, CC_CALLBACK_1(GameUILayer::onEventRoleUserPotion,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_USE_SCROLL, CC_CALLBACK_1(GameUILayer::onEventRoleUserScroll,this));
    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_MONSTER_HURT, CC_CALLBACK_1(GameUILayer::onEventMonsterHud,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_MONSTER_PROPERTY_DIRTY, CC_CALLBACK_1(GameUILayer::onEventMonsterUpdateProp,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_MONSTER_DEATH, CC_CALLBACK_1(::GameUILayer::onEventMonsterDead,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_MONSTER_ALERT, CC_CALLBACK_1(::GameUILayer::onEventMonsterAlert,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_MONSTER_CONFUSING, CC_CALLBACK_1(::GameUILayer::onEventMonsterConfusing,this));
    

    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_TRIGGER_TOXIC_TRAP, CC_CALLBACK_1(GameUILayer::onEventTriggerToxic,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_TRIGGER_FIRE_TRAP, CC_CALLBACK_1(GameUILayer::onEventTriggerFire,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_TRIGGER_PARALYTIC_TRAP, CC_CALLBACK_1(GameUILayer::onEventTriggerParalyic,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_TRIGGER_GRIPPING_TRAP, CC_CALLBACK_1(GameUILayer::onEventTriggerGripping,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_TRIGGER_SUMMONING_TRAP, CC_CALLBACK_1(GameUILayer::onEventTriggerSummoning,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_TRIGGER_WEAK_TRAP, CC_CALLBACK_1(GameUILayer::onEventTriggerWeak,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_TRIGGER_FROZEN_TRAP, CC_CALLBACK_1(GameUILayer::onEventTriggerFrozen,this)) ;
    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_USE_GOLD_CHEST_KEY, CC_CALLBACK_1(GameUILayer::onEventUseGoldChestKey,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_USE_SILVER_CHEST_KEY, CC_CALLBACK_1(GameUILayer::onEventUseSilverChestKey,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_USE_COPPER_CHEST_KEY, CC_CALLBACK_1(GameUILayer::onEventUseCopperChestKey,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_USE_ROOM_KEY, CC_CALLBACK_1(GameUILayer::onEventUseRoomKey,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_PLAYER_USE_BOSS_KEY, CC_CALLBACK_1(GameUILayer::onEventUseBossKey,this));
    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_DOOR_MAGIC_LOCKED, CC_CALLBACK_1(GameUILayer::onEventDoorMagicLocked,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_DOOR_MAGIC_CLOSED, CC_CALLBACK_1(GameUILayer::onEventDoorMagicClosed,this));
    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_DOOR, CC_CALLBACK_1(GameUILayer::onEventFoundHidderDoor,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_TOXIC_TRAP, CC_CALLBACK_1(GameUILayer::onEventFoundHidderTrapToxic,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_FIRE_TRAP, CC_CALLBACK_1(GameUILayer::onEventFoundHidderTrapFire,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_PARALYTIC_TRAP, CC_CALLBACK_1(GameUILayer::onEventFoundHidderTrapParalyic,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_GRIPPING_TRAP, CC_CALLBACK_1(GameUILayer::onEventFoundHidderTrapGripping,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_SUMMONING_TRAP, CC_CALLBACK_1(GameUILayer::onEventFoundHidderTrapSummoning,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_WEAK_TRAP, CC_CALLBACK_1(GameUILayer::onEventFoundHidderTrapWeak,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_MSG, CC_CALLBACK_1(GameUILayer::onEventFoundHidderMsg,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FOUND_HIDDEN_ITEM, CC_CALLBACK_1(GameUILayer::onEventFoundHidderItem,this)) ;
    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_GO_UPSTAIRS, CC_CALLBACK_1(GameUILayer::onEventGoUpStairs,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_GO_UPSTAIRS_FORBIDDEN, CC_CALLBACK_1(GameUILayer::onEventGoUpStairsForbidden,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_GO_DOWNSTAIRS, CC_CALLBACK_1(GameUILayer::onEventGoDownStairs,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_GO_BOSSROOM, CC_CALLBACK_1(GameUILayer::onEventGoBossRoom,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_GO_CHASM, CC_CALLBACK_1(GameUILayer::onEventGoChasm,this));

    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_NPC_KNIGHT_ANSWER, CC_CALLBACK_1(GameUILayer::onEventNpcKnightAnsWer,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_NPC_CHILD_ANSWER, CC_CALLBACK_1(GameUILayer::onEventNpcChildAnsWer,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_NPC_SHOPGIRL_ANSWER, CC_CALLBACK_1(GameUILayer::onEventNpcShopGirlAnsWer,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_NPC_OLDLADY_ANSWER, CC_CALLBACK_1(GameUILayer::onEventNpcOldLadyAnsWer,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_NPC_WEIRDO_ANSWER, CC_CALLBACK_1(GameUILayer::onEventNpcWeiRdoAnsWer,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_NPC_OLDMAN_ANSWER, CC_CALLBACK_1(GameUILayer::onEventNpcOldManAnsWer,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_NPC_LITTLEWITCH_ANSWER, CC_CALLBACK_1(GameUILayer::onEventNpcLittleWitchAnsWer,this)) ;
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_NPC_NURSE_ANSWER, CC_CALLBACK_1(GameUILayer::onEventNpcNurseAnsWer,this));

    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_BOSS_PROPERTY_DIRTY, CC_CALLBACK_1(GameUILayer::onEventBossUpdateProp,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_BOSS_DEATH, CC_CALLBACK_1(GameUILayer::onEventBossDeath,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_BOSS_HURT, CC_CALLBACK_1(GameUILayer::onEventBossHud,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_BOSS_ALERT, CC_CALLBACK_1(GameUILayer::onEventBossAlert,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_BOSS_SKILL1, CC_CALLBACK_1(GameUILayer::onEventBossSkill1,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_BOSS_SKILL2, CC_CALLBACK_1(GameUILayer::onEventBossSkill2,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_BOSS_SKILL3, CC_CALLBACK_1(GameUILayer::onEventBossSkill3,this));
 
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_ACHIEVE_COMPLETE, CC_CALLBACK_1(GameUILayer::onEvenetAchieveComplete,this));
    
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_FALL_DOWNSTAIRS, CC_CALLBACK_1(GameUILayer::onEventFallDownStairs,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_SELECT_LEFT_DUNGEON_NODE, CC_CALLBACK_1(GameUILayer::onEventSelectLeftDungeonNode,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_SELECT_RIGHT_DUNGEON_NODE, CC_CALLBACK_1(GameUILayer::onEventSelectRightDungeonNode,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_USE_SMALL_PROTAL, CC_CALLBACK_1(GameUILayer::onEventUseSmallProtal,this));
    Director::getInstance()->getEventDispatcher()->addCustomEventListener(EVENT_STANDARD_PROTAL_NO_ENERGY, CC_CALLBACK_1(GameUILayer::onEventStandardProtalNoEnergy,this));
    
}
void GameUILayer::onExit()
{
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_PROPERTY_DIRTY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_LEVEL_UP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_DEATH);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_FALL_AND_DIE);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_HURT);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_MONEY_NOT_ENOUGH);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_NO_MANA);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_BAG_NO_SPACE);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_NO_COPPER_KEY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_NO_SILVER_KEY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_NO_GOLD_KEY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_NO_ROOM_KEY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_NO_BOSS_KEY);
    
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_USE_POTION);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_USE_SCROLL);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_MONSTER_HURT);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_MONSTER_PROPERTY_DIRTY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_MONSTER_DEATH);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_MONSTER_ALERT);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_MONSTER_CONFUSING);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_TRIGGER_TOXIC_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_TRIGGER_FIRE_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_TRIGGER_PARALYTIC_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_TRIGGER_GRIPPING_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_TRIGGER_SUMMONING_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_TRIGGER_WEAK_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_TRIGGER_FROZEN_TRAP);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_USE_GOLD_CHEST_KEY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_USE_SILVER_CHEST_KEY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_USE_COPPER_CHEST_KEY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_USE_ROOM_KEY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_PLAYER_USE_BOSS_KEY);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_DOOR_MAGIC_LOCKED);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_DOOR_MAGIC_CLOSED);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_DOOR);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_TOXIC_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_FIRE_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_PARALYTIC_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_GRIPPING_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_SUMMONING_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_WEAK_TRAP);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_MSG);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FOUND_HIDDEN_ITEM);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_GO_UPSTAIRS);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_GO_UPSTAIRS_FORBIDDEN);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_GO_DOWNSTAIRS);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_GO_BOSSROOM);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_GO_CHASM);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_NPC_KNIGHT_ANSWER);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_NPC_CHILD_ANSWER);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_NPC_SHOPGIRL_ANSWER);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_NPC_OLDLADY_ANSWER);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_NPC_WEIRDO_ANSWER);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_NPC_OLDMAN_ANSWER);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_NPC_LITTLEWITCH_ANSWER);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_NPC_NURSE_ANSWER);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_BOSS_PROPERTY_DIRTY);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_BOSS_DEATH);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_BOSS_HURT);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_BOSS_ALERT);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_BOSS_SKILL1);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_BOSS_SKILL2);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_BOSS_SKILL3);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_ACHIEVE_COMPLETE);
    
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_FALL_DOWNSTAIRS);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_SELECT_LEFT_DUNGEON_NODE);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_SELECT_RIGHT_DUNGEON_NODE);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_USE_SMALL_PROTAL);
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(EVENT_STANDARD_PROTAL_NO_ENERGY);
    
    WrapperUILayer::onExit();
}
