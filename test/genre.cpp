// test_genre.cpp
// Test untuk ADT genre (single linked list, hanya pointer first) pada genre.h
//
// Compile (sesuaikan path):
//   g++ -std=c++11 -g -Wall -Wextra -fsanitize=address,undefined test_genre.cpp <file implementasi genre>.cpp -I<folder genre.h dan song.h> -o test_genre
// Jalankan:
//   ./test_genre             (atau ./test_genre --isolate)
//
// Catatan:
//   - Fixture list dibuat manual lewat pointer (rawList), independen dari ADT.
//   - CHECK = wajib benar. ASSUME = hal yang tidak ditegaskan header (jadi peringatan).
//   - Kalau implementasi genre.cpp-mu perlu file lain (mis. song.cpp), ikut di-compile.

#include <map>
#include <random>
#include <set>

#include "genre.h"
#include "test_helper.h"

namespace {

const int kGuard = 100000;

// ----------------------------- helper struktur -----------------------------

std::vector<std::string> names(const GenreList& l) {
  std::vector<std::string> v;
  int guard = 0;
  for (GenreAddress p = l.first; p != nullptr && guard < kGuard; p = p->next, ++guard)
    v.push_back(p->info.name);
  return v;
}

int countGenres(const GenreList& l) { return static_cast<int>(names(l).size()); }

// Tidak boleh melingkar; semua elemen terhubung sampai nullptr.
bool genreListValid(const GenreList& l, std::string& why) {
  int n = 0;
  for (GenreAddress p = l.first; p != nullptr; p = p->next) {
    if (++n > kGuard) {
      why = "list melingkar / terlalu panjang";
      return false;
    }
  }
  return true;
}

#define CHECK_LIST(l)                                                           \
  do {                                                                          \
    std::string why_;                                                           \
    th::report(genreListValid((l), why_), false, "struktur list valid: " #l,    \
               __FILE__, __LINE__, why_);                                       \
  } while (0)

#define CHECK_NAMES(l, expected) CHECK_EQ(th::join(names(l)), expected)

// ----------------------------- fixture manual ------------------------------

GenreAddress rawNode(const std::string& n) {
  GenreAddress g = new GenreElement();
  g->info.name = n;
  g->next = nullptr;
  return g;
}

GenreList rawList(const std::vector<std::string>& v) {
  GenreList l;
  l.first = nullptr;
  GenreAddress last = nullptr;
  for (size_t i = 0; i < v.size(); ++i) {
    GenreAddress g = rawNode(v[i]);
    if (last == nullptr) l.first = g;
    else last->next = g;
    last = g;
  }
  return l;
}

void rawFree(GenreList& l) {
  std::string why;
  if (genreListValid(l, why)) {
    GenreAddress p = l.first;
    while (p != nullptr) {
      GenreAddress nx = p->next;
      delete p;
      p = nx;
    }
  }
  l.first = nullptr;
}

GenreAddress nth(const GenreList& l, int i) {
  GenreAddress p = l.first;
  while (p != nullptr && i-- > 0) p = p->next;
  return p;
}

}  // namespace

// ===========================================================================
// initializeGenreList / isGenreListEmpty / createGenreElement
// ===========================================================================

TEST(init_menjadikan_first_nullptr) {
  GenreList l;
  l.first = reinterpret_cast<GenreAddress>(0x10);  // sengaja berisi sampah
  initializeGenreList(l);
  CHECK(l.first == nullptr);
}

TEST(isEmpty_true_untuk_list_baru_false_jika_ada_isi) {
  GenreList l;
  initializeGenreList(l);
  CHECK(isGenreListEmpty(l));
  GenreList a = rawList({"Pop"});
  GenreList b = rawList({"Pop", "Rock"});
  CHECK(!isGenreListEmpty(a));
  CHECK(!isGenreListEmpty(b));
  rawFree(a);
  rawFree(b);
}

TEST(create_element_mengisi_nama_dan_next_null) {
  GenreAddress g = createGenreElement("Hip Hop");
  CHECK(g != nullptr);
  if (g == nullptr) return;
  CHECK_EQ(g->info.name, "Hip Hop");
  CHECK(g->next == nullptr);
  delete g;
}

