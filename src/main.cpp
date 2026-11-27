#include <iostream>

#include "user.h"
#include "song.h"

int main() {
  UserList user_list;
  SongList song_list;

  initializeUserList(user_list);
  initializeSongList(song_list);

  // code here

  destroyUserList(user_list);
  return 0;
}
