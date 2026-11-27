#ifndef INCLUDE_GENRE_H
#define INCLUDE_GENRE_H

#include "song.h"
#include <string>

struct GenreElement;
typedef GenreElement* GenreAddress;

struct GenreInfoType {
  std::string name;
};

struct GenreElement {
  GenreInfoType info;
  GenreAddress next;
};

struct GenreList {
  GenreAddress first;
};

/**
 * Menginisialisasi genre list
 * I.S : sebuah genre list
 * F.S : element first menjadi nullptr
 */
void initializeGenreList(GenreList& list);

/**
 * Menghancurkan genre list
 * I.S : sebuah genre list
 * F.S : semua genre pada list akan dilepaskan dan dihancurkan
 */
void destroyGenreList(GenreList& list);

/**
 * Menampilkan semua genre pada list
 * I.S : sebuah genre list
 * F.S : menampilkan informasi untuk setiap genre pada list
 */
void dumpGenreList(GenreList list);

/**
 * Mengecek apakah genre list kosong atau tidak dimana akan memberikan
 * hasil "true" jika kosong dan "false" jika tidak kosong
 */
bool isGenreListEmpty(GenreList list);

/**
 * Mencari address genre yang diingikan berdasarkan nama dan akan memberikan
 * address yang dituju jika genre ditemukan dan "nullptr" jika tidak
 * ditemukan.
 */
GenreAddress searchGenre(GenreList list, std::string name);

/**
 * Mengecek apakah genre ada pada list dimana akan memberikan hasil
 * "true" jika ditemukan dan "false" jika tidak ditemukan
 */
bool doesGenreExist(GenreList list, std::string name);

/**
 * Membuat genre element baru dan akan memberikan hasil dalam
 * GenreAddress.
 */
GenreAddress createGenreElement(std::string name);

/**
 * Menambahkan genre baru pada list
 * I.S : genre list, nama genre (unique), dan nama
 * F.S : genre baru akan masuk ke list dan jika nama genre sudah
 *       digunakan maka proses akan dibatalkan
 */
void addGenre(GenreList& list, std::string name);

/**
 * Menghapus genre pada list
 * I.S : genre list dan nama genre
 * F.S : genre yang ditargetkan akan dihapus dari list dan dihancurkan
 */
void removeGenre(GenreList& list, std::string name);

/**
 * Menambahkan genre element baru ke posisi terdepan list
 * I.S : genre list dan genre element baru
 * F.S : genre element ditambahkan ke posisi yang paling di depan
 */
void insertGenreFirst(GenreList& list, GenreAddress new_genre);

/**
 * Menambahkan genre element baru ke posisi belakang list
 * I.S : genre list dan genre element baru
 * F.S : genre element ditambahkan ke posisi yang paling belakang
 */
void insertGenreLast(GenreList& list, GenreAddress new_genre);

/**
 * Menambahkan genre element baru setelah genre yang ditargetkan
 * I.S : genre list dan genre element baru
 * F.S : genre element ditambahkan setelah genre yang ditargetkan
 */
void insertGenreAfter(GenreList& list, GenreAddress target, GenreAddress new_genre);

/**
 * Mengeluarkan genre di posisi pertama dari list
 * I.S : genre list dan "refrensi" ke lokasi genre
 * F.S : genre di posisi pertama sudah dikeluarkan dan diberikan
 *       melalui "refrenrensi" genre
 */
void removeFirstGenre(GenreList& list, GenreAddress& out);

/**
 * Mengeluarkan genre di posisi paling belakang list
 * I.S : genre list dan "refrensi" ke lokasi genre
 * F.S : genre di posisi paling belakang sudah dikeluarkan dan diberikan
 *       melalui "refrenrensi" genre
 */
void removeLastGenre(GenreList& list, GenreAddress& out);

/**
 * Mengeluarkan genre dari list dimana posisinya terletak setelah genre
 * yang ditargetkan
 * I.S : genre list, genre yang ditargetkan, dan "renfrensi" ke lokasi genre
 * F.S : genre dengan posisi setelah dari genre yang ditargetkan sudah di keluarkan
 *       dari list
 */
void removeAfterGenre(GenreList& list, GenreAddress target, GenreAddress& out);

#endif // INCLUDE_GENRE_H
