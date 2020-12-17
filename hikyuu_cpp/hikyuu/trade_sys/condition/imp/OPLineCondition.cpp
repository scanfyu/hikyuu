/*
 * OPCondition.cpp
 *
 *  Created on: 2016年5月9日
 *      Author: Administrator
 */

#include "OPLineCondition.h"
#include "../../../trade_manage/crt/crtTM.h"
#include "../../../trade_manage/crt/TC_Zero.h"
#include "../../../indicator/crt/PRICELIST.h"
#include "../../system/crt/SYS_Simple.h"
#include "../../moneymanager/crt/MM_FixedCount.h"

namespace hku {

OPLineCondition::OPLineCondition() : ConditionBase("OPLine") {}

OPLineCondition::OPLineCondition(const Indicator& op) : ConditionBase("OPLine"), m_op(op) {}

OPLineCondition::~OPLineCondition() {}

ConditionPtr OPLineCondition::_clone() {
    OPLineCondition* ptr = new OPLineCondition(m_op);
    return ConditionPtr(ptr);
}

void OPLineCondition::_reset() {}

void OPLineCondition::_calculate() {
    HKU_IF_RETURN(m_kdata.size() == 0, void());
    Stock stock = m_kdata.getStock();
    KQuery query = m_kdata.getQuery();
    SYSPtr sys = SYS_Simple();

    TMPtr tm = crtTM(m_kdata[0].datetime, 0.0, TC_Zero());
    MMPtr mm = MM_FixedCount(stock.minTradeNumber());
    mm->setParam<bool>("auto-checkin", true);

    sys->setTM(tm);
    sys->setMM(mm);
    sys->setSG(m_sg);

    sys->run(m_kdata.getStock(), m_kdata.getQuery());
    KQuery::KType ktype = query.kType();
    DatetimeList dates = m_kdata.getDatetimeList();
    Indicator profit = PRICELIST(tm->getProfitCurve(dates, ktype));
    Indicator op = m_op(profit);

    Indicator x = profit - op;
    for (size_t i = 0; i < x.size(); i++) {
        if (x[i] > 0) {
            _addValid(dates[i]);
        }
    }
}

CNPtr HKU_API CN_OPLine(const Indicator& op) {
    return make_shared<OPLineCondition>(op);
}

} /* namespace hku */
