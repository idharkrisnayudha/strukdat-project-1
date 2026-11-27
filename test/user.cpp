// test_user.cpp
// Test untuk ADT user (double linked list) pada user.h
//
// Compile (sesuaikan path):
//   g++ -std=c++11 -g -Wall -Wextra -fsanitize=address,undefined test_user.cpp <file implementasi user>.cpp -I<folder user.h> -o test_user
// Jalankan:
//   ./test_user              (atau ./test_user --isolate)
//
// Catatan:
//   - Fixture list dibuat manual lewat pointer (rawList), bukan lewat fungsi ADT,
//     supaya bug di insert*/register* tidak merusak test fungsi lain.
//   - CHECK  = wajib benar. ASSUME = hal yang tidak ditegaskan header; kalau
//     gagal hanya jadi peringatan "[ASUMSI?]", putuskan sendiri itu bug atau bukan.

#include <map>
#include <random>
#include <set>

#include "test_helper.h"
#include "user.h"

namespace {

const int kGuard = 100000;

// ----------------------------- helper struktur -----------------------------

std::vector<std::string> usernames(const UserList& l) {
  std::vector<std::string> v;
  int guard = 0;
  for (UserAddress p = l.first; p != nullptr && guard < kGuard; p = p->next, ++guard)
    v.push_back(p->info.username);
  return v;
}

int countUsers(const UserList& l) { return static_cast<int>(usernames(l).size()); }

// Memeriksa first/last, prev/next konsisten, tidak melingkar.
bool userListValid(const UserList& l, std::string& why) {
  if ((l.first == nullptr) != (l.last == nullptr)) {
    why = "first dan last tidak konsisten (salah satunya nullptr)";
    return false;
  }
  if (l.first == nullptr) return true;
  if (l.first->prev != nullptr) { why = "first->prev bukan nullptr"; return false; }
  if (l.last->next != nullptr) { why = "last->next bukan nullptr"; return false; }
  int n = 0;
  UserAddress p = l.first, before = nullptr;
  while (p != nullptr) {
    if (++n > kGuard) { why = "list melingkar / terlalu panjang"; return false; }
    if (p->prev != before) {
      why = "prev tidak cocok pada elemen ke-" + std::to_string(n);
      return false;
    }
    before = p;
    p = p->next;
  }
  if (before != l.last) { why = "last tidak menunjuk elemen terakhir"; return false; }
  return true;
}

#define CHECK_LIST(l)                                                          \
  do {                                                                         \
    std::string why_;                                                          \
    th::report(userListValid((l), why_), false, "struktur list valid: " #l,    \
               __FILE__, __LINE__, why_);                                      \
  } while (0)

#define CHECK_NAMES(l, expected) CHECK_EQ(th::join(usernames(l)), expected)

// ----------------------------- fixture manual ------------------------------

UserAddress rawNode(const std::string& u, const std::string& type = "free") {
  UserAddress n = new UserElement();
  n->info.username = u;
  n->info.name = "Nama " + u;
  n->info.type = type;
  n->next = nullptr;
  n->prev = nullptr;
  return n;
}

UserList rawList(const std::vector<std::string>& names) {
  UserList l;
  l.first = nullptr;
  l.last = nullptr;
  for (size_t i = 0; i < names.size(); ++i) {
    UserAddress n = rawNode(names[i]);
    if (l.last == nullptr) {
      l.first = l.last = n;
    } else {
      l.last->next = n;
      n->prev = l.last;
      l.last = n;
    }
  }
  return l;
}

// Membebaskan list hanya jika strukturnya valid (menghindari crash tambahan).
void rawFree(UserList& l) {
  std::string why;
  if (userListValid(l, why)) {
    UserAddress p = l.first;
    while (p != nullptr) {
      UserAddress nx = p->next;
      delete p;
      p = nx;
    }
  }
  l.first = nullptr;
  l.last = nullptr;
}

UserAddress nth(const UserList& l, int i) {
  UserAddress p = l.first;
  while (p != nullptr && i-- > 0) p = p->next;
  return p;
}

std::string typeOf(const UserList& l, const std::string& u) {
  for (UserAddress p = l.first; p != nullptr; p = p->next)
    if (p->info.username == u) return p->info.type;
  return "<tidak ada>";
}

std::string nameOf(const UserList& l, const std::string& u) {
  for (UserAddress p = l.first; p != nullptr; p = p->next)
    if (p->info.username == u) return p->info.name;
  return "<tidak ada>";
}

}  // namespace

// ===========================================================================
// initializeUserList / isUserListEmpty / createUserElement
// ===========================================================================

TEST(init_menjadikan_first_dan_last_nullptr) {
  UserList l;
  l.first = reinterpret_cast<UserAddress>(0x10);  // sengaja berisi sampah
  l.last = reinterpret_cast<UserAddress>(0x20);
  initializeUserList(l);
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
}

TEST(isEmpty_true_untuk_list_baru) {
  UserList l;
  initializeUserList(l);
  CHECK(isUserListEmpty(l));
}

TEST(isEmpty_false_untuk_list_berisi_satu_atau_banyak) {
  UserList a = rawList({"A"});
  UserList b = rawList({"A", "B", "C"});
  CHECK(!isUserListEmpty(a));
  CHECK(!isUserListEmpty(b));
  rawFree(a);
  rawFree(b);
}

TEST(create_element_mengisi_info_dan_pointer_null) {
  UserAddress n = createUserElement("andi", "Andi Wicaksono", "premium");
  CHECK(n != nullptr);
  if (n == nullptr) return;
  CHECK_EQ(n->info.username, "andi");
  CHECK_EQ(n->info.name, "Andi Wicaksono");
  CHECK_EQ(n->info.type, "premium");
  CHECK(n->next == nullptr);
  CHECK(n->prev == nullptr);
  delete n;
}

