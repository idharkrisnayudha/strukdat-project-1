#include "user.h"

#include <iostream>

void initializeUserList(UserList& list) {
  list.first = nullptr;
  list.last = nullptr;
}

void destroyUserList(UserList& list) {
  UserAddress temp;

  if(!isUserListEmpty(list)) {
    removeFirstUser(list, temp);
    delete temp;
  }
}

void dumpUserList(UserList list) {
  UserAddress current;

  current = list.first;
  while(current != nullptr) {
    std::cout << "Username: " << current->info.username << std::endl;
    std::cout << "Name: " << current->info.name << std::endl;
    std::cout << "Account Type: " << current->info.account_type << std::endl;

    if(current->next != nullptr) {
      std::cout << std::endl;
    }

    current = current->next;
  }
}

bool isUserListEmpty(UserList& list) {
  return list.first == nullptr && list.last == nullptr;
}

UserAddress searchUser(UserList list, std::string username) {
  UserAddress current_user;

  current_user = list.first;
  while(current_user != nullptr && current_user->info.username != username) {
    current_user = current_user->next;
  }

  return current_user;
}

bool doesUserExist(UserList list, std::string username) {
  return searchUser(list, username) != nullptr;
}

UserAddress createUserElement(std::string username, std::string name, std::string type) {
  UserAddress new_user = new UserElement;
  new_user->info.username = username;
  new_user->info.name = name;
  new_user->info.account_type = type;
  new_user->next = nullptr;
  new_user->prev = nullptr;
  return new_user;
}

void setUserType(UserList& list, std::string username, std::string new_type) {
  UserAddress user;

  if(doesUserExist(list, username)) {
    if(new_type == "free" || new_type == "premium") {
      user = searchUser(list, username);
      user->info.account_type = new_type;
    } else {
      // TODO: tampilkan pesan error 'new_type' tidak valid
    }
  } else {
    // TODO: tampilkan pesan error user dengan 'username' tidak ditemukan!
  }
}

void setUserName(UserList& list, std::string username, std::string new_name) {
  UserAddress user;

  if(doesUserExist(list, username)) {
    user = searchUser(list, username);
    user->info.name = new_name;
  } else {
    // TODO: tampilkan pesan error user dengan 'username' tidak ditemukan!
  }
}

void registerUser(UserList& list, std::string username, std::string name) {
  UserAddress new_user;

  if(!doesUserExist(list, username)) {
    new_user = createUserElement(username, name, "free");
    insertUserLast(list, new_user);
  } else {
    // TODO : tampilkan pesan error "username sudah digunakan!"
  }
}

void deactivateUser(UserList& list, std::string username) {
  UserAddress user, temp;

  if(doesUserExist(list, username)) {
    user = searchUser(list, username);
    if(user == list.first) {
      removeFirstUser(list, temp);
    } else if(user == list.last) {
      removeLastUser(list, temp);
    } else {
      removeAfterUser(list, user->prev, temp);
    }

    delete temp;
  } else {
    // TODO: tampilkan pesan error "username '<username>' tidak ditemukan!"
  }
}

void insertUserFirst(UserList& list, UserAddress new_user) {
  if(isUserListEmpty(list)) {
    list.first = new_user;
    list.last = new_user;
  } else {
    list.first->prev = new_user;
    new_user->next = list.first;
    list.first = new_user;
  }
}

void insertUserLast(UserList& list, UserAddress new_user) {
  if(isUserListEmpty(list)) {
    list.first = new_user;
    list.last = new_user;
  } else {
    list.last->next = new_user;
    new_user->prev = list.last;
    list.last = new_user;
  }
}

void insertUserAfter(UserList& list, UserAddress target, UserAddress new_user) {
  if(target != nullptr) {
    if(target == list.last) {
      insertUserLast(list, new_user);
    } else {
      target->next->prev = new_user;
      new_user->next = target->next;
      new_user->prev = target;
      target->next = new_user;
    }
  }
}

void removeFirstUser(UserList& list, UserAddress& out) {
  UserAddress user;

  if(!isUserListEmpty(list)) {
    user = list.first;

    if(list.first == list.last) {
      list.first = nullptr;
      list.last = nullptr;
    } else {
      list.first = list.first->next;
    }

    user->next = nullptr;
    out = user;
  }
}

void removeLastUser(UserList& list, UserAddress& out) {
  UserAddress user;

  if(!isUserListEmpty(list)) {
    user = list.last;

    if(list.first == list.last) {
      list.first = nullptr;
      list.last = nullptr;
    } else {
      list.last = list.last->prev;
      list.last->next = nullptr;
    }

    user->prev = nullptr;
    out = user;
  }
}

void removeAfterUser(UserList& list, UserAddress target, UserAddress& out) {
  UserAddress user;

  if(!isUserListEmpty(list) && target != nullptr && target->next != nullptr) {
    user = target->next;

    if(target->next == list.last) {
      list.last = target;
      target->next = nullptr;
    } else {
      target->next = target->next->next;
    }

    user->next = nullptr;
    user->prev = nullptr;
    out = user;
  }
}
