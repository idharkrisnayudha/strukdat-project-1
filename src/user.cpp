#include <iostream>

#include "user.h"

void createUserList(UserList& list) {
    list.first = nullptr;
}

bool isUserListEmpty(UserList list) {
    return list.first == nullptr;
}

void dumpUserList(UserList list) {
    UserAddress current;

    current = list.first;
    while(current != nullptr) {
        std::cout << "Username: " << current->info.username << std::endl;
        std::cout << "Fullname: " << current->info.fullname << std::endl;
        std::cout << "Account Type: " << current->info.account_type << std::endl;
        
        if(current->next != nullptr) {
            std::cout << std::endl;
        }

        current = current->next;
    }
}

void freeUserList(UserList& list) {
    UserAddress current, temp;

    temp = nullptr;
    current = list.first;
    
    while(current != nullptr) {
        temp = current;
        current = current->next;

        delete temp;
        temp = nullptr;
    }

    if(current != nullptr) {
        delete current;
    }
    
    list.first = nullptr;
}

bool isUserExist(UserList list, std::string username) {
    return searchUser(list, username) != nullptr;
}

UserAddress searchUser(UserList list, std::string username) {
    UserAddress current, foundAt;
    
    current = list.first;
    foundAt = nullptr;

    while(current != nullptr && foundAt == nullptr) {
        if(current->info.username == username) {
            foundAt = current;
        }
        current = current->next;
    }

    return foundAt;
}

UserAddress createUser(UserInfoType user_info) {
    UserAddress newUser = new UserElement;
    newUser->info.username = user_info.username;
    newUser->info.fullname = user_info.fullname;
    newUser->info.account_type = user_info.account_type;
    newUser->next = nullptr;
    return newUser;
}

void registerUser(UserList& list, std::string username, std::string fullname) {
    UserAddress newUser, current;
    UserInfoType userInfo;

    // filling user information
    userInfo.username = username;
    userInfo.fullname = fullname;
    userInfo.account_type = "free";

    if(!isUserExist(list, username)) {
        newUser = createUser(userInfo);

        // insert user on last position
        if(isUserListEmpty(list)) {
            list.first = newUser;
        } else {
            current = list.first;

            while(current->next != nullptr) {
                current = current->next;
            }

            current->next = newUser;
        }
    } else {
        std::cout << "Username " << username << " has been used!" << std::endl;
    }
}

void updateUserFullname(UserList& list, std::string target_username, std::string new_fullname) {
    UserAddress target_user;

    if(isUserExist(list, target_username)) {
        target_user = searchUser(list, target_username);
        target_user->info.fullname = new_fullname;
    } else {
        std::cout << "Can not find " << target_username << " to change fullname!" << std::endl;
    }
}

void updateUserAccountType(UserList& list, std::string target_username, std::string account_type) {
    UserAddress target_user;

    if(isUserExist(list, target_username)) {
        if(account_type == "free" || account_type == "premium") {
            target_user = searchUser(list, target_username);
            target_user->info.account_type = account_type;
        } else {
            std::cout << "Invalid account type '" << account_type << "' to change on username " << target_username << "!" << std::endl;
        }
    } else {
        std::cout << "Can not find " << target_username << " to change fullname!" << std::endl;
    }
}

void deleteUser(UserList& list, std::string target_username) {
    UserAddress previous, current, target_user;

    if(isUserExist(list, target_username)) {
        previous = nullptr;
        current = list.first;

        while(current != nullptr && current->info.username != target_username) {
            previous = current;
            current = current->next;
        }

        target_user = current;

        // check if the element in first position
        if(previous == nullptr) {
            list.first = list.first->next;
            target_user->next = nullptr;
        }
        // not in the first position
        else {
            previous->next = target_user->next;
            target_user->next = nullptr;
        }

        delete target_user;
    } else {
        std::cout << "Can not find username " << target_username << " to delete!" << std::endl;
    }
}