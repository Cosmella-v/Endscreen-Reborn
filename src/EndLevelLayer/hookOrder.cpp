#include "../EndLevelLayer.hpp"
using namespace geode::prelude;
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>

void endScreenRB::onModify(auto &self) {
	(void)self.setHookPriority("EndLevelLayer::showLayer", Priority::Last);
	(void)self.setHookPriority("EndLevelLayer::onHideLayer", Priority::Replace);
	// Delay functions
	(void)self.setHookPriority("EndLevelLayer::playStarEffect", Priority::Last);
	(void)self.setHookPriority("EndLevelLayer::playCoinEffect", Priority::Last);
	(void)self.setHookPriority("EndLevelLayer::playCurrencyEffect", Priority::Last);
	(void)self.setHookPriority("EndLevelLayer::playDiamondEffect", Priority::Last);
}