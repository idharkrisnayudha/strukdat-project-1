#ifndef INCLUDE_GENRE_H
#define INCLUDE_GENRE_H
#include "song.h"
#include <string>

using namespace std;

typedef struct GenreElement *Genreaddress;

struct GenreElement {
    std::string info; 
    int count;      
    Genreaddress next;
};

struct GenreList {
    Genreaddress first;
};

/**
 * Menginisialisasi genre list
 * I.S : sebuah genre list
 * F.S : element node menjadi nullptr
 */
void createListGenre(GenreList &list);

/**
 * Mencari address genre yang diingikan berdasarkan nama genre dari list
 * dan akan memberikan address yang dituju jika genre ditemukan
 * dan "nullptr" jika tidak ditemukan.
 */
SongAddress searchGenre(SongList list, std::string genre);

/**
 * Menambahkan genre element baru ke posisi terdepan list
 * I.S : genre list dan genre element baru
 * F.S : genre element ditambahkan ke posisi yang paling di depan
 */
void insertFirstGenre(GenreList &list, Genreaddress P);

/**
 * Menambahkan genre element baru ke posisi belakang list
 * I.S : genre list dan genre element baru
 * F.S : genre element ditambahkan ke posisi yang paling belakang
 */
void insertAfterGenre(GenreList &list, Genreaddress prec, Genreaddress P);

/**
 * Menambahkan genre element baru setelah genre yang ditargetkan
 * I.S : genre list dan genre element baru
 * F.S : genre element ditambahkan setelah genre yang ditargetkan
 */
void insertGenreAfter(SongList& list, SongAddress target, SongAddress new_genre);

/**
 * Mengeluarkan genre di posisi pertama dari list
 * I.S : genre list dan "refrensi" ke lokasi genre
 * F.S : genre di posisi pertama sudah dikeluarkan dan diberikan
 *       melalui "refrenrensi" genre
 */
void removeFirstGenre(GenreList &list, Genreaddress &P);

/**
 * Mengeluarkan genre di posisi paling belakang list
 * I.S : genre list dan "refrensi" ke lokasi genre
 * F.S : genre di posisi paling belakang sudah dikeluarkan dan diberikan
 *       melalui "refrenrensi" genre
 */
void removeLastGenre(GenreList &list, Genreaddress &P);

/**
 * Mengeluarkan genre dari list dimana posisinya terletak setelah genre
 * yang ditargetkan
 * I.S : genre list, genre yang ditargetkan, dan "renfrensi" ke lokasi genre
 * F.S : genre dengan posisi setelah dari genre yang ditargetkan sudah di keluarkan
 *       dari list
 */
void removeAfterGenre(GenreList &list, Genreaddress Prec, Genreaddress &P);

/**
 * Mengecek apakah genre ada pada list dimana akan memberikan hasil
 * "true" jika ditemukan dan "false" jika tidak ditemukan.
 * I.S : genre list dan nama genre
 * F.S : memberikan hasil apakah genre ada pada list atau tidak
 */
bool doesGenreExist(Genreaddress& list, std::string genre);

#endif // INCLUDE_GENRE_H