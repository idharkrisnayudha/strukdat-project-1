#include <iostream>

#include "song.h"

int main() {
  SongList song_list;

  initializeSongList(song_list);

  // add song
  std::cout << "Add song" << std::endl;
  addSong(song_list, "1", "Lagu A1", "A", 3);
  addSong(song_list, "3", "Lagu A2", "A", 4);
  addSong(song_list, "2", "Lagu A2", "A", 2);
  addSong(song_list, "4", "Lagu B1", "B", 1);

  dumpSongList(song_list);

  // remove song
  std::cout << "\nRemove song" << std::endl;
  deleteSong(song_list, "4");
  dumpSongList(song_list);

  // update
  updateSongTitle(song_list, "1", "A keren");
  dumpSongList(song_list);
  return 0;
}
