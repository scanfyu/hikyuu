/*
 * MoneyManagerBase.cpp
 *
 *  Created on: 2013-3-3
 *      Author: fasiondog
 */

#include "MoneyManagerBase.h"

namespace hku {

HKU_API std::ostream& operator<<(std::ostream& os, const MoneyManagerBase& mm) {
    os << "MoneyManager(" << mm.name() << ", " << mm.getParameter() << ")";
    return os;
}

HKU_API std::ostream& operator<<(std::ostream& os, const MoneyManagerPtr& mm) {
    if (mm) {
        os << *mm;
    } else {
        os << "MoneyManager(NULL)";
    }

    return os;
}

MoneyManagerBase::MoneyManagerBase() : m_name("MoneyManagerBase") {
    setParam<bool>("auto-checkin", false);
    setParam<int>("max-stock", 20000);
    setParam<bool>("disable_ev_force_clean_position", false);
    setParam<bool>("disable_cn_force_clean_position", false);
}

MoneyManagerBase::MoneyManagerBase(const string& name) : m_name(name) {
    setParam<bool>("auto-checkin", false);
    setParam<int>("max-stock", 20000);
    setParam<bool>("disable_ev_force_clean_position", false);
    setParam<bool>("disable_cn_force_clean_position", false);
}

MoneyManagerBase::~MoneyManagerBase() {}

void MoneyManagerBase::buyNotify(const TradeRecord&) {}

void MoneyManagerBase::sellNotify(const TradeRecord&) {}

MoneyManagerPtr MoneyManagerBase::clone() {
    MoneyManagerPtr p;
    try {
        p = _clone();
    } catch (...) {
        HKU_ERROR("Subclass _clone failed!");
        p = MoneyManagerPtr();
    }

    if (!p || p.get() == this) {
        HKU_ERROR("Failed clone! Will use self-ptr!");
        return shared_from_this();
    }

    p->m_params = m_params;
    p->m_name = m_name;
    // p->m_tm = m_tm;
    p->m_query = m_query;
    return p;
}

double MoneyManagerBase ::getSellNumber(const Datetime& datetime, const Stock& stock, price_t price,
                                        price_t risk, SystemPart from) {
    HKU_ERROR_IF_RETURN(!m_tm, 0.0,
                        "m_tm is null! Datetime({}) Stock({}) price({:<.4f}) risk({:<.2f})",
                        datetime, stock.market_code(), price, risk);

    if (PART_ENVIRONMENT == from) {
        //强制全部卖出
        HKU_IF_RETURN(!getParam<bool>("disable_ev_force_clean_position"), MAX_DOUBLE);
    }

    if (PART_CONDITION == from) {
        HKU_IF_RETURN(!getParam<bool>("disable_cn_force_clean_position"), MAX_DOUBLE);
    }

    HKU_IF_RETURN(risk <= 0.0, 0.0);
    return _getSellNumber(datetime, stock, price, risk, from);
}

double MoneyManagerBase ::getBuyNumber(const Datetime& datetime, const Stock& stock, price_t price,
                                       price_t risk, SystemPart from) {
    HKU_ERROR_IF_RETURN(!m_tm, 0.0,
                        "m_tm is null! Datetime({}) Stock({}) price({:<.3f}) risk({:<.2f})",
                        datetime, stock.market_code(), price, risk);
    HKU_ERROR_IF_RETURN(stock.isNull(), 0.0, "stock is Null!");

    HKU_ERROR_IF_RETURN(
      risk <= 0.0, 0.0,
      "risk is zero! Datetime({}) Stock({}) price({:<.3f}) risk({:<.2f}) Part({})", datetime,
      stock.market_code(), price, risk, getSystemPartName(from));

    HKU_TRACE_IF_RETURN(m_tm->getStockNumber() >= getParam<int>("max-stock"), 0.0,
                        "Ignore! TM had max-stock number!");

    double n = _getBuyNumber(datetime, stock, price, risk, from);
    double min_trade = stock.minTradeNumber();

    if (n < min_trade) {
        HKU_TRACE("Ignore! Is less than the minimum number of transactions({})", min_trade);
        return 0;
    }

    //转换为最小交易量的整数倍
    n = long(n / min_trade) * min_trade;
    double max_trade = stock.maxTradeNumber();

    if (n > max_trade) {
        n = max_trade;
        HKU_INFO("Over stock.maxTradeNumber({})!", max_trade);
    }

    //在现金不足时，自动补充存入现金
    if (getParam<bool>("auto-checkin")) {
        price_t cash = m_tm->currentCash();
        CostRecord cost = m_tm->getBuyCost(datetime, stock, price, n);
        int precision = m_tm->precision();
        price_t money = roundUp(price * n * stock.unit() + cost.total, precision);
        if (money > cash) {
            m_tm->checkin(datetime, roundUp(money - cash, precision));
        }
    } else {
        CostRecord cost = m_tm->getBuyCost(datetime, stock, price, n);
        price_t need_cash = n * price + cost.total;
        price_t current_cash = m_tm->currentCash();
        while (n > min_trade && need_cash > current_cash) {
            n = n - min_trade;
            cost = m_tm->getBuyCost(datetime, stock, price, n);
            need_cash = n * price + cost.total;
        }
        n = need_cash > current_cash ? 0 : n;
    }

    return n;
}

double MoneyManagerBase ::getSellShortNumber(const Datetime& datetime, const Stock& stock,
                                             price_t price, price_t risk, SystemPart from) {
    HKU_ERROR_IF_RETURN(!m_tm, 0.0,
                        "m_tm is null! Datetime({}) Stock({}) price({:<.3f}) risk({:<.2f})",
                        datetime, stock.market_code(), price, risk);
    HKU_ERROR_IF_RETURN(risk <= 0.0, 0.0,
                        "risk is zero! Datetime({}) Stock({}) price({:<.3f}) risk({:<.2f})",
                        datetime, stock.market_code(), price, risk);
    return _getSellShortNumber(datetime, stock, price, risk, from);
}

double MoneyManagerBase ::getBuyShortNumber(const Datetime& datetime, const Stock& stock,
                                            price_t price, price_t risk, SystemPart from) {
    HKU_ERROR_IF_RETURN(!m_tm, 0.0,
                        "m_tm is null! Datetime({}) Stock({}) price({:<.3f}) risk({:<.2f})",
                        datetime, stock.market_code(), price, risk);
    HKU_ERROR_IF_RETURN(risk <= 0.0, 0.0,
                        "m_tm is null! Datetime({}) Stock({}) price({:<.3f}) risk({:<.2f})",
                        datetime, stock.market_code(), price, risk);
    return _getBuyShortNumber(datetime, stock, price, risk, from);
}

double MoneyManagerBase::_getSellNumber(const Datetime& datetime, const Stock& stock, price_t price,
                                        price_t risk, SystemPart from) {
    //默认卖出全部
    return MAX_DOUBLE;
}

double MoneyManagerBase::_getSellShortNumber(const Datetime& datetime, const Stock& stock,
                                             price_t price, price_t risk, SystemPart from) {
    return 0;
}

double MoneyManagerBase::_getBuyShortNumber(const Datetime& datetime, const Stock& stock,
                                            price_t price, price_t risk, SystemPart from) {
    return 0;
}

} /* namespace hku */
