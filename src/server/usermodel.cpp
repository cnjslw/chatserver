#include "usermodel.h"
#include "db.h"
#include <iostream>
using namespace std;

bool UserModel::insert(User& user)
{
    char sql[128] = { 0 };
    sprintf(sql, "insert into user(name,passwor,state) values('%s','%s','%s')",
        user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());

    MySQL mysql;
    if (mysql.connect()) {
        if (mysql.update(sql)) {
            // 返回用户主键，用作用户ID
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }

    return false;
}

User UserModel::query(int id)
{
    char sql[128] = { 0 };
    sprintf(sql, "select * from user where id = %d", id);
    MySQL mysql;
    if (mysql.connect()) {
        MYSQL_RES* res = mysql.query(sql);
        if (res != nullptr) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row != nullptr) {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }

    return User(); // 默认ID为-1
}

bool UserModel::update(User user)
{
    char sql[128] = { 0 };
    sprintf(sql, "update user set state = %s where id = %d", user.getState().c_str(), user.getId());
    MySQL mysql;
    if (mysql.connect()) {
        if (mysql.update(sql))
            return true;
    }
    return false;
}

void UserModel::resetState()
{
    char sql[128] = "update user set state = 'offline' where state='online' ";
    MySQL mysql;
    if (mysql.connect()) {
        mysql.update(sql);
    }
}
