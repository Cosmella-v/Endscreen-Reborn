#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
#include <Geode/utils/cocos.hpp>
using namespace geode::cocos;
class $modify(InstantEndLevelLayer, EndLevelLayer) {
	static void onModify(auto &self) {
			(void)self.setHookPriority("EndLevelLayer::showLayer", Priority::Stub);
	}
	virtual void showLayer(bool instant) {
		/*
		this is the proper way of instant showLayer
		*/
		instant = true;
		EndLevelLayer::showLayer(instant);
	};
};
class $modify(play, PlayLayer) {
	static void onModify(auto& self) {
       	(void)self.setHookPriority("PlayLayer::levelComplete", Priority::Last);
		(void)self.setHookPriority("PlayLayer::showEndLayer", Priority::Stub);
    }
	struct Fields {
		bool m_hookEnabled = false;
	};
    void levelComplete() {
         /*
            Anyone wondering what this does in this case it does
            showEndLayer() which shows the endscreen
            calls levelComplete which has a action to call showCompleteText or showCompleteEffect depending on if practice mode is on or not
            so all this does it block it
        */
        PlayLayer::levelComplete();

        m_fields->m_hookEnabled = true;
		showEndLayer();
		m_fields->m_hookEnabled = false;
    }
/*
    // old method broke practice mode
	void showCompleteEffect() {
       
		m_fields->m_hookEnabled = true;
		showEndLayer();
		m_fields->m_hookEnabled = false;
		PlayLayer::showCompleteEffect();
	}
*/
	void showEndLayer() {
        /*
            Practice mode? where are you?
        */
		if (!m_fields->m_hookEnabled) {
			return;
		}
		PlayLayer::showEndLayer();
	}
	void showCompleteText() {
        /*
        No node ids, as soon as i add node ids to this i will be using it instead
        */
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