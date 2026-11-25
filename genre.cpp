#include "genre.h"

#include <iostream>

using namespace std;

void initializeGenreList(SongList& list){
  list.node = nullptr;
}

SongAddress searchGenre(SongList list, std::string genre){
  SongAddress current_genre;

  current_genre = list.node;
  while(current_genre != nullptr) {
    if(genre < current_genre->info.id) {
      return current_genre;
    } 
    current_genre = current_genre->right;
  }
}

void insertGenreFirst(SongList& list, SongAddress new_genre){
  if (new_genre == nullptr) return;

    if (list.node == nullptr) {
        list.node = new_genre;
        new_genre->right = nullptr;
    } else {
        new_genre->right = list.node;
        list.node = new_genre;
    }
}

void insertGenreLast(SongList& list, SongAddress new_genre){
  if (new_genre == nullptr) return;

    if (list.node == nullptr) {
        list.node = new_genre;
    } else {
        SongAddress current = list.node;
        while (current->right != nullptr) {
            current = current->right;
        }
        current->right = new_genre;
    }
}

void insertGenreAfter(SongList& list, SongAddress target, SongAddress new_genre){
  if (target == nullptr || new_genre == nullptr) return;

    new_genre->right = target->right;
    target->right = new_genre;
}

void removeFirstGenre(SongList& list, SongAddress& out){
 if (list.node == nullptr) {
        out = nullptr;
    } else {
        out = list.node;
        list.node = out->right;
        out->right = nullptr;
    }
}

void removeLastGenre(SongList& list, SongAddress& out){
  SongAddress current;
  if (list.node == nullptr) {
        out = nullptr;
    }
  else if (list.node->right == nullptr) {
        out = list.node;
        list.node = nullptr;
    } else {
        current = list.node;
        while (current->right->right != nullptr) {
            current = current->right;
        }
        out = current->right;
        current->right = nullptr;
    }
}

void removeAfterGenre(SongList& list, SongAddress target, SongAddress& out){
  if (target == nullptr || target->right == nullptr) {
        out = nullptr;
    } else {
        out = target->right;
        target->right = out->right;
        out->right = nullptr;
    }
}

bool doesGenreExist(SongList& list, std::string genre) {
  SongAddress Q = list.node;
    while (Q != nullptr) {
        if (Q->info.genre == genre) {
            return true;
        }
        Q = Q->right;
    }
    return false;
  }