TEST(create_element_dengan_string_kosong_tidak_crash) {
  UserAddress n = createUserElement("", "", "");
  CHECK(n != nullptr);
  if (n == nullptr) return;
  CHECK_EQ(n->info.username, "");
  CHECK(n->next == nullptr);
  CHECK(n->prev == nullptr);
  delete n;
}

// ===========================================================================
// searchUser / doesUserExist
// ===========================================================================

TEST(search_pada_list_kosong_mengembalikan_nullptr) {
  UserList l;
  initializeUserList(l);
  CHECK(searchUser(l, "A") == nullptr);
  CHECK(!doesUserExist(l, "A"));
}

TEST(search_menemukan_elemen_pertama_tengah_terakhir) {
  UserList l = rawList({"A", "B", "C", "D", "E"});
  CHECK(searchUser(l, "A") == nth(l, 0));
  CHECK(searchUser(l, "C") == nth(l, 2));
  CHECK(searchUser(l, "E") == nth(l, 4));
  CHECK(doesUserExist(l, "A"));
  CHECK(doesUserExist(l, "C"));
  CHECK(doesUserExist(l, "E"));
  rawFree(l);
}

TEST(search_username_tidak_ada_mengembalikan_nullptr) {
  UserList l = rawList({"A", "B", "C"});
  CHECK(searchUser(l, "Z") == nullptr);
  CHECK(!doesUserExist(l, "Z"));
  rawFree(l);
}

TEST(search_harus_cocok_penuh_bukan_sebagian) {
  UserList l = rawList({"Andi"});
  CHECK(searchUser(l, "And") == nullptr);    // awalan saja
  CHECK(searchUser(l, "ndi") == nullptr);    // akhiran saja
  CHECK(searchUser(l, "Andii") == nullptr);  // lebih panjang
  CHECK(searchUser(l, "") == nullptr);       // string kosong
  CHECK(!doesUserExist(l, "And"));
  rawFree(l);
}

TEST(search_berdasarkan_username_bukan_nama) {
  UserList l = rawList({"andi"});  // name = "Nama andi"
  CHECK(searchUser(l, "Nama andi") == nullptr);
  CHECK(!doesUserExist(l, "Nama andi"));
  rawFree(l);
}

TEST(search_membedakan_huruf_besar_kecil) {
  UserList l = rawList({"Andi"});
  ASSUME(searchUser(l, "andi") == nullptr);
  ASSUME(!doesUserExist(l, "ANDI"));
  rawFree(l);
}