TEST(create_element_nama_kosong_tidak_crash) {
  GenreAddress g = createGenreElement("");
  CHECK(g != nullptr);
  if (g == nullptr) return;
  CHECK_EQ(g->info.name, "");
  CHECK(g->next == nullptr);
  delete g;
}

// ===========================================================================
// searchGenre / doesGenreExist
// ===========================================================================

TEST(search_pada_list_kosong_nullptr) {
  GenreList l;
  initializeGenreList(l);
  CHECK(searchGenre(l, "Pop") == nullptr);
  CHECK(!doesGenreExist(l, "Pop"));
}

TEST(search_menemukan_pertama_tengah_terakhir) {
  GenreList l = rawList({"Pop", "Rock", "Jazz", "Blues", "Folk"});
  CHECK(searchGenre(l, "Pop") == nth(l, 0));
  CHECK(searchGenre(l, "Jazz") == nth(l, 2));
  CHECK(searchGenre(l, "Folk") == nth(l, 4));
  CHECK(doesGenreExist(l, "Pop"));
  CHECK(doesGenreExist(l, "Jazz"));
  CHECK(doesGenreExist(l, "Folk"));
  rawFree(l);
}

TEST(search_nama_tidak_ada_nullptr) {
  GenreList l = rawList({"Pop", "Rock"});
  CHECK(searchGenre(l, "Metal") == nullptr);
  CHECK(!doesGenreExist(l, "Metal"));
  rawFree(l);
}

TEST(search_harus_cocok_penuh_bukan_sebagian) {
  GenreList l = rawList({"Pop"});
  CHECK(searchGenre(l, "Po") == nullptr);
  CHECK(searchGenre(l, "op") == nullptr);
  CHECK(searchGenre(l, "Pops") == nullptr);
  CHECK(searchGenre(l, "") == nullptr);
  CHECK(!doesGenreExist(l, "Po"));
  rawFree(l);
}

TEST(search_nama_dengan_spasi) {
  GenreList l = rawList({"Hip Hop", "K Pop"});
  CHECK(doesGenreExist(l, "Hip Hop"));
  CHECK(!doesGenreExist(l, "Hip"));
  CHECK(!doesGenreExist(l, "HipHop"));
  rawFree(l);
}

TEST(search_membedakan_huruf_besar_kecil) {
  GenreList l = rawList({"Pop"});
  ASSUME(searchGenre(l, "pop") == nullptr);
  ASSUME(!doesGenreExist(l, "POP"));
  rawFree(l);
}

