#include <iostream>

#include "user.h"

int main() {
    // Begin Testing User
    UserList userList;
    createUserList(userList);

    registerUser(userList, "Budi", "Budianti");
    registerUser(userList, "Susi", "Susianto");
    registerUser(userList, "Cika", "Cikawanto");
    registerUser(userList, "Loki", "Lokiwanto");

    updateUserFullname(userList, "Budi", "Budianto"); // correct
    updateUserFullname(userList, "SusiAir", "SusiAiranto"); // incorrect

    updateUserAccountType(userList, "Cika", "premium");
    updateUserAccountType(userList, "Cika", "VIP");

    deleteUser(userList, "Susi");
    dumpUserList(userList);
    std::cout << "=====================================" << std::endl;

    deleteUser(userList, "Budi");
    dumpUserList(userList);
    std::cout << "=====================================" << std::endl;


    deleteUser(userList, "Loki");
    dumpUserList(userList);
    std::cout << "=====================================" << std::endl;
    
    deleteUser(userList, "Levi"); // delete Levianto

    freeUserList(userList);

    // End Testing User
    return 0;
}