TEST(search_tidak_mengubah_list) {
  UserList l = rawList({"A", "B", "C"});
  searchUser(l, "B");
  doesUserExist(l, "Z");
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// insertUserFirst
// ===========================================================================

TEST(insertFirst_ke_list_kosong) {
  UserList l;
  initializeUserList(l);
  UserAddress a = rawNode("A");
  insertUserFirst(l, a);
  CHECK(l.first == a);
  CHECK(l.last == a);
  CHECK(a->next == nullptr);
  CHECK(a->prev == nullptr);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertFirst_ke_list_berisi_menaruh_di_depan) {
  UserList l = rawList({"B", "C"});
  UserAddress b = l.first, c = l.last;
  UserAddress a = rawNode("A");
  insertUserFirst(l, a);
  CHECK(l.first == a);
  CHECK(l.last == c);
  CHECK(a->next == b);
  CHECK(b->prev == a);
  CHECK(a->prev == nullptr);
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertFirst_berulang_membalik_urutan) {
  UserList l;
  initializeUserList(l);
  insertUserFirst(l, rawNode("A"));
  insertUserFirst(l, rawNode("B"));
  insertUserFirst(l, rawNode("C"));
  CHECK_NAMES(l, "C,B,A");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// insertUserLast
// ===========================================================================

TEST(insertLast_ke_list_kosong) {
  UserList l;
  initializeUserList(l);
  UserAddress a = rawNode("A");
  insertUserLast(l, a);
  CHECK(l.first == a);
  CHECK(l.last == a);
  CHECK(a->next == nullptr);
  CHECK(a->prev == nullptr);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertLast_ke_list_berisi_menaruh_di_belakang) {
  UserList l = rawList({"A", "B"});
  UserAddress a = l.first, b = l.last;
  UserAddress c = rawNode("C");
  insertUserLast(l, c);
  CHECK(l.first == a);
  CHECK(l.last == c);
  CHECK(b->next == c);
  CHECK(c->prev == b);
  CHECK(c->next == nullptr);
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertLast_berulang_menjaga_urutan) {
  UserList l;
  initializeUserList(l);
  insertUserLast(l, rawNode("A"));
  insertUserLast(l, rawNode("B"));
  insertUserLast(l, rawNode("C"));
  insertUserLast(l, rawNode("D"));
  CHECK_NAMES(l, "A,B,C,D");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// insertUserAfter
// ===========================================================================

TEST(insertAfter_di_tengah_list) {
  UserList l = rawList({"A", "C"});
  UserAddress a = l.first, c = l.last;
  UserAddress b = rawNode("B");
  insertUserAfter(l, a, b);
  CHECK(a->next == b);
  CHECK(b->prev == a);
  CHECK(b->next == c);
  CHECK(c->prev == b);
  CHECK(l.first == a);
  CHECK(l.last == c);
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertAfter_target_elemen_terakhir_memperbarui_last) {
  UserList l = rawList({"A", "B"});
  UserAddress b = l.last;
  UserAddress c = rawNode("C");
  insertUserAfter(l, b, c);
  CHECK(l.last == c);
  CHECK(b->next == c);
  CHECK(c->prev == b);
  CHECK(c->next == nullptr);
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertAfter_pada_list_satu_elemen) {
  UserList l = rawList({"A"});
  UserAddress a = l.first;
  UserAddress b = rawNode("B");
  insertUserAfter(l, a, b);
  CHECK(l.first == a);
  CHECK(l.last == b);
  CHECK_NAMES(l, "A,B");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertAfter_pada_elemen_tengah_list_panjang) {
  UserList l = rawList({"A", "B", "D", "E"});
  UserAddress b = nth(l, 1);
  insertUserAfter(l, b, rawNode("C"));
  CHECK_NAMES(l, "A,B,C,D,E");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(insertAfter_berulang_pada_target_sama) {
  UserList l = rawList({"A", "Z"});
  UserAddress a = l.first;
  insertUserAfter(l, a, rawNode("X"));
  insertUserAfter(l, a, rawNode("Y"));
  CHECK_NAMES(l, "A,Y,X,Z");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// removeFirstUser
// ===========================================================================

TEST(removeFirst_list_kosong) {
  UserList l;
  initializeUserList(l);
  UserAddress sentinel = rawNode("sentinel");
  UserAddress out = sentinel;
  removeFirstUser(l, out);
  ASSUME(out == nullptr);
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
  delete sentinel;
}

TEST(removeFirst_satu_elemen_membuat_list_kosong) {
  UserList l = rawList({"A"});
  UserAddress a = l.first;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeFirstUser(l, out);
  CHECK(out == a);
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
  CHECK(isUserListEmpty(l));
  CHECK_EQ(th::liveAllocs(), live);  // hanya dikeluarkan, tidak dihancurkan
  FREE_IF_ALIVE(a, live);
}

TEST(removeFirst_banyak_elemen) {
  UserList l = rawList({"A", "B", "C"});
  UserAddress a = l.first, b = nth(l, 1), c = l.last;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeFirstUser(l, out);
  CHECK(out == a);
  CHECK(l.first == b);
  CHECK(l.last == c);
  CHECK(b->prev == nullptr);
  CHECK_NAMES(l, "B,C");
  CHECK_LIST(l);
  CHECK_EQ(th::liveAllocs(), live);
  if (th::liveAllocs() == live) {
    CHECK_EQ(a->info.username, "A");  // data elemen yang keluar masih utuh
    ASSUME(a->next == nullptr);       // elemen keluar sebaiknya terlepas
    ASSUME(a->prev == nullptr);
  }
  FREE_IF_ALIVE(a, live);
  rawFree(l);
}

TEST(removeFirst_dua_elemen_menyisakan_satu) {
  UserList l = rawList({"A", "B"});
  UserAddress a = l.first, b = l.last;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeFirstUser(l, out);
  CHECK(out == a);
  CHECK(l.first == b);
  CHECK(l.last == b);
  CHECK(b->prev == nullptr);
  CHECK(b->next == nullptr);
  CHECK_LIST(l);
  FREE_IF_ALIVE(a, live);
  rawFree(l);
}

TEST(removeFirst_berulang_sampai_kosong_urutan_benar) {
  UserList l = rawList({"A", "B", "C", "D"});
  std::string got;
  for (int i = 0; i < 4; ++i) {
    UserAddress out = nullptr;
    long live = th::liveAllocs();
    removeFirstUser(l, out);
    bool intact = (th::liveAllocs() == live);
    CHECK(out != nullptr);
    if (out == nullptr) break;
    got += out->info.username;
    CHECK_LIST(l);
    if (intact) delete out;
  }
  CHECK_EQ(got, "ABCD");
  CHECK(isUserListEmpty(l));
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
  UserAddress out = nullptr;  // satu kali lagi pada list yang sudah kosong
  removeFirstUser(l, out);
  CHECK(out == nullptr);
}

// ===========================================================================
// removeLastUser
// ===========================================================================

TEST(removeLast_list_kosong) {
  UserList l;
  initializeUserList(l);
  UserAddress sentinel = rawNode("sentinel");
  UserAddress out = sentinel;
  removeLastUser(l, out);
  ASSUME(out == nullptr);
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
  delete sentinel;
}

TEST(removeLast_satu_elemen_membuat_list_kosong) {
  UserList l = rawList({"A"});
  UserAddress a = l.first;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeLastUser(l, out);
  CHECK(out == a);
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
  CHECK_EQ(th::liveAllocs(), live);
  FREE_IF_ALIVE(a, live);
}

TEST(removeLast_banyak_elemen) {
  UserList l = rawList({"A", "B", "C"});
  UserAddress a = l.first, b = nth(l, 1), c = l.last;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeLastUser(l, out);
  CHECK(out == c);
  CHECK(l.first == a);
  CHECK(l.last == b);
  CHECK(b->next == nullptr);
  CHECK_NAMES(l, "A,B");
  CHECK_LIST(l);
  CHECK_EQ(th::liveAllocs(), live);
  if (th::liveAllocs() == live) {
    CHECK_EQ(c->info.username, "C");
    ASSUME(c->next == nullptr);
    ASSUME(c->prev == nullptr);
  }
  FREE_IF_ALIVE(c, live);
  rawFree(l);
}

TEST(removeLast_dua_elemen_menyisakan_satu) {
  UserList l = rawList({"A", "B"});
  UserAddress a = l.first, b = l.last;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeLastUser(l, out);
  CHECK(out == b);
  CHECK(l.first == a);
  CHECK(l.last == a);
  CHECK(a->next == nullptr);
  CHECK(a->prev == nullptr);
  CHECK_LIST(l);
  FREE_IF_ALIVE(b, live);
  rawFree(l);
}

TEST(removeLast_berulang_sampai_kosong_urutan_benar) {
  UserList l = rawList({"A", "B", "C", "D"});
  std::string got;
  for (int i = 0; i < 4; ++i) {
    UserAddress out = nullptr;
    long live = th::liveAllocs();
    removeLastUser(l, out);
    bool intact = (th::liveAllocs() == live);
    CHECK(out != nullptr);
    if (out == nullptr) break;
    got += out->info.username;
    CHECK_LIST(l);
    if (intact) delete out;
  }
  CHECK_EQ(got, "DCBA");
  CHECK(isUserListEmpty(l));
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
}

// ===========================================================================
// removeAfterUser
// ===========================================================================

TEST(removeAfter_target_elemen_pertama) {
  UserList l = rawList({"A", "B", "C"});
  UserAddress a = l.first, b = nth(l, 1), c = l.last;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeAfterUser(l, a, out);
  CHECK(out == b);
  CHECK(a->next == c);
  CHECK(c->prev == a);
  CHECK(l.first == a);
  CHECK(l.last == c);
  CHECK_NAMES(l, "A,C");
  CHECK_LIST(l);
  CHECK_EQ(th::liveAllocs(), live);
  if (th::liveAllocs() == live) {
    CHECK_EQ(b->info.username, "B");
    ASSUME(b->next == nullptr);
    ASSUME(b->prev == nullptr);
  }
  FREE_IF_ALIVE(b, live);
  rawFree(l);
}

TEST(removeAfter_elemen_yang_dikeluarkan_adalah_last_memperbarui_last) {
  UserList l = rawList({"A", "B", "C"});
  UserAddress b = nth(l, 1), c = l.last;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeAfterUser(l, b, out);
  CHECK(out == c);
  CHECK(l.last == b);
  CHECK(b->next == nullptr);
  CHECK_NAMES(l, "A,B");
  CHECK_LIST(l);
  FREE_IF_ALIVE(c, live);
  rawFree(l);
}

TEST(removeAfter_dua_elemen_target_pertama) {
  UserList l = rawList({"A", "B"});
  UserAddress a = l.first, b = l.last;
  UserAddress out = nullptr;
  long live = th::liveAllocs();
  removeAfterUser(l, a, out);
  CHECK(out == b);
  CHECK(l.first == a);
  CHECK(l.last == a);
  CHECK(a->next == nullptr);
  CHECK(a->prev == nullptr);
  CHECK_LIST(l);
  FREE_IF_ALIVE(b, live);
  rawFree(l);
}

TEST(removeAfter_target_elemen_terakhir_tidak_ada_yang_dikeluarkan) {
  UserList l = rawList({"A", "B", "C"});
  UserAddress c = l.last;
  UserAddress sentinel = rawNode("sentinel");
  UserAddress out = sentinel;
  removeAfterUser(l, c, out);
  ASSUME(out == nullptr);
  CHECK_NAMES(l, "A,B,C");  // list tidak berubah
  CHECK_LIST(l);
  delete sentinel;
  rawFree(l);
}

TEST(removeAfter_pada_list_satu_elemen) {
  UserList l = rawList({"A"});
  UserAddress sentinel = rawNode("sentinel");
  UserAddress out = sentinel;
  removeAfterUser(l, l.first, out);
  ASSUME(out == nullptr);
  CHECK_NAMES(l, "A");
  CHECK_LIST(l);
  delete sentinel;
  rawFree(l);
}

// ===========================================================================
// registerUser
// ===========================================================================

TEST(register_ke_list_kosong_mengisi_semua_field) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "andi", "Andi Wicaksono");
  CHECK_EQ(countUsers(l), 1);
  CHECK(l.first != nullptr);
  if (l.first == nullptr) return;
  CHECK_EQ(l.first->info.username, "andi");
  CHECK_EQ(l.first->info.name, "Andi Wicaksono");
  CHECK_EQ(l.first->info.type, "free");  // tipe default
  CHECK(l.first == l.last);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(register_empat_user_semuanya_ada_dan_bertipe_free) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "Andi", "Andi Wicaksono");
  registerUser(l, "Budi", "Budi Gunawan");
  registerUser(l, "Cika", "Cika Yunita");
  registerUser(l, "Dodi", "Dodi Supriadi");
  CHECK_EQ(countUsers(l), 4);
  CHECK(doesUserExist(l, "Andi"));
  CHECK(doesUserExist(l, "Budi"));
  CHECK(doesUserExist(l, "Cika"));
  CHECK(doesUserExist(l, "Dodi"));
  CHECK_EQ(nameOf(l, "Cika"), "Cika Yunita");
  CHECK_EQ(typeOf(l, "Andi"), "free");
  CHECK_EQ(typeOf(l, "Dodi"), "free");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(register_urutan_user_baru_ditaruh_di_belakang) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "A", "a");
  registerUser(l, "B", "b");
  registerUser(l, "C", "c");
  ASSUME_EQ(th::join(usernames(l)), "A,B,C");  // header tidak menyebut posisi
  rawFree(l);
}

TEST(register_tipe_default_free_walau_user_sebelumnya_premium) {
  UserList l = rawList({"A"});
  l.first->info.type = "premium";
  registerUser(l, "B", "Bambang");
  CHECK_EQ(typeOf(l, "B"), "free");
  CHECK_EQ(typeOf(l, "A"), "premium");
  rawFree(l);
}

TEST(register_username_duplikat_dibatalkan) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "Andi", "Andi Wicaksono");
  registerUser(l, "Andi", "Orang Lain");
  CHECK_EQ(countUsers(l), 1);
  CHECK_EQ(nameOf(l, "Andi"), "Andi Wicaksono");  // data asli tidak tertimpa
  CHECK_LIST(l);
  rawFree(l);
}

