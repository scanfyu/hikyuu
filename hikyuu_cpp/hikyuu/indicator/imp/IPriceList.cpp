/*
 * IPriceList.cpp
 *
 *  Created on: 2013-2-12
 *      Author: fasiondog
 */

#include "IPriceList.h"

#if HKU_SUPPORT_SERIALIZATION
BOOST_CLASS_EXPORT(hku::IPriceList)
#endif

namespace hku {

IPriceList::IPriceList() : IndicatorImp("PRICELIST", 1) {
    setParam<int>("result_index", 0);
    setParam<PriceList>("data", PriceList());
    setParam<int>("discard", 0);
}

IPriceList::IPriceList(const PriceList& data, int in_discard) : IndicatorImp("PRICELIST", 1) {
    setParam<int>("result_index", 0);
    setParam<PriceList>("data", data);
    setParam<int>("discard", in_discard);
}

IPriceList::~IPriceList() {}

bool IPriceList::check() {
    return (getParam<int>("discard") >= 0 && getParam<int>("result_index") >= 0);
}

void IPriceList::_calculate(const Indicator& data) {
    //如果在叶子节点，直接取自身的data参数
    if (isLeaf()) {
        PriceList x = getParam<PriceList>("data");
        int discard = getParam<int>("discard");

        size_t total = x.size();
        _readyBuffer(total, 1);

        //更新抛弃数量
        m_discard = discard > total ? total : discard;

        for (size_t i = m_discard; i < total; ++i) {
            _set(x[i], i);
        }

        for (size_t i = m_discard; i < total && std::isnan(get(i)); ++i) {
            m_discard++;
        }

        return;
    }

    //不在叶子节点上，则忽略本身的data参数，认为其输入实际为函数入参中的data
    int result_index = getParam<int>("result_index");
    HKU_ERROR_IF_RETURN(result_index < 0 || result_index >= data.getResultNumber(), void(),
                        "result_index out of range!");

    size_t total = data.size();
    _readyBuffer(total, 1);

    for (size_t i = data.discard(); i < total; ++i) {
        _set(data.get(i, result_index), i);
    }

    //更新抛弃数量
    m_discard = data.discard();
}

Indicator HKU_API PRICELIST(const PriceList& data, int discard) {
    return make_shared<IPriceList>(data, discard)->calculate();
}

Indicator HKU_API PRICELIST(const Indicator& data, int result_index) {
    IndicatorImpPtr p = make_shared<IPriceList>();
    p->setParam<int>("result_index", result_index);
    return Indicator(p)(data);
}

Indicator HKU_API PRICELIST(int result_index) {
    IndicatorImpPtr p = make_shared<IPriceList>();
    p->setParam<int>("result_index", result_index);
    return Indicator(p);
}

Indicator HKU_API PRICELIST(price_t* data, size_t total) {
    return data ? PRICELIST(PriceList(data, data + total), 0)
                : Indicator(make_shared<IPriceList>());
}

} /* namespace hku */
