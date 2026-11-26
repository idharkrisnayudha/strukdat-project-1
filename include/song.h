#ifndef INCLUDE_SONG_H
#define INCLUDE_SONG_H

#include <string>

struct SongElement;
typedef struct SongElement *SongAddress;
=========
// NOTE: Implementasi song menggunakan binary tree
// NOTE: Implementasi binary tree akan disimiliarkan dengan SLL dan DLL

#include <string>

struct SongNode;
typedef SongNode* SongAddress;

struct SongInfoType {
  std::string id;
  std::string title;
  std::string artist;
  int duration;
};

struct SongNode {
  SongInfoType info;
  SongAddress left;
  SongAddress right;
};

struct SongList {
  SongAddress node;
};

/**
 * Menginisialisasi song list
 * I.S : sebuah song list
 * F.S : element node pada song list menjadi "nullptr"
 */
void initializeSongList(SongList& list);

/**
 * Menampilkan informasi song di node dan berlanjut hingga childern yang
 * dimiliki
 * I.S : sebuah song node
 * F.S : menampilkan semua informasi song di node dan childernnya
 */
void dumpSongNode(SongAddress node);

/**
 * Menampilkan informasi song pada semua song di list
 * I.S : sebuah song list
 * F.S : menampilkan semua informasi song di song list
 */
void dumpSongList(SongList& list);

/**
 * Mengecek apakah song list kosong atau tidak dan akan mengembalikan
 * "true" jika kosong dan "false" jika tidak kosong.
 */
bool isSongListEmpty(SongList list);

/**
 * Mencari song pada node tree dan akan mengembalikan address song
 * jika ditemukan dan "nullptr" jika tidak ditemukan.
 */
SongAddress findSongNode(SongAddress node, std::string id);

/**
 * Mengecek apakah song ada di list dan akan mengembalikan "true"
 * jika ditemukan dan "false" jika tidak ditemukan.
 */
bool doesSongExist(SongList& list, std::string id);

/**
 * Mencari song pada list dan akan mengembalikan address song jika
 * ditemukan dan "nullptr" jika tidak ditemukan.
 */
SongAddress searchSong(SongList list, std::string id);

/**
 * Membuat song node dari informasi yang diberikan dan akan mengembalikannya
 * sebagai song address.
 */
SongAddress createSongNode(std::string id, std::string title, std::string artist, int duration);

/**
 * Menambahkan song node baru di node tree
 * I.S : song node dan song node baru
 * F.S : menambahkan song node baru ke parent atau root, dimana
 *       jika parent akan diisi bila childern masih "nullptr"
 *       dengan aturan kiri untuk id song baru lebih kecil
 *       dibandingkan id parent, dan kanan jika lebih besar.
 */
void insertSongNode(SongAddress& node, SongAddress new_song);

/**
 * Menambahkan song baru ke list
 * I.S : song list, id, judul, artist, dan durasi
 * F.S : song baru telah ditambahkan ke list
 */
void addSong(SongList& list, std::string id, std::string title, std::string artist, int duration);




#endif // INCLUDE_SONG_H_