TEST(register_duplikat_tidak_menimpa_perubahan_sebelumnya) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "Andi", "Andi");
  setUserType(l, "Andi", "premium");
  setUserName(l, "Andi", "Andi Baru");
  registerUser(l, "Andi", "Andi Lama");
  CHECK_EQ(countUsers(l), 1);
  CHECK_EQ(typeOf(l, "Andi"), "premium");
  CHECK_EQ(nameOf(l, "Andi"), "Andi Baru");
  rawFree(l);
}

TEST(register_duplikat_di_posisi_tengah_dan_akhir_juga_ditolak) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "A", "a");
  registerUser(l, "B", "b");
  registerUser(l, "C", "c");
  registerUser(l, "B", "b2");
  registerUser(l, "C", "c2");
  registerUser(l, "A", "a2");
  CHECK_EQ(countUsers(l), 3);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(register_duplikat_tidak_bocor_memori) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "A", "Nama A");
  long live = th::liveAllocs();
  registerUser(l, "A", "Nama lain yang cukup panjang supaya heap");
  CHECK_EQ(th::liveAllocs(), live);  // elemen yang ditolak tidak boleh menggantung
  rawFree(l);
}

TEST(register_nama_sama_username_beda_diperbolehkan) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "andi1", "Andi");
  registerUser(l, "andi2", "Andi");
  CHECK_EQ(countUsers(l), 2);  // yang unik adalah username, bukan nama
  rawFree(l);
}

