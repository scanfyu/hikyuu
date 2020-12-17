.. py:currentmodule:: hikyuu.trade_sys
.. highlightlang:: python

系统框架
=============

系统是指针对针对单个交易对象的完整策略，包括环境判断、系统有效条件、资金管理、止损、止盈、盈利目标、移滑价差的完整策略，用于模拟回测。

公共参数：

    * **delay=True** *(bool)* : 是否延迟到下一个bar开盘时进行交易
    * **delay_use_current_price=True** *(bool)* : 延迟操作的情况下，是使用当前交易时bar的价格计算新的止损价/止赢价/目标价还是使用上次计算的结果
    * **max_delay_count=3** *(int)* : 连续延迟交易请求的限制次数，应大于等于0，0表示只允许延迟1次
    * **tp_monotonic=True** *(bool)* : 止赢单调递增
    * **tp_delay_n=3** *(int)* : 止盈延迟开始的天数，即止盈策略判断从实际交易几天后开始生效
    * **ignore_sell_sg=False** *(bool)* : 忽略卖出信号，只使用止损/止赢等其他方式卖出
    * **ev_open_position=False** *(bool)*: 是否使用市场环境判定进行初始建仓
    * **cn_open_position=False** *(bool)*: 是否使用系统有效性条件进行初始建仓
    
    
创建系统并执行回测
-----------------------

