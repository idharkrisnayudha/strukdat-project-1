#ifndef INCLUDE_USER_H_
#define INCLUDE_USER_H_

#include <string>

struct UserElement;
using UserAddress = UserElement*;

struct UserInfoType {
  std::string username;
  std::string name;
  std::string type;
};

struct UserElement {
  UserInfoType info;
  UserAddress next;
  UserAddress prev;
};

struct UserList {
  UserAddress first;
  UserAddress last;
};

void initializeUserList(UserList& list);

void destroyUserList(UserList& list);

void dumpUserList(UserList list);

bool isUserListEmpty(UserList& list);

UserAddress searchUser(UserList list, std::string username);

bool doesUserExist(UserList list, std::string username);

UserAddress createUserElement(std::string username, std::string name, std::string type);

void setUserType(UserList& list, std::string username, std::string new_type);

void setUserName(UserList& list, std::string username, std::string new_name);

void registerUser(UserList& list, std::string username, std::string name);

void deactivateUser(UserList& list, std::string username);

void insertUserFirst(UserList& list, UserAddress new_user);

void insertUserLast(UserList& list, UserAddress new_user);

void insertUserAfter(UserList& list, UserAddress target, UserAddress new_user);

void removeFirstUser(UserList& list, UserAddress& out);

void removeLastUser(UserList& list, UserAddress& out);

void removeAfterUser(UserList& list, UserAddress target, UserAddress& out);

#endif // INCLUDE_USER_H_