TEST(register_username_beda_huruf_besar_kecil_dianggap_beda) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "Andi", "a");
  registerUser(l, "andi", "b");
  ASSUME_EQ(countUsers(l), 2);
  rawFree(l);
}

TEST(register_ulang_setelah_deactivate_boleh) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "Andi", "Lama");
  deactivateUser(l, "Andi");
  registerUser(l, "Andi", "Baru");
  CHECK_EQ(countUsers(l), 1);
  CHECK_EQ(nameOf(l, "Andi"), "Baru");
  CHECK_EQ(typeOf(l, "Andi"), "free");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(register_banyak_user_500) {
  UserList l;
  initializeUserList(l);
  for (int i = 0; i < 500; ++i) registerUser(l, "u" + std::to_string(i), "Nama");
  CHECK_EQ(countUsers(l), 500);
  CHECK(doesUserExist(l, "u0"));
  CHECK(doesUserExist(l, "u250"));
  CHECK(doesUserExist(l, "u499"));
  CHECK(!doesUserExist(l, "u500"));
  CHECK_LIST(l);
  rawFree(l);
}

TEST(register_username_kosong_tidak_merusak_list) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "A", "a");
  registerUser(l, "", "kosong");
  registerUser(l, "", "kosong lagi");
  CHECK_LIST(l);  // boleh diterima / ditolak, yang penting list tetap valid
  rawFree(l);
}

// ===========================================================================
// setUserType
// ===========================================================================

TEST(setType_free_ke_premium_dan_kembali) {
  UserList l = rawList({"A", "B", "C"});
  setUserType(l, "B", "premium");
  CHECK_EQ(typeOf(l, "B"), "premium");
  setUserType(l, "B", "free");
  CHECK_EQ(typeOf(l, "B"), "free");
  rawFree(l);
}

TEST(setType_hanya_target_yang_berubah) {
  UserList l = rawList({"A", "B", "C"});
  setUserType(l, "B", "premium");
  CHECK_EQ(typeOf(l, "A"), "free");
  CHECK_EQ(typeOf(l, "C"), "free");
  CHECK_EQ(nameOf(l, "B"), "Nama B");  // field lain tidak ikut berubah
  CHECK_NAMES(l, "A,B,C");
  rawFree(l);
}

TEST(setType_posisi_pertama_tengah_terakhir) {
  UserList l = rawList({"A", "B", "C"});
  setUserType(l, "A", "premium");
  setUserType(l, "B", "premium");
  setUserType(l, "C", "premium");
  CHECK_EQ(typeOf(l, "A"), "premium");
  CHECK_EQ(typeOf(l, "B"), "premium");
  CHECK_EQ(typeOf(l, "C"), "premium");
  rawFree(l);
}

TEST(setType_dua_kali_nilai_sama_idempoten) {
  UserList l = rawList({"A"});
  setUserType(l, "A", "premium");
  setUserType(l, "A", "premium");
  CHECK_EQ(typeOf(l, "A"), "premium");
  rawFree(l);
}

