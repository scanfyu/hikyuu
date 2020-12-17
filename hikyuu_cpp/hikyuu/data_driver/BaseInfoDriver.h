/*
 * BaseInfoDriver.h
 *
 *  Created on: 2012-8-14
 *      Author: fasiondog
 */

#pragma once
#ifndef BASEINFODRIVER_H_
#define BASEINFODRIVER_H_

#include "../utilities/Parameter.h"
#include "../MarketInfo.h"
#include "../StockTypeInfo.h"
#include "../Stock.h"

namespace hku {

/**
 * 基本信息数据获取驱动基类
 * @ingroup DataDriver
 */
class HKU_API BaseInfoDriver {
    PARAMETER_SUPPORT

public:
    typedef unordered_map<string, MarketInfo> MarketInfoMap;
    typedef unordered_map<uint32_t, StockTypeInfo> StockTypeInfoMap;

    /**
     * 构造函数
     * @param name 驱动名称
     */
    BaseInfoDriver(const string& name);
    virtual ~BaseInfoDriver() {}

    /** 获取驱动名称 */
    const string& name() const;

    /**
     * 驱动初始化
     * @param params
     * @return
     */
    bool init(const Parameter& params);

    /**
     * 加载基础信息
     * @return
     */
    bool loadBaseInfo();

    /**
     * 获取指定日期范围内 [start, end) 的权限列表
     * @param market 市场简称
     * @param code 证券代码
     * @param start 起始日期
     * @param end 结束日期
     */
    virtual StockWeightList getStockWeightList(const string& market, const string& code,
                                               Datetime start, Datetime end);

    /**
     * 获取当前财务信息
     * @param market 市场标识
     * @param code 证券代码
     */
    virtual Parameter getFinanceInfo(const string& market, const string& code);

    /**
     * 驱动初始化，具体实现时应注意将之前打开的相关资源关闭。
     * @return
     */
    virtual bool _init() = 0;

    /**
     * 加载市场信息
     * @return true 成功 | false 失败
     */
    virtual bool _loadMarketInfo() = 0;

    /**
     * 加载证券类型信息
     * @return true 成功 | false 失败
     */
    virtual bool _loadStockTypeInfo() = 0;

    /**
     * 加载股票信息
     * @return true 成功 | false 失败
     */
    virtual bool _loadStock() = 0;

private:
    bool checkType();

protected:
    string m_name;
};

typedef shared_ptr<BaseInfoDriver> BaseInfoDriverPtr;

HKU_API std::ostream& operator<<(std::ostream&, const BaseInfoDriver&);
HKU_API std::ostream& operator<<(std::ostream&, const BaseInfoDriverPtr&);

inline const string& BaseInfoDriver::name() const {
    return m_name;
}

} /* namespace hku */
#endif /* BASEINFODRIVER_H_ */
