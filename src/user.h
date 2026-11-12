#ifndef USER_H_
#define USER_H_

#include <string>

struct UserElement;
struct UserInfoType {
    std::string username;
    std::string fullname;
    std::string account_type;
};

typedef UserElement* UserAddress;

struct UserElement {
    UserInfoType info;
    UserAddress next;
};

struct UserList {
    UserAddress first;
};

void createUserList(UserList& list);
bool isUserListEmpty(UserList list);
void dumpUserList(UserList list);
void freeUserList(UserList& list);

bool isUserExist(UserList list, std::string username);
UserAddress searchUser(UserList list, std::string username);

UserAddress createUser(UserInfoType user_info);
void registerUser(UserList& list, std::string username, std::string fullname);
void updateUserFullname(UserList& list, std::string target_username, std::string new_fullname);
void updateUserAccountType(UserList& list, std::string target_username, std::string account_type);
void deleteUser(UserList& list, std::string target_username);

#endif // USER_H_