TEST(setType_user_tidak_ada_tidak_mengubah_apa_apa) {
  UserList l = rawList({"A", "B"});
  setUserType(l, "Z", "premium");
  CHECK_EQ(typeOf(l, "A"), "free");
  CHECK_EQ(typeOf(l, "B"), "free");
  CHECK_NAMES(l, "A,B");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(setType_pada_list_kosong_tidak_crash) {
  UserList l;
  initializeUserList(l);
  setUserType(l, "A", "premium");
  CHECK(isUserListEmpty(l));
}

TEST(setType_tipe_tidak_valid_ditolak) {
  UserList l = rawList({"A"});
  setUserType(l, "A", "gold");
  ASSUME_EQ(typeOf(l, "A"), "free");  // header: tipe hanya "free" atau "premium"
  setUserType(l, "A", "");
  ASSUME_EQ(typeOf(l, "A"), "free");
  rawFree(l);
}

// ===========================================================================
// setUserName
// ===========================================================================

TEST(setName_mengubah_nama_saja) {
  UserList l = rawList({"A", "B", "C"});
  setUserType(l, "B", "premium");
  setUserName(l, "B", "Budi Baru");
  CHECK_EQ(nameOf(l, "B"), "Budi Baru");
  CHECK_EQ(typeOf(l, "B"), "premium");  // tipe tidak ikut berubah
  CHECK_NAMES(l, "A,B,C");              // username tidak berubah
  CHECK_EQ(nameOf(l, "A"), "Nama A");
  CHECK_EQ(nameOf(l, "C"), "Nama C");
  rawFree(l);
}

TEST(setName_posisi_pertama_dan_terakhir) {
  UserList l = rawList({"A", "B", "C"});
  setUserName(l, "A", "Awal");
  setUserName(l, "C", "Akhir");
  CHECK_EQ(nameOf(l, "A"), "Awal");
  CHECK_EQ(nameOf(l, "C"), "Akhir");
  CHECK_EQ(nameOf(l, "B"), "Nama B");
  rawFree(l);
}

TEST(setName_dengan_spasi_dan_string_panjang) {
  UserList l = rawList({"A"});
  std::string panjang(300, 'x');
  setUserName(l, "A", "Dodi Caknan");
  CHECK_EQ(nameOf(l, "A"), "Dodi Caknan");
  setUserName(l, "A", panjang);
  CHECK_EQ(nameOf(l, "A"), panjang);
  rawFree(l);
}

TEST(setName_user_tidak_ada_tidak_mengubah_apa_apa) {
  UserList l = rawList({"A", "B"});
  setUserName(l, "Z", "Siapa");
  CHECK_EQ(nameOf(l, "A"), "Nama A");
  CHECK_EQ(nameOf(l, "B"), "Nama B");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(setName_pada_list_kosong_tidak_crash) {
  UserList l;
  initializeUserList(l);
  setUserName(l, "A", "Baru");
  CHECK(isUserListEmpty(l));
}

TEST(setName_tidak_bocor_memori) {
  UserList l = rawList({"A"});
  setUserName(l, "A", "nama awal yang cukup panjang untuk heap");
  long live = th::liveAllocs();
  for (int i = 0; i < 20; ++i) setUserName(l, "A", "nama berikutnya yang juga panjang sekali");
  CHECK_EQ(th::liveAllocs(), live);
  rawFree(l);
}

// ===========================================================================
// deactivateUser
// ===========================================================================

TEST(deactivate_pada_list_kosong_tidak_crash) {
  UserList l;
  initializeUserList(l);
  deactivateUser(l, "A");
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
}

TEST(deactivate_user_tidak_ada_tidak_mengubah_list) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "A", "a");
  registerUser(l, "B", "b");
  deactivateUser(l, "Z");
  CHECK_EQ(countUsers(l), 2);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(deactivate_satu_satunya_user_membuat_list_kosong) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "A", "a");
  deactivateUser(l, "A");
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
  CHECK(isUserListEmpty(l));
}

