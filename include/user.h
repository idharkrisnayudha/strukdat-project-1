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
};

struct UserList {
  UserAddress first;
};

// Procedure initialize an empty user list.
// I.S. : The user list has been declared.
// F.S. : The first pointer is set to nullptr.
void initializeUserList(UserList& list);

// Function check whether the user list is empty.
// Returns true if the list is empty, otherwise false.
bool isUserListEmpty(UserList list);

// Procedure delete all elements from the user list.
// I.S. : The user list has been initialized.
// F.S. : All elements are deleted and the first pointer is set to nullptr.
void destroyUserList(UserList& list);

// Function find a user by their username.
// Returns the address of the matching user, or nullptr if not found.
UserAddress searchUser(UserList list, std::string username);

// Function check whether a user exists in the list.
// Returns true if the user exists, otherwise false.
bool doesUserExist(UserList list, std::string username);

// Procedure display all users stored in the list.
// I.S. : The user list has been initialized.
// F.S. : All user information is displayed without modifying the list.
void dumpUserList(UserList list);

// Function create a new user element.
// Returns the address of the newly allocated user element.
// The new element contains the given information and has next set to nullptr.
UserAddress createUserElement(
  std::string username,
  std::string name,
  std::string type
);

// Procedure insert a user element at the end of the list.
// I.S. : The user list is initialized and the new element is detached.
// F.S. : The new element is inserted at the end of the list.
void insertUserLast(UserList& list, UserAddress user);

// Procedure remove the first element from the list.
// I.S. : The user list has been initialized.
// F.S. : The first element is removed and its address is stored in out.
//        If the list is empty, out is set to nullptr.
void removeFirstUser(UserList& list, UserAddress& out);

// Procedure remove the last element from the list.
// I.S. : The user list has been initialized.
// F.S. : The last element is removed and its address is stored in out.
//        If the list is empty, out is set to nullptr.
void removeLastUser(UserList& list, UserAddress& out);

// Procedure remove the element immediately after the specified user.
// I.S. : The user list has been initialized and user belongs to the list.
// F.S. : The element after user is removed and its address is stored in out.
//        If no successor exists, out is set to nullptr.
void removeAfterUser(UserList& list, UserAddress user, UserAddress& out);

// Procedure register a new user with the default free account type.
// I.S. : The user list has been initialized.
// F.S. : A new user is inserted at the end of the list if the username
//        does not already exist. The account type is set to "free".
//        Otherwise, the list remains unchanged.
void registerUser(
  UserList& list,
  std::string username,
  std::string name
);

// Procedure upgrade an existing user's account to premium.
// I.S. : The user list has been initialized.
// F.S. : The account type is changed to "premium" if the user exists.
//        Otherwise, the list remains unchanged.
void upgradeUserType(UserList& list, std::string username);

// Procedure downgrade an existing user's account to free.
// I.S. : The user list has been initialized.
// F.S. : The account type is changed to "free" if the user exists.
//        Otherwise, the list remains unchanged.
void downgradeUserType(UserList& list, std::string username);

// Procedure deactivate a user and remove their element from the list.
// I.S. : The user list has been initialized.
// F.S. : The user element is removed and its memory is deallocated.
//        If the user does not exist, the list remains unchanged.
void deactivateUser(UserList& list, std::string username);

#endif // INCLUDE_USER_H_
