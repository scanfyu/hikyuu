/*
 * SQLiteStatement.h
 *
 *  Copyright (c) 2019, hikyuu.org
 *
 *  Created on: 2019-7-11
 *      Author: linjinhai
 */
#pragma once
#ifndef HIKYUU_DB_CONNECT_SQLITE_SQLITESTATEMENT_H
#define HIKYUU_DB_CONNECT_SQLITE_SQLITESTATEMENT_H

#include <sqlite3.h>
#include "../SQLStatementBase.h"

namespace hku {

/**
 * SQLite Statemen
 * @ingroup SQLite
 */
class HKU_API SQLiteStatement : public SQLStatementBase {
public:
    SQLiteStatement() = delete;

    /**
     * 构造函数
     * @param driver 数据库连接
     * @param sql_statement SQL语句
     */
    SQLiteStatement(DBConnectBase* driver, const string& sql_statement);

    /** 析构函数 */
    virtual ~SQLiteStatement();

    virtual bool sub_isValid() const override;
    virtual void sub_exec() override;
    virtual bool sub_moveNext() override;

    virtual void sub_bindNull(int idx) override;
    virtual void sub_bindInt(int idx, int64_t value) override;
    virtual void sub_bindDouble(int idx, double item) override;
    virtual void sub_bindText(int idx, const string& item) override;
    virtual void sub_bindBlob(int idx, const string& item) override;

    virtual int sub_getNumColumns() const override;
    virtual void sub_getColumnAsInt64(int idx, int64_t& item) override;
    virtual void sub_getColumnAsDouble(int idx, double& item) override;
    virtual void sub_getColumnAsText(int idx, string& item) override;
    virtual void sub_getColumnAsBlob(int idx, string& item) override;

private:
    void _reset();

private:
    bool m_needs_reset;  // true if sqlite3_step() has been called more recently than
                         // sqlite3_reset()
    int m_step_status;
    bool m_at_first_step;
    sqlite3* m_db;
    sqlite3_stmt* m_stmt;
};

inline bool SQLiteStatement::sub_isValid() const {
    return m_stmt ? true : false;
}

} /* namespace hku */

#endif /* HIKYUU_DB_CONNECT_SQLITE_SQLITESTATEMENT_H */