TEST(deactivate_elemen_pertama) {
  UserList l = rawList({"A", "B", "C"});
  UserAddress b = nth(l, 1);
  deactivateUser(l, "A");
  CHECK_NAMES(l, "B,C");
  CHECK(l.first == b);
  CHECK(b->prev == nullptr);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(deactivate_elemen_terakhir) {
  UserList l = rawList({"A", "B", "C"});
  UserAddress b = nth(l, 1);
  deactivateUser(l, "C");
  CHECK_NAMES(l, "A,B");
  CHECK(l.last == b);
  CHECK(b->next == nullptr);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(deactivate_elemen_tengah) {
  UserList l = rawList({"A", "B", "C"});
  UserAddress a = l.first, c = l.last;
  deactivateUser(l, "B");
  CHECK_NAMES(l, "A,C");
  CHECK(a->next == c);
  CHECK(c->prev == a);
  CHECK(l.first == a);
  CHECK(l.last == c);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(deactivate_dua_elemen_hapus_satu_per_satu) {
  UserList l = rawList({"A", "B"});
  deactivateUser(l, "A");
  CHECK_NAMES(l, "B");
  CHECK(l.first == l.last);
  CHECK_LIST(l);
  deactivateUser(l, "B");
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
}

TEST(deactivate_skenario_test_lama_budi_andi_dodi) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "Andi", "Andi Wicaksono");
  registerUser(l, "Budi", "Budi Gunawan");
  registerUser(l, "Cika", "Cika Yunita");
  registerUser(l, "Dodi", "Dodi Supriadi");
  setUserType(l, "Andi", "premium");
  setUserType(l, "Cika", "premium");
  setUserName(l, "Dodi", "Dodi Caknan");
  deactivateUser(l, "Budi");
  deactivateUser(l, "Andi");
  deactivateUser(l, "Dodi");
  CHECK_EQ(countUsers(l), 1);
  CHECK(doesUserExist(l, "Cika"));
  CHECK_EQ(typeOf(l, "Cika"), "premium");  // data user yang tersisa tetap utuh
  CHECK_EQ(nameOf(l, "Cika"), "Cika Yunita");
  CHECK(l.first == l.last);
  CHECK_LIST(l);
  rawFree(l);
}

TEST(deactivate_dua_kali_yang_kedua_tidak_berpengaruh) {
  UserList l = rawList({"A", "B", "C"});
  deactivateUser(l, "B");
  deactivateUser(l, "B");
  CHECK_NAMES(l, "A,C");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(deactivate_user_hilang_dari_pencarian_dan_lainnya_utuh) {
  UserList l = rawList({"A", "B", "C"});
  setUserType(l, "C", "premium");
  deactivateUser(l, "B");
  CHECK(!doesUserExist(l, "B"));
  CHECK(searchUser(l, "B") == nullptr);
  CHECK(doesUserExist(l, "A"));
  CHECK(doesUserExist(l, "C"));
  CHECK_EQ(typeOf(l, "C"), "premium");
  rawFree(l);
}

TEST(deactivate_semua_user_urutan_depan_ke_belakang) {
  UserList l = rawList({"A", "B", "C", "D", "E"});
  const char* order[] = {"A", "B", "C", "D", "E"};
  for (int i = 0; i < 5; ++i) {
    deactivateUser(l, order[i]);
    CHECK_LIST(l);
  }
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
}

TEST(deactivate_semua_user_urutan_belakang_ke_depan) {
  UserList l = rawList({"A", "B", "C", "D", "E"});
  const char* order[] = {"E", "D", "C", "B", "A"};
  for (int i = 0; i < 5; ++i) {
    deactivateUser(l, order[i]);
    CHECK_LIST(l);
  }
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
}

TEST(deactivate_semua_user_urutan_acak_tengah_dulu) {
  UserList l = rawList({"A", "B", "C", "D", "E"});
  const char* order[] = {"C", "A", "E", "B", "D"};
  for (int i = 0; i < 5; ++i) {
    deactivateUser(l, order[i]);
    CHECK_LIST(l);
  }
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
}

TEST(deactivate_menghancurkan_elemen_tidak_bocor) {
  long base = th::liveAllocs();
  {
    UserList l;
    initializeUserList(l);
    registerUser(l, "Andi", "Andi Wicaksono yang namanya panjang");
    registerUser(l, "Budi", "Budi Gunawan yang namanya panjang");
    registerUser(l, "Cika", "Cika Yunita yang namanya panjang");
    deactivateUser(l, "Budi");
    deactivateUser(l, "Andi");
    deactivateUser(l, "Cika");
  }
  CHECK_EQ(th::liveAllocs(), base);
}

// ===========================================================================
// destroyUserList
// ===========================================================================

TEST(destroy_pada_list_kosong_tidak_crash) {
  UserList l;
  initializeUserList(l);
  destroyUserList(l);
  CHECK(l.first == nullptr);
  CHECK(l.last == nullptr);
}

TEST(destroy_membebaskan_semua_elemen) {
  long base = th::liveAllocs();
  UserList l;
  initializeUserList(l);
  for (int i = 0; i < 10; ++i) registerUser(l, "u" + std::to_string(i), "Nama panjang supaya alokasi heap");
  destroyUserList(l);
  CHECK_EQ(th::liveAllocs(), base);
  ASSUME(l.first == nullptr);
  ASSUME(l.last == nullptr);
}

TEST(destroy_satu_elemen) {
  long base = th::liveAllocs();
  UserList l;
  initializeUserList(l);
  registerUser(l, "A", "Nama panjang supaya alokasi heap");
  destroyUserList(l);
  CHECK_EQ(th::liveAllocs(), base);
}

TEST(destroy_lalu_pakai_ulang_list) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "A", "a");
  registerUser(l, "B", "b");
  destroyUserList(l);
  initializeUserList(l);
  registerUser(l, "C", "c");
  CHECK_NAMES(l, "C");
  CHECK_LIST(l);
  destroyUserList(l);
}

// ===========================================================================
// dumpUserList
// ===========================================================================

TEST(dump_list_kosong_tidak_crash) {
  UserList l;
  initializeUserList(l);
  th::CoutCapture cap;
  dumpUserList(l);
}

TEST(dump_menampilkan_info_setiap_user) {
  UserList l;
  initializeUserList(l);
  registerUser(l, "Andi", "Andi Wicaksono");
  registerUser(l, "Budi", "Budi Gunawan");
  setUserType(l, "Budi", "premium");
  std::string out;
  {
    th::CoutCapture cap;
    dumpUserList(l);
    out = cap.str();
  }
  ASSUME(th::contains(out, "Andi"));
  ASSUME(th::contains(out, "Andi Wicaksono"));
  ASSUME(th::contains(out, "Budi"));
  ASSUME(th::contains(out, "Budi Gunawan"));
  ASSUME(th::contains(out, "premium"));
  ASSUME(th::contains(out, "free"));
  rawFree(l);
}

TEST(dump_tidak_mengubah_isi_list) {
  UserList l = rawList({"A", "B", "C"});
  {
    th::CoutCapture cap;
    dumpUserList(l);
    dumpUserList(l);
  }
  CHECK_NAMES(l, "A,B,C");
  CHECK_LIST(l);
  rawFree(l);
}

// ===========================================================================
// Uji beban & pembandingan dengan model
// ===========================================================================

TEST(beban_register_2000_lalu_deactivate_yang_ganjil) {
  long base = th::liveAllocs();
  {
    UserList l;
    initializeUserList(l);
    for (int i = 0; i < 2000; ++i) registerUser(l, "u" + std::to_string(i), "Nama");
    for (int i = 1; i < 2000; i += 2) deactivateUser(l, "u" + std::to_string(i));
    CHECK_EQ(countUsers(l), 1000);
    CHECK(doesUserExist(l, "u0"));
    CHECK(!doesUserExist(l, "u1"));
    CHECK(doesUserExist(l, "u1998"));
    CHECK_LIST(l);
    destroyUserList(l);
  }
  CHECK_EQ(th::liveAllocs(), base);
}

