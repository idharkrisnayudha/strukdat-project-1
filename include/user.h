#ifndef INCLUDE_USER_H_
#define INCLUDE_USER_H_

// NOTE: Implementasi user menggunakan double linked list

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
  UserAddress prev;
};

struct UserList {
  UserAddress first;
  UserAddress last;
};



/**
 * Menginisialisasi user list
 * I.S : sebuah user list
 * F.S : element first dan last menjadi nullptr
 */
void initializeUserList(UserList& list);

/**
 * Menghancurkan user list
 * I.S : sebuah user list
 * F.S : semua user pada list akan dilepaskan dan dihancurkan
 */
void destroyUserList(UserList& list);

/**
 * Menampilkan semua user pada list
 * I.S : sebuah user list
 * F.S : menampilkan informasi untuk setiap user pada list
 */
void dumpUserList(UserList list);

/**
 * Mengecek apakah user list kosong atau tidak dimana akan memberikan
 * hasil "true" jika kosong dan "false" jika tidak kosong
 */
bool isUserListEmpty(UserList& list);

/**
 * Mencari address user yang diingikan berdasarkan username dari list
 * dan akan memberikan address yang dituju jika user ditemukan
 * dan "nullptr" jika tidak ditemukan.
 */
UserAddress searchUser(UserList list, std::string username);

/**
 * Mengecek apakah user ada pada list dimana akan memberikan hasil
 * "true" jika ditemukan dan "false" jika tidak ditemukan
 */
bool doesUserExist(UserList list, std::string username);

/**
 * Membuat user element baru dan akan memberikan hasil dalam
 * UserAddress.
 */
UserAddress createUserElement(std::string username, std::string name, std::string type);

/**
 * Mengubah tipe pada sebuah user
 * I.S : user list, username, dan tipe baru ("free" atau "premium")
 * F.S : tipe akun user dengan username yang ditargetkan akan berubah sesuai
 *       dengan tipe yang diberikan
 */
void setUserType(UserList& list, std::string username, std::string new_type);

/**
 * Mengubah nama pada sebuah user
 * I.S : user list, username, dan nama baru
 * F.S : nama pada sebuah user yang ditargetkan akan berubah sesuai dengan
 *       nama yang diberikan
 */
void setUserName(UserList& list, std::string username, std::string new_name);

/**
 * Menambahkan user baru pada list
 * I.S : user list, username (unique), dan nama
 * F.S : user baru akan masuk ke list dengan tipe akun default "free" dan
 *       jika username sudah digunakan maka proses akan dibatalkan
 */
void registerUser(UserList& list, std::string username, std::string name);

/**
 * Menghapus user pada list
 * I.S : user list dan username
 * F.S : user yang ditargetkan akan dihapus dari list dan dihancurkan
 */
void deactivateUser(UserList& list, std::string username);

/**
 * Menambahkan user element baru ke posisi terdepan list
 * I.S : user list dan user element baru
 * F.S : user element ditambahkan ke posisi yang paling di depan
 */
void insertUserFirst(UserList& list, UserAddress new_user);

/**
 * Menambahkan user element baru ke posisi belakang list
 * I.S : user list dan user element baru
 * F.S : user element ditambahkan ke posisi yang paling belakang
 */
void insertUserLast(UserList& list, UserAddress new_user);

/**
 * Menambahkan user element baru setelah user yang ditargetkan
 * I.S : user list dan user element baru
 * F.S : user element ditambahkan setelah user yang ditargetkan
 */
void insertUserAfter(UserList& list, UserAddress target, UserAddress new_user);

/**
 * Mengeluarkan user di posisi pertama dari list
 * I.S : user list dan "refrensi" ke lokasi user
 * F.S : user di posisi pertama sudah dikeluarkan dan diberikan
 *       melalui "refrenrensi" user
 */
void removeFirstUser(UserList& list, UserAddress& out);

/**
 * Mengeluarkan user di posisi paling belakang list
 * I.S : user list dan "refrensi" ke lokasi user
 * F.S : user di posisi paling belakang sudah dikeluarkan dan diberikan
 *       melalui "refrenrensi" user
 */
void removeLastUser(UserList& list, UserAddress& out);

/**
 * Mengeluarkan user dari list dimana posisinya terletak setelah user
 * yang ditargetkan
 * I.S : user list, user yang ditargetkan, dan "renfrensi" ke lokasi user
 * F.S : user dengan posisi setelah dari user yang ditargetkan sudah di keluarkan
 *       dari list
 */
void removeAfterUser(UserList& list, UserAddress target, UserAddress& out);

#endif // INCLUDE_USER_H_