TEST(search_tidak_mengubah_list) {
  GenreList l = rawList({"A", "B", "C"});
  searchGenre(l, "B");
  doesGenreExist(l, "Z");
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// insertGenreFirst
// ===========================================================================

TEST(insertFirst_ke_list_kosong) {
  GenreList l;
  initializeGenreList(l);
  GenreAddress a = rawNode("A");
  insertGenreFirst(l, a);
  CHECK(l.first == a);
  CHECK(a->next == nullptr);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertFirst_ke_list_berisi_menaruh_di_depan) {
  GenreList l = rawList({"B", "C"});
  GenreAddress b = l.first;
  GenreAddress a = rawNode("A");
  insertGenreFirst(l, a);
  CHECK(l.first == a);
  CHECK(a->next == b);
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertFirst_berulang_membalik_urutan) {
  GenreList l;
  initializeGenreList(l);
  insertGenreFirst(l, rawNode("A"));
  insertGenreFirst(l, rawNode("B"));
  insertGenreFirst(l, rawNode("C"));
  CHECK_NAMES(l, "C,B,A");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// insertGenreLast
// ===========================================================================

TEST(insertLast_ke_list_kosong) {
  GenreList l;
  initializeGenreList(l);
  GenreAddress a = rawNode("A");
  insertGenreLast(l, a);
  CHECK(l.first == a);
  CHECK(a->next == nullptr);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertLast_ke_list_berisi_menaruh_di_belakang) {
  GenreList l = rawList({"A", "B"});
  GenreAddress a = l.first, b = nth(l, 1);
  GenreAddress c = rawNode("C");
  insertGenreLast(l, c);
  CHECK(l.first == a);
  CHECK(b->next == c);
  CHECK(c->next == nullptr);
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertLast_berulang_menjaga_urutan) {
  GenreList l;
  initializeGenreList(l);
  insertGenreLast(l, rawNode("A"));
  insertGenreLast(l, rawNode("B"));
  insertGenreLast(l, rawNode("C"));
  insertGenreLast(l, rawNode("D"));
  CHECK_NAMES(l, "A,B,C,D");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertLast_pada_list_satu_elemen) {
  GenreList l = rawList({"A"});
  GenreAddress a = l.first;
  GenreAddress b = rawNode("B");
  insertGenreLast(l, b);
  CHECK(l.first == a);
  CHECK(a->next == b);
  CHECK_NAMES(l, "A,B");
  rawFree(l);
}

// ===========================================================================
// insertGenreAfter
// ===========================================================================

TEST(insertAfter_di_tengah_list) {
  GenreList l = rawList({"A", "C"});
  GenreAddress a = l.first, c = nth(l, 1);
  GenreAddress b = rawNode("B");
  insertGenreAfter(l, a, b);
  CHECK(a->next == b);
  CHECK(b->next == c);
  CHECK(l.first == a);
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertAfter_target_elemen_terakhir) {
  GenreList l = rawList({"A", "B"});
  GenreAddress b = nth(l, 1);
  GenreAddress c = rawNode("C");
  insertGenreAfter(l, b, c);
  CHECK(b->next == c);
  CHECK(c->next == nullptr);
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertAfter_pada_list_satu_elemen) {
  GenreList l = rawList({"A"});
  GenreAddress a = l.first;
  insertGenreAfter(l, a, rawNode("B"));
  CHECK(l.first == a);
  CHECK_NAMES(l, "A,B");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertAfter_berulang_pada_target_sama) {
  GenreList l = rawList({"A", "Z"});
  GenreAddress a = l.first;
  insertGenreAfter(l, a, rawNode("X"));
  insertGenreAfter(l, a, rawNode("Y"));
  CHECK_NAMES(l, "A,Y,X,Z");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// removeFirstGenre
// ===========================================================================

TEST(removeFirst_list_kosong) {
  GenreList l;
  initializeGenreList(l);
  GenreAddress sentinel = rawNode("sentinel");
  GenreAddress out = sentinel;
  removeFirstGenre(l, out);
  ASSUME(out == nullptr);
  CHECK(l.first == nullptr);
  delete sentinel;
}

TEST(removeFirst_satu_elemen_membuat_list_kosong) {
  GenreList l = rawList({"A"});
  GenreAddress a = l.first;
  GenreAddress out = nullptr;
  long live = th::liveAllocs();
  removeFirstGenre(l, out);
  CHECK(out == a);
  CHECK(l.first == nullptr);
  CHECK(isGenreListEmpty(l));
  CHECK_EQ(th::liveAllocs(), live);  // hanya dikeluarkan, tidak dihancurkan
  FREE_IF_ALIVE(a, live);
}

TEST(removeFirst_banyak_elemen) {
  GenreList l = rawList({"A", "B", "C"});
  GenreAddress a = l.first, b = nth(l, 1);
  GenreAddress out = nullptr;
  long live = th::liveAllocs();
  removeFirstGenre(l, out);
  CHECK(out == a);
  CHECK(l.first == b);
  CHECK_NAMES(l, "B,C");
  CHECK_LIST(l);
  CHECK_EQ(th::liveAllocs(), live);
  if (th::liveAllocs() == live) {
    CHECK_EQ(a->info.name, "A");  // data elemen yang keluar masih utuh
    ASSUME(a->next == nullptr);   // elemen keluar sebaiknya terlepas dari list
  }
  FREE_IF_ALIVE(a, live);
  rawFree(l);
}

TEST(removeFirst_berulang_sampai_kosong_urutan_benar) {
  GenreList l = rawList({"A", "B", "C", "D"});
  std::string got;
  for (int i = 0; i < 4; ++i) {
    GenreAddress out = nullptr;
    long live = th::liveAllocs();
    removeFirstGenre(l, out);
    bool intact = (th::liveAllocs() == live);
    CHECK(out != nullptr);
    if (out == nullptr) break;
    got += out->info.name;
    CHECK_LIST(l);
    if (intact) delete out;
  }
  CHECK_EQ(got, "ABCD");
  CHECK(isGenreListEmpty(l));
  GenreAddress out = nullptr;
  removeFirstGenre(l, out);  // sekali lagi pada list yang sudah kosong
  CHECK(out == nullptr);
}

// ===========================================================================
// removeLastGenre
// ===========================================================================

TEST(removeLast_list_kosong) {
  GenreList l;
  initializeGenreList(l);
  GenreAddress sentinel = rawNode("sentinel");
  GenreAddress out = sentinel;
  removeLastGenre(l, out);
  ASSUME(out == nullptr);
  CHECK(l.first == nullptr);
  delete sentinel;
}

TEST(removeLast_satu_elemen_membuat_list_kosong) {
  GenreList l = rawList({"A"});
  GenreAddress a = l.first;
  GenreAddress out = nullptr;
  long live = th::liveAllocs();
  removeLastGenre(l, out);
  CHECK(out == a);
  CHECK(l.first == nullptr);  // kesalahan umum: first masih menunjuk elemen yang keluar
  CHECK_EQ(th::liveAllocs(), live);
  FREE_IF_ALIVE(a, live);
}

TEST(removeLast_dua_elemen_next_sisa_harus_null) {
  GenreList l = rawList({"A", "B"});
  GenreAddress a = l.first, b = nth(l, 1);
  GenreAddress out = nullptr;
  long live = th::liveAllocs();
  removeLastGenre(l, out);
  CHECK(out == b);
  CHECK(l.first == a);
  CHECK(a->next == nullptr);  // kesalahan umum: next elemen sebelumnya tidak di-null-kan
  CHECK_LIST(l);
  FREE_IF_ALIVE(b, live);
  rawFree(l);
}

TEST(removeLast_banyak_elemen) {
  GenreList l = rawList({"A", "B", "C"});
  GenreAddress c = nth(l, 2);
  GenreAddress out = nullptr;
  long live = th::liveAllocs();
  removeLastGenre(l, out);
  CHECK(out == c);
  CHECK_NAMES(l, "A,B");
  CHECK_LIST(l);
  CHECK_EQ(th::liveAllocs(), live);
  if (th::liveAllocs() == live) {
    CHECK_EQ(c->info.name, "C");
    ASSUME(c->next == nullptr);
  }
  FREE_IF_ALIVE(c, live);
  rawFree(l);
}

TEST(removeLast_berulang_sampai_kosong_urutan_benar) {
  GenreList l = rawList({"A", "B", "C", "D"});
  std::string got;
  for (int i = 0; i < 4; ++i) {
    GenreAddress out = nullptr;
    long live = th::liveAllocs();
    removeLastGenre(l, out);
    bool intact = (th::liveAllocs() == live);
    CHECK(out != nullptr);
    if (out == nullptr) break;
    got += out->info.name;
    CHECK_LIST(l);
    if (intact) delete out;
  }
  CHECK_EQ(got, "DCBA");
  CHECK(isGenreListEmpty(l));
}

// ===========================================================================
// removeAfterGenre
// ===========================================================================

TEST(removeAfter_target_elemen_pertama) {
  GenreList l = rawList({"A", "B", "C"});
  GenreAddress a = l.first, b = nth(l, 1), c = nth(l, 2);
  GenreAddress out = nullptr;
  long live = th::liveAllocs();
  removeAfterGenre(l, a, out);
  CHECK(out == b);
  CHECK(a->next == c);
  CHECK(l.first == a);
  CHECK_NAMES(l, "A,C");
  CHECK_LIST(l);
  CHECK_EQ(th::liveAllocs(), live);
  if (th::liveAllocs() == live) {
    CHECK_EQ(b->info.name, "B");
    ASSUME(b->next == nullptr);
  }
  FREE_IF_ALIVE(b, live);
  rawFree(l);
}

TEST(removeAfter_elemen_yang_dikeluarkan_adalah_yang_terakhir) {
  GenreList l = rawList({"A", "B", "C"});
  GenreAddress b = nth(l, 1), c = nth(l, 2);
  GenreAddress out = nullptr;
  long live = th::liveAllocs();
  removeAfterGenre(l, b, out);
  CHECK(out == c);
  CHECK(b->next == nullptr);
  CHECK_NAMES(l, "A,B");
  CHECK_LIST(l);
  FREE_IF_ALIVE(c, live);
  rawFree(l);
}

TEST(removeAfter_dua_elemen_target_pertama) {
  GenreList l = rawList({"A", "B"});
  GenreAddress a = l.first, b = nth(l, 1);
  GenreAddress out = nullptr;
  long live = th::liveAllocs();
  removeAfterGenre(l, a, out);
  CHECK(out == b);
  CHECK(l.first == a);
  CHECK(a->next == nullptr);
  CHECK_LIST(l);
  FREE_IF_ALIVE(b, live);
  rawFree(l);
}

TEST(removeAfter_target_elemen_terakhir_tidak_ada_yang_dikeluarkan) {
  GenreList l = rawList({"A", "B", "C"});
  GenreAddress c = nth(l, 2);
  GenreAddress sentinel = rawNode("sentinel");
  GenreAddress out = sentinel;
  removeAfterGenre(l, c, out);
  ASSUME(out == nullptr);
  CHECK_NAMES(l, "A,B,C");  // list tidak berubah
  CHECK_LIST(l);
  delete sentinel;
  rawFree(l);
}

TEST(removeAfter_pada_list_satu_elemen) {
  GenreList l = rawList({"A"});
  GenreAddress sentinel = rawNode("sentinel");
  GenreAddress out = sentinel;
  removeAfterGenre(l, l.first, out);
  ASSUME(out == nullptr);
  CHECK_NAMES(l, "A");
  delete sentinel;
  rawFree(l);
}

// ===========================================================================
// addGenre
// ===========================================================================

TEST(add_ke_list_kosong) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Pop");
  CHECK_EQ(countGenres(l), 1);
  CHECK(l.first != nullptr);
  if (l.first == nullptr) return;
  CHECK_EQ(l.first->info.name, "Pop");
  CHECK(l.first->next == nullptr);
  rawFree(l);
}

TEST(add_beberapa_genre_semuanya_ada) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Pop");
  addGenre(l, "Rock");
  addGenre(l, "Jazz");
  addGenre(l, "Hip Hop");
  CHECK_EQ(countGenres(l), 4);
  CHECK(doesGenreExist(l, "Pop"));
  CHECK(doesGenreExist(l, "Rock"));
  CHECK(doesGenreExist(l, "Jazz"));
  CHECK(doesGenreExist(l, "Hip Hop"));
  CHECK_LIST(l);
  rawFree(l);
}

TEST(add_urutan_genre_baru_ditaruh_di_belakang) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "A");
  addGenre(l, "B");
  addGenre(l, "C");
  ASSUME_EQ(th::join(names(l)), "A,B,C");  // header tidak menyebut posisi
  rawFree(l);
}

