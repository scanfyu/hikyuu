/*
 * _DataDriverFactory.cpp
 *
 *  Created on: 2017年10月7日
 *      Author: fasiondog
 */

#include <boost/python.hpp>
#include <hikyuu/data_driver/BaseInfoDriver.h>
#include "../_Parameter.h"

using namespace hku;
using namespace boost::python;

class BaseInfoDriverWrap : public BaseInfoDriver, public wrapper<BaseInfoDriver> {
public:
    BaseInfoDriverWrap(const string& name) : BaseInfoDriver(name) {}

    bool _init() {
        return this->get_override("_init")();
    }

    bool _loadMarketInfo() {
        return this->get_override("_loadMarketInfo")();
    }

    bool _loadStockTypeInfo() {
        return this->get_override("_loadStockTypeInfo")();
    }

    bool _loadStock() {
        return this->get_override("_loadStock")();
    }

    Parameter getFinanceInfo(const string& market, const string& code) {
        if (override call = get_override("getFinanceInfo")) {
            return call(market, code);
        }
        return this->BaseInfoDriver::getFinanceInfo(market, code);
    }

    Parameter default_getFinanceInfo(const string& market, const string& code) {
        return this->BaseInfoDriver::getFinanceInfo(market, code);
    }

    StockWeightList getStockWeightList(const string& market, const string& code, Datetime start,
                                       Datetime end) {
        if (override call = get_override("getStockWeightList")) {
            return call(market, code, start, end);
        }
        return this->BaseInfoDriver::getStockWeightList(market, code, start, end);
    }

    StockWeightList default_getStockWeightList(const string& market, const string& code,
                                               Datetime start, Datetime end) {
        return this->BaseInfoDriver::getStockWeightList(market, code, start, end);
    }
};

void export_BaseInfoDriver() {
    class_<BaseInfoDriverWrap, boost::noncopyable>("BaseInfoDriver", init<const string&>())
      .def(self_ns::str(self))
      .add_property(
        "name", make_function(&BaseInfoDriver::name, return_value_policy<copy_const_reference>()))
      .def("getParam", &BaseInfoDriver::getParam<boost::any>)

      .def("init", &BaseInfoDriver::init)
      .def("loadBaseInfo", &BaseInfoDriver::loadBaseInfo)
      .def("_init", pure_virtual(&BaseInfoDriver::_init))
      .def("_loadMarketInfo", pure_virtual(&BaseInfoDriver::_loadMarketInfo))
      .def("_loadStockTypeInfo", pure_virtual(&BaseInfoDriver::_loadStockTypeInfo))
      .def("_loadStock", pure_virtual(&BaseInfoDriver::_loadStock))
      .def("getFinanceInfo", &BaseInfoDriver::getFinanceInfo,
           &BaseInfoDriverWrap::default_getFinanceInfo)
      .def("getStockWeightList", &BaseInfoDriver::getStockWeightList,
           &BaseInfoDriverWrap::getStockWeightList);

    register_ptr_to_python<BaseInfoDriverPtr>();
}
