#ifndef INCLUDE_SONG_H
#define INCLUDE_SONG_H

#include <string>

struct SongElement;
typedef struct SongElement *SongAddress;

struct SongInfoType {
  std::string id;
  std::string title;
  std::string artist;
  std::string genre;
  int duration;
};

struct SongElement {
  SongAddress next;
  SongInfoType info;
};

struct SongList{
  SongAddress first;
};

typedef struct BSTNode *BSTAddress;

struct BSTNode {
  std::string id;
  int playcount;
  SongAddress playing_song;
  BSTAddress left;
  BSTAddress right;
};

/**
 * Procedure berguna untuk menginisialisasi
 * SongList agar element first menjadi nullptr
 * I.S: sebuah refrence ke SongList
 * F.S: element first pada SongList menjadi nullptr
 */
void createSongList(SongList& list);

/**
 * Sebuah function yang berguna untuk membuat
 * SongElement dan akan mengembalikannya
 * sebagai SongAddress.
 */
SongAddress createSong(std::string id, std::string title, std::string artist, std::string genre, int duration);

/**
 * Sebuah procedure yang berguna untuk menambahkan
 * SongElement ke bagian terakhir list.
 * I.S: refrence SongList dan SongAddress
 * F.S: bertambahnya SongElement pada list
 */
void insertLastSong(SongList& list, SongAddress song);

/**
 * Sebuah function yang berguna untuk mencari
 * address song dari sebuah id yang
 * diberikan. Dan akan memberikan address
 * jika ada, dan nullptr jika tidak ada.
 */
SongAddress searchSong(SongList song, std::string id);

/**
 * Sebuah procedure yang berguna untuk menghapus
 * SongElement dari list. Dengan cara mengeluarkan dan
 * delete song.
 * I.S: refrence ke SongList dan id dari song
 * F.S: bertambahnya SongElement pada list
 */
void deleteSong(SongList song, std::string id);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * semua song yang ada pada list.
 * I.S: refrence ke SongList
 * F.S: menampilkan semua song yang ada pada list
 */
void showAllSongs(SongList song);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * semua song yang ada pada list.
 * I.S: refrence ke SongList
 * F.S: menampilkan semua song yang ada pada list
 */
void createBSTList(BSTAddress &root);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * semua song yang ada pada list.
 * I.S: refrence ke SongList
 * F.S: menampilkan semua song yang ada pada list
 */
BSTAddress createBST(std::string id, SongAddress song);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * semua song yang ada pada list.
 * I.S: refrence ke SongList
 * F.S: menampilkan semua song yang ada pada list
 */
void insertBST(BSTAddress &root, BSTAddress new_node);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * semua song yang ada pada list.
 * I.S: refrence ke SongList
 * F.S: menampilkan semua song yang ada pada list
 */
BSTAddress searchBST(BSTAddress root, std::string id);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * semua song yang ada pada list.
 * I.S: refrence ke SongList
 * F.S: menampilkan semua song yang ada pada list
 */
void deleteBST(BSTAddress &root, std::string id);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * semua song yang ada pada list.
 * I.S: refrence ke SongList
 * F.S: menampilkan semua song yang ada pada list
 */
void playSong(BSTAddress root, std::string id);

/**
 * Sebuah procedure yang berguna untuk menampilkan
 * semua song yang ada pada list.
 * I.S: refrence ke SongList
 * F.S: menampilkan semua song yang ada pada list
 */
void showTrending5(BSTAddress root, SongAddress);

#endif // INCLUDE_SONG_H_