TEST(add_nama_duplikat_dibatalkan) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Pop");
  addGenre(l, "Pop");
  CHECK_EQ(countGenres(l), 1);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(add_duplikat_di_posisi_pertama_tengah_terakhir_ditolak) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "A");
  addGenre(l, "B");
  addGenre(l, "C");
  addGenre(l, "A");
  addGenre(l, "B");
  addGenre(l, "C");
  CHECK_EQ(countGenres(l), 3);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(add_duplikat_tidak_bocor_memori) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Genre dengan nama panjang agar alokasi heap");
  long live = th::liveAllocs();
  addGenre(l, "Genre dengan nama panjang agar alokasi heap");
  CHECK_EQ(th::liveAllocs(), live);  // elemen yang ditolak tidak boleh menggantung
  rawFree(l);
}

TEST(add_huruf_besar_kecil_dianggap_beda) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Pop");
  addGenre(l, "pop");
  ASSUME_EQ(countGenres(l), 2);
  rawFree(l);
}

TEST(add_ulang_setelah_remove_boleh) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Pop");
  removeGenre(l, "Pop");
  addGenre(l, "Pop");
  CHECK_EQ(countGenres(l), 1);
  CHECK(doesGenreExist(l, "Pop"));
  CHECK_LIST(l);
  rawFree(l);
}

