/*
 * Stock.h
 *
 *  Created on: 2011-11-9
 *      Author: fasiondog
 */

#pragma once
#ifndef STOCK_H_
#define STOCK_H_

#include <shared_mutex>
#include "StockWeight.h"
#include "KQuery.h"
#include "TimeLineRecord.h"
#include "TransRecord.h"

namespace hku {

class HKU_API StockManager;
class KDataDriver;
typedef shared_ptr<KDataDriver> KDataDriverPtr;
class HKU_API KData;
class HKU_API Parameter;

/**
 * Stock基类，Application中一般使用StockPtr进行操作
 * @ingroup StockManage
 */
class HKU_API Stock {
    friend class StockManager;

private:
    static const string default_market;
    static const string default_code;
    static const string default_market_code;
    static const string default_name;
    static const uint32_t default_type;
    static const bool default_valid;
    static const Datetime default_startDate;
    static const Datetime default_lastDate;
    static const price_t default_tick;
    static const price_t default_tickValue;
    static const price_t default_unit;
    static const int default_precision;
    static const size_t default_minTradeNumber;
    static const size_t default_maxTradeNumber;

public:
    Stock();

    Stock(const Stock&);
    Stock(const string& market, const string& code, const string& name);

    Stock(const string& market, const string& code, const string& name, uint32_t type, bool valid,
          const Datetime& startDate, const Datetime& lastDate);
    Stock(const string& market, const string& code, const string& name, uint32_t type, bool valid,
          const Datetime& startDate, const Datetime& lastDate, price_t tick, price_t tickValue,
          int precision, size_t minTradeNumber, size_t maxTradeNumber);
    virtual ~Stock();
    Stock& operator=(const Stock&);
    bool operator==(const Stock&) const;
    bool operator!=(const Stock&) const;

    /** 获取内部id，一般用于作为map的键值使用，该id实质为m_data的内存地址 */
    uint64_t id() const;

    /** 获取所属市场简称，市场简称是市场的唯一标识 */
    const string& market() const;

    /** 获取证券代码 */
    const string& code() const;

    /** 市场简称+证券代码，如: sh000001 */
    const string& market_code() const;

    /** 获取证券名称 */
    const string& name() const;

    /** 获取证券类型 */
    uint32_t type() const;

    /** 该证券当前是否有效 */
    bool valid() const;

    /** 获取证券起始日期 */
    Datetime startDatetime() const;

    /** 获取证券最后日期 */
    Datetime lastDatetime() const;

    /** 获取最小跳动量 */
    price_t tick() const;

    /** 最小跳动量价值 */
    price_t tickValue() const;

    /** 每单位价值 = tickValue / tick */
    price_t unit() const;

    /** 获取价格精度 */
    int precision() const;

    /** 获取最小交易数量，同minTradeNumber */
    size_t atom() const;

    /** 获取最小交易数量 */
    size_t minTradeNumber() const;

    /** 获取最大交易量 */
    size_t maxTradeNumber() const;

    /**
     * 获取指定时间段[start,end)内的权息信息
     * @param start 起始日期
     * @param end 结束日期
     * @return 满足要求的权息信息列表指针
     */
    StockWeightList getWeight(const Datetime& start = Datetime::min(),
                              const Datetime& end = Null<Datetime>()) const;

    /** 获取不同类型K线数据量 */
    size_t getCount(KQuery::KType dataType = KQuery::DAY) const;

    /** 获取指定日期时刻的市值，即小于等于指定日期的最后一条记录的收盘价 */
    price_t getMarketValue(const Datetime&, KQuery::KType) const;

    /**
     * 根据KQuery指定的条件，获取对应的K线位置范围
     * @param query [in] 指定的查询条件
     * @param out_start [out] 对应的K线起始范围
     * @param out_end [out] 对应的K线结束范围，不包含自身
     * @return true 成功 | false 失败
     */
    bool getIndexRange(const KQuery& query, size_t& out_start, size_t& out_end) const;

    /** 获取指定索引的K线数据记录，未作越界检查 */
    KRecord getKRecord(size_t pos, KQuery::KType dataType = KQuery::DAY) const;

    /** 根据数据类型（日线/周线等），获取指定日期的KRecord */
    KRecord getKRecord(const Datetime&, KQuery::KType ktype = KQuery::DAY) const;

    /** 获取K线数据 */
    KData getKData(const KQuery&) const;

    /**
     * 根据查询条件获取 KRecordList，不建议在客户端直接使用
     * @note 该方法不支持复权
     * @param query 查询条件
     */
    KRecordList getKRecordList(const KQuery& query) const;

