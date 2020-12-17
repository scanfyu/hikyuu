/*
 * IVarp.cpp
 *
 *  Copyright (c) 2019 hikyuu.org
 *
 *  Created on: 2013-4-18
 *      Author: fasiondog
 */

#include "IVarp.h"
#include "../crt/MA.h"

#if HKU_SUPPORT_SERIALIZATION
BOOST_CLASS_EXPORT(hku::IVarp)
#endif

namespace hku {

IVarp::IVarp() : IndicatorImp("VARP", 1) {
    setParam<int>("n", 10);
}

IVarp::~IVarp() {}

bool IVarp::check() {
    return getParam<int>("n") >= 2;
}

void IVarp::_calculate(const Indicator& data) {
    size_t total = data.size();
    int n = getParam<int>("n");

    m_discard = data.discard() + n - 1;
    if (m_discard >= total) {
        m_discard = total;
        return;
    }

    Indicator ma = MA(data, n);
    for (size_t i = discard(); i < total; ++i) {
        price_t mean = ma[i];
        price_t sum = 0.0;
        for (size_t j = i + 1 - n; j <= i; ++j) {
            sum += std::pow(data[j] - mean, 2);
        }
        _set(sum / n, i);
    }
}

Indicator HKU_API VARP(int n) {
    IndicatorImpPtr p = make_shared<IVarp>();
    p->setParam<int>("n", n);
    return Indicator(p);
}

} /* namespace hku */
