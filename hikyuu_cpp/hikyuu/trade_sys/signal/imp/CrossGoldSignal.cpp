/*
 * CrossGoldSignal.cpp
 *
 *  Created on: 2017年6月13日
 *      Author: fasiondog
 */

#include "../../../indicator/crt/KDATA.h"
#include "CrossGoldSignal.h"

namespace hku {

CrossGoldSignal::CrossGoldSignal() : SignalBase("SG_CrossGold") {
    setParam<string>("kpart", "CLOSE");
}

CrossGoldSignal::CrossGoldSignal(const Indicator& fast, const Indicator& slow, const string& kpart)
: SignalBase("SG_CrossGold"), m_fast(fast), m_slow(slow) {
    setParam<string>("kpart", kpart);
}

CrossGoldSignal::~CrossGoldSignal() {}

SignalPtr CrossGoldSignal::_clone() {
    CrossGoldSignal* p = new CrossGoldSignal();
    p->m_fast = m_fast;
    p->m_slow = m_slow;
    return SignalPtr(p);
}

void CrossGoldSignal::_calculate() {
    string kpart = getParam<string>("kpart");
    Indicator kdata = KDATA_PART(m_kdata, kpart);
    Indicator fast = m_fast(kdata);
    Indicator slow = m_slow(kdata);
    HKU_ERROR_IF_RETURN(fast.size() != slow.size(), void(), "fast.size() != slow.size()");

    size_t discard = fast.discard() > slow.discard() ? fast.discard() : slow.discard();
    size_t total = fast.size();
    for (size_t i = discard + 1; i < total; ++i) {
        if (fast[i - 1] < slow[i - 1] && fast[i] > slow[i] && fast[i - 1] < fast[i] &&
            slow[i - 1] < slow[i]) {
            _addBuySignal(m_kdata[i].datetime);
        } else if (fast[i - 1] > slow[i - 1] && fast[i] < slow[i] && fast[i - 1] > fast[i] &&
                   slow[i - 1] > slow[i]) {
            _addSellSignal(m_kdata[i].datetime);
        }
    }
}

SignalPtr HKU_API SG_CrossGold(const Indicator& fast, const Indicator& slow, const string& kpart) {
    return SignalPtr(new CrossGoldSignal(fast, slow, kpart));
}

} /* namespace hku */
