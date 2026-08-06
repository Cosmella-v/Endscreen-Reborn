#include "../EndLevelLayer.hpp"

using namespace geode::prelude;

#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

#define ACTIONTAG 12341
#define StopAction(node)if (node) node->stopActionByTag(ACTIONTAG);
#define C_Duration 1.0f
static inline void TriggerMOVE(CCNode* node,CCPoint position = CCPointZero, bool easein = false, float speed = C_Duration) {
    if (node != nullptr) {
        CCActionEase* action = easein 
                ? (CCActionEase*)CCEaseExponentialIn::create(CCMoveTo::create(speed, position))
                : (CCActionEase*)CCEaseExponentialOut::create(CCMoveTo::create(speed, position));
        action->setTag(ACTIONTAG);
        node->runAction(action);
    }
}
void endScreenRB::hideAnimation(bool hidden, bool cleanup) {
    float Speed = cleanup ? 0.5 : C_Duration;
    /* 
    cleaned decomp by Cosmella 
    macros to make it easier to read
    */
	StopAction(m_fields->m_downAnimation);
	StopAction(m_fields->m_upAnimation);
	StopAction(m_mainLayer);
	StopAction(this);
	TriggerMOVE(
	    m_mainLayer,
	    {hidden ? -m_mainLayer->getContentWidth() : 0.f, m_mainLayer->getPositionY()},
	    hidden, Speed
    );
    TriggerMOVE(
            m_fields->m_downAnimation,
            {m_fields->m_downAnimation ? m_fields->m_downAnimation->getPositionX() : 0.f,
            hidden ? -500.f : 0.f},
            hidden, Speed);

            
        TriggerMOVE(
            m_fields->m_upAnimation,
            {m_fields->m_upAnimation ? m_fields->m_upAnimation->getPositionX() : 0.f,
            hidden ? getContentHeight() + (m_fields->m_upAnimation ? m_fields->m_upAnimation->getContentHeight() : 0.f) : 0.f },
            hidden, Speed);
    
	auto fade = CCFadeTo::create(Speed, hidden ? 0.f : 100.f);
	fade->setTag(ACTIONTAG);
	this->runAction(fade);
    if (cleanup) {
        this->runAction(CCWait(Speed, cocos2d::CCCallFunc::create(this,callfunc_selector(GJDropDownLayer::layerHidden))));
    };
}

void endScreenRB::onHideLayer(CCObject*) {
    m_hidden = !m_hidden;
    hideAnimation(m_hidden);
};