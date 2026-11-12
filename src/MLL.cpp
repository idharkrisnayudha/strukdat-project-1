#include <iostream>

#include "MLL.h"

// Pengguna
void buat_list_pengguna(List_Pengguna& L) {
    L.first = nullptr;
}

bool check_list_pengguna_kosong(List_Pengguna L) {
    return L.first == nullptr;
}

Addr_Pengguna register_pengguna(std::string username, std::string name, std::string tipe_akun) {
    Addr_Pengguna elm_pengguna_baru = new Elm_Pengguna;
    elm_pengguna_baru->info.username = username;
    elm_pengguna_baru->info.nama = name;
    elm_pengguna_baru->info.tipe_akun = tipe_akun;
    elm_pengguna_baru->next = nullptr;
    return elm_pengguna_baru;
}

void tambah_pengguna(List_Pengguna& L, Addr_Pengguna p) {
    Addr_Pengguna q;

    if(cari_pengguna(L, p->info.username) == nullptr) {
        if(check_list_pengguna_kosong(L)) {
            L.first = p;
        } else {
            q = L.first;
            
            while(q->next != nullptr) {
                q = q->next;
            }

            q->next = p;
        }
    } else {
        std::cout << "Username " << p->info.username << " sudah dipakai, gunakan username lain!" << std::endl;
    }
}

Addr_Pengguna cari_pengguna(List_Pengguna L, std::string username) {
    Addr_Pengguna p, addr_pengguna;

    p = L.first;
    addr_pengguna = nullptr;

    while(p != nullptr && addr_pengguna == nullptr) {
        if(p->info.username == username) {
            addr_pengguna = p;
        }
        p = p->next;
    }

    return addr_pengguna;
}

void hapus_pengguna(List_Pengguna& L, Addr_Pengguna p, Addr_Pengguna& out) {
    Addr_Pengguna prev, q;

    q = L.first;
    out = nullptr;
    prev = nullptr;

    while(q != nullptr && p != nullptr && q != p) {
        prev = q;
        q = q->next;
    }

    if(q == p && p != nullptr) {
        if(prev == nullptr) {
            out = L.first;
            L.first = L.first->next;
            out->next = nullptr;
        } else {
            out = prev->next;
            prev->next = prev->next->next;
            out->next = nullptr;
        }
    } else {
        std::cout << "Tidak dapat menghapus pengguna " << p->info.username << "!" << std::endl;
    }
}