#include "../EndLevelLayer.hpp"
using namespace geode::prelude;
#include <Geode/Geode.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
/*
    we need to play it later because we have a animation!
    it should be zero unless another mod is doing something to it which then we make sure it appears to atleast 3.7 delay
*/
#define delay(fun)                      \
	void endScreenRB::fun(float startDelay) {        \
		if (startDelay < 3.7)           \
			startDelay = 3.7;           \
		EndLevelLayer::fun(startDelay); \
	};

delay(playStarEffect)
delay(playCoinEffect)
delay(playCurrencyEffect)
delay(playDiamondEffect)