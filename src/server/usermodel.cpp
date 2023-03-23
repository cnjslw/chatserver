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