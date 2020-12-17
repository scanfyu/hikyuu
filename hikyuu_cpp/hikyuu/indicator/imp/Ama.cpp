/*
 * Ama.cpp
 *
 *  Created on: 2013-4-7
 *      Author: fasiondog
 */

#include <cmath>
#include "Ama.h"

#if HKU_SUPPORT_SERIALIZATION
BOOST_CLASS_EXPORT(hku::Ama)
#endif

namespace hku {

Ama::Ama() : IndicatorImp("AMA", 2) {
    setParam<int>("n", 10);
    setParam<int>("fast_n", 2);
    setParam<int>("slow_n", 30);
}

Ama::~Ama() {}

bool Ama::check() {
    return getParam<int>("n") >= 1 && getParam<int>("fast_n") >= 0 && getParam<int>("slow_n") >= 0;
}

void Ama::_calculate(const Indicator& data) {
    size_t total = data.size();
    m_discard = data.discard();
    if (m_discard >= total) {
        m_discard = total;
        return;
    }

    int n = getParam<int>("n");
    int fast_n = getParam<int>("fast_n");
    int slow_n = getParam<int>("slow_n");

    size_t start = m_discard;

    price_t fastest = 2.0 / (fast_n + 1);
    price_t slowest = 2.0 / (slow_n + 1);
    price_t delta = fastest - slowest;

    price_t prevol = 0.0, vol = 0.0, er = 1.0, c(0.0);
    price_t ama = data[start];
    size_t first_end = start + n + 1 >= total ? total : start + n + 1;
    _set(ama, start, 0);
    _set(er, start, 1);
    for (size_t i = start + 1; i < first_end; ++i) {
        vol += std::fabs(data[i] - data[i - 1]);
        er = (vol == 0.0) ? 1.0 : (data[i] - data[start]) / vol;
        if (er > 1.0)
            er = 1.0;
        c = std::pow((std::fabs(er) * delta + slowest), 2);
        ama += c * (data[i] - ama);
        _set(ama, i, 0);
        _set(er, i, 1);
    }

    prevol = vol;
    for (size_t i = first_end; i < total; ++i) {
        vol = prevol + std::fabs(data[i] - data[i - 1]) - std::fabs(data[i + 1 - n] - data[i - n]);
        er = (vol == 0.0) ? 1.0 : (data[i] - data[i - n]) / vol;
        if (er > 1.0)
            er = 1.0;
        if (er < -1.0)
            er = -1.0;
        c = std::pow((std::fabs(er) * delta + slowest), 2);
        ama += c * (data[i] - ama);
        prevol = vol;
        _set(ama, i, 0);
        _set(er, i, 1);
    }
}

Indicator HKU_API AMA(int n, int fast_n, int slow_n) {
    IndicatorImpPtr p = make_shared<Ama>();
    p->setParam<int>("n", n);
    p->setParam<int>("fast_n", fast_n);
    p->setParam<int>("slow_n", slow_n);
    return Indicator(p);
}

Indicator HKU_API AMA(const Indicator& ind, int n, int fast_n, int slow_n) {
    return AMA(n, fast_n, slow_n)(ind);
}

} /* namespace hku */
