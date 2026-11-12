#ifndef SONG_H_
#define SONG_H_

#include <string>

struct SongElement;
struct SongInfoType {
    std::string id;
    std::string title;
    std::string singer;
    std::string genre;
    int duration;
};

typedef SongElement* SongAddress;

struct SongElement {
    SongInfoType info;
    SongAddress next;
};

struct SongList {
    SongAddress first;
};

#endif // SONG_H_