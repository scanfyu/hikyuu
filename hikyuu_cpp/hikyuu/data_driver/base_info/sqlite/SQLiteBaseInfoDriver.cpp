/*
 * SQLiteBaseInfoDriver.cpp
 *
 *  Copyright (c) 2019 fasiondog
 *
 *  Created on: 2019-8-11
 *      Author: fasiondog
 */

#include <thread>
#include "../../../StockManager.h"
#include "SQLiteBaseInfoDriver.h"
#include "../table/MarketInfoTable.h"
#include "../table/StockTypeInfoTable.h"
#include "../table/StockWeightTable.h"
#include "../table/StockTable.h"

namespace hku {

SQLiteBaseInfoDriver::SQLiteBaseInfoDriver() : BaseInfoDriver("sqlite3"), m_pool(nullptr) {}

SQLiteBaseInfoDriver::~SQLiteBaseInfoDriver() {
    if (m_pool) {
        delete m_pool;
    }
}

bool SQLiteBaseInfoDriver::_init() {
    string dbname = tryGetParam<string>("db", "");
    HKU_ERROR_IF_RETURN(dbname == "", false, "Can't get Sqlite3 filename!");
    HKU_TRACE("SQLITE3: {}", dbname);
    m_pool = new ConnectPool<SQLiteConnect>(m_params);
    HKU_CHECK(m_pool, "Failed malloc ConnectPool!");
    return true;
}

bool SQLiteBaseInfoDriver::_loadMarketInfo() {
    HKU_ERROR_IF_RETURN(!m_pool, false, "Connect pool ptr is null!");

    try {
        auto con = m_pool->getConnect();
        vector<MarketInfoTable> infoTables;
        con->batchLoad(infoTables);

        StockManager& sm = StockManager::instance();
        for (auto& info : infoTables) {
            try {
                sm.loadMarketInfo(MarketInfo(
                  info.market(), info.name(), info.description(), info.code(), info.lastDate(),
                  info.openTime1(), info.closeTime1(), info.openTime2(), info.closeTime2()));
            } catch (std::exception& e) {
                HKU_ERROR("Failed load market, {}", e.what());
            } catch (...) {
                HKU_ERROR("Unknown error!");
            }
        }

    } catch (std::exception& e) {
        HKU_FATAL("load Market table failed! {}", e.what());
        return false;
    } catch (...) {
        HKU_FATAL("load Market table failed!");
        return false;
    }

    return true;
}

bool SQLiteBaseInfoDriver::_loadStockTypeInfo() {
    HKU_ERROR_IF_RETURN(!m_pool, false, "Connect pool ptr is null!");
    auto con = m_pool->getConnect();
    vector<StockTypeInfoTable> infoTables;
    try {
        con->batchLoad(infoTables);
    } catch (std::exception& e) {
        HKU_FATAL("load StockTypeInfo table failed! {}", e.what());
        return false;
    } catch (...) {
        HKU_FATAL("load StockTypeInfo table failed!");
        return false;
    }

    StockManager& sm = StockManager::instance();
    for (auto& info : infoTables) {
        sm.loadStockTypeInfo(StockTypeInfo(info.type(), info.description(), info.tick(),
                                           info.tickValue(), info.precision(),
                                           info.minTradeNumber(), info.maxTradeNumber()));
    }

    return true;
}

bool SQLiteBaseInfoDriver::_loadStock() {
    HKU_ERROR_IF_RETURN(!m_pool, false, "Connect pool ptr is null!");
    auto con = m_pool->getConnect();
    vector<MarketInfoTable> marketTable;
    try {
        con->batchLoad(marketTable);
    } catch (std::exception& e) {
        HKU_FATAL("load Market table failed! {}", e.what());
        return false;
    } catch (...) {
        HKU_FATAL("load Market table failed!");
        return false;
    }

    unordered_map<uint64_t, string> marketDict;
    for (auto& m : marketTable) {
        marketDict[m.id()] = m.market();
    }

    vector<StockTable> table;
    try {
        con->batchLoad(table);
    } catch (std::exception& e) {
        HKU_FATAL("load Stock table failed! {}", e.what());
        return false;
    } catch (...) {
        HKU_FATAL("load Stock table failed!");
        return false;
    }

    Stock stock;
    StockTypeInfo stockTypeInfo;
    StockTypeInfo null_stockTypeInfo;
    StockManager& sm = StockManager::instance();
    for (auto& r : table) {
        Datetime startDate, endDate;
        if (r.startDate > r.endDate || r.startDate == 0 || r.endDate == 0) {
            //日期非法，置为Null<Datetime>
            startDate = Null<Datetime>();
            endDate = Null<Datetime>();
        } else {
            startDate =
              (r.startDate == 99999999LL) ? Null<Datetime>() : Datetime(r.startDate * 10000);
            endDate = (r.endDate == 99999999LL) ? Null<Datetime>() : Datetime(r.endDate * 10000);
        }

        stockTypeInfo = sm.getStockTypeInfo(r.type);
        if (stockTypeInfo != null_stockTypeInfo) {
            stock =
              Stock(marketDict[r.marketid], r.code, r.name, r.type, r.valid, startDate, endDate,
                    stockTypeInfo.tick(), stockTypeInfo.tickValue(), stockTypeInfo.precision(),
                    stockTypeInfo.minTradeNumber(), stockTypeInfo.maxTradeNumber());

        } else {
            stock =
              Stock(marketDict[r.marketid], r.code, r.name, r.type, r.valid, startDate, endDate);
        }

        if (sm.loadStock(stock)) {
            StockWeightList weightList =
              getStockWeightList(marketDict[r.marketid], r.code, Datetime::min(), Null<Datetime>());
            stock.setWeightList(weightList);
        }
    }

    return true;
}

StockWeightList SQLiteBaseInfoDriver::getStockWeightList(const string& market, const string& code,
                                                         Datetime start, Datetime end) {
    HKU_ASSERT(m_pool);
    StockWeightList result;

    try {
        auto con = m_pool->getConnect();
        HKU_CHECK(con, "Failed fetch connect!");

        vector<StockWeightTable> table;
        Datetime new_end = end.isNull() ? Datetime::max() : end;
        con->batchLoad(
          table,
          format(
            "stockid=(select stockid from stock where marketid=(select marketid from "
            "market where market='{}') and code='{}') and date>={} and date<{} order by date asc",
            market, code, start.year() * 10000 + start.month() * 100 + start.day(),
            new_end.year() * 10000 + new_end.month() * 100 + new_end.day()));

        for (auto& w : table) {
            try {
                result.push_back(StockWeight(Datetime(w.date * 10000), w.countAsGift * 0.0001,
                                             w.countForSell * 0.0001, w.priceForSell * 0.001,
                                             w.bonus * 0.001, w.countOfIncreasement * 0.0001,
                                             w.totalCount, w.freeCount));
            } catch (std::out_of_range& e) {
                HKU_WARN("Date of id({}) is invalid! {}", w.id(), e.what());
            } catch (std::exception& e) {
                HKU_WARN("Error StockWeight Record id({}) {}", w.id(), e.what());
            } catch (...) {
                HKU_WARN("Error StockWeight Record id({})! Unknow reason!", w.id());
            }
        }

    } catch (std::exception& e) {
        HKU_FATAL("load StockWeight table failed! {}", e.what());
    } catch (...) {
        HKU_FATAL("load StockWeight table failed!");
    }

    return result;
}

Parameter SQLiteBaseInfoDriver ::getFinanceInfo(const string& market, const string& code) {
    Parameter result;
    HKU_IF_RETURN(!m_pool, result);

    std::stringstream buf;
    buf << "select f.updated_date, f.ipo_date, f.province,"
        << "f.industry, f.zongguben, f.liutongguben, f.guojiagu, f.faqirenfarengu,"
        << "f.farengu, f.bgu, f.hgu, f.zhigonggu, f.zongzichan, f.liudongzichan,"
        << "f.gudingzichan, f.wuxingzichan, f.gudongrenshu, f.liudongfuzhai,"
        << "f.changqifuzhai, f.zibengongjijin, f.jingzichan, f.zhuyingshouru,"
        << "f.zhuyinglirun, f.yingshouzhangkuan, f.yingyelirun, f.touzishouyu,"
        << "f.jingyingxianjinliu, f.zongxianjinliu, f.cunhuo, f.lirunzonghe,"
        << "f.shuihoulirun, f.jinglirun, f.weifenpeilirun, f.meigujingzichan,"
        << "f.baoliu2 from stkfinance f, stock s, market m "
        << "where m.market='" << market << "'"
        << " and s.code = '" << code << "'"
        << " and s.marketid = m.marketid"
        << " and f.stockid = s.stockid"
        << " order by updated_date DESC limit 1";

    auto con = m_pool->getConnect();

    auto st = con->getStatement(buf.str());
    st->exec();
    if (!st->moveNext()) {
        return result;
    }

    int updated_date(0), ipo_date(0);
    price_t province(0), industry(0), zongguben(0), liutongguben(0), guojiagu(0), faqirenfarengu(0);
    price_t farengu(0), bgu(0), hgu(0), zhigonggu(0), zongzichan(0), liudongzichan(0),
      gudingzichan(0);
    price_t wuxingzichan(0), gudongrenshu(0), liudongfuzhai(0), changqifuzhai(0), zibengongjijin(0);
    price_t jingzichan(0), zhuyingshouru(0), zhuyinglirun(0), yingshouzhangkuan(0), yingyelirun(0);
    price_t touzishouyi(0), jingyingxianjinliu(0), zongxianjinliu(0), cunhuo(0), lirunzonghe(0);
    price_t shuihoulirun(0), jinglirun(0), weifenpeilirun(0), meigujingzichan(0), baoliu2(0);

    st->getColumn(0, updated_date, ipo_date, province, industry, zongguben, liutongguben, guojiagu,
                  faqirenfarengu, farengu, bgu, hgu, zhigonggu, zongzichan, liudongzichan,
                  gudingzichan, wuxingzichan, gudongrenshu, liudongfuzhai, changqifuzhai,
                  zibengongjijin, jingzichan, zhuyingshouru, zhuyinglirun, yingshouzhangkuan,
                  yingyelirun, touzishouyi, jingyingxianjinliu, zongxianjinliu, cunhuo, lirunzonghe,
                  shuihoulirun, jinglirun, weifenpeilirun, meigujingzichan, baoliu2);

    result.set<string>("market", market);
    result.set<string>("code", code);
    result.set<int>("updated_date", updated_date);
    result.set<int>("ipo_date", ipo_date);
    result.set<price_t>("province", province);
    result.set<price_t>("industry", industry);
    result.set<price_t>("zongguben", zongguben);
    result.set<price_t>("liutongguben", liutongguben);
    result.set<price_t>("guojiagu", guojiagu);
    result.set<price_t>("faqirenfarengu", faqirenfarengu);
    result.set<price_t>("farengu", farengu);
    result.set<price_t>("bgu", bgu);
    result.set<price_t>("hgu", hgu);
    result.set<price_t>("zhigonggu", zhigonggu);
    result.set<price_t>("zongzichan", zongzichan);
    result.set<price_t>("liudongzichan", liudongzichan);
    result.set<price_t>("gudingzichan", gudingzichan);
    result.set<price_t>("wuxingzichan", wuxingzichan);
    result.set<price_t>("gudongrenshu", gudongrenshu);
    result.set<price_t>("liudongfuzhai", liudongfuzhai);
    result.set<price_t>("changqifuzhai", changqifuzhai);
    result.set<price_t>("zibengongjijin", zibengongjijin);
    result.set<price_t>("jingzichan", jingzichan);
    result.set<price_t>("zhuyingshouru", zhuyingshouru);
    result.set<price_t>("zhuyinglirun", zhuyinglirun);
    result.set<price_t>("yingshouzhangkuan", yingshouzhangkuan);
    result.set<price_t>("yingyelirun", yingyelirun);
    result.set<price_t>("touzishouyi", touzishouyi);
    result.set<price_t>("jingyingxianjinliu", jingyingxianjinliu);
    result.set<price_t>("zongxianjinliu", zongxianjinliu);
    result.set<price_t>("cunhuo", cunhuo);
    result.set<price_t>("lirunzonghe", lirunzonghe);
    result.set<price_t>("shuihoulirun", shuihoulirun);
    result.set<price_t>("jinglirun", jinglirun);
    result.set<price_t>("weifenpeilirun", weifenpeilirun);
    result.set<price_t>("meigujingzichan", meigujingzichan);
    result.set<price_t>("baoliu2", baoliu2);
    return result;
}

}  // namespace hku
