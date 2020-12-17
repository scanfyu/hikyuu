/*
 * RightShift.cpp
 *
 *  Created on: 2015年3月21日
 *      Author: fasiondog
 */

#include "RightShift.h"

#if HKU_SUPPORT_SERIALIZATION
BOOST_CLASS_EXPORT(hku::RightShift)
#endif

namespace hku {

RightShift::RightShift() : IndicatorImp("REF", 1) {
    setParam<int>("n", 1);
}

RightShift::~RightShift() {}

bool RightShift::check() {
    return getParam<int>("n") >= 0;
}

void RightShift::_calculate(const Indicator& data) {
    size_t total = data.size();
    int n = getParam<int>("n");

    m_discard = data.discard() + n;
    if (m_discard >= total) {
        m_discard = total;
        return;
    }
    for (size_t i = m_discard; i < total; ++i) {
        _set(data[i - n], i);
    }
}

Indicator HKU_API REF(int n) {
    IndicatorImpPtr p = make_shared<RightShift>();
    p->setParam<int>("n", n);
    return Indicator(p);
}

Indicator HKU_API REF(const Indicator& ind, int n) {
    return REF(n)(ind);
}

} /* namespace hku */
