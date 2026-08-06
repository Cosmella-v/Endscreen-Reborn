#include "../EndLevelLayer.hpp"
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/GJDropDownLayer.hpp>
class $modify(EndLevelLayerDropHook, GJDropDownLayer) {
	static void onModify(auto &self) {
		(void)self.setHookPriority("GJDropDownLayer::hideLayer", Priority::Replace-1);
	}
	virtual void hideLayer(bool instant)
    {
        if (EndLevelLayer* endl = typeinfo_cast<EndLevelLayer*>(this)) {
            auto ret = reinterpret_cast<endScreenRB*>(endl);
            if ( m_closeOnHide )
            {
                if ( m_delegate ) m_delegate->dropDownLayerWillClose(this);
            } else { ret->setVisible(false); }
            if (auto l = ret->m_mainLayer) l->stopAllActions();
            ret->hideAnimation(true, m_closeOnHide);
            return;
        } else {return GJDropDownLayer::hideLayer(instant);}
    }
};