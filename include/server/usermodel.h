#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.h"

// User表的数据操作
class UserModel {
public:
    bool insert(User& user);

    // 根据用户ID,查询用户信息
    User query(int id);

    // 更新用户的状态信息
    bool update(User user);

    // 重置状态信息
    void resetState();

private:
};

#endif