.. py:function:: SYS_Simple([tm=None, mm=None, ev=None, cn=None, sg=None, st=None, tp=None, pg=None, sp=None])

    创建简单系统实例（每次交易不进行多次加仓或减仓，即每次买入后在卖出时全部卖出），  系统实例在运行时(调用run方法），至少需要一个配套的交易管理实例、一个资金管理策略
    和一个信号指示器），可以在创建系统实例后进行指定。如果出现调用run时没有任何输出，
    且没有正确结果的时候，可能是未设置tm、sg、mm。进行回测时，使用 run 方法，如::
    
        #创建模拟交易账户进行回测，初始资金30万
        my_tm = crtTM(initCash = 300000)

        #创建信号指示器（以5日EMA为快线，5日EMA自身的10日EMA最为慢线，快线向上穿越慢线时买入，反之卖出）
        my_sg = SG_Flex(OP(EMA(n=5)), slow_n=10)

        #固定每次买入1000股
        my_mm = MM_FixedCount(1000)

        #创建交易系统并运行
        sys = SYS_Simple(tm = my_tm, sg = my_sg, mm = my_mm)
        sys.run(sm['sz000001'], Query(-150))
    
    :param TradeManager tm: 交易管理实例 
    :param MoneyManager mm: 资金管理策略
    :param EnvironmentBase ev: 市场环境判断策略
    :param ConditionBase cn: 系统有效条件
    :param SignalBase sg: 信号指示器
    :param StoplossBase st: 止损策略
    :param StoplossBase tp: 止盈策略
    :param ProfitGoalBase pg: 盈利目标策略
    :param SlippageBase sp: 移滑价差算法
    :return: system实例

    
    
系统部件枚举定义
------------------

.. py:class:: System.Part

    系统部件枚举值，系统的买入/卖出等操作可由这些部件触发，用于标识实际交易指令的来源，参见：:py:class:`TradeRecord`。
    
    实际使用中，可使用 System.ENVIRONMENT 的简化方式 代替 System.Part.ENVIRONMENT，其他与此类似。

    - System.Part.ENVIRONMENT  - 市场环境判断策略
    - System.Part.CONDITION    - 系统有效条件
    - System.Part.SIGNAL       - 信号指示器
    - System.Part.STOPLOSS     - 止损策略
    - System.Part.TAKEPROFIT   - 止盈策略
    - System.Part.MONEYMANAGER - 资金管理策略
    - System.Part.PROFITGOAL   - 盈利目标策略
    - System.Part.SLIPPAGE     - 移滑价差算法
    - System.Part.INVALID      - 无效值边界，大于等于该值时为无效部件

    
.. py:function:: get_system_part_name(part)

    获取部件的字符串名称
    
        - System.Part.ENVIRONMENT  - "EV"
        - System.Part.CONDITION    - "CN"
        - System.Part.SIGNAL       - "SG"
        - System.Part.STOPLOSS     - "ST"
        - System.Part.TAKEPROFIT   - "TP"
        - System.Part.MONEYMANAGER - "MM"
        - System.Part.PROFITGOAL   - "PG"
        - System.Part.SLIPPAGE     - "SP"
        - System.Part.INVALID      - "--"

    :param int part: System.Part 枚举值
    :rtype: str
    

.. py:function:: get_system_part_enum(part_name)

     根据系统部件的字符串名称获取相应的枚举值

    :param str part_name: 系统部件的字符串名称，参见：:py:func:`getSystemPartName`
    :rtype: System.Part


    
系统基类定义
-------------

.. py:class:: System

    系统基类。需要扩展或实现更复杂的系统交易行为，可从此类继承。
    
    .. py:attribute:: name  
    
        系统名称
    
    .. py:attribute:: tm  
    
        关联的交易管理实例
        
    .. py:attribute:: mm  
    
        资金管理策略
        
    .. py:attribute:: ev  
    
        市场环境判断策略
        
    .. py:attribute:: cn  
    
        系统有效条件
        
    .. py:attribute:: sg

        信号指示器
        
    .. py:attribute:: st
    
        止损策略
        
    .. py:attribute:: tp
    
        止盈策略
        
    .. py:attribute:: pg
    
        盈利目标策略
        
    .. py:attribute:: sp
    
        移滑价差算法

    .. py:method:: get_param(self, name)

        获取指定的参数
    
        :param str name: 参数名称
        :return: 参数值
        :raises out_of_range: 无此参数
        
    .. py:method:: set_param(self, name, value)
    
        设置参数
        
        :param str name: 参数名称
        :param value: 参数值
        :type value: int | bool | float | string
        :raises logic_error: Unsupported type! 不支持的参数类型
                
    .. py:method:: get_stock(self)
    
        获取关联的证券
        
        :rtype: Stock
        
    .. py:method:: get_trade_record_list(self)
    
        获取交易记录
        
        :rtype: TradeRecordList
        
    .. py:method:: get_buy_trade_request(self)
    
        获取买入请求，“delay”模式下查看下一时刻是否存在买入操作
        
        :rtype: TradeRequest

    .. py:method:: get_sell_trade_request(self)
    
        获取卖出请求，“delay”模式下查看下一时刻是否存在卖出操作
        
        :rtype: TradeRequest
                
    .. py:function:: run(self, stock, query[, reset=True])
    
        运行系统，执行回测
        
        :param Stock stock: 交易的证券
        :param Query query: K线数据查询条件
        :param bool reset: 是否同时复位所有组件，尤其是tm实例
        
    .. py:method:: reset(self, with_tm, with_ev)
    
        复位操作。TM、EV是和具体系统无关的策略组件，可以在不同的系统中进行共享，复位将引起系统运行时被重新清空并计算。尤其是在共享TM时需要注意！
        
        :param bool with_tm: 是否复位TM组件
        :param bool with_ev: 是否复位EV组件
        
    .. py:method:: clone(self)
    
        克隆操作。

        
        
交易请求记录
--------------

.. py:class:: TradeRequest

    交易请求记录。系统内部在实现延迟操作时登记的交易请求信息。暴露该结构的主要目的是用于在“delay”模式（延迟到下一个bar开盘时进行交易）的情况下，系统实际已知下一个Bar将要进行交易，此时可通过 :py:meth:`System.getBuyTradeRequest` 、 :py:meth:`System.getSellTradeRequest` 来获知下一个BAR是否需要买入/卖出。主要用于提醒或打印下一个Bar需要进行操作。对于系统本身的运行没有影响。
    
    .. py:attribute:: valid 
        
        该交易请求记录是否有效（True | False）
    
    .. py:attribute:: business
    
        交易业务类型，参见：:py:class:`hikyuu.trade_manage.BUSINESS`
    
    .. py:attribute:: datetime
    
        发出交易请求的时刻
    
    .. py:attribute:: stoploss
    
        发出交易请求时刻的止损价
    
    .. py:attribute:: part
    
        发出交易请求的来源，参见：:py:class:`System.Part`
    
    .. py:attribute:: count
    
        因操作失败，连续延迟的次数
