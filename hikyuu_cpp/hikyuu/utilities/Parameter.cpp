/*
 * Parameter.cpp
 *
 *  Created on: 2013-2-28
 *      Author: fasiondog
 */

#include <boost/algorithm/string.hpp>
#include "../Log.h"
#include "Parameter.h"

namespace hku {

HKU_API std::ostream& operator<<(std::ostream& os, const Parameter& param) {
    // os << std::fixed;
    // os.precision(4);
    os << "params[";
    string strip(", ");
    string colon(": ");
    Parameter::param_map_t::const_iterator iter = param.m_params.begin();
    for (; iter != param.m_params.end(); ++iter) {
        os << iter->first;
        if (iter->second.type() == typeid(int)) {
            os << "(int): " << boost::any_cast<int>(iter->second) << strip;
        } else if (iter->second.type() == typeid(bool)) {
            os << "(bool): " << boost::any_cast<bool>(iter->second) << strip;
        } else if (iter->second.type() == typeid(double)) {
            os << "(double): " << boost::any_cast<double>(iter->second) << strip;
        } else if (iter->second.type() == typeid(string)) {
            os << "(string): " << boost::any_cast<string>(iter->second) << strip;
        } else if (iter->second.type() == typeid(Stock)) {
            os << "(Stock): " << boost::any_cast<Stock>(iter->second).market_code() << strip;
        } else if (iter->second.type() == typeid(KQuery)) {
            os << "(Query): " << boost::any_cast<KQuery>(iter->second) << strip;
        } else if (iter->second.type() == typeid(KData)) {
            os << "(KData): ";
            Stock stk = boost::any_cast<KData>(iter->second).getStock();
            if (stk.isNull()) {
                os << "Null" << strip;
            } else {
                os << stk.market_code() << strip;
            }
        } else if (iter->second.type() == typeid(PriceList)) {
            os << "(PriceList): " << boost::any_cast<PriceList>(iter->second).size() << strip;
        } else if (iter->second.type() == typeid(DatetimeList)) {
            os << "(DatetimeList): " << boost::any_cast<DatetimeList>(iter->second).size() << strip;
        } else {
            os << "Unsupported" << strip;
        }
    }
    os << "]";
    return os;
}

Parameter::Parameter() {}

Parameter::Parameter(const Parameter& p) : m_params(p.m_params) {}

Parameter::~Parameter() {}

Parameter& Parameter::operator=(const Parameter& p) {
    if (this == &p) {
        return *this;
    }

    m_params = p.m_params;
    return *this;
}

bool Parameter::support(const boost::any& value) {
    return value.type() == typeid(int) || value.type() == typeid(bool) ||
           value.type() == typeid(double) || value.type() == typeid(string) ||
           value.type() == typeid(Stock) || value.type() == typeid(KQuery) ||
           value.type() == typeid(KData) || value.type() == typeid(PriceList) ||
           value.type() == typeid(DatetimeList);
}

string Parameter::type(const string& name) const {
    auto iter = m_params.find(name);
    HKU_CHECK_THROW(iter != m_params.end(), std::out_of_range,
                    "out_of_range in Parameter::get : {}", name);
    HKU_IF_RETURN(iter->second.type() == typeid(int), "int");
    HKU_IF_RETURN(iter->second.type() == typeid(bool), "bool");
    HKU_IF_RETURN(iter->second.type() == typeid(double), "double");
    HKU_IF_RETURN(iter->second.type() == typeid(string), "string");
    HKU_IF_RETURN(iter->second.type() == typeid(Stock), "Stock");
    HKU_IF_RETURN(iter->second.type() == typeid(KQuery), "KQuery");
    HKU_IF_RETURN(iter->second.type() == typeid(KData), "KData");
    HKU_IF_RETURN(iter->second.type() == typeid(PriceList), "PriceList");
    HKU_IF_RETURN(iter->second.type() == typeid(DatetimeList), "DatetimeList");
    return "Unknow";
}

StringList Parameter::getNameList() const {
    vector<string> result;
    param_map_t::const_iterator iter = m_params.begin();
    for (; iter != m_params.end(); ++iter) {
        result.push_back(iter->first);
    }
    return result;
}

string Parameter::getNameValueList() const {
    std::stringstream os;
    Parameter::param_map_t::const_iterator iter = m_params.begin();
    Parameter::param_map_t::const_iterator next_iter = iter;
    string equal("=");
    for (; iter != m_params.end(); ++iter) {
        if (iter->second.type() == typeid(int)) {
            os << iter->first << equal << boost::any_cast<int>(iter->second);
        } else if (iter->second.type() == typeid(bool)) {
            os << iter->first << equal << boost::any_cast<bool>(iter->second);
        } else if (iter->second.type() == typeid(double)) {
            os << iter->first << equal << boost::any_cast<double>(iter->second);
        } else if (iter->second.type() == typeid(string)) {
            os << "\"" << iter->first << "\"" << equal << boost::any_cast<string>(iter->second);
        } else if (iter->second.type() == typeid(Stock)) {
            os << iter->first << equal << boost::any_cast<Stock>(iter->second);
        } else if (iter->second.type() == typeid(KQuery)) {
            os << iter->first << equal << boost::any_cast<KQuery>(iter->second);
        } else if (iter->second.type() == typeid(KData)) {
            //    os << iter->first << equal
            //       << boost::any_cast<KData>(iter->second);
        } else if (iter->second.type() == typeid(PriceList)) {
            os << iter->first << equal << "PriceList(...)";
        } else if (iter->second.type() == typeid(DatetimeList)) {
            os << iter->first << equal << "DatetimeList(...)";
        } else {
            os << "Unsupported";
        }

        ++next_iter;
        if (next_iter != m_params.end() && iter->second.type() != typeid(KData)) {
            os << ",";
        }
    }
    return os.str();
}

HKU_API bool operator==(const Parameter& p1, const Parameter& p2) {
    //注意：参数大小写敏感
    return p1.getNameValueList() == p2.getNameValueList();
}

HKU_API bool operator!=(const Parameter& p1, const Parameter& p2) {
    //注意：参数大小写敏感
    return p1.getNameValueList() != p2.getNameValueList();
}

HKU_API bool operator<(const Parameter& p1, const Parameter& p2) {
    return p1.getNameValueList() < p2.getNameValueList();
}

} /* namespace hku */
