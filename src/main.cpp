using namespace geode::prelude;

#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

#define getChildFromMainLayer(id) this->m_mainLayer->getChildByID(id)

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

AddSetting(bool, summarySetting, "summary-bg-setting")

static void HandleNode(CCNode* mainLayer, WeakRef<CCNode> left, WeakRef<CCNode> right, CCNode* Target) {
	auto pos = Target->convertToWorldSpace({0, 0});
	pos = mainLayer->convertToNodeSpace(pos);
	auto ParentNoderef = (pos.x > (mainLayer->getContentWidth() / 2)) ? left: right;
	if (auto ParentNode = ParentNoderef.lock()) {
		Target->retain();
		Target->removeFromParentAndCleanup(false);
		ParentNode->addChild(Target);
		Target->release();
		ParentNode->updateLayout();
	}
}
class $modify(endscreen, EndLevelLayer) {
	struct Fields {
		CCSize m_winSize;
		CCNode *m_downAnimation = nullptr;
	};
	static void onModify(auto &self) {
		(void)self.setHookPriority("EndLevelLayer::showLayer", Priority::Last);
		(void)self.setHookPriority("EndLevelLayer::onHideLayer", Priority::Replace);
		(void)self.setHookPriority("EndLevelLayer::customSetup", Priority::First);
	}

	void SetupModCompact() {
		auto winSize = m_fields->m_winSize;
		m_fields->m_downAnimation = CCNode::create();
		m_fields->m_downAnimation->setID("mod-storage"_spr);

		auto Right = CCNode::create();
		Right->setAnchorPoint({1, 0});
		Right->setContentSize({70, winSize.height / 2});
		Right->setPositionX(winSize.width - 50);
		Right->setLayout(RowLayout::create()
		                     ->setGap(3.f)
		                     ->setAxisAlignment(AxisAlignment::End)
		                     ->setCrossAxisAlignment(AxisAlignment::Start)
		                     ->setCrossAxisReverse(true)
		                     ->setGrowCrossAxis(true)
		                     ->setCrossAxisOverflow(false));
		Right->setID("right-side"_spr);
		m_fields->m_downAnimation->addChild(Right);

		auto Left = CCNode::create();
		Left->setAnchorPoint({1, 0});
		Left->setContentSize({70, winSize.height / 2});
		Left->setPositionX(winSize.width - 120);
		Left->setLayout(RowLayout::create()
		                    ->setGap(3.f)
		                    ->setAxisAlignment(AxisAlignment::Start)
		                    ->setCrossAxisAlignment(AxisAlignment::Start)
		                    ->setCrossAxisReverse(true)
		                    ->setGrowCrossAxis(true)
		                    ->setCrossAxisOverflow(false));
		Left->setID("left-side"_spr);
		m_fields->m_downAnimation->addChild(Left);

		m_mainLayer->addChild(m_fields->m_downAnimation);
		geode::Loader::get()->queueInMainThread(
		    [wfLeft = geode::WeakRef<CCNode>(Left),
		     wfRight = geode::WeakRef<CCNode>(Right),
		     wfmainLayer = geode::WeakRef<CCNode>(m_mainLayer)] {
			    if (auto mainLayer = wfmainLayer.lock()) {
				    for (auto child : CCArrayExt<CCNode *>(mainLayer->getChildren())) {
						if (child->getUserObject("handledByMod"_spr)) continue;
					    if (typeinfo_cast<CCMenu*>(child)) {
							for (auto item : CCArrayExt<CCNode *>(child->getChildren())) {
								if (!typeinfo_cast<CCMenuItem *>(item))
									continue;
								item->retain();
								item->removeFromParentAndCleanup(false);
								auto Menu = CCMenu::create();
								Menu->addChild(item);
								item->release();
								Menu->setContentSize(item->getContentSize());
								HandleNode(mainLayer, wfLeft, wfRight, Menu);
							}
							child->setVisible(false); // prevent touch being eaten + i don't remove just in case it would crash :shrug:
						} else if (typeinfo_cast<geode::Button*>(child)) {
							HandleNode(mainLayer, wfLeft, wfRight, child);
						}
				    }
			    }
		    });
	};

