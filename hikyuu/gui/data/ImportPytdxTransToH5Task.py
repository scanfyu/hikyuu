# coding:utf-8
#
# The MIT License (MIT)
#
# Copyright (c) 2010-2017 fasiondog/hikyuu
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import logging
import sqlite3
from pytdx.hq import TdxHq_API
from hikyuu.data.pytdx_to_h5 import import_data, import_trans


class ProgressBar:
    def __init__(self, src):
        self.src = src

    def __call__(self, cur, total):
        self.src.queue.put(
            [self.src.task_name, self.src.market, 'TRANS', (cur + 1) * 100 // total, 0]
        )


class ImportPytdxTransToH5:
    def __init__(self, queue, sqlitefile, market, quotations, ip, port, dest_dir, max_days):
        self.logger = logging.getLogger(self.__class__.__name__)
        self.task_name = 'IMPORT_TRANS'
        self.queue = queue
        self.sqlitefile = sqlitefile
        self.market = market
        self.quotations = quotations
        self.ip = ip
        self.port = port
        self.dest_dir = dest_dir
        self.max_days = int(max_days)

    def __call__(self):
        count = 0
        connect = sqlite3.connect(self.sqlitefile, timeout=1800)
        try:

            progress = ProgressBar(self)
            api = TdxHq_API()
            api.connect(self.ip, self.port)
            count = import_trans(
                connect,
                self.market,
                self.quotations,
                api,
                self.dest_dir,
                max_days=self.max_days,
                progress=progress
            )
        except Exception as e:
            self.logger.error(e)
        finally:
            connect.commit()
            connect.close()

        self.queue.put([self.task_name, self.market, 'TRANS', None, count])
