.. py:currentmodule:: hikyuu.data_driver
.. highlightlang:: python

自定义K线驱动
==============

可参见详见安装目录或源码目录下“data_driverpytdx_data_driver.py”示例，该示例使用pytdx作为数据源（不建议直接使用，仅供参考）。如有需要使用MySQL、CSV等存储K线数据的，可参考该示例自行实现。



K线数据驱动基类
----------------

.. py:class:: KDataDriver

    K线数据驱动基类
    
    自定义K线数据驱动接口：

    * :py:meth:`KDataDriver._init` - 【可选】初始化子类私有变量
    * :py:meth:`KDataDriver.isIndexFirst` - 【必须】指示该引擎是按位置索引查询方式更快还是按日期
    * :py:meth:`KDataDriver.getKRecordList` - 【必须】初始化子类私有变量
    * :py:meth:`KDataDriver.getCount` - 【必须】初始化子类私有变量
    * :py:meth:`KDataDriver._getIndexRangeByDate` - 【必须】初始化子类私有变量
    
    .. py:attribute:: name 名称
    
    .. py:method:: getParam(self, name)

        获取指定的参数
    
        :param str name: 参数名称
        :return: 参数值
        :raises out_of_range: 无此参数    
    
    .. py:method:: _init(self)
    
        【重载接口】（可选）初始化子类私有变量
        
    .. py:method:: isIndexFirst(self)

        【重载接口】（必须）指示该引擎是按位置索引查询方式更快还是按日期

    .. py:method:: getKRecordList(self, market, code, query)
    
        【重载接口】（必须）按指定的位置[start_ix, end_ix)读取K线数据至out_buffer
        
        :param str market: 市场标识
        :param str code: 证券代码
        :param Query query: 查询条件
        :rtype: getKRecordList
        
    .. py:method:: getCount(self, market, code, ktype)
    
        【重载接口】（必须）获取K线数量
        
        :param str market: 市场标识
        :param str code: 证券代码
        :param Query.KType ktype: K线类型
        
    .. py:method:: _getIndexRangeByDate(self, market, code, query)
    
        【重载接口】（必须）按日期获取指定的K线数据
        
        :param str market: 市场标识
        :param str code: 证券代码
        :param Query query: 日期查询条件（QueryByDate）
    