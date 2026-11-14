#ifndef INCLUDE_USER_H_
#define INCLUDE_USER_H_

#include <string>

struct UserElement;
typedef UserElement* UserAddress;

struct UserInfoType {
  std::string username;
  std::string name;
  std::string account_type;
};

struct UserElement {
  UserInfoType info;
  UserAddress next;
};

struct UserList {
  UserAddress first;
};

/**
 * Procedure berguna untuk menginisialisasi
 * UserList agar element first menjadi nullptr
 * I.S: sebuah refrence ke UserList
 * F.S: element first pada UserList menjadi nullptr
 */
void createUserList(UserList& list);

/**
 * Sebuah function yang berguna untuk mengecek
 * apakah sebuah UserList kosong atau tidak.
 * mengembalikan true jika kosong, dan
 * false jika tidak.
 */
bool isUserListEmpty(UserList list);

/**
 * Sebuah function yang berguna untuk mencari
 * address user dari sebuah username yang
 * diberikan. Dan akan memberikan address
 * jika ada, dan nullptr jika tidak ada.
 */
UserAddress searchUserByUsername(UserList list, std::string username);

/**
 * Sebuah function yang berguna untuk mengecek
 * apakah sebuah user itu ada atau tidak. Dan
 * memberikan hasil true jika ada, dan false
 * jika tidak.
 */
bool isUserExistByUsername(UserList list, std::string username);

/**
 * Sebuah function yang berguna untuk membuat
 * UserElement dan akan mengembalikannya
 * sebagai UserAddress.
 */
UserAddress createUserElement(std::string username, std::string name, std::string account_type);

/**
 * Sebuah procedure yang berguna untuk membuat
 * UserElement jika username belum digunakan,
 * dan melakukan insert ke bagian terakhir
 * list.
 * I.S: refrence UserList, username, name, dan account_type
 * F.S: bertambahnya user baru jika username belum terpakai
 */
void registeUser(UserList& list, std::string username, std::string name, std::string account_type);

/**
 * Sebuah procedure yang berguna untuk mengeluarkan
 * user dari list, dan mengembalikan UserAddress
 * untuk digunakan atau dihancurkan
 * I.S: refrence UserList, username, dan refrence UserAddress
 * F.S: Mengeluarkan user dari list, dan mengembalikan user lewat user_removed
 */
void removeUser(UserList& list, std::string username, UserAddress& user_removed);

/**
 * Sebuah procedure yang berguna untuk mengeluarkan
 * user dari list, dan mendealokasikan user
 * I.S: refrence UserList dan username
 * F.S: Mengeluarkan user dari list, dan mendealokasikan user
 */
void deactivateUser(UserList& list, std::string username);

#endif // INCLUDE_USER_H_