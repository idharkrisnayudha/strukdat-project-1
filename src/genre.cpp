#include "genre.h"

#include <iostream>

using namespace std;

void createListGenre(GenreList &list) {
    list.first = nullptr;
}

Genreaddress allocateGenre(string Genre) {
    Genreaddress P = new GenreElement;
    P->info = Genre;
    P->count = 1;
    P->next = nullptr;
    return P;
}

void insertFirstGenre(GenreList &list, Genreaddress P) {
    if (P == nullptr) return;

    if (list.first == nullptr) {
        list.first = P;
        P->next = nullptr;
    } else {
        P->next = list.first;
        list.first = P;
    }
}

void insertLastGenre(GenreList &list, Genreaddress P) {
    if (P == nullptr) return;
    P->next = nullptr;

    if (list.first == nullptr) {
        list.first = P;
    } else {
        Genreaddress Q = list.first;
        while (Q->next != nullptr) {
            Q = Q->next;
        }
        Q->next = P;
    }
}

void insertAfterGenre(GenreList &list, Genreaddress prec, Genreaddress P) {
    if (prec == nullptr || P == nullptr) return;

    P->next = prec->next;
    prec->next = P;
}

void removeFirstGenre(GenreList &list, Genreaddress &P) {
    if (list.first == nullptr) {
        P = nullptr;
    } else {
        P = list.first;
        list.first = P->next;
        P->next = nullptr;
    }
}

void removeLastGenre(GenreList &list, Genreaddress &P) {
    if (list.first == nullptr) {
        P = nullptr;
    } else if (list.first->next == nullptr) {
        P = list.first;
        list.first = nullptr;
    } else {
        Genreaddress Q = list.first;
        while (Q->next->next != nullptr) {
            Q = Q->next;
        }
        P = Q->next;
        Q->next = nullptr;
    }
}

void removeAfterGenre(GenreList &list, Genreaddress prec, Genreaddress &P) {
    if (prec == nullptr || prec->next == nullptr) {
        P = nullptr;
    } else {
        P = prec->next;
        prec->next = P->next;
        P->next = nullptr;
    }
}

Genreaddress searchGenre(GenreList list, std::string Genre) {
    Genreaddress Q = list.first;
    while (Q != nullptr) {
        if (Q->info == Genre) {
            return Q;
        }
        Q = Q->next;
    }
    return nullptr;
}

bool isExistGenre(GenreList list, string Genre) {
    return searchGenre(list, Genre) != nullptr;
}