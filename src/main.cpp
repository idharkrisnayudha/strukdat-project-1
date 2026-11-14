#include <iostream>

#include "user.h"

int main() {
  UserList user_list;
  UserAddress user_address;

  createUserList(user_list);

  registeUser(user_list, "budi", "Budianto", "premium");

  user_address = user_list.first;
  while(user_address != nullptr) {
    std::cout << "Username: " << user_address->info.username << std::endl;
    std::cout << "Name: " << user_address->info.name << std::endl;
    std::cout << "Account Type: " << user_address->info.account_type << std::endl;
    if(user_address->next != nullptr) {
      std::cout << std::endl;
    }
    user_address = user_address->next;
  }

  deactivateUser(user_list, "budi");

  user_address = user_list.first;
  while(user_address != nullptr) {
    std::cout << "Username: " << user_address->info.username << std::endl;
    std::cout << "Name: " << user_address->info.name << std::endl;
    std::cout << "Account Type: " << user_address->info.account_type << std::endl;
    if(user_address->next != nullptr) {
      std::cout << std::endl;
    }
    user_address = user_address->next;
  }

  return 0;
}