TEST(add_banyak_genre_500) {
  GenreList l;
  initializeGenreList(l);
  for (int i = 0; i < 500; ++i) addGenre(l, "g" + std::to_string(i));
  CHECK_EQ(countGenres(l), 500);
  CHECK(doesGenreExist(l, "g0"));
  CHECK(doesGenreExist(l, "g499"));
  CHECK(!doesGenreExist(l, "g500"));
  CHECK_LIST(l);
  rawFree(l);
}

TEST(add_nama_kosong_tidak_merusak_list) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Pop");
  addGenre(l, "");
  addGenre(l, "");
  CHECK_LIST(l);  // boleh diterima / ditolak, yang penting list tetap valid
  rawFree(l);
}

// ===========================================================================
// removeGenre
// ===========================================================================

TEST(remove_pada_list_kosong_tidak_crash) {
  GenreList l;
  initializeGenreList(l);
  removeGenre(l, "Pop");
  CHECK(l.first == nullptr);
}

TEST(remove_nama_tidak_ada_tidak_mengubah_list) {
  GenreList l = rawList({"A", "B"});
  removeGenre(l, "Z");
  CHECK_NAMES(l, "A,B");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(remove_satu_satunya_genre_membuat_list_kosong) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Pop");
  removeGenre(l, "Pop");
  CHECK(l.first == nullptr);
  CHECK(isGenreListEmpty(l));
}