TEST(acak_operasi_posisi_dibandingkan_model_vector) {
  std::mt19937 rng(12345);
  UserList l;
  initializeUserList(l);
  std::vector<std::string> model;
  int counter = 0;
  for (int step = 0; step < 3000; ++step) {
    int op = static_cast<int>(rng() % 6);
    if (op == 0) {
      std::string nm = "u" + std::to_string(counter++);
      insertUserFirst(l, rawNode(nm));
      model.insert(model.begin(), nm);
    } else if (op == 1) {
      std::string nm = "u" + std::to_string(counter++);
      insertUserLast(l, rawNode(nm));
      model.push_back(nm);
    } else if (op == 2) {
      if (model.empty()) continue;
      size_t idx = rng() % model.size();
      std::string nm = "u" + std::to_string(counter++);
      insertUserAfter(l, nth(l, static_cast<int>(idx)), rawNode(nm));
      model.insert(model.begin() + idx + 1, nm);
    } else if (op == 3) {
      UserAddress out = nullptr;
      removeFirstUser(l, out);
      if (model.empty()) {
        CHECK(out == nullptr);
      } else {
        CHECK(out != nullptr);
        if (out != nullptr) {
          CHECK_EQ(out->info.username, model.front());
          delete out;
        }
        model.erase(model.begin());
      }
    } else if (op == 4) {
      UserAddress out = nullptr;
      removeLastUser(l, out);
      if (model.empty()) {
        CHECK(out == nullptr);
      } else {
        CHECK(out != nullptr);
        if (out != nullptr) {
          CHECK_EQ(out->info.username, model.back());
          delete out;
        }
        model.pop_back();
      }
    } else {
      if (model.empty()) continue;
      size_t idx = rng() % model.size();
      UserAddress out = nullptr;
      removeAfterUser(l, nth(l, static_cast<int>(idx)), out);
      if (idx + 1 < model.size()) {
        CHECK(out != nullptr);
        if (out != nullptr) {
          CHECK_EQ(out->info.username, model[idx + 1]);
          delete out;
        }
        model.erase(model.begin() + idx + 1);
      } else {
        CHECK(out == nullptr);
      }
    }
    std::string why;
    bool ok = userListValid(l, why) && usernames(l) == model;
    if (!ok) {
      th::report(false, false, "list == model", __FILE__, __LINE__,
                 "langkah " + std::to_string(step) + ", operasi " + std::to_string(op) + " " + why);
      break;
    }
  }
  rawFree(l);
}

TEST(acak_register_deactivate_setType_setName_dibandingkan_model_map) {
  struct Rec {
    std::string name, type;
  };
  std::mt19937 rng(777);
  UserList l;
  initializeUserList(l);
  std::map<std::string, Rec> model;
  for (int step = 0; step < 3000; ++step) {
    std::string u = "user" + std::to_string(rng() % 25);
    int op = static_cast<int>(rng() % 4);
    if (op == 0) {
      std::string nm = "Nama" + std::to_string(step);
      registerUser(l, u, nm);
      if (!model.count(u)) {
        Rec r = {nm, "free"};
        model[u] = r;
      }
    } else if (op == 1) {
      deactivateUser(l, u);
      model.erase(u);
    } else if (op == 2) {
      std::string t = (rng() % 2) ? "premium" : "free";
      setUserType(l, u, t);
      if (model.count(u)) model[u].type = t;
    } else {
      std::string nm = "Baru" + std::to_string(step);
      setUserName(l, u, nm);
      if (model.count(u)) model[u].name = nm;
    }

    std::string why;
    bool ok = userListValid(l, why);
    if (ok && countUsers(l) != static_cast<int>(model.size())) {
      ok = false;
      why = "jumlah user " + std::to_string(countUsers(l)) + " != model " + std::to_string(model.size());
    }
    std::set<std::string> seen;
    for (UserAddress p = l.first; ok && p != nullptr; p = p->next) {
      std::map<std::string, Rec>::iterator it = model.find(p->info.username);
      if (it == model.end()) { ok = false; why = "user tak dikenal di list: " + p->info.username; }
      else if (!seen.insert(p->info.username).second) { ok = false; why = "username ganda: " + p->info.username; }
      else if (it->second.name != p->info.name) { ok = false; why = "nama salah untuk " + p->info.username; }
      else if (it->second.type != p->info.type) { ok = false; why = "tipe salah untuk " + p->info.username; }
    }
    if (!ok) {
      th::report(false, false, "list == model", __FILE__, __LINE__,
                 "langkah " + std::to_string(step) + " operasi " + std::to_string(op) + ": " + why);
      break;
    }
  }
  destroyUserList(l);
}

// ===========================================================================
// Ketahanan terhadap argumen nullptr (rawan crash, sengaja di paling akhir)
// ===========================================================================

TEST(robust_insert_nullptr_tidak_mengubah_list) {
  UserList l = rawList({"A", "B"});
  insertUserFirst(l, nullptr);
  insertUserLast(l, nullptr);
  insertUserAfter(l, l.first, nullptr);
  ASSUME_EQ(th::join(usernames(l)), "A,B");
  CHECK_LIST(l);
  rawFree(l);
}

TEST(robust_removeAfter_target_nullptr) {
  UserList l = rawList({"A", "B"});
  UserAddress sentinel = rawNode("sentinel");
  UserAddress out = sentinel;
  removeAfterUser(l, nullptr, out);
  ASSUME(out == nullptr);
  CHECK_NAMES(l, "A,B");
  delete sentinel;
  rawFree(l);
}

TEST_MAIN()
