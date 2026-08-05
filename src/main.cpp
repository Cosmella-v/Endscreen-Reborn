#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/utils/cocos.hpp>
using namespace geode::cocos;
#define getChildFromMainLayer(id) this->m_mainLayer->getChildByID(id)
#define CCWait(time, target) CCSequence::createWithTwoActions(CCDelayTime::create(time), target)

bool summarySetting = true;
$on_mod(Loaded) {
	summarySetting = Mod::get()->getSettingValue<bool>("summary-bg-setting");
	listenForSettingChanges<bool>("summary-bg-setting", [](bool value) {
		summarySetting = value;
	});
};

class $modify(endscreen, EndLevelLayer) {
	static void onModify(auto& self) {
        (void)self.setHookPriority("EndLevelLayer::onHideLayer", Priority::Replace);
    }
	void onHideLayer(CCObject *caller) {
		/* cleaned decomp by Cosmella */
		#define ACTIONTAG 12341
		m_hidden = !m_hidden;
		m_mainLayer->stopActionByTag(ACTIONTAG);
		this->stopActionByTag(ACTIONTAG);
		int BGAlpha = 100;
		float x = 0;
		CCActionEase* action;
		if (m_hidden) {
			BGAlpha = 0;
			x = -m_mainLayer->getContentWidth();
			action = cocos2d::CCEaseExponentialIn::create(cocos2d::CCMoveTo::create(1.0, {
				x,m_mainLayer->getPositionY()
			}));
		} else {
			action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {
				x,m_mainLayer->getPositionY()
			}));
		}
		action->setTag(ACTIONTAG);
		m_mainLayer->runAction(action);
		auto fade = cocos2d::CCFadeTo::create(1.0, BGAlpha);
		fade->setTag(ACTIONTAG);
		this->runAction(fade);
		#undef ACTIONTAG
	};
	void megahackEndAnimation() {
		auto director = CCDirector::get();
		auto winSize = director->getWinSize();
		/*
		Megahack doesn't have ids so instead we check if it installed
		we then go and check the contents of the strings
		*/
		if (auto megahack = Loader::get()->getInstalledMod("absolllute.megahack")) {
			std::string megahackstring = megahack->getVersion().toVString();
			int patched = 0;
			for (auto child : CCArrayExt<CCNode *>(m_mainLayer->getChildren())) {
				if (auto textlabel = typeinfo_cast<CCLabelBMFont *>(child)) {
					if (patched >= 2)
						break; // finished
					if (!textlabel->getID().empty())
						continue; // has ID
					auto text = std::string_view(textlabel->getString());

					if (text == ".") {
						textlabel->setPosition({-500, winSize.height * 0.719f});
						textlabel->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {40, winSize.height * 0.719f}))));
						patched++;
						continue;
					}
					if (text == megahackstring) {
						textlabel->setPosition({-500, winSize.height * 0.688f});
						textlabel->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {60, winSize.height * 0.688f}))));
						patched++;
						continue;
					}
				}
			}
		}
	}
	void moveCoins() {
		auto director = CCDirector::get();
		auto winSize = director->getWinSize();
		auto coinOneBG = getChildFromMainLayer("coin-1-background");
		auto coinTwoBG = getChildFromMainLayer("coin-2-background");
		auto coinThreeBG = getChildFromMainLayer("coin-3-background");
		// coin y = 0.266
		//  111 170 230

		if (coinOneBG) {
			coinOneBG->setPosition(-500, winSize.height * 0.266f);
			coinOneBG->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {111, winSize.height * 0.266f}))));
			if (auto coinOne = getChildFromMainLayer("coin-1-sprite")) {
				coinOne->setPosition(-500, winSize.height * 0.266f);
				coinOne->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {111, winSize.height * 0.266f}))));
			}
		}
		if (coinTwoBG) {
			coinTwoBG->setPosition(-500, winSize.height * 0.266f);
			coinTwoBG->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {170, winSize.height * 0.266f}))));
			if (auto coinTwo = getChildFromMainLayer("coin-2-sprite")) {
				coinTwo->setPosition(-500, winSize.height * 0.266f);
				coinTwo->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {170, winSize.height * 0.266f}))));
			}
		}
		if (coinThreeBG) {
			coinThreeBG->setPosition(-500, winSize.height * 0.266f);
			coinThreeBG->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {230, winSize.height * 0.266f}))));
			if (auto coinThree = getChildFromMainLayer("coin-3-sprite")) {
				coinThree->setPosition(-500, winSize.height * 0.266f);
				coinThree->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {230, winSize.height * 0.266f}))));
			}
		}
	}

	void playEndAnimation() {
		auto director = CCDirector::get();
		auto winSize = director->getWinSize();
		auto summary = getChildFromMainLayer("summary-container");
		auto summarySize = summary->getContentSize();
		auto summaryBG = CCScale9Sprite::create("GJ_square01.png");
		auto hideMenu = this->getChildByID("hide-layer-menu");
		auto inEditorLevel = false;

		if (hideMenu) {
			auto layout = ColumnLayout::create();
			layout->setAxisReverse(true);
			layout->setAxisAlignment(AxisAlignment::Center);

			hideMenu->ignoreAnchorPointForPosition(false);
			hideMenu->setAnchorPoint({0.5, 1.0});
			hideMenu->setContentSize({27, 280});
			hideMenu->setPositionY(this->getContentHeight() - 6);
			hideMenu->setLayout(layout);

			hideMenu->setPosition({1000, hideMenu->getPositionY()});
		}

		/*
		POSITIONS (X, Y MULT) (top down)
		level-complete-text: 170, 0.75
		summary-container: 100, 0.525
		complete-message: 150, 0.34
		button-menu: 150, 0.5
		*/
		// node->runAction(CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1,{-500,winSize.height*0.75}),1));
		if (auto node = getChildFromMainLayer("level-complete-text")) {
			node->runAction(CCWait(1.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {185, winSize.height * 0.77f}))));
		}
		if (summary) {
			if (summarySetting) {
				summaryBG->setContentSize({summarySize.width + 20, summarySize.height + 20});
				summaryBG->setPosition(summary->getPosition());
				summaryBG->setColor({0, 0, 0});
				summaryBG->setOpacity(100);
				summaryBG->setID("SummaryBG"_spr);
				summaryBG->setZOrder(-1);
				m_mainLayer->addChild(summaryBG);
				summaryBG->runAction(CCWait(2.4, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {175, winSize.height * 0.525f}))));
			}
			summary->runAction(CCWait(2.4, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {175, winSize.height * 0.525f}))));
		}

		megahackEndAnimation();

		if (auto node = getChildFromMainLayer("complete-message")) {
			node->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {180, winSize.height * 0.32f}))));
		}
		if (auto node = getChildFromMainLayer("end-text")) {
			node->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {170, winSize.height * 0.32f}))));
		}
		if (hideMenu) {
			hideMenu->runAction(CCWait(2.9, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {winSize.width * 0.9701f, hideMenu->getPositionY()}))));
		}
		moveCoins();
		if (auto node = getChildFromMainLayer("button-menu")) {
			auto retryButton = node->getChildByID("retry-button");
			auto exitButton = node->getChildByID("exit-button");
			auto editButton = node->getChildByID("edit-button");
			if (!editButton) {
				retryButton->setPosition({-60, retryButton->getPositionY()});
				exitButton->setPosition({60, exitButton->getPositionY()});
			}
			node->runAction(CCWait(3.1, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {175, winSize.height * 0.5f}))));
		}
	}

	virtual void showLayer(bool instant) { // end layer
		EndLevelLayer::showLayer(true);

		auto director = CCDirector::get();
		auto winSize = director->getWinSize();
		if (auto bg = getChildFromMainLayer("background")) {
			bg->setVisible(false);
		}
		if (auto cleft = getChildFromMainLayer("chain-left")) {
			cleft->setVisible(false);
		}
		if (auto cright = getChildFromMainLayer("chain-right")) {
			cright->setVisible(false);
		}

		if (auto node = getChildFromMainLayer("level-complete-text")) {
			node->setPosition({-500, winSize.height * 0.75f});
		}
		if (auto node = getChildFromMainLayer("summary-container")) {
			node->setPosition({-500, winSize.height * 0.525f});
		}
		if (auto node = getChildFromMainLayer("complete-message")) {
			node->setPosition({-500, winSize.height * 0.34f});
		}
		if (auto node = getChildFromMainLayer("end-text")) {
			node->setPosition({-500, winSize.height * 0.34f});
		}
		if (auto node = getChildFromMainLayer("button-menu")) {
			node->setPosition({-500, winSize.height * 0.5f});
		}
		this->setOpacity(0);
		this->runAction(cocos2d::CCFadeTo::create(1.5, 0x64u));
		this->playEndAnimation();
	}
};

class $modify(play, PlayLayer) {
	static void onModify(auto& self) {
       	(void)self.setHookPriority("PlayLayer::showCompleteEffect", Priority::Replace);
		(void)self.setHookPriority("PlayLayer::showEndLayer", Priority::First);
    }
	struct Fields {
		bool m_HookEnabled = false;
	};

	void showCompleteEffect() {
		m_fields->m_HookEnabled = true;
		showEndLayer();
		m_fields->m_HookEnabled = false;
		PlayLayer::showCompleteEffect();
	}
	void showEndLayer() {
		if (!m_fields->m_HookEnabled) {
			return;
		}
		PlayLayer::showEndLayer();
	}
	void showCompleteText() {
		PlayLayer::showCompleteText();
		auto ggtext = "GJ_practiceComplete_001.png";
		if (!m_isPracticeMode) {
			ggtext = "GJ_levelComplete_001.png";
		}
		if (auto gg = getChildBySpriteFrameName(this, ggtext)) {
			gg->setVisible(false);
		}
	}
};