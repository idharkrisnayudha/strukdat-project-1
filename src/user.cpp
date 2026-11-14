#include "user.h"

#include <iostream>

void createUserList(UserList& list) {
  list.first = nullptr;
}

bool isUserListEmpty(UserList list) {
  return list.first == nullptr;
}

UserAddress searchUserByUsername(UserList list, std::string username) {
  UserAddress current_position;
  UserAddress user_position;

  user_position = nullptr;
  current_position = list.first;

  while(current_position != nullptr && user_position == nullptr) {
    if(current_position->info.username == username) {
      user_position = current_position;
    }
    current_position = current_position->next;
  }

  return current_position;
}

bool isUserExistByUsername(UserList list, std::string username) {
  return searchUserByUsername(list, username) != nullptr;
}

UserAddress createUserElement(std::string username, std::string name, std::string account_type) {
  UserAddress newUserElement = new UserElement;
  newUserElement->info.username = username;
  newUserElement->info.name = name;
  newUserElement->info.account_type = account_type;
  newUserElement->next = nullptr;
  return newUserElement;
}

void registeUser(UserList& list, std::string username, std::string name, std::string account_type) {
  UserAddress current_position;
  UserAddress new_user;

  if(!isUserExistByUsername(list, username)) {
    new_user = createUserElement(username, name, account_type);

    if(isUserListEmpty(list)) {
      list.first = new_user;
    } else {
      current_position = list.first;

      while(current_position->next != nullptr) {
        current_position = current_position->next;
      }

      current_position->next = new_user;
    }
  } else {
    std::cout << "Tidak dapat melakukan register karena username '" << username << "' telah digunakan!" << std::endl;
  }
}

void removeUser(UserList& list, std::string username, UserAddress& user_removed) {
  UserAddress previous_position, current_position;
  UserAddress targeted_user;

  user_removed = nullptr;
  previous_position = nullptr;
  current_position = list.first;
  
  while(current_position != nullptr && current_position->info.username != username) {
    previous_position = current_position;
    current_position = current_position->next;
  }

  if(current_position != nullptr) {
    if(previous_position == nullptr) {
      targeted_user = list.first;
      list.first = list.first->next;

      targeted_user->next = nullptr;
      user_removed = targeted_user;
    } else {
      targeted_user = current_position;
      previous_position = current_position->next;

      targeted_user->next = nullptr;
      user_removed = targeted_user;
    }
  }
}

void deactivateUser(UserList& list, std::string username) {
  UserAddress user_target;

  removeUser(list, username, user_target);

  if(user_target != nullptr) {
    delete user_target;
    user_target = nullptr;
  } else {
    std::cout << "Tidak bisa deaktivasi user karena user '" << username << "' tidak ditemukan!" << std::endl;
  }
}