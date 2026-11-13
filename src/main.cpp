#include <iostream>

#include "user.h"

int main() {
  UserList user_list;
  createUserList(user_list);

  std::cout << "User list terinisialisasi" << std::endl;

  return 0;
}
