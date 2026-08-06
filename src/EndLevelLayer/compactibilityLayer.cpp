#include "../EndLevelLayer.hpp"
using namespace geode::prelude;
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

/*add the qolmod to affect the CheatIndicator because it is kinda idless*/
namespace qolmod {
    class CheatIndicator : public cocos2d::CCNode {};
}


#define HandleNode(mainLayer, left, right, TopLeft, TopRight, Target) { \
	auto pos = Target->convertToWorldSpace({0, 0}); \
	pos = mainLayer->convertToNodeSpace(pos); \
	WeakRef<CCNode> ParentNoderef =  \
	(pos.y < mainLayer->getContentHeight() * 0.75) ?  \
	((pos.x > (mainLayer->getContentWidth() / 2)) ? right: left)  \
	: \
	((pos.x > (mainLayer->getContentWidth() / 2)) ? TopRight: TopLeft); \
 \
	if (auto ParentNode = ParentNoderef.lock()) { \
        moddedFlag(Target) \
		Target->retain(); \
		Target->removeFromParentAndCleanup(false); \
		ParentNode->addChild(Target); \
		Target->release(); \
		ParentNode->updateLayout(); \
	} \
} \


#define setBox(varname, node)                   \
	CCRect varname = CCRectMake(-100, 0, 0, 0); \
	if (auto k = node)                          \
		varname = k->boundingBox();

void endScreenRB::modCompactibilityLayer() {
    if (auto qolCheat = m_mainLayer->getChildByType<qolmod::CheatIndicator *>(-1)) {
		qolCheat->setPosition({-500, m_fields->m_winSize.height * 0.65f});
		qolCheat->runAction(CCWait(2.8, CCEaseExponentialOut::create(CCMoveTo::create(1.3, {40, m_fields->m_winSize.height * 0.65f}))));
	};
	megahackEndAnimation();
	CompactibilityScan();
}

void endScreenRB::megahackEndAnimation() {
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

void endScreenRB::CompactibilityScan() {
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
    auto queueInMainThread = 
        [wfthis = geode::WeakRef<endScreenRB>(this),
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
				        topRight.y - bottomLeft.y);
				        setBox(retrybutton, node->getChildByID("retry-button"));
				        setBox(exitButton, node->getChildByID("exit-button"));
				        setBox(editButton, node->getChildByID("edit-button")) 
                        for (auto child : CCArrayExt<CCNode *>(node->getChildren())) {
                            if (child->getUserObject("handledByMod"_spr))
                                continue;
                            CCRect childRect = child->boundingBox();
                            // log::debug("node {} | rect: {} screenrect: {} ScreenrecMinX: {} ScreenrecMaxX{}",child, childRect, screenRect, screenRect.getMinX(), screenRect.getMaxX() );
                            if (editButton.intersectsRect(childRect) || retrybutton.intersectsRect(childRect) || exitButton.intersectsRect(childRect) || childRect.getMinX() < screenRect.getMinX() ||
                                childRect.getMaxX() > screenRect.getMaxX() ||
                                childRect.getMinY() < screenRect.getMinY() ||
                                childRect.getMaxY() > screenRect.getMaxY()) {
                                if (auto ParentNode = 
                                    (childRect.getMaxY() > screenRect.getMaxY() * 0.75) ? wfTopLeft.lock() : wfLeft.lock()) 
                                    {
                                    child->retain();
                                    child->removeFromParentAndCleanup(false);
                                    child->setPosition(CCPointZero);
                                    child->ignoreAnchorPointForPosition(true);
                                    auto Menu = CCMenu::create();
                                    Menu->addChild(child);
                                    child->release();
                                    Menu->setContentSize(child->getContentSize());
                                    moddedFlag(Menu);
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
				    if (child->getUserObject("handledByMod"_spr))
					    continue;
				    if (typeinfo_cast<CCMenu *>(child)) {
					    for (auto item : CCArrayExt<CCNode *>(child->getChildren())) {
						    if (!typeinfo_cast<CCMenuItem *>(item))
							    continue;
						    item->retain();
						    item->removeFromParentAndCleanup(false);
						    item->setPosition({0, 0});
						    auto Menu = CCMenu::create();
						    Menu->addChild(item);
						    item->ignoreAnchorPointForPosition(true);
						    item->release();
						    Menu->setContentSize(item->getContentSize());
						    HandleNode(mainLayer, wfLeft, wfRight, wfTopLeft, wfTopRight, Menu);
					    }
					    child->setVisible(false); // prevent touch being eaten + i don't remove just in case it would crash :shrug:
				    } else if (typeinfo_cast<geode::Button *>(child)) {
					    HandleNode(mainLayer, wfLeft, wfRight, wfTopLeft, wfTopRight, child);
				    }
			    }
		    }
	    };
        queueInMainThread();
        geode::Loader::get()->queueInMainThread(queueInMainThread);
};