    /** 获取日期列表 */
    DatetimeList getDatetimeList(const KQuery& query) const;

    /** 获取分时线 */
    TimeLineList getTimeLineList(const KQuery& query) const;

    /** 获取历史分笔数据 */
    TransList getTransList(const KQuery& query) const;

    /**
     * 获取当前财务信息
     */
    Parameter getFinanceInfo() const;

    /**
     * 获取历史财务信息
     * @param date 指定日期必须是0331、0630、0930、1231，如 Datetime(201109300000)
     */
    PriceList getHistoryFinanceInfo(const Datetime& date) const;

    /** 设置权息信息, 仅供初始化时调用 */
    void setWeightList(const StockWeightList&);

    /** 设置K线数据获取驱动 */
    void setKDataDriver(const KDataDriverPtr& kdataDriver);

    /** 获取K线数据获取驱动 */
    KDataDriverPtr getKDataDriver() const;

    /**
     * 将K线数据做自身缓存
     *  @note 一般不主动调用，谨慎
     */
    void loadKDataToBuffer(KQuery::KType);

    /** 释放对应的K线缓存 */
    void releaseKDataBuffer(KQuery::KType);

    /** 指定类型的K线数据是否被缓存 */
    bool isBuffer(KQuery::KType) const;

    /** 是否为Null */
    bool isNull() const;

    /** （临时函数）只用于更新缓存中的日线数据 **/
    void realtimeUpdate(const KRecord&, KQuery::KType ktype = KQuery::DAY);

    /** 仅用于python的__str__ */
    string toString() const;

private:
    bool _getIndexRangeByIndex(const KQuery&, size_t& out_start, size_t& out_end) const;

    // 以下函数属于基础操作添加了读锁
    size_t _getCountFromBuffer(KQuery::KType ktype) const;
    KRecord _getKRecordFromBuffer(size_t pos, KQuery::KType ktype) const;
    KRecordList _getKRecordListFromBuffer(size_t start_ix, size_t end_ix,
                                          KQuery::KType ktype) const;
    bool _getIndexRangeByDateFromBuffer(const KQuery&, size_t&, size_t&) const;

private:
    struct HKU_API Data;
    shared_ptr<Data> m_data;
    KDataDriverPtr m_kdataDriver;
};

struct HKU_API Stock::Data {
    string m_market;       //所属的市场简称
    string m_code;         //证券代码
    string m_market_code;  //市场简称证券代码
    string m_name;         //证券名称
    uint32_t m_type;       //证券类型
    bool m_valid;          //当前证券是否有效
    Datetime m_startDate;  //证券起始日期
    Datetime m_lastDate;   //证券最后日期

    StockWeightList m_weightList;     //权息信息列表
    Datetime m_lastUpdateWeightDate;  //上次更新权息列表缓存的时刻

    price_t m_tick;
    price_t m_tickValue;
    price_t m_unit;
    int m_precision;
    size_t m_minTradeNumber;
    size_t m_maxTradeNumber;

    unordered_map<string, KRecordList*> pKData;
    unordered_map<string, std::shared_mutex*> pMutex;

    Data();
    Data(const string& market, const string& code, const string& name, uint32_t type, bool valid,
         const Datetime& startDate, const Datetime& lastDate, price_t tick, price_t tickValue,
         int precision, size_t minTradeNumber, size_t maxTradeNumber);

    virtual ~Data();
};

/**
 * 输出Stock信息，如：Stock(market, code, name, type, valid, startDatetime, lastDatetime)
 * @ingroup StockManage
 */
HKU_API std::ostream& operator<<(std::ostream& os, const Stock& stock);

/** @ingroup StockManage */
typedef vector<Stock> StockList;

/**
 * 获取Stock，目的是封装StockManager，客户端不直接使用StockManager对象
 * @param querystr 格式：“市场简称证券代码”，如"sh000001"
 * @return 对应的证券实例，如果实例不存在，则Null<Stock>()，不抛出异常
 * @ingroup StockManage
 */
Stock HKU_API getStock(const string& querystr);

/* 用于将Stock实例作为map的key，一般建议使用stock.id做键值，
 * 否则map还要利用拷贝构造函数，创建新对象，效率低 */
bool operator<(const Stock& s1, const Stock& s2);
inline bool operator<(const Stock& s1, const Stock& s2) {
    return s1.id() < s2.id();
}

inline uint64_t Stock::id() const {
    return isNull() ? 0 : (int64_t)m_data.get();
}

inline bool Stock::operator==(const Stock& stock) const {
    return (*this != stock) ? false : true;
}

}  // namespace hku

#endif /* STOCK_H_ */
