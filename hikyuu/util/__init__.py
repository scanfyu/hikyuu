#!/usr/bin/python
# -*- coding: utf8 -*-
# cp936

#===============================================================================
# 作者：fasiondog
# 历史：1）20090527, Added by fasiondog
#===============================================================================

#from singleton import Singleton

from .mylog import *
from .check import *

__all__ = [
    'spend_time',
    'hku_logger',
    'class_logger',
    'add_class_logger_handler',
    'HKUCheckError',
    'hku_check',
    'hku_check_throw',
    'hku_check_ignore',
    'hku_catch',
    'hku_trace',
    'hku_debug',
    'hku_info',
    'hku_warn',
    'hku_error',
    'hku_fatal',
    'hku_trace_if',
    'hku_debug_if',
    'hku_info_if',
    'hku_warn_if',
    'hku_info_if',
    'hku_warn_if',
    'hku_error_if',
    'hku_fatal_if',
    'with_trace',
]
