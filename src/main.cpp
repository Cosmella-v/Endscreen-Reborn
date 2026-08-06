using namespace geode::prelude;

#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

/*add the qolmod to affect the CheatIndicator because it is kinda idless*/
namespace qolmod {
    class CheatIndicator : public cocos2d::CCNode {};
}

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

static void HandleNode(CCNode* mainLayer, 
	WeakRef<CCNode> left, WeakRef<CCNode> right, 
	WeakRef<CCNode> TopLeft, WeakRef<CCNode> TopRight,
	CCNode* Target) {
	auto pos = Target->convertToWorldSpace({0, 0});
	pos = mainLayer->convertToNodeSpace(pos);
	WeakRef<CCNode> ParentNoderef = 
	(pos.y < mainLayer->getContentHeight() * 0.75) ? 
	((pos.x > (mainLayer->getContentWidth() / 2)) ? right: left) 
	:
	((pos.x > (mainLayer->getContentWidth() / 2)) ? TopRight: TopLeft);

	if (auto ParentNode = ParentNoderef.lock()) {
		Target->retain();
		Target->removeFromParentAndCleanup(false);
		ParentNode->addChild(Target);
		Target->release();
		ParentNode->updateLayout();
	}
}
class $modify(endscreenRB, EndLevelLayer) {
	struct Fields {
		CCSize m_winSize;
		CCNode *m_downAnimation = nullptr;
		CCNode *m_upAnimation = nullptr;
	};
	static void onModify(auto &self) {
		(void)self.setHookPriority("EndLevelLayer::showLayer", Priority::Last);
		(void)self.setHookPriority("EndLevelLayer::onHideLayer", Priority::Replace);
		// Delay functions
		(void)self.setHookPriority("EndLevelLayer::playStarEffect", Priority::Last);
		(void)self.setHookPriority("EndLevelLayer::playCoinEffect", Priority::Last);
		(void)self.setHookPriority("EndLevelLayer::playCurrencyEffect", Priority::Last);
		(void)self.setHookPriority("EndLevelLayer::playDiamondEffect", Priority::Last);
	}
	/* 
		we need to play it later because we have a animation!
		it should be zero unless another mod is doing something to it which then we make sure it appears to atleast 3.7 delay 
	*/ 
	#define delay(fun) void fun(float startDelay) { \
		if (startDelay < 3.7) startDelay = 3.7; \
		EndLevelLayer::fun(startDelay); \
	};
	delay(playStarEffect)
	delay(playCoinEffect)
	delay(playCurrencyEffect)
	delay(playDiamondEffect)
	#undef delay
	

	void SetupModCompact() {
		auto winSize = m_fields->m_winSize;
		m_fields->m_downAnimation = CCNode::create();
		m_fields->m_downAnimation->setID("mod-storage-bottom"_spr);
		m_fields->m_upAnimation = CCNode::create();
		m_fields->m_upAnimation->setID("mod-storage-top"_spr);
		auto Right = CCNode::create();
		Right->setAnchorPoint({1, 0});
		Right->setContentSize({65, winSize.height / 2});
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

		auto TopRight = CCNode::create();
		TopRight->setAnchorPoint({1, 0});
		TopRight->setContentSize({65, winSize.height / 2 - 5});
		TopRight->setPosition({winSize.width - 50, winSize.height / 2});
		TopRight->setLayout(RowLayout::create()
		                     ->setGap(3.f)
		                     ->setAxisAlignment(AxisAlignment::End)
		                     ->setCrossAxisAlignment(AxisAlignment::End)
		                     ->setCrossAxisReverse(true)
		                     ->setGrowCrossAxis(true)
		                     ->setCrossAxisOverflow(false));
		TopRight->setID("top-right-side"_spr);
		m_fields->m_upAnimation->addChild(TopRight);

		auto Left = CCNode::create();
		Left->setAnchorPoint({1, 0});
		Left->setContentSize({65, winSize.height / 2});
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
		auto TopLeft = CCNode::create();
		TopLeft->setAnchorPoint({1, 0});
		TopLeft->setContentSize({65, winSize.height / 2 - 5});
		TopLeft->setPosition({winSize.width - 120, winSize.height / 2});
		TopLeft->setLayout(RowLayout::create()
		                     ->setGap(3.f)
		                     ->setAxisAlignment(AxisAlignment::Start)
		                     ->setCrossAxisAlignment(AxisAlignment::End)
		                     ->setCrossAxisReverse(true)
		                     ->setGrowCrossAxis(true)
		                     ->setCrossAxisOverflow(false));
		TopLeft->setID("top-left-side"_spr);
		m_fields->m_upAnimation->addChild(TopLeft);



		if(auto node = m_fields->m_downAnimation){
			node->setPositionY(-500);
			auto action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {node->getPositionX(), 0}));
			node->runAction(CCWait(2.8,action));
			this->addChild(node);
		}
		if(auto node = m_fields->m_upAnimation){
			node->setPositionY( this->getContentHeight()+node->getContentHeight());
			auto action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {node->getPositionX(), 0}));
			node->runAction(CCWait(2.8,action));
			this->addChild(node);
		}
		geode::Loader::get()->queueInMainThread(
		    [
			 wfthis =  geode::WeakRef<endscreenRB>(this),
			 wfLeft = geode::WeakRef<CCNode>(Left),
		     wfRight = geode::WeakRef<CCNode>(Right),
			 wfTopRight = geode::WeakRef<CCNode>(TopRight),
		     wfTopLeft = geode::WeakRef<CCNode>(TopLeft),
		     wfmainLayer = geode::WeakRef<CCNode>(m_mainLayer)] {
				if (auto _this = wfthis.lock()) {
					if (auto node = _this->m_sideMenu ?: _this->m_mainLayer->getChildByID("button-menu")) {
						/* Spoof it to the old position to scan */
						auto XPos = node->getPositionX();
						node->setPositionX(175.5);
						CCSize winSize = _this->m_fields->m_winSize;
						CCPoint bottomLeft = node->convertToNodeSpace(CCPointZero);
						CCPoint topRight = node->convertToNodeSpace(ccp(winSize.width, winSize.height));
						CCRect screenRect = CCRectMake(
							bottomLeft.x,
							bottomLeft.y,
							topRight.x - bottomLeft.x,
							topRight.y - bottomLeft.y
						);
						#define setBox(varname, node) \
							CCRect varname = CCRectMake(-100,0,0,0); \
							if (auto k = node) varname = k->boundingBox();
						
						
						setBox(retrybutton,node->getChildByID("retry-button"));
						setBox(exitButton,node->getChildByID("exit-button"));
						setBox(editButton,node->getChildByID("edit-button"))

						#undef setBox
						for (auto child : CCArrayExt<CCNode *>(node->getChildren())) {
							if (child->getUserObject("handledByMod"_spr)) continue;
							CCRect childRect = child->boundingBox();
							//log::debug("node {} | rect: {} screenrect: {} ScreenrecMinX: {} ScreenrecMaxX{}",child, childRect, screenRect, screenRect.getMinX(), screenRect.getMaxX() );
							if (editButton.intersectsRect(childRect) || retrybutton.intersectsRect(childRect)  || exitButton.intersectsRect(childRect) ||childRect.getMinX() < screenRect.getMinX() ||
								childRect.getMaxX() > screenRect.getMaxX() ||
								childRect.getMinY() < screenRect.getMinY() ||
								childRect.getMaxY() > screenRect.getMaxY()) 
								{
									if (
									auto ParentNode = (childRect.getMaxY() > screenRect.getMaxY() * 0.75) ?
									 	wfTopLeft.lock() :wfLeft.lock()
									) {
										child->retain();
										child->removeFromParentAndCleanup(false);
										child->setPosition(CCPointZero);
										child->ignoreAnchorPointForPosition(true);
										auto Menu = CCMenu::create();
										Menu->addChild(child);
										child->release();
										Menu->setContentSize(child->getContentSize());
										ParentNode->addChild(Menu);
										ParentNode->updateLayout();
									};
								}
						}
						node->setPositionX(XPos);
					}
				}
			    if (auto mainLayer = wfmainLayer.lock()) {
				    for (auto child : CCArrayExt<CCNode *>(mainLayer->getChildren())) {
						if (child->getUserObject("handledByMod"_spr)) continue;
					    if (typeinfo_cast<CCMenu*>(child)) {
							for (auto item : CCArrayExt<CCNode *>(child->getChildren())) {
								if (!typeinfo_cast<CCMenuItem *>(item))
									continue;
								item->retain();
								item->removeFromParentAndCleanup(false);
								item->setPosition({0,0});
								auto Menu = CCMenu::create();
								Menu->addChild(item);
								item->ignoreAnchorPointForPosition(true);
								item->release();
								Menu->setContentSize(item->getContentSize());
								HandleNode(mainLayer, wfLeft, wfRight, wfTopLeft, wfTopRight, Menu);
							}
							child->setVisible(false); // prevent touch being eaten + i don't remove just in case it would crash :shrug:
						} else if (typeinfo_cast<geode::Button*>(child)) {
							HandleNode(mainLayer, wfLeft, wfRight, wfTopLeft, wfTopRight, child);
						}
				    }
			    }
		    });
	};

	void onHideLayer(CCObject *caller) {
		/* cleaned decomp by Cosmella */
		#define ACTIONTAG 12341
		m_hidden = !m_hidden;
		if(m_fields->m_downAnimation) m_fields->m_downAnimation->stopActionByTag(ACTIONTAG);
		if(m_fields->m_upAnimation) m_fields->m_upAnimation->stopActionByTag(ACTIONTAG);
		m_mainLayer->stopActionByTag(ACTIONTAG);
		this->stopActionByTag(ACTIONTAG);
		int BGAlpha = 100;
		float x = 0;
		CCActionEase *action;
		if (m_hidden) {
			BGAlpha = 0;
			x = -m_mainLayer->getContentWidth();
			action = cocos2d::CCEaseExponentialIn::create(cocos2d::CCMoveTo::create(1.0, {x, m_mainLayer->getPositionY()}));
			if(auto node = m_fields->m_downAnimation){
				auto action = cocos2d::CCEaseExponentialIn::create(cocos2d::CCMoveTo::create(1.0, {node->getPositionX(), -500}));
				action->setTag(ACTIONTAG);
				node->runAction(action);
			}
			if(auto node = m_fields->m_upAnimation){
				auto action = cocos2d::CCEaseExponentialIn::create(cocos2d::CCMoveTo::create(1.0, {node->getPositionX(), this->getContentHeight()+node->getContentHeight()}));
				action->setTag(ACTIONTAG);
				node->runAction(action);
			}
		} else {
			action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {x, m_mainLayer->getPositionY()}));
			if(auto node = m_fields->m_downAnimation){
				auto action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {node->getPositionX(), 0}));
				action->setTag(ACTIONTAG);
				node->runAction(action);
			}
			if(auto node = m_fields->m_upAnimation){
				auto action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {node->getPositionX(), 0}));
				action->setTag(ACTIONTAG);
				node->runAction(action);
			}
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
			if (editButton){ 
				editButton->setUserObject("handledByMod"_spr,CCBool::create(true));
			}
			if (exitButton){ 
				exitButton->setUserObject("handledByMod"_spr,CCBool::create(true));
			}
			if (retryButton){ 
				retryButton->setUserObject("handledByMod"_spr,CCBool::create(true));
			}
			if (!editButton) {
				if (retryButton && exitButton) {
					retryButton->setPositionX(-60);
					exitButton->setPositionX(60);
				} else {
					if (retryButton) retryButton->setPositionX(0);
					if (exitButton) exitButton->setPositionX(0);
				}
			}
			if (auto Pretry = node->getChildByID("practice-retry-button")) {
				Pretry->setPositionX(160);
			};
			node->runAction(CCWait(3.1, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {175, winSize.height * 0.5f}))));
		}
	}

	virtual void showLayer(bool instant) { // end layer
		/*
			when my pr gets accepted it will add this to jam!
			zilko.jam user object to delay the jam reward
		*/
		this->setUserObject("zilko.jam/jam-reward-delay", CCFloat::create(3.7));
		instant = true;
		EndLevelLayer::showLayer(instant); 

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
		if (auto qolCheat = m_mainLayer->getChildByType<qolmod::CheatIndicator*>(-1)) {
			qolCheat->setPosition({-500, winSize.height * 0.65f});
			qolCheat->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {40, winSize.height * 0.65f}))));
		};

		this->setOpacity(0);
		this->runAction(cocos2d::CCFadeTo::create(1.5, 0x64u /*from decomp number is 100*/));
		this->playAnimation();
		SetupModCompact();
	}
};
