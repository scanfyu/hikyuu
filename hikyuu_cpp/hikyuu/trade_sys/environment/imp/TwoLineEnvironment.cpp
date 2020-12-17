/*
 * TwoLineEnviroment.cpp
 *
 *  Created on: 2016年5月17日
 *      Author: Administrator
 */

#include "../../../StockManager.h"
#include "../../../indicator/crt/KDATA.h"
#include "TwoLineEnvironment.h"

namespace hku {

TwoLineEnvironment::TwoLineEnvironment() : EnvironmentBase("TwoLine") {
    setParam<string>("market", "SH");
}

TwoLineEnvironment::TwoLineEnvironment(const Indicator& fast, const Indicator& slow)
: EnvironmentBase("TwoLine"), m_fast(fast), m_slow(slow) {
    setParam<string>("market", "SH");
}

TwoLineEnvironment::~TwoLineEnvironment() {}

EnvironmentPtr TwoLineEnvironment::_clone() {
    TwoLineEnvironment* ptr = new TwoLineEnvironment;
    ptr->m_fast = m_fast;
    ptr->m_slow = m_slow;
    return EnvironmentPtr(ptr);
}

void TwoLineEnvironment::_calculate() {
    string market = getParam<string>("market");
    const StockManager& sm = StockManager::instance();
    MarketInfo market_info = sm.getMarketInfo(market);
    HKU_IF_RETURN(market_info == Null<MarketInfo>(), void());

    Stock stock = sm.getStock(market + market_info.code());
    KData kdata = stock.getKData(m_query);
    Indicator close = CLOSE(kdata);
    Indicator fast = m_fast(close);
    Indicator slow = m_slow(close);

    size_t total = close.size();
    size_t start = fast.discard() > slow.discard() ? fast.discard() : slow.discard();
    for (size_t i = start; i < total; i++) {
        if (fast[i] > slow[i]) {
            _addValid(kdata[i].datetime);
        }
    }
}

EVPtr HKU_API EV_TwoLine(const Indicator& fast, const Indicator& slow, const string& market) {
    TwoLineEnvironment* ptr = new TwoLineEnvironment(fast, slow);
    ptr->setParam<string>("market", market);
    return EVPtr(ptr);
}

} /* namespace hku */
