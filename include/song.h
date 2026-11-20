#ifndef INCLUDE_SONG_H
#define INCLUDE_SONG_H

#include <string>

struct SongElement;
typedef SongElement* SongAddress;

struct SongInfoType {
  std::string Id;
  std::string Title;
  std::string Artist;
  std::string Genret;
  int Duration;
};

struct SongElement {
  SongAddress next;
  SongInfoType info;
};

struct SongList{
  SongAddress first;
};


/**
 * Procedure berguna untuk menginisialisasi
 * SongList agar element first menjadi nullptr
 * I.S: sebuah refrence ke SongList
 * F.S: element first pada SongList menjadi nullptr
 */
void createSongList(SongList& list);

/**
 * Procedure berguna untuk membuat sebuah SongElement
 * I.S: refrence ke SongAddress dan informasi song
 * F.S: SongElement dibuat dengan informasi yang diberikan
 */
SongAddress createSongelement(SongAddress& song, std::string Id, std::string Title, std::string Artist, std::string Genret, int Duration);

/**
 * Procedure berguna untuk menghapus semua song
 * pada list. Dengan cara mengeluarkan dan
 * delete song.
 * I.S: refrence ke SongList
 * F.S: list menjadi kosong
 */
void freeSongList(SongList& list);

/**
 * Sebuah function yang berguna untuk mengecek
 * apakah sebuah SongList kosong atau tidak.
 * mengembalikan true jika kosong, dan
 * false jika tidak.
 */
bool isSongListEmpty(const SongList& list);

/**
 * Procedure berguna untuk memasukkan sebuah song
 * ke dalam list.
 * I.S: refrence ke SongList dan informasi song
 * F.S: song ditambahkan ke dalam list
 */
void insertSong(SongList& list, std::string Id, std::string Title, std::string Artist, std::string Genret, int Duration);

/**
 * Procedure berguna untuk menghapus sebuah song
 * dari list.
 * I.S: refrence ke SongList dan Id song
 * F.S: song dihapus dari list
 */
void removeSong(SongList& list, std::string Id);

/**
 * Sebuah function yang berguna untuk mengecek
 * apakah sebuah song itu ada atau tidak. Dan
 * memberikan hasil true jika ada, dan false
 * jika tidak.
 */
bool isSongExistbyTitle(const SongList& list, std::string Title);

/**
 * Procedure berguna untuk menampilkan semua song
 * pada list.
 * I.S: refrence ke SongList
 * F.S: informasi semua song ditampilkan
 */
void dumpSongList(const SongList& list);

/**
 * Procedure berguna untuk memutar sebuah song
 * dari list.
 * I.S: refrence ke SongList dan Id song
 * F.S: song diputar
 */
void playSong(const SongList& list, std::string Id);

/**
 * Procedure berguna untuk menampilkan song yang paling banyak disimpan
 * dari list.
 * I.S: refrence ke SongList dan Id song
 * F.S: song yang paling banyak disimpan ditampilkan
 */
void mostsavedSong(const SongList& list, std::string Id);

/**
 * Procedure berguna untuk menampilkan song yang paling banyak diputar
 * dari list.
 * I.S: refrence ke SongList dan Id song
 * F.S: song yang paling banyak diputar ditampilkan
 */
void recommendSong(const SongList& list, std::string Id);

/**
 * Sebuah function yang berguna untuk menghitung total durasi
 * dari semua lagu dalam list.
 * I.S: refrence ke SongList
 * F.S: mengembalikan total durasi dari semua lagu dalam list
 */
int totalSongDuration(const SongList& list, std::string Id);

/**
 * Sebuah procedure yang berguna untuk mengacak urutan lagu
 * dalam list.
 * I.S: refrence ke SongList
 * F.S: urutan lagu dalam list diacak
 */
void shuffleSongList(SongList& list);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * daftar lagu yang sedang trending.
 * I.S: refrence ke SongList
 * F.S: menampilkan daftar lagu yang sedang trending
 */
void showTrendingSongs(const SongList& list);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * daftar lagu yang sama antara dua list.
 * I.S: refrence ke dua SongList
 * F.S: menampilkan daftar lagu yang sama antara dua list
 */
void showcommonSongs(const SongList& list1, const SongList& list2);

/**
 * Sebuah function yang berguna untuk mencari
 * address song dari sebuah Title yang
 * diberikan. Dan akan memberikan address
 * jika ada, dan nullptr jika tidak ada.
 */
SongAddress searchSongByTitle(const SongList& list, std::string Title);

#endif // INCLUDE_SONG_H_