TEST(remove_elemen_pertama) {
  GenreList l = rawList({"A", "B", "C"});
  GenreAddress b = nth(l, 1);
  removeGenre(l, "A");
  CHECK_NAMES(l, "B,C");
  CHECK(l.first == b);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(remove_elemen_terakhir) {
  GenreList l = rawList({"A", "B", "C"});
  GenreAddress b = nth(l, 1);
  removeGenre(l, "C");
  CHECK_NAMES(l, "A,B");
  CHECK(b->next == nullptr);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(remove_elemen_tengah) {
  GenreList l = rawList({"A", "B", "C"});
  GenreAddress a = l.first, c = nth(l, 2);
  removeGenre(l, "B");
  CHECK_NAMES(l, "A,C");
  CHECK(a->next == c);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(remove_dua_elemen_satu_per_satu) {
  GenreList l = rawList({"A", "B"});
  removeGenre(l, "B");
  CHECK_NAMES(l, "A");
  CHECK(l.first->next == nullptr);
  removeGenre(l, "A");
  CHECK(l.first == nullptr);
}

TEST(remove_dua_kali_yang_kedua_tidak_berpengaruh) {
  GenreList l = rawList({"A", "B", "C"});
  removeGenre(l, "B");
  removeGenre(l, "B");
  CHECK_NAMES(l, "A,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(remove_genre_hilang_dari_pencarian_lainnya_tetap_ada) {
  GenreList l = rawList({"A", "B", "C"});
  removeGenre(l, "B");
  CHECK(!doesGenreExist(l, "B"));
  CHECK(searchGenre(l, "B") == nullptr);
  CHECK(doesGenreExist(l, "A"));
  CHECK(doesGenreExist(l, "C"));
  rawFree(l);
}

TEST(remove_semua_genre_urutan_depan_ke_belakang) {
  GenreList l = rawList({"A", "B", "C", "D", "E"});
  const char* order[] = {"A", "B", "C", "D", "E"};
  for (int i = 0; i < 5; ++i) {
    removeGenre(l, order[i]);
    CHECK_LIST(l);
  }
  CHECK(l.first == nullptr);
}

TEST(remove_semua_genre_urutan_belakang_ke_depan) {
  GenreList l = rawList({"A", "B", "C", "D", "E"});
  const char* order[] = {"E", "D", "C", "B", "A"};
  for (int i = 0; i < 5; ++i) {
    removeGenre(l, order[i]);
    CHECK_LIST(l);
  }
  CHECK(l.first == nullptr);
}

TEST(remove_semua_genre_urutan_acak_tengah_dulu) {
  GenreList l = rawList({"A", "B", "C", "D", "E"});
  const char* order[] = {"C", "A", "E", "B", "D"};
  for (int i = 0; i < 5; ++i) {
    removeGenre(l, order[i]);
    CHECK_LIST(l);
  }
  CHECK(l.first == nullptr);
}

TEST(remove_menghancurkan_elemen_tidak_bocor) {
  long base = th::liveAllocs();
  {
    GenreList l;
    initializeGenreList(l);
    addGenre(l, "Genre pertama dengan nama panjang sekali");
    addGenre(l, "Genre kedua dengan nama panjang sekali");
    addGenre(l, "Genre ketiga dengan nama panjang sekali");
    removeGenre(l, "Genre kedua dengan nama panjang sekali");
    removeGenre(l, "Genre pertama dengan nama panjang sekali");
    removeGenre(l, "Genre ketiga dengan nama panjang sekali");
  }
  CHECK_EQ(th::liveAllocs(), base);
}

// ===========================================================================
// destroyGenreList
// ===========================================================================

TEST(destroy_pada_list_kosong_tidak_crash) {
  GenreList l;
  initializeGenreList(l);
  destroyGenreList(l);
  CHECK(l.first == nullptr);
}

TEST(destroy_membebaskan_semua_elemen) {
  long base = th::liveAllocs();
  GenreList l;
  initializeGenreList(l);
  for (int i = 0; i < 10; ++i) addGenre(l, "Genre dengan nama panjang nomor " + std::to_string(i));
  destroyGenreList(l);
  CHECK_EQ(th::liveAllocs(), base);
  ASSUME(l.first == nullptr);
}

TEST(destroy_satu_elemen) {
  long base = th::liveAllocs();
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Genre dengan nama panjang agar alokasi heap");
  destroyGenreList(l);
  CHECK_EQ(th::liveAllocs(), base);
}

TEST(destroy_lalu_pakai_ulang_list) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "A");
  addGenre(l, "B");
  destroyGenreList(l);
  initializeGenreList(l);
  addGenre(l, "C");
  CHECK_NAMES(l, "C");
  CHECK_LIST(l);
  destroyGenreList(l);
}

// ===========================================================================
// dumpGenreList
// ===========================================================================

TEST(dump_list_kosong_tidak_crash) {
  GenreList l;
  initializeGenreList(l);
  th::CoutCapture cap;
  dumpGenreList(l);
}

TEST(dump_menampilkan_nama_setiap_genre) {
  GenreList l;
  initializeGenreList(l);
  addGenre(l, "Pop");
  addGenre(l, "Rock");
  addGenre(l, "Hip Hop");
  std::string out;
  {
    th::CoutCapture cap;
    dumpGenreList(l);
    out = cap.str();
  }
  ASSUME(th::contains(out, "Pop"));
  ASSUME(th::contains(out, "Rock"));
  ASSUME(th::contains(out, "Hip Hop"));
  rawFree(l);
}

TEST(dump_tidak_mengubah_isi_list) {
  GenreList l = rawList({"A", "B", "C"});
  {
    th::CoutCapture cap;
    dumpGenreList(l);
    dumpGenreList(l);
  }
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// Uji beban & pembandingan dengan model
// ===========================================================================

TEST(beban_add_2000_lalu_remove_yang_ganjil) {
  long base = th::liveAllocs();
  {
    GenreList l;
    initializeGenreList(l);
    for (int i = 0; i < 2000; ++i) addGenre(l, "g" + std::to_string(i));
    for (int i = 1; i < 2000; i += 2) removeGenre(l, "g" + std::to_string(i));
    CHECK_EQ(countGenres(l), 1000);
    CHECK(doesGenreExist(l, "g0"));
    CHECK(!doesGenreExist(l, "g1"));
    CHECK(doesGenreExist(l, "g1998"));
    CHECK_LIST(l);
    destroyGenreList(l);
  }
  CHECK_EQ(th::liveAllocs(), base);
}

TEST(acak_operasi_posisi_dibandingkan_model_vector) {
  std::mt19937 rng(54321);
  GenreList l;
  initializeGenreList(l);
  std::vector<std::string> model;
  int counter = 0;
  for (int step = 0; step < 3000; ++step) {
    int op = static_cast<int>(rng() % 6);
    if (op == 0) {
      std::string nm = "g" + std::to_string(counter++);
      insertGenreFirst(l, rawNode(nm));
      model.insert(model.begin(), nm);
    } else if (op == 1) {
      std::string nm = "g" + std::to_string(counter++);
      insertGenreLast(l, rawNode(nm));
      model.push_back(nm);
    } else if (op == 2) {
      if (model.empty()) continue;
      size_t idx = rng() % model.size();
      std::string nm = "g" + std::to_string(counter++);
      insertGenreAfter(l, nth(l, static_cast<int>(idx)), rawNode(nm));
      model.insert(model.begin() + idx + 1, nm);
    } else if (op == 3) {
      GenreAddress out = nullptr;
      removeFirstGenre(l, out);
      if (model.empty()) {
        CHECK(out == nullptr);
      } else {
        CHECK(out != nullptr);
        if (out != nullptr) {
          CHECK_EQ(out->info.name, model.front());
          delete out;
        }
        model.erase(model.begin());
      }
    } else if (op == 4) {
      GenreAddress out = nullptr;
      removeLastGenre(l, out);
      if (model.empty()) {
        CHECK(out == nullptr);
      } else {
        CHECK(out != nullptr);
        if (out != nullptr) {
          CHECK_EQ(out->info.name, model.back());
          delete out;
        }
        model.pop_back();
      }
    } else {
      if (model.empty()) continue;
      size_t idx = rng() % model.size();
      GenreAddress out = nullptr;
      removeAfterGenre(l, nth(l, static_cast<int>(idx)), out);
      if (idx + 1 < model.size()) {
        CHECK(out != nullptr);
        if (out != nullptr) {
          CHECK_EQ(out->info.name, model[idx + 1]);
          delete out;
        }
        model.erase(model.begin() + idx + 1);
      } else {
        CHECK(out == nullptr);
      }
    }
    std::string why;
    bool ok = genreListValid(l, why) && names(l) == model;
    if (!ok) {
      th::report(false, false, "list == model", __FILE__, __LINE__,
                 "langkah " + std::to_string(step) + ", operasi " + std::to_string(op) + " " + why);
      break;
    }
  }
  rawFree(l);
}

TEST(acak_add_remove_dibandingkan_model_set) {
  std::mt19937 rng(8888);
  GenreList l;
  initializeGenreList(l);
  std::set<std::string> model;
  for (int step = 0; step < 3000; ++step) {
    std::string g = "genre" + std::to_string(rng() % 25);
    bool add = (rng() % 2) == 0;
    if (add) {
      addGenre(l, g);
      model.insert(g);
    } else {
      removeGenre(l, g);
      model.erase(g);
    }
    std::string why;
    bool ok = genreListValid(l, why);
    std::vector<std::string> got = names(l);
    std::set<std::string> gotSet(got.begin(), got.end());
    if (ok && gotSet.size() != got.size()) { ok = false; why = "ada nama ganda di list"; }
    if (ok && gotSet != model) { ok = false; why = "isi list berbeda dengan model"; }
    if (!ok) {
      th::report(false, false, "list == model", __FILE__, __LINE__,
                 "langkah " + std::to_string(step) + (add ? " add " : " remove ") + g + ": " + why);
      break;
    }
    CHECK_EQ(doesGenreExist(l, g), model.count(g) > 0);
  }
  destroyGenreList(l);
}

// ===========================================================================
// Ketahanan terhadap argumen nullptr (rawan crash, sengaja di paling akhir)
// ===========================================================================

TEST(robust_insert_nullptr_tidak_mengubah_list) {
  GenreList l = rawList({"A", "B"});
  insertGenreFirst(l, nullptr);
  insertGenreLast(l, nullptr);
  insertGenreAfter(l, l.first, nullptr);
  ASSUME_EQ(th::join(names(l)), "A,B");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(robust_removeAfter_target_nullptr) {
  GenreList l = rawList({"A", "B"});
  GenreAddress sentinel = rawNode("sentinel");
  GenreAddress out = sentinel;
  removeAfterGenre(l, nullptr, out);
  ASSUME(out == nullptr);
  CHECK_NAMES(l, "A,B");
  delete sentinel;
  rawFree(l);
}

TEST_MAIN()
