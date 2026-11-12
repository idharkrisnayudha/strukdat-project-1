#ifndef MLL_H_
#define MLL_H_

#include <string>

// Bagian pengguna
struct Elm_Pengguna;
struct Infotype_Pengguna {
    std::string username;
    std::string nama;
    std::string tipe_akun;
};

typedef Elm_Pengguna* Addr_Pengguna;

struct Elm_Pengguna {
    Infotype_Pengguna info;
    Addr_Pengguna next;
};

struct List_Pengguna {
    Addr_Pengguna first;
};

void buat_list_pengguna(List_Pengguna& L);
bool check_list_pengguna_kosong(List_Pengguna L);
Addr_Pengguna register_pengguna(std::string username, std::string name, std::string tipe_akun);
void tambah_pengguna(List_Pengguna& L, Addr_Pengguna p);
Addr_Pengguna cari_pengguna(List_Pengguna L, std::string username);
void hapus_pengguna(List_Pengguna& L, Addr_Pengguna p, Addr_Pengguna& out);

// Bagian lagu
struct Elm_Lagu;
struct Infotype_Lagu {
    std::string id;
    std::string judul;
    std::string penyanyi;
    std::string genre;
    int durasi;
};

typedef Elm_Lagu* Addr_Lagu;

struct Elm_Lagu {
    Infotype_Lagu info;
    Addr_Lagu next;
};

struct List_Lagu {
    Addr_Lagu first;
};

// void buat_list_lagu(List_Pengguna& L);
// void tambah_lagu(List_Pengguna& L, Addr_Pengguna p);
// Addr_Pengguna cari_lagu(List_Pengguna& L, std::string id);
// void hapus_lagu(List_Pengguna& L, Addr_Pengguna p);

#endif // MLL_H_