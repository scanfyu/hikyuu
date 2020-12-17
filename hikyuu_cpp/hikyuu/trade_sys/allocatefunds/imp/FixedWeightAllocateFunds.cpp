/*
 * Copyright (c) 2019 hikyuu.org
 *
 *  Created on: 2018年2月8日
 *      Author: fasiondog
 */

#include "FixedWeightAllocateFunds.h"

namespace hku {

FixedWeightAllocateFunds::FixedWeightAllocateFunds() : AllocateFundsBase("AF_FixedWeight") {
    setParam<double>("weight", 0.1);
}

FixedWeightAllocateFunds::~FixedWeightAllocateFunds() {}

SystemWeightList FixedWeightAllocateFunds ::_allocateWeight(const Datetime& date,
                                                            const SystemList& se_list) {
    SystemWeightList result;
    double weight = getParam<double>("weight");
    for (auto iter = se_list.begin(); iter != se_list.end(); ++iter) {
        SystemWeight sw;
        sw.setSYS(*iter);
        sw.setWeight(weight);
        result.push_back(sw);
    }

    return result;
}

AFPtr HKU_API AF_FixedWeight(double weight) {
    HKU_CHECK_THROW(weight > 0 && weight <= 1, std::out_of_range,
                    "input weigth ({}) is out of range [0, 1]!", weight);
    auto p = make_shared<FixedWeightAllocateFunds>();
    p->setParam<double>("weight", weight);
    return p;
}

} /* namespace hku */
