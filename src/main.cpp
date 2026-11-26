#include <iostream>

#include "user.h"
#include "song.h"

int main() {
  UserList user_list;
  SongList song_list;

  initializeUserList(user_list);
  initializeSongList(song_list);

  goto SONG_TEST;

  // user test
  // register
  USER_TEST:

  std::cout << "User Register" << std::endl;
  registerUser(user_list, "Andi", "Andi Wicaksono");
  registerUser(user_list, "Budi", "Budi Gunawan");
  registerUser(user_list, "Cika", "Cika Yunita");
  registerUser(user_list, "Dodi", "Dodi Supriadi");
  dumpUserList(user_list);

  // edit
  std::cout << "\nUpdate User" << std::endl;
  setUserType(user_list, "Andi", "premium");
  setUserType(user_list, "Cika", "premium");
  setUserName(user_list, "Dodi", "Dodi Caknan");
  dumpUserList(user_list);

  // deactivate
  std::cout << "\nUser Deactivate" << std::endl;
  deactivateUser(user_list, "Budi");
  deactivateUser(user_list, "Andi");
  deactivateUser(user_list, "Dodi");
  dumpUserList(user_list);

  // song test
  SONG_TEST:
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

  destroyUserList(user_list);
  return 0;
}
