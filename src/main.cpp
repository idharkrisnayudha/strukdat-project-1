#include <iostream>

#include "user.h"

int main() {
  UserList user_list;
  initializeUserList(user_list);

  // user test
  // register
  std::cout << "User Register" << std::endl;
  registerUser(user_list, "Andi", "Andi Wicaksono");
  registerUser(user_list, "Budi", "Budi Gunawan");
  registerUser(user_list, "Cika", "Cika Yunita");
  registerUser(user_list, "Dodi", "Dodi Supriadi");
  dumpUserList(user_list);

  // edit
  std::cout << "\nUpdate User" << std::endl;
  setUserType(user_list, "Andi", "premium");
  setUserType(user_list, "Cika", "premium");
  setUserName(user_list, "Dodi", "Dodi Caknan");
  dumpUserList(user_list);

  // deactivate
  std::cout << "\nUser Deactivate" << std::endl;
  deactivateUser(user_list, "Budi");
  deactivateUser(user_list, "Andi");
  deactivateUser(user_list, "Dodi");
  dumpUserList(user_list);

  destroyUserList(user_list);
  return 0;
}
