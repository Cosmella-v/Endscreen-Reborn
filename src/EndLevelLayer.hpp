#pragma once
using namespace geode::prelude;
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

#define getChildFromMainLayer(id) m_mainLayer->getChildByID(id)

#define moddedFlag(node) node->setUserObject("handledByMod"_spr,CCBool::create(true));

#define CCWait(time, target) CCSequence::createWithTwoActions(CCDelayTime::create(time), target)

#define AddSetting(type, var, id)                              \
	namespace Settings {                                       \
		static type var;                                       \
	}                                                          \
	$on_mod(Loaded) {                                          \
		Settings::var = Mod::get()->getSettingValue<type>(id); \
		listenForSettingChanges<type>(id, [](type value) {     \
			Settings::var = value;                             \
		});                                                    \
	};

class $modify(endScreenRB, EndLevelLayer) {
	struct Fields {
		CCSize m_winSize;
		CCNode *m_downAnimation = nullptr;
		CCNode *m_upAnimation = nullptr;
	};
	static void onModify(auto &self);

    // currency.cpp
    void playStarEffect(float);
    void playCoinEffect(float);
    void playCurrencyEffect(float);
    void playDiamondEffect(float);

    //compactibilityLayer.cpp
    void modCompactibilityLayer();
    void megahackEndAnimation();
	void CompactibilityScan();

    // showLayer.cpp
    virtual void showLayer(bool instant);
	void moveCoins();
	void playAnimation();

    // Hidelayer.cpp
    void onHideLayer(CCObject *caller);
    void hideAnimation(bool toggle, bool cleanup = false);
};
