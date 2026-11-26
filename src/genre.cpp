#include "genre.h"

#include <iostream>

using namespace std;

void createListGenre(GenreList &LG) {
    LG.first = nullptr;
}

Genreaddress allocateGenre(string namaGenre) {
    Genreaddress P = new GenreElement;
    P->info = namaGenre;
    P->count = 1;
    P->next = nullptr;
    return P;
}

void insertFirstGenre(GenreList &LG, Genreaddress P) {
    if (P == nullptr) return;

    if (LG.first == nullptr) {
        LG.first = P;
        P->next = nullptr;
    } else {
        P->next = LG.first;
        LG.first = P;
    }
}

void insertLastGenre(GenreList &LG, Genreaddress P) {
    if (P == nullptr) return;
    P->next = nullptr;

    if (LG.first == nullptr) {
        LG.first = P;
    } else {
        Genreaddress Q = LG.first;
        while (Q->next != nullptr) {
            Q = Q->next;
        }
        Q->next = P;
    }
}

void insertAfterGenre(GenreList &LG, Genreaddress prec, Genreaddress P) {
    if (prec == nullptr || P == nullptr) return;

    P->next = prec->next;
    prec->next = P;
}

void removeFirstGenre(GenreList &LG, Genreaddress &P) {
    if (LG.first == nullptr) {
        P = nullptr;
    } else {
        P = LG.first;
        LG.first = P->next;
        P->next = nullptr;
    }
}

void removeLastGenre(GenreList &LG, Genreaddress &P) {
    if (LG.first == nullptr) {
        P = nullptr;
    } else if (LG.first->next == nullptr) {
        P = LG.first;
        LG.first = nullptr;
    } else {
        Genreaddress Q = LG.first;
        while (Q->next->next != nullptr) {
            Q = Q->next;
        }
        P = Q->next;
        Q->next = nullptr;
    }
}

void removeAfterGenre(GenreList &LG, Genreaddress prec, Genreaddress &P) {
    if (prec == nullptr || prec->next == nullptr) {
        P = nullptr;
    } else {
        P = prec->next;
        prec->next = P->next;
        P->next = nullptr;
    }
}

Genreaddress searchGenre(GenreList LG, std::string namaGenre) {
    Genreaddress Q = LG.first;
    while (Q != nullptr) {
        if (Q->info == namaGenre) {
            return Q;
        }
        Q = Q->next;
    }
    return nullptr;
}

bool isExistGenre(GenreList LG, string namaGenre) {
    return searchGenre(LG, namaGenre) != nullptr;
}