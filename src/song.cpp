#include <song.h>

#include <iostream>

void createSongList(SongList &list) {
    list.first = nullptr;
}

SongAddress createSong(std::string id, std::string title, std::string artist, std::string genre, int duration) {
  SongAddress newsongelement = new SongElement;
  newsongelement->info.id = id;
  newsongelement->info.title = title;
  newsongelement->info.artist = artist;
  newsongelement->info.genre = genre;
  newsongelement->info.duration = duration;
  newsongelement->next = nullptr;
  return newsongelement;
}

void insertLastSong(SongList &list, SongAddress song) {
    if (song == nullptr) return;
    song->next = nullptr;

    if (list.first == nullptr) {
        list.first = song;
    } else {
        SongAddress P = list.first;
        while (P->next != nullptr) {
            P = P->next;
        }
        P->next = song;
    }
}

SongAddress searchSong(SongList list, std::string id) {
    SongAddress P = list.first;
    while (P != nullptr) {
        if (P->info.id == id) {
            return P;
        }
        P = P->next;
    }
    return nullptr;
}

void deleteSong(SongList list, std::string id) {
  if (list.first == nullptr) return;

    SongAddress P = list.first;
    SongAddress prev = nullptr;

    while (P != nullptr && P->info.id != id) {
        prev = P;
        P = P->next;
    }

    if (P != nullptr) {
        if (prev == nullptr) {
            list.first = P->next;
        } else {
            prev->next = P->next;
        }
        P->next = nullptr;
    }
}

void emptySongList(SongList &list) {
  SongAddress P = list.first;
  while (P != nullptr) {
      SongAddress temp = P;
      P = P->next;
      temp->next = nullptr;
  }
  list.first = nullptr;
} 

void createBSTList(BSTAddress &root) {
    root = nullptr;
}

BSTAddress createBST(std::string id, SongAddress song) {
    BSTAddress newnode = new BSTNode;
    newnode->id = id;
    newnode->playcount = 0;
    newnode->playing_song = song;
    newnode->left = nullptr;
    newnode->right = nullptr;
    return newnode;
}

void insertBST(BSTAddress &root, BSTAddress newnode) {
  if (newnode == nullptr){
    return nullptr;
  }
  if (root == nullptr) {
    root = newnode;
  } else if (newnode->id < root->id) {
    insertBST(root->left, newnode);
  } else if (newnode->id > root->id) {
    insertBST(root->right, newnode);
  }
}

BSTAddress searchBST(BSTAddress root, std::string id) {
  if (root == nullptr || root->id == id) {
    return root;
  } else if (id < root->id) {
    return searchBST(root->left, id);
  } else {
    return searchBST(root->right, id);
  }
}

void playSong(BSTAddress root, std::string id) {
  BSTAddress target = searchBST(root, id);
  if (target != nullptr) {
    target->playcount++;
    if (target->playing_song != nullptr) {
      std::cout << "Memutar: " << target->playing_song->info.title << " - " << target->playing_song->info.artist << " (Total diputar: " << target->playcount << ")" << std::endl;
    } else {
      std::cout << "Memutar lagu ID " << target->id << " (Total diputar: " << target->playcount << ")" << std::endl;
    }
  } else {
    std::cout << "Lagu dengan ID " << id << " tidak ditemukan di katalog." << std::endl;
  }
  // note: ini katanya mau bener bener putar lagu kan jo?
}

void showTrending5(BSTAddress root, SongAddress){
  hehe ga paham ini mah
}