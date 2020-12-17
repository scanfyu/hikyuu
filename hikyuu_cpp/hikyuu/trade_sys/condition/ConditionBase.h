/*
 * ConditionBase.h
 *
 *  Created on: 2013-3-3
 *      Author: fasiondog
 */

#pragma once
#ifndef CONDITIONBASE_H_
#define CONDITIONBASE_H_

#include <set>
#include "../../utilities/Parameter.h"
#include "../../utilities/util.h"
#include "../../KData.h"
#include "../../trade_manage/TradeManager.h"
#include "../signal/SignalBase.h"

#if HKU_SUPPORT_SERIALIZATION
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/assume_abstract.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/set.hpp>
#endif

namespace hku {

/**
 * 系统有效条件基类
 * @note 系统有效性和待交易的对象有关也可能没关，所以保留setTradeObj接口
 * @ingroup Condition
 */
class HKU_API ConditionBase : public enable_shared_from_this<ConditionBase> {
    PARAMETER_SUPPORT

public:
    ConditionBase();
    ConditionBase(const string& name);
    virtual ~ConditionBase();

    /** 获取名称 */
    string name() const;

    /** 设置名称 */
    void name(const string& name);

    /** 复位操作 */
    void reset();

    /** 设置交易对象 */
    void setTO(const KData& kdata);

    /** 获取交易对象 */
    KData getTO() const;

    /** 设置交易管理实例 */
    void setTM(const TradeManagerPtr& tm);

    /** 获取交易管理实例 */
    TradeManagerPtr getTM() const;

    /** 设置系统信号指示器 */
    void setSG(const SGPtr& sg);

    /** 获取系统信号指示器 */
    SGPtr getSG() const;

    /**
     * 加入有效时间，在_calculate中调用
     * @param datetime 系统有效日期
     */
    void _addValid(const Datetime& datetime);

    typedef shared_ptr<ConditionBase> ConditionPtr;
    /** 克隆操作 */
    ConditionPtr clone();

    /**
     * 指定时间系统是否有效
     * @param datetime 指定时间
     * @return true 有效 | false 失效
     */
    bool isValid(const Datetime& datetime);

    /** 子类计算接口 */
    virtual void _calculate() = 0;

    /** 子类reset接口 */
    virtual void _reset() {}

    /** 子类克隆接口 */
    virtual ConditionPtr _clone() = 0;

protected:
    string m_name;
    KData m_kdata;
    TMPtr m_tm;
    SGPtr m_sg;
    std::set<Datetime> m_valid;

//============================================
// 序列化支持
//============================================
#if HKU_SUPPORT_SERIALIZATION
private:
    friend class boost::serialization::access;
    template <class Archive>
    void save(Archive& ar, const unsigned int version) const {
        ar& BOOST_SERIALIZATION_NVP(m_name);
        ar& BOOST_SERIALIZATION_NVP(m_params);
        ar& BOOST_SERIALIZATION_NVP(m_valid);
        // m_kdata/m_tm/m_sg是系统运行时临时设置，不需要序列化
    }

    template <class Archive>
    void load(Archive& ar, const unsigned int version) {
        ar& BOOST_SERIALIZATION_NVP(m_name);
        ar& BOOST_SERIALIZATION_NVP(m_params);
        ar& BOOST_SERIALIZATION_NVP(m_valid);
        // m_kdata/m_tm/m_sg是系统运行时临时设置，不需要序列化
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
#endif /* HKU_SUPPORT_SERIALIZATION */
};

#if HKU_SUPPORT_SERIALIZATION
BOOST_SERIALIZATION_ASSUME_ABSTRACT(ConditionBase)
#endif

#if HKU_SUPPORT_SERIALIZATION
/**
 * 对于没有私有变量的继承子类，可直接使用该宏定义序列化
 * @code
 * class Drived: public ConditionBase {
 *     CONDITION_NO_PRIVATE_MEMBER_SERIALIZATION
 *
 * public:
 *     Drived();
 *     ...
 * };
 * @endcode
 * @ingroup Condition
 */
#define CONDITION_NO_PRIVATE_MEMBER_SERIALIZATION               \
private:                                                        \
    friend class boost::serialization::access;                  \
    template <class Archive>                                    \
    void serialize(Archive& ar, const unsigned int version) {   \
        ar& BOOST_SERIALIZATION_BASE_OBJECT_NVP(ConditionBase); \
    }
#else
#define CONDITION_NO_PRIVATE_MEMBER_SERIALIZATION
#endif

/**
 * 客户程序都应使用该指针类型
 * @ingroup Condition
 */
typedef shared_ptr<ConditionBase> ConditionPtr;
typedef shared_ptr<ConditionBase> CNPtr;

#define CONDITION_IMP(classname)              \
public:                                       \
    virtual ConditionPtr _clone() {           \
        return ConditionPtr(new classname()); \
    }                                         \
    virtual void _calculate();

HKU_API std::ostream& operator<<(std::ostream&, const ConditionPtr&);
HKU_API std::ostream& operator<<(std::ostream&, const ConditionBase&);

inline string ConditionBase::name() const {
    return m_name;
}

inline void ConditionBase::name(const string& name) {
    m_name = name;
}

inline KData ConditionBase::getTO() const {
    return m_kdata;
}

inline void ConditionBase::setTM(const TradeManagerPtr& tm) {
    m_tm = tm;
}

inline SGPtr ConditionBase::getSG() const {
    return m_sg;
}

inline void ConditionBase::setSG(const SGPtr& sg) {
    m_sg = sg;
}

inline TradeManagerPtr ConditionBase::getTM() const {
    return m_tm;
}

} /* namespace hku */
#endif /* CONDITIONBASE_H_ */
