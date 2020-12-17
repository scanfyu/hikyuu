/*
 * _Constant.cpp
 *
 *  Created on: 2013-4-10
 *      Author: fasiondog
 */

#include <boost/python.hpp>
#include <hikyuu/DataType.h>
#include "pickle_support.h"

using namespace boost::python;
using namespace hku;

struct Constant {
    Constant()
    : null_datetime(Null<Datetime>()),
      inf(std::numeric_limits<double>::infinity()),
      nan(std::numeric_limits<double>::quiet_NaN()),
      max_double(std::numeric_limits<double>::max()),
      null_double(Null<double>()),
      null_price(Null<price_t>()),
      null_int(Null<int>()),
      null_size(Null<size_t>()),
      null_int64(Null<int64_t>()),
      STOCKTYPE_BLOCK(0),
      STOCKTYPE_A(1),
      STOCKTYPE_INDEX(2),
      STOCKTYPE_B(3),
      STOCKTYPE_FUND(4),
      STOCKTYPE_ETF(5),
      STOCKTYPE_ND(6),
      STOCKTYPE_BOND(7),
      STOCKTYPE_GEM(8),
      STOCKTYPE_BTC(9),
      STOCKTYPE_TMP(999) {
#if HKU_PYTHON_SUPPORT_PICKLE
        pickle_support = true;
#else
        pickle_support = false;
#endif /* HKU_PYTHON_SUPPORT_PICKLE */
    }

    Datetime null_datetime;
    double inf;
    double nan;
    double null_double;
    double max_double;
    double null_price;
    int null_int;
    size_t null_size;
    int64_t null_int64;
    bool pickle_support;  //是否支持pickle

    int STOCKTYPE_BLOCK;  /// 板块
    int STOCKTYPE_A;      /// A股
    int STOCKTYPE_INDEX;  /// 指数
    int STOCKTYPE_B;      /// B股
    int STOCKTYPE_FUND;   /// 基金
    int STOCKTYPE_ETF;    /// ETF
    int STOCKTYPE_ND;     /// 国债
    int STOCKTYPE_BOND;   /// 债券
    int STOCKTYPE_GEM;    /// 创业板
    int STOCKTYPE_BTC;    /// 比特币
    int STOCKTYPE_TMP;    /// 临时Stock
};

void export_Constant() {
    class_<Constant>("Constant")
      .def_readonly("null_datetime", &Constant::null_datetime, "无效Datetime")
      .def_readonly("inf", &Constant::inf, "无穷大或无穷小")
      .def_readonly("nan", &Constant::nan, "非数字")
      .def_readonly("null_double", &Constant::null_double, "同 nan")
      .def_readonly("max_double", &Constant::max_double, "最大double值")
      .def_readonly("null_price", &Constant::null_price, "同 nan")
      .def_readonly("null_int", &Constant::null_int, "无效int")
      .def_readonly("null_size", &Constant::null_size, "无效size")
      .def_readonly("null_int64", &Constant::null_int64, "无效int64_t")
      .def_readonly("pickle_support", &Constant::pickle_support, "是否支持 pickle")

      .def_readonly("STOCKTYPE_BLOCK", &Constant::STOCKTYPE_BLOCK, "板块")
      .def_readonly("STOCKTYPE_A", &Constant::STOCKTYPE_A, "A股")
      .def_readonly("STOCKTYPE_INDEX", &Constant::STOCKTYPE_INDEX, "指数")
      .def_readonly("STOCKTYPE_B", &Constant::STOCKTYPE_B, "B股")
      .def_readonly("STOCKTYPE_FUND", &Constant::STOCKTYPE_FUND, "基金")
      .def_readonly("STOCKTYPE_ETF", &Constant::STOCKTYPE_ETF, "ETF")
      .def_readonly("STOCKTYPE_ND", &Constant::STOCKTYPE_ND, "国债")
      .def_readonly("STOCKTYPE_BOND", &Constant::STOCKTYPE_BOND, "债券")
      .def_readonly("STOCKTYPE_GEM", &Constant::STOCKTYPE_GEM, "创业板")
      .def_readonly("STOCKTYPE_BTC", &Constant::STOCKTYPE_GEM, "比特币")
      .def_readonly("STOCKTYPE_TMP", &Constant::STOCKTYPE_GEM, "临时Stock");
}
