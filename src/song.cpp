#include <song.h>

#include <iostream>

void createSongList(SongList& list) {
  list.first = nullptr;
}

SongAddress createSongelement(SongAddress& song, std::string Id, std::string Title, std::string Artist, std::string Genret, int Duration) {
  SongAddress newSong = new SongElement;
  newSong->info.Id = Id;
  newSong->info.Title = Title;
  newSong->info.Artist = Artist;
  newSong->info.Genret = Genret;
  newSong->info.Duration = Duration;
  song->next = nullptr;
  return newSong;
}

void freeSongList(SongList& list) {
  SongAddress song_temp;

  while(!isSongListEmpty(list)) {
    song_temp = list.first;
    list.first = list.first->next;
    delete song_temp;
  }
}

void insertSong(SongList& list, std::string Id, std::string Title, std::string Artist, std::string Genret, int Duration) {
  SongAddress newSong = new SongElement;
  newSong->info.Id = Id;
  newSong->info.Title = Title;
  newSong->info.Artist = Artist;
  newSong->info.Genret = Genret;
  newSong->info.Duration = Duration;
  newSong->next = nullptr;

  if (list.first == nullptr) {
    list.first = newSong;
  } else {
    SongAddress current = list.first;
    while (current->next != nullptr) {
      current = current->next;
    }
    current->next = newSong;
  }
}

void removeSong(SongList& list, std::string Id) {
  if (list.first == nullptr) return;

  if (list.first->info.Id == Id) {
    SongAddress toDelete = list.first;
    list.first = list.first->next;
    delete toDelete;
    return;
  }

  SongAddress current = list.first;
  while (current->next != nullptr && current->next->info.Id != Id) {
    current = current->next;
  }

  if (current->next != nullptr) {
    SongAddress toDelete = current->next;
    current->next = current->next->next;
    delete toDelete;
  }
}

void dumpSongList(const SongList& list) {
  SongAddress current = list.first;
  while (current != nullptr) {
    std::cout << "Id: " << current->info.Id << ", Title: " << current->info.Title
              << ", Artist: " << current->info.Artist << ", Genre: " << current->info.Genret
              << ", Duration: " << current->info.Duration << std::endl;
    current = current->next;
  }
}

SongAddress searchSongByTitle(const SongList& list, std::string Title) {
  SongAddress current = list.first;
  while (current != nullptr) {
    if (current->info.Title == Title) {
      return current;
    }
    current = current->next;
  }
  return nullptr;
}

void playSong(const SongList& list, std::string Title) {
  SongAddress song;
  song = searchSongByTitle(list, Title);
  if (song != nullptr) {
    std::cout << "Playing song: " << song->info.Title << " by " << song->info.Artist << std::endl;
  } else {
    std::cout << "Song with Title " << Title << " not found." << std::endl;
  }
}

bool isSongListEmpty(const SongList& list) {
  return list.first == nullptr;
}

