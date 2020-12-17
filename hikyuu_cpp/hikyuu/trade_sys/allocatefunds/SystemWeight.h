/*
 * SystemWeight.h
 *
 *  Created on: 2018年1月29日
 *      Author: fasiondog
 */

#pragma once
#ifndef TRADE_SYS_ALLOCATEFUNDS_SYSTEMWEIGHT_H_
#define TRADE_SYS_ALLOCATEFUNDS_SYSTEMWEIGHT_H_

#include "../system/System.h"

namespace hku {

class HKU_API SystemWeight {
public:
    SystemWeight();
    SystemWeight(const SystemPtr& sys, double weight);
    virtual ~SystemWeight();

    void setSYS(const SystemPtr& sys);
    SystemPtr getSYS() const;

    void setWeight(double weight);
    double getWeight() const;

public:
    SystemPtr m_sys;
    double m_weight;

private:
//============================================
// 序列化支持
//============================================
#if HKU_SUPPORT_SERIALIZATION
private:
    friend class boost::serialization::access;
    template <class Archive>
    void save(Archive& ar, const unsigned int version) const {
        ar& BOOST_SERIALIZATION_NVP(m_sys);
        ar& BOOST_SERIALIZATION_NVP(m_weight);
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version) {
        ar& BOOST_SERIALIZATION_NVP(m_sys);
        ar& BOOST_SERIALIZATION_NVP(m_weight);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif /* HKU_SUPPORT_SERIALIZATION */
};

typedef vector<SystemWeight> SystemWeightList;

HKU_API std::ostream& operator<<(std::ostream&, const SystemWeight&);

inline void SystemWeight::setSYS(const SystemPtr& sys) {
    m_sys = sys;
}

inline SystemPtr SystemWeight::getSYS() const {
    return m_sys;
}

inline void SystemWeight::setWeight(double weight) {
    HKU_CHECK_THROW(weight >= 0 && weight <= 1, std::out_of_range,
                    "weigth ({}) is out of range [0, 1]!", weight);
    m_weight = weight;
}

inline double SystemWeight::getWeight() const {
    return m_weight;
}

} /* namespace hku */

#endif /* TRADE_SYS_ALLOCATEFUNDS_SYSTEMWEIGHT_H_ */
