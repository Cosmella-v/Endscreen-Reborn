#include "../EndLevelLayer.hpp"
using namespace geode::prelude;
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

AddSetting(bool, summarySetting, "summary-bg-setting")

void endScreenRB::showLayer(bool instant) { // end layer
	/*
	    when my pr gets accepted it will add this to jam!
	    zilko.jam user object to delay the jam reward
	*/
	this->setUserObject("zilko.jam/jam-reward-delay", CCFloat::create(3.7));
	instant = true;
	EndLevelLayer::showLayer(instant);
	NodeIDs::get()->provide(this);
	std::cout << "showLayer called" << std::endl;
	auto director = CCDirector::get();
	auto winSize = director->getWinSize();
	m_fields->m_winSize = winSize;

	geode::Loader::get()->queueInMainThread([=]{
		if(auto node = getChildFromMainLayer("star-container")){
			moddedFlag(node);
			std::cout << "star container found" << std::endl;
			node->setPositionX(winSize.width * 0.527f);
			node->runAction(CCWait(3.7, CCEaseExponentialOut::create(CCMoveTo::create(1, {winSize.width*0.569f, node->getPositionY()}))));
		};
		if(auto node = getChildFromMainLayer("orb-container")){
			moddedFlag(node);
			node->setPositionX(winSize.width * 0.527f);
			node->runAction(CCWait(3.7, CCEaseExponentialOut::create(CCMoveTo::create(1, {winSize.width*0.569f, node->getPositionY()}))));
		};
		if(auto node = getChildFromMainLayer("diamond-container")){
			moddedFlag(node);
			node->setPositionX(winSize.width * 0.527f);
			node->runAction(CCWait(3.7, CCEaseExponentialOut::create(CCMoveTo::create(1, {winSize.width*0.569f, node->getPositionY()}))));
		};
	});

	if (auto node = getChildFromMainLayer("hide-dropdown-menu")) moddedFlag(node)
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
    this->moveCoins();
    this->modCompactibilityLayer();
    if (auto node = m_fields->m_downAnimation) {
		node->setPositionY(-500);
		auto action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {node->getPositionX(), 0}));
		node->runAction(CCWait(2.8, action));
		this->addChild(node);
	}
	if (auto node = m_fields->m_upAnimation) {
		node->setPositionY(this->getContentHeight() + node->getContentHeight());
		auto action = cocos2d::CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1.0, {node->getPositionX(), 0}));
		node->runAction(CCWait(2.8, action));
		this->addChild(node);
	}
};
/*
    POSITIONS (X, Y MULT) (top down)
    level-complete-text: 170, 0.75
    summary-container: 100, 0.525
    complete-message: 150, 0.34
    button-menu: 150, 0.5
*/
// node->runAction(CCEaseExponentialOut::create(cocos2d::CCMoveTo::create(1,{-500,winSize.height*0.75}),1));
void endScreenRB::playAnimation() {
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
			/*
			 Megahack seems to put after the hook, again it's breaking it
			*/
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


	int completemsgOffset = 0;
	if (auto node = getChildFromMainLayer("thesillydoggo.qolmod/info-menu")) {
		node->setUserObject("handledByMod"_spr, CCBool::create(true));
		node->setPosition({-500, winSize.height * 0.3f});
		completemsgOffset += 13;
		node->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {180, winSize.height * 0.26f}))));
		if (auto edit = getChildFromMainLayer("thesillydoggo.qolmod/safe-mode-text")) {
			edit->setAnchorPoint({0.5, 1});
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


	if (auto node = m_sideMenu ?: getChildFromMainLayer("button-menu")) {
		node->setUserObject("handledByMod"_spr, CCBool::create(true));
		node->setPosition({-500, winSize.height * 0.5f});

		auto retryButton = node->getChildByID("retry-button");
		auto exitButton = node->getChildByID("exit-button");
		auto editButton = node->getChildByID("edit-button");
		if (editButton) moddedFlag(editButton) if (exitButton) moddedFlag(exitButton) if (retryButton) moddedFlag(retryButton) 
        if (!editButton) {
				if (retryButton && exitButton) {
					retryButton->setPositionX(-60);
					exitButton->setPositionX(60);
				} else {
					if (retryButton)
						retryButton->setPositionX(0);
					if (exitButton)
						exitButton->setPositionX(0);
				}
			}
		if (auto Pretry = node->getChildByID("practice-retry-button")) {
			moddedFlag(Pretry)
			Pretry->setPositionX(160);
		};
		node->runAction(CCWait(3.1, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {175, winSize.height * 0.5f}))));
	}
}

void endScreenRB::moveCoins() {
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