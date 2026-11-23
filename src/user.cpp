#include "user.h"

#include <iostream>

void initializeUserList(UserList& list) {
  list.first = nullptr;
}

void destroyUserList(UserList& list) {
  UserAddress temp;
  while(!isUserListEmpty(list)) {
    removeFirstUser(list, temp);
    delete temp;
  }
}

bool isUserListEmpty(UserList list) {
  return list.first == nullptr;
}

void dumpUserList(UserList list) {
  UserAddress curr;

  curr = list.first;
  while(curr != nullptr) {
    std::cout << "Username: " << curr->info.username << std::endl;
    std::cout << "Name: " << curr->info.name << std::endl;
    std::cout << "Account Type: " << curr->info.type << std::endl;
    if(curr->next != nullptr) {
      std::cout << std::endl;
    }
    curr = curr->next;
  }
}

UserAddress searchUser(UserList list, std::string username) {
  UserAddress curr;

  curr = list.first;
  while(curr != nullptr && curr->info.username != username) {
    curr = curr->next;
  }

  return curr;
}

bool doesUserExist(UserList list, std::string username) {
  return searchUser(list, username) != nullptr;
}

UserAddress createUserElement(
  std::string username,
  std::string name,
  std::string type
) {
  UserAddress new_user = new UserElement;
  new_user->info.username = username;
  new_user->info.name = name;
  new_user->info.type = type;
  new_user->next = nullptr;
  return new_user;
}

void insertUserLast(UserList& list, UserAddress user) {
  UserAddress curr;

  if(isUserListEmpty(list)) {
    list.first = user;
  } else {
    curr = list.first;
    while(curr->next != nullptr) {
      curr = curr->next;
    }
    curr->next = user;
  }
}

void removeFirstUser(UserList& list, UserAddress& out) {
  UserAddress user;

  out = nullptr;

  if(!isUserListEmpty(list)) {
    user = list.first;
    list.first = list.first->next;
    user->next = nullptr;
    out = user;
  }
}

void removeLastUser(UserList& list, UserAddress& out) {
  UserAddress prev, curr, user;

  out = nullptr;

  if(!isUserListEmpty(list)) {
    prev = nullptr;
    curr = list.first;

    while(curr->next != nullptr) {
      prev = curr;
      curr = curr->next;
    }

    if(prev == nullptr) {
      removeFirstUser(list, out);
    } else {
      user = curr;
      prev->next = curr->next;
      user->next = nullptr;
      out = user;
    }
  }
}

void removeAfterUser(UserList& list, UserAddress target, UserAddress& out) {
  UserAddress curr, user;

  out = nullptr;

  if(!isUserListEmpty(list)) {
    curr = list.first;
    while(curr != nullptr && curr != target) {
      curr = curr->next;
    }

    if(curr != nullptr) {
      user = curr->next;
      curr->next = curr->next->next;
      user->next = nullptr;
      out = user;
    }
  }
}

void registerUser(
  UserList& list,
  std::string username,
  std::string name
) {
  UserAddress new_user;

  if(!doesUserExist(list, username)) {
    new_user = createUserElement(username, name, "free");
    insertUserLast(list, new_user);
  } else {
    // TODO : Handle error message username has been used
  }
}

void upgradeUserType(UserList& list, std::string username) {
  UserAddress user;
  if(doesUserExist(list, username)) {
    user = searchUser(list, username);
    user->info.type = "premium";
  } else {
    // TODO : Handle error message user doesn't exist
  }
}

void downgradeUserType(UserList& list, std::string username) {
  UserAddress user;
  if(doesUserExist(list, username)) {
    user = searchUser(list, username);
    user->info.type = "free";
  } else {
    // TODO : Handle error message user doesn't exist
  }
}

void deactivateUser(UserList& list, std::string username) {
  UserAddress prev, curr, user;

  if(doesUserExist(list, username)) {
    prev = nullptr;
    curr = list.first;

    while(curr != nullptr && curr->info.username != username) {
      prev = curr;
      curr = curr->next;
    }

    if(prev == nullptr && curr != nullptr) {
      removeFirstUser(list, user);
    } else if(prev != nullptr && curr->next == nullptr) {
      removeLastUser(list, user);
    } else {
      removeAfterUser(list, prev, user);
    }
    delete user;
  } else {
    // TODO : Handle error message user doesn't exist
  }
}
