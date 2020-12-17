/*
 * SingleSignal.cpp
 *
 *  Created on: 2015年2月22日
 *      Author: fasiondog
 */

#include "../../../indicator/crt/KDATA.h"
#include "../../../indicator/crt/DIFF.h"
#include "../../../indicator/crt/STDEV.h"
#include "SingleSignal.h"

namespace hku {

SingleSignal::SingleSignal() : SignalBase("SG_Single") {
    setParam<int>("filter_n", 10);
    setParam<double>("filter_p", 0.1);
    setParam<string>("kpart", "CLOSE");
}

SingleSignal::SingleSignal(const Indicator& ind) : SignalBase("SG_Single"), m_ind(ind) {
    setParam<int>("filter_n", 10);
    setParam<double>("filter_p", 0.1);
    setParam<string>("kpart", "CLOSE");
}

SingleSignal::~SingleSignal() {}

SignalPtr SingleSignal::_clone() {
    SingleSignal* p = new SingleSignal();
    p->m_ind = m_ind;
    return SignalPtr(p);
}

void SingleSignal::_calculate() {
    int filter_n = getParam<int>("filter_n");
    double filter_p = getParam<double>("filter_p");
    string kpart(getParam<string>("kpart"));

    Indicator ind = m_ind(KDATA_PART(m_kdata, kpart));
    Indicator dev = STDEV(DIFF(ind), filter_n);

    size_t start = dev.discard();
    HKU_IF_RETURN(start < 3, void());

    size_t total = dev.size();
    for (size_t i = start; i < total; ++i) {
        double dama = ind[i] - ind[i - 1];
        double dama2 = ind[i] - ind[i - 2];
        double dama3 = ind[i] - ind[i - 3];
        double sdama = dev[i] * filter_p;
        if (dama > 0 && (dama > sdama || dama2 > sdama || dama3 > sdama)) {
            _addBuySignal(m_kdata[i].datetime);
        } else if (dama < 0 && (dama < sdama || dama2 < sdama || dama3 < sdama)) {
            _addSellSignal(m_kdata[i].datetime);
        }
    }
}

SignalPtr HKU_API SG_Single(const Indicator& ind, int filter_n, double filter_p,
                            const string& kpart) {
    SingleSignal* p = new SingleSignal(ind);
    p->setParam<int>("filter_n", filter_n);
    p->setParam<double>("filter_p", filter_p);
    p->setParam<string>("kpart", kpart);
    return SignalPtr(p);
}

} /* namespace hku */