	void onHideLayer(CCObject *caller) {
		/* cleaned decomp by Cosmella */
		#define ACTIONTAG 12341
		m_hidden = !m_hidden;
		m_mainLayer->stopActionByTag(ACTIONTAG);
		this->stopActionByTag(ACTIONTAG);
		int BGAlpha = 100;
		float x = 0;
		CCActionEase *action;
		if (m_hidden) {
			BGAlpha = 0;
			x = -m_mainLayer->getContentWidth();
			action = cocos2d::CCEaseExponentialIn::create(cocos2d::CCMoveTo::create(1.0, {x, m_mainLayer->getPositionY()}));
		} else {
			action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {x, m_mainLayer->getPositionY()}));
		}
		action->setTag(ACTIONTAG);
		m_mainLayer->runAction(action);
		auto fade = cocos2d::CCFadeTo::create(1.0, BGAlpha);
		fade->setTag(ACTIONTAG);
		this->runAction(fade);
	#undef ACTIONTAG
	};

	void megahackEndAnimation() {
		/*
		    Megahack doesn't have ids so instead we check if it installed
		    we then go and check the contents of the strings
		*/
		auto winSize = m_fields->m_winSize;
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
		auto winSize = m_fields->m_winSize;
		auto coinOneBG = getChildFromMainLayer("coin-0-background");
		auto coinTwoBG = getChildFromMainLayer("coin-1-background");
		auto coinThreeBG = getChildFromMainLayer("coin-2-background");
		// coin y = 0.266
		//  111 170 230

		if (coinOneBG) {
			coinOneBG->setPosition(-500, winSize.height * 0.266f);
			coinOneBG->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {111, winSize.height * 0.266f}))));
			if (auto coinOne = getChildFromMainLayer("coin-0-sprite")) {
				coinOne->setPosition(-500, winSize.height * 0.266f);
				coinOne->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {111, winSize.height * 0.266f}))));
			}
		}
		if (coinTwoBG) {
			coinTwoBG->setPosition(-500, winSize.height * 0.266f);
			coinTwoBG->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {170, winSize.height * 0.266f}))));
			if (auto coinTwo = getChildFromMainLayer("coin-1-sprite")) {
				coinTwo->setPosition(-500, winSize.height * 0.266f);
				coinTwo->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {170, winSize.height * 0.266f}))));
			}
		}
		if (coinThreeBG) {
			coinThreeBG->setPosition(-500, winSize.height * 0.266f);
			coinThreeBG->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {230, winSize.height * 0.266f}))));
			if (auto coinThree = getChildFromMainLayer("coin-2-sprite")) {
				coinThree->setPosition(-500, winSize.height * 0.266f);
				coinThree->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {230, winSize.height * 0.266f}))));
			}
		}
	}

	void playAnimation() {
		auto winSize = m_fields->m_winSize;

		if (auto hideMenu = this->getChildByID("hide-layer-menu")) {
			if (auto layout = hideMenu->getLayout()) {
				if (auto t_Layout = typeinfo_cast<AxisLayout *>(layout)) {
					t_Layout->setAxisAlignment(AxisAlignment::Center);
					hideMenu->updateLayout();
				}
			}

			hideMenu->setPositionX(winSize.width * 2);
			hideMenu->runAction(CCWait(2.9, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {winSize.width - (hideMenu->getContentWidth() / 2), hideMenu->getPositionY()}))));
		}

		/*
		    POSITIONS (X, Y MULT) (top down)
		    level-complete-text: 170, 0.75
		    summary-container: 100, 0.525
		    complete-message: 150, 0.34
		    button-menu: 150, 0.5
		*/
		// node->runAction(CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1,{-500,winSize.height*0.75}),1));
		/*
		if this doesn't compile then use another compiler lol
		*/
		if (auto node = getChildFromMainLayer("practice-complete-text") ?: getChildFromMainLayer("level-complete-text")) {
			node->setPosition({-500, winSize.height * 0.75f});
			node->runAction(CCWait(1.8, CCEaseExponentialOut::create(CCMoveTo::create(1.5, {185, winSize.height * 0.77f}))));
		}
		if (auto summary = getChildFromMainLayer("summary-container")) {
			summary->setPosition({-500, winSize.height * 0.525f});
			if (Settings::summarySetting) {
				auto summarySize = summary->getScaledContentSize();
				auto summaryBG = CCScale9Sprite::create("GJ_square01.png");
				summaryBG->setContentSize({summarySize.width + 20, summarySize.height + 20});
				/*Megahack seems to put after the hook, again it's breaking it*/
				geode::Loader::get()->queueInMainThread(
				    [wfsummary = geode::WeakRef<CCNode>(summary),
				     wfnode = geode::WeakRef<CCNode>(summaryBG)] {
					    if (auto summary = wfsummary.lock()) {
						    auto summarySize = summary->getScaledContentSize();

						    if (auto node = wfnode.lock()) {
							    node->setContentSize({summarySize.width + 20, summarySize.height + 20});
						    }
					    }
				    });
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
		int completemsgOffset = 0;
		if (auto node = getChildFromMainLayer("thesillydoggo.qolmod/info-menu")) {
			node->setUserObject("handledByMod"_spr,CCBool::create(true));
			node->setPosition({-500, winSize.height * 0.3f});
			completemsgOffset += 13;
			node->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {180, winSize.height * 0.26f}))));
			if (auto edit = getChildFromMainLayer("thesillydoggo.qolmod/safe-mode-text")) {
				edit->setAnchorPoint({0.5,1});
			};
		}
		if (auto node = getChildFromMainLayer("complete-message") ?: getChildFromMainLayer("thesillydoggo.qolmod/safe-mode-text")) {
			node->setPosition({-500, (winSize.height * 0.34f) + completemsgOffset});
			node->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {180, (winSize.height * 0.32f) + completemsgOffset}))));
		}

		if (auto node = getChildFromMainLayer("end-text")) {
			node->setPosition({-500, winSize.height * 0.34f});
			node->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {170, winSize.height * 0.32f}))));
		}
		moveCoins();
		if (auto node = m_sideMenu ?: getChildFromMainLayer("button-menu")) {
			node->setUserObject("handledByMod"_spr,CCBool::create(true));
			node->setPosition({-500, winSize.height * 0.5f});

			auto retryButton = node->getChildByID("retry-button");
			auto exitButton = node->getChildByID("exit-button");
			auto editButton = node->getChildByID("edit-button");
			if (!editButton) {
				retryButton->setPositionX(-60);
				exitButton->setPositionX(60);
			}
			if (auto Pretry = node->getChildByID("practice-retry-button")) {
				Pretry->setPositionX(160);
			};
			node->runAction(CCWait(3.1, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {175, winSize.height * 0.5f}))));
		}
	}

	virtual void showLayer(bool instant) { // end layer
		EndLevelLayer::showLayer(true);

		auto director = CCDirector::get();
		auto winSize = director->getWinSize();
		m_fields->m_winSize = winSize;

		if (auto node = getChildFromMainLayer("hide-dropdown-menu")) {
			node->setUserObject("handledByMod"_spr,CCBool::create(true));
		}

		if (auto bg = getChildFromMainLayer("background")) {
			bg->setVisible(false);
		}
		if (auto cleft = getChildFromMainLayer("chain-left")) {
			cleft->setVisible(false);
		}
		if (auto cright = getChildFromMainLayer("chain-right")) {
			cright->setVisible(false);
		}

		this->setOpacity(0);
		this->runAction(cocos2d::CCFadeTo::create(1.5, 0x64u /*from decomp number is 100*/));
		this->playAnimation();
		SetupModCompact();
	}
};
