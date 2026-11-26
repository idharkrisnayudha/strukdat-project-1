#include "song.h"

#include <iostream>

void initializeSongList(SongList& list) {
  list.node = nullptr;
}

void dumpSongNode(SongAddress node) {
  if(node != nullptr) {
    std::cout << "ID: " << node->info.id << std::endl;
    std::cout << "Title: " << node->info.title << std::endl;
    std::cout << "Artist: " << node->info.artist << std::endl;
    std::cout << "Duration: " << node->info.duration << " minutes" << std::endl;
    std::cout << std::endl;

    if(node->left != nullptr) {
      dumpSongNode(node->left);
    }

    if(node->right != nullptr) {
      dumpSongNode(node->right);
    }
  }
}

void dumpSongList(SongList& list) {
  dumpSongNode(list.node);
}

bool isSongListEmpty(SongList list) {
  return list.node == nullptr;
}

SongAddress findSongNode(SongAddress node, std::string id) {
  SongAddress song;

  song = nullptr;

  if(node != nullptr) {
    if(node->info.id == id) {
      song = node;
    } else if(id < node->info.id) {
      song = findSongNode(node->left, id);
    } else {
      song = findSongNode(node->right, id);
    }
  }

  return song;
}

bool doesSongExist(SongList list, std::string id) {
  return findSongNode(list.node, id) != nullptr;
}

SongAddress searchSong(SongList list, std::string id) {
  return findSongNode(list.node, id);
}

SongAddress createSongNode(std::string id, std::string title, std::string artist, int duration) {
  SongAddress new_node = new SongNode;
  new_node->info.id = id;
  new_node->info.title = title;
  new_node->info.artist = artist;
  new_node->info.duration = duration;
  new_node->left = nullptr;
  new_node->right = nullptr;
  return new_node;
}

void insertSongNode(SongAddress& node, SongAddress new_song) {
  if(node == nullptr) {
    node = new_song;
  } else {
    if(new_song->info.id < node->info.id) {
      insertSongNode(node->left, new_song);
    } else {
      insertSongNode(node->right, new_song);
    }
  }
}

void addSong(SongList& list, std::string id, std::string title, std::string artist, int duration) {
  SongAddress new_song;

  if(!doesSongExist(list, id)) {
    new_song = createSongNode(id, title, artist, duration);
    insertSongNode(list.node, new_song);
  } else {
    // NOTE: tampilkan pesan error bahwa 'id' sudah digunakan!
  }
}

SongAddress findMinSongNode(SongAddress node) {
  SongAddress min_node;

  min_node = node;
  if(node != nullptr) {
    if(node->left != nullptr) {
      min_node = findMinSongNode(node->left);
    }
  }

  return node;
}

SongAddress removeSongNode(SongAddress& node, std::string id) {
  SongAddress temp;

  if(node != nullptr) {
    // search phase
    if(id < node->info.id) {
      node->left = removeSongNode(node->left, id);
    } else if(id > node->info.id) {
      node->right = removeSongNode(node->right, id);
    }

    // remove phase
    else {
      if(node->left == nullptr && node->right == nullptr) {
        delete node;
        node = nullptr;
      } else if(node->right == nullptr) {
        temp = node;
        node = node->left;
        delete temp;
      } else if(node->left == nullptr) {
        temp = node;
        node = node->right;
        delete temp;
      } else {
        temp = findMinSongNode(node->right);
        node->info = temp->info;
        node->right = removeSongNode(node->right, id);
      }
    }
  }

    return node;
}

void removeSong(SongList& list, std::string id) {
  removeSongNode(list.node, id);
}

void updateSongArtist(SongList& list, std::string id, std::string new_artist) {
  SongAddress song;

  if(doesSongExist(list, id)) {
    song = searchSong(list, id);
    song->info.artist = new_artist;
  }
}

void updateSongTitle(SongList& list, std::string id, std::string new_title) {
  SongAddress song;

  if(doesSongExist(list, id)) {
    song = searchSong(list, id);
    song->info.title = new_title;
  }
}

void updateSongDuration(SongList& list, std::string id, int new_duration) {
  SongAddress song;

  if(doesSongExist(list, id)) {
    song = searchSong(list, id);
    song->info.duration = new_duration;
  }
}
