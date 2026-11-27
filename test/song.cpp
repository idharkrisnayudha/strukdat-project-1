// test_song.cpp
// Test untuk ADT song (binary search tree berdasarkan id) pada song.h
//
// Compile (sesuaikan path):
//   g++ -std=c++11 -g -Wall -Wextra -fsanitize=address,undefined test_song.cpp <file implementasi song>.cpp -I<folder song.h> -o test_song
// Jalankan:
//   ./test_song              (atau ./test_song --isolate)
//
// Catatan:
//   - id disimpan sebagai std::string, jadi urutannya leksikografis. Kebanyakan
//     test memakai id dengan panjang sama (mis. "0042") supaya urutan leksikografis
//     = urutan numerik dan tidak ambigu. Satu test ASSUME khusus mengecek "10" vs "9".
//   - Return value deleteSongNode() sengaja TIDAK diperiksa karena header tidak
//     menjelaskan artinya (node baru? node yang dihapus?). Kalau kamu tegaskan
//     artinya, tambahkan pengecekannya.
//   - Fixture tree dibuat manual (rawList) supaya independen dari addSong/insertSongNode.
//   - CHECK = wajib benar. ASSUME = hal yang tidak ditegaskan header (jadi peringatan).

#include <algorithm>
#include <map>
#include <random>

#include "song.h"
#include "test_helper.h"

namespace {

// ----------------------------- helper struktur -----------------------------

std::string sid(int i) {
  char b[16];
  std::snprintf(b, sizeof b, "%04d", i);
  return b;
}

int expectedDuration(const std::string& id) { return 100 + std::atoi(id.c_str()) % 50; }

void inorder(SongAddress n, std::vector<std::string>& out, int depth = 0) {
  if (n == nullptr || depth > 20000) return;
  inorder(n->left, out, depth + 1);
  out.push_back(n->info.id);
  inorder(n->right, out, depth + 1);
}

std::vector<std::string> ids(SongAddress root) {
  std::vector<std::string> v;
  inorder(root, v);
  return v;
}

int countNodes(SongAddress n, int depth = 0) {
  if (n == nullptr || depth > 20000) return 0;
  return 1 + countNodes(n->left, depth + 1) + countNodes(n->right, depth + 1);
}

int height(SongAddress n, int depth = 0) {
  if (n == nullptr || depth > 20000) return 0;
  return 1 + std::max(height(n->left, depth + 1), height(n->right, depth + 1));
}

// Bentuk pohon: "(root kiri kanan)", "." untuk kosong.
std::string shape(SongAddress n, int depth = 0) {
  if (n == nullptr) return ".";
  if (depth > 5000) return "!";
  return "(" + n->info.id + " " + shape(n->left, depth + 1) + " " + shape(n->right, depth + 1) + ")";
}

// Setiap id di kiri < induk < setiap id di kanan (ketat, jadi juga tanpa duplikat).
bool bstValid(SongAddress n, const std::string* lo, const std::string* hi,
              std::string& why, int depth = 0) {
  if (n == nullptr) return true;
  if (depth > 20000) { why = "tree terlalu dalam / melingkar"; return false; }
  if (lo != nullptr && !(*lo < n->info.id)) {
    why = "id " + n->info.id + " seharusnya > " + *lo;
    return false;
  }
  if (hi != nullptr && !(n->info.id < *hi)) {
    why = "id " + n->info.id + " seharusnya < " + *hi;
    return false;
  }
  return bstValid(n->left, lo, &n->info.id, why, depth + 1) &&
         bstValid(n->right, &n->info.id, hi, why, depth + 1);
}

#define CHECK_BST(root)                                                       \
  do {                                                                        \
    std::string why_;                                                         \
    th::report(bstValid((root), nullptr, nullptr, why_), false,               \
               "BST valid: " #root, __FILE__, __LINE__, why_);                \
  } while (0)

#define CHECK_IDS(root, expected) CHECK_EQ(th::join(ids(root)), expected)

std::string idOf(SongAddress n) { return n ? n->info.id : "<null>"; }

SongAddress rawFind(SongAddress n, const std::string& id) {
  int guard = 0;
  while (n != nullptr && guard++ < 20000) {
    if (id == n->info.id) return n;
    n = (id < n->info.id) ? n->left : n->right;
  }
  return nullptr;
}

// ----------------------------- fixture manual ------------------------------

SongAddress rawSong(const std::string& id) {
  SongAddress n = new SongNode();
  n->info.id = id;
  n->info.title = "Judul" + id;
  n->info.artist = "Artis" + id;
  n->info.duration = expectedDuration(id);
  n->left = nullptr;
  n->right = nullptr;
  return n;
}

void rawInsert(SongAddress& root, SongAddress n) {
  SongAddress* p = &root;
  int guard = 0;
  while (*p != nullptr && guard++ < 20000) p = (n->info.id < (*p)->info.id) ? &(*p)->left : &(*p)->right;
  *p = n;
}

SongList rawList(const std::vector<std::string>& idsInOrder) {
  SongList l;
  l.node = nullptr;
  for (size_t i = 0; i < idsInOrder.size(); ++i) rawInsert(l.node, rawSong(idsInOrder[i]));
  return l;
}

void rawFreeNode(SongAddress n) {
  if (n == nullptr) return;
  rawFreeNode(n->left);
  rawFreeNode(n->right);
  delete n;
}

void rawFree(SongList& l) {
  std::string why;
  if (bstValid(l.node, nullptr, nullptr, why)) rawFreeNode(l.node);
  l.node = nullptr;
}

// Semua node SELAIN `exceptId` harus masih berisi data bawaan rawSong().
bool othersUntouched(SongAddress n, const std::string& exceptId) {
  if (n == nullptr) return true;
  if (n->info.id != exceptId) {
    if (n->info.title != "Judul" + n->info.id) return false;
    if (n->info.artist != "Artis" + n->info.id) return false;
    if (n->info.duration != expectedDuration(n->info.id)) return false;
  }
  return othersUntouched(n->left, exceptId) && othersUntouched(n->right, exceptId);
}

// Pohon seimbang 7 node: (50 (30 (20) (40)) (70 (60) (80)))
const char* kBalanced[] = {"50", "30", "70", "20", "40", "60", "80"};
std::vector<std::string> balanced() { return std::vector<std::string>(kBalanced, kBalanced + 7); }

}  // namespace

// ===========================================================================
// initializeSongList / isSongListEmpty / createSongNode
// ===========================================================================

TEST(init_menjadikan_node_nullptr) {
  SongList l;
  l.node = reinterpret_cast<SongAddress>(0x10);  // sengaja berisi sampah
  initializeSongList(l);
  CHECK(l.node == nullptr);
}

TEST(isEmpty_true_untuk_list_baru_false_jika_ada_isi) {
  SongList l;
  initializeSongList(l);
  CHECK(isSongListEmpty(l));
  SongList b = rawList({"0001"});
  CHECK(!isSongListEmpty(b));
  rawFree(b);
}

TEST(create_node_mengisi_info_dan_anak_nullptr) {
  SongAddress n = createSongNode("7", "Judul Lagu", "Artis", 215);
  CHECK(n != nullptr);
  if (n == nullptr) return;
  CHECK_EQ(n->info.id, "7");
  CHECK_EQ(n->info.title, "Judul Lagu");
  CHECK_EQ(n->info.artist, "Artis");
  CHECK_EQ(n->info.duration, 215);
  CHECK(n->left == nullptr);
  CHECK(n->right == nullptr);
  delete n;
}

TEST(create_node_durasi_nol_dan_string_kosong) {
  SongAddress n = createSongNode("", "", "", 0);
  CHECK(n != nullptr);
  if (n == nullptr) return;
  CHECK_EQ(n->info.duration, 0);
  CHECK(n->left == nullptr);
  CHECK(n->right == nullptr);
  delete n;
}

// ===========================================================================
// insertSongNode
// ===========================================================================

TEST(insertNode_ke_tree_kosong_menjadi_root) {
  SongAddress root = nullptr;
  SongAddress a = rawSong("50");
  insertSongNode(root, a);
  CHECK(root == a);
  CHECK_EQ(shape(root), "(50 . .)");
  rawFreeNode(root);
}

TEST(insertNode_lebih_kecil_ke_kiri_lebih_besar_ke_kanan) {
  SongAddress root = nullptr;
  SongAddress a = rawSong("50"), b = rawSong("30"), c = rawSong("70");
  insertSongNode(root, a);
  insertSongNode(root, b);
  insertSongNode(root, c);
  CHECK(root == a);
  CHECK(a->left == b);
  CHECK(a->right == c);
  CHECK_EQ(shape(root), "(50 (30 . .) (70 . .))");
  rawFreeNode(root);
}

TEST(insertNode_bentuk_pohon_seimbang_7_node) {
  SongAddress root = nullptr;
  std::vector<std::string> v = balanced();
  for (size_t i = 0; i < v.size(); ++i) insertSongNode(root, rawSong(v[i]));
  CHECK_EQ(shape(root), "(50 (30 (20 . .) (40 . .)) (70 (60 . .) (80 . .)))");
  CHECK_BST(root);
  rawFreeNode(root);
}

TEST(insertNode_menurun_dan_menaik_membentuk_rantai) {
  SongAddress asc = nullptr, desc = nullptr;
  for (int i = 0; i < 200; ++i) insertSongNode(asc, rawSong(sid(i)));
  for (int i = 199; i >= 0; --i) insertSongNode(desc, rawSong(sid(i)));
  CHECK_EQ(countNodes(asc), 200);
  CHECK_EQ(countNodes(desc), 200);
  CHECK_EQ(height(asc), 200);
  CHECK_EQ(height(desc), 200);
  CHECK_BST(asc);
  CHECK_BST(desc);
  rawFreeNode(asc);
  rawFreeNode(desc);
}

TEST(insertNode_urutan_acak_inorder_terurut) {
  std::mt19937 rng(42);
  for (int round = 0; round < 10; ++round) {
    std::vector<int> nums;
    for (int i = 0; i < 60; ++i) nums.push_back(i);
    std::shuffle(nums.begin(), nums.end(), rng);
    SongAddress root = nullptr;
    for (size_t i = 0; i < nums.size(); ++i) insertSongNode(root, rawSong(sid(nums[i])));
    std::vector<std::string> expected;
    for (int i = 0; i < 60; ++i) expected.push_back(sid(i));
    CHECK(ids(root) == expected);
    CHECK_BST(root);
    rawFreeNode(root);
  }
}

TEST(insertNode_tidak_mengubah_data_node_lama) {
  SongList l = rawList(balanced());
  SongAddress extra = rawSong("55");
  insertSongNode(l.node, extra);
  CHECK(othersUntouched(l.node, ""));
  CHECK_EQ(shape(l.node), "(50 (30 (20 . .) (40 . .)) (70 (60 (55 . .) .) (80 . .)))");
  rawFree(l);
}

TEST(insertNode_id_dibandingkan_sebagai_string_leksikografis) {
  SongAddress root = nullptr;
  insertSongNode(root, rawSong("9"));
  insertSongNode(root, rawSong("10"));
  // "10" < "9" sebagai string. Jika kamu ingin perbandingan numerik, ini wajar gagal.
  ASSUME(root->left != nullptr && root->left->info.id == "10");
  rawFreeNode(root);
}

// ===========================================================================
// addSong
// ===========================================================================

TEST(addSong_pertama_menjadi_root_dengan_field_benar) {
  SongList l;
  initializeSongList(l);
  addSong(l, "0001", "Lagu A1", "A", 3);
  CHECK(l.node != nullptr);
  if (l.node == nullptr) return;
  CHECK_EQ(l.node->info.id, "0001");
  CHECK_EQ(l.node->info.title, "Lagu A1");
  CHECK_EQ(l.node->info.artist, "A");
  CHECK_EQ(l.node->info.duration, 3);
  CHECK(l.node->left == nullptr);
  CHECK(l.node->right == nullptr);
  rawFree(l);
}

TEST(addSong_skenario_test_lama_1_3_2_4) {
  SongList l;
  initializeSongList(l);
  addSong(l, "1", "Lagu A1", "A", 3);
  addSong(l, "3", "Lagu A2", "A", 4);
  addSong(l, "2", "Lagu A2", "A", 2);
  addSong(l, "4", "Lagu B1", "B", 1);
  CHECK_EQ(shape(l.node), "(1 . (3 (2 . .) (4 . .)))");
  CHECK_IDS(l.node, "1,2,3,4");
  CHECK_EQ(rawFind(l.node, "2")->info.duration, 2);
  CHECK_EQ(rawFind(l.node, "4")->info.artist, "B");
  rawFree(l);
}

TEST(addSong_bentuk_pohon_seimbang) {
  SongList l;
  initializeSongList(l);
  std::vector<std::string> v = balanced();
  for (size_t i = 0; i < v.size(); ++i) addSong(l, v[i], "t", "a", 1);
  CHECK_EQ(shape(l.node), "(50 (30 (20 . .) (40 . .)) (70 (60 . .) (80 . .)))");
  rawFree(l);
}

TEST(addSong_semua_field_tersimpan_di_node_yang_benar) {
  SongList l;
  initializeSongList(l);
  addSong(l, "0002", "Dua", "Artis Dua", 20);
  addSong(l, "0001", "Satu", "Artis Satu", 10);
  addSong(l, "0003", "Tiga", "Artis Tiga", 30);
  SongAddress one = rawFind(l.node, "0001");
  SongAddress three = rawFind(l.node, "0003");
  CHECK(one != nullptr && three != nullptr);
  if (one == nullptr || three == nullptr) { rawFree(l); return; }
  CHECK_EQ(one->info.title, "Satu");
  CHECK_EQ(one->info.artist, "Artis Satu");
  CHECK_EQ(one->info.duration, 10);
  CHECK_EQ(three->info.title, "Tiga");
  CHECK_EQ(three->info.duration, 30);
  rawFree(l);
}

TEST(addSong_judul_dan_artis_sama_dengan_id_beda_semuanya_masuk) {
  SongList l;
  initializeSongList(l);
  addSong(l, "1", "Lagu Sama", "Artis Sama", 5);
  addSong(l, "2", "Lagu Sama", "Artis Sama", 5);
  addSong(l, "3", "Lagu Sama", "Artis Sama", 5);
  CHECK_EQ(countNodes(l.node), 3);
  rawFree(l);
}

TEST(addSong_id_duplikat_dibatalkan) {
  SongList l;
  initializeSongList(l);
  addSong(l, "0002", "Asli", "A", 10);
  addSong(l, "0001", "Kiri", "A", 10);
  addSong(l, "0002", "Duplikat", "B", 99);
  addSong(l, "0001", "Duplikat Kiri", "B", 99);
  ASSUME_EQ(countNodes(l.node), 2);
  SongAddress p = rawFind(l.node, "0002");
  CHECK(p != nullptr);
  if (p != nullptr) ASSUME_EQ(p->info.title, "Asli");  // data lama tidak tertimpa
  rawFree(l);
}

TEST(addSong_duplikat_tidak_bocor_memori) {
  SongList l;
  initializeSongList(l);
  addSong(l, "0002", "Asli judul yang panjang sekali", "A", 10);
  int before = countNodes(l.node);
  long live = th::liveAllocs();
  addSong(l, "0002", "Duplikat judul yang panjang sekali", "B", 99);
  if (countNodes(l.node) == before) CHECK_EQ(th::liveAllocs(), live);
  rawFree(l);
}

TEST(addSong_urutan_masuk_berbeda_inorder_selalu_terurut) {
  SongList asc, desc, mix;
  initializeSongList(asc);
  initializeSongList(desc);
  initializeSongList(mix);
  for (int i = 0; i < 100; ++i) addSong(asc, sid(i), "t", "a", 1);
  for (int i = 99; i >= 0; --i) addSong(desc, sid(i), "t", "a", 1);
  for (int i = 0; i < 100; ++i) addSong(mix, sid((i * 37) % 100), "t", "a", 1);
  std::vector<std::string> expected;
  for (int i = 0; i < 100; ++i) expected.push_back(sid(i));
  CHECK(ids(asc.node) == expected);
  CHECK(ids(desc.node) == expected);
  CHECK(ids(mix.node) == expected);
  CHECK_BST(asc.node);
  CHECK_BST(desc.node);
  CHECK_BST(mix.node);
  rawFree(asc);
  rawFree(desc);
  rawFree(mix);
}

// ===========================================================================
// findSongNode / doesSongExist / searchSong
// ===========================================================================

TEST(find_pada_tree_kosong_nullptr) {
  CHECK(findSongNode(nullptr, "0001") == nullptr);
  SongList l;
  initializeSongList(l);
  CHECK(searchSong(l, "0001") == nullptr);
  CHECK(!doesSongExist(l, "0001"));
}

TEST(find_menemukan_root_kiri_kanan_dan_daun_dalam) {
  SongList l = rawList(balanced());
  SongAddress root = l.node;
  CHECK(findSongNode(root, "50") == root);
  CHECK(findSongNode(root, "30") == root->left);
  CHECK(findSongNode(root, "70") == root->right);
  CHECK(findSongNode(root, "20") == root->left->left);
  CHECK(findSongNode(root, "40") == root->left->right);
  CHECK(findSongNode(root, "60") == root->right->left);
  CHECK(findSongNode(root, "80") == root->right->right);
  rawFree(l);
}

TEST(find_id_tidak_ada_nullptr) {
  SongList l = rawList(balanced());
  CHECK(findSongNode(l.node, "10") == nullptr);  // lebih kecil dari semua
  CHECK(findSongNode(l.node, "99") == nullptr);  // lebih besar dari semua
  CHECK(findSongNode(l.node, "45") == nullptr);  // di antara
  CHECK(findSongNode(l.node, "") == nullptr);
  CHECK(findSongNode(l.node, "5") == nullptr);   // awalan saja
  rawFree(l);
}

TEST(find_dari_subtree_tidak_menemukan_id_di_luar_subtree) {
  SongList l = rawList(balanced());
  CHECK(findSongNode(l.node->left, "70") == nullptr);
  CHECK(findSongNode(l.node->left, "50") == nullptr);
  CHECK(findSongNode(l.node->right, "20") == nullptr);
  CHECK(findSongNode(l.node->left, "40") != nullptr);
  rawFree(l);
}

TEST(search_dan_doesExist_konsisten_dengan_find) {
  SongList l = rawList(balanced());
  std::vector<std::string> probe = balanced();
  probe.push_back("10");
  probe.push_back("45");
  probe.push_back("99");
  for (size_t i = 0; i < probe.size(); ++i) {
    SongAddress f = findSongNode(l.node, probe[i]);
    CHECK(searchSong(l, probe[i]) == f);
    CHECK_EQ(doesSongExist(l, probe[i]), f != nullptr);
    if (f != nullptr) CHECK_EQ(f->info.id, probe[i]);
  }
  rawFree(l);
}

TEST(search_tidak_mengubah_tree) {
  SongList l = rawList(balanced());
  std::string before = shape(l.node);
  searchSong(l, "40");
  searchSong(l, "99");
  doesSongExist(l, "60");
  CHECK_EQ(shape(l.node), before);
  rawFree(l);
}

TEST(search_berdasarkan_id_bukan_judul) {
  SongList l = rawList({"0001"});  // judul = "Judul0001"
  CHECK(searchSong(l, "Judul0001") == nullptr);
  CHECK(!doesSongExist(l, "Judul0001"));
  rawFree(l);
}

// ===========================================================================
// findMinSongNode
// ===========================================================================

TEST(findMin_satu_node_adalah_dirinya) {
  SongList l = rawList({"0005"});
  CHECK(findMinSongNode(l.node) == l.node);
  rawFree(l);
}

TEST(findMin_pohon_seimbang_adalah_20) {
  SongList l = rawList(balanced());
  SongAddress m = findMinSongNode(l.node);
  CHECK(m != nullptr);
  if (m != nullptr) CHECK_EQ(m->info.id, "20");
  rawFree(l);
}

TEST(findMin_root_tanpa_anak_kiri_adalah_root) {
  SongList l = rawList({"10", "20", "30"});  // rantai ke kanan
  CHECK(findMinSongNode(l.node) == l.node);
  rawFree(l);
}

TEST(findMin_rantai_kiri_dalam) {
  SongList l = rawList({"90", "80", "70", "60", "50"});
  SongAddress m = findMinSongNode(l.node);
  CHECK(m != nullptr);
  if (m != nullptr) CHECK_EQ(m->info.id, "50");
  rawFree(l);
}

TEST(findMin_dari_subtree_kanan) {
  SongList l = rawList(balanced());
  SongAddress m = findMinSongNode(l.node->right);
  CHECK(m != nullptr);
  if (m != nullptr) CHECK_EQ(m->info.id, "60");
  rawFree(l);
}

TEST(findMin_tidak_mengubah_tree) {
  SongList l = rawList(balanced());
  std::string before = shape(l.node);
  findMinSongNode(l.node);
  CHECK_EQ(shape(l.node), before);
  rawFree(l);
}

// ===========================================================================
// deleteSongNode / deleteSong
// ===========================================================================

TEST(delete_pada_tree_kosong_tidak_crash) {
  SongAddress root = nullptr;
  deleteSongNode(root, "0001");
  CHECK(root == nullptr);
  SongList l;
  initializeSongList(l);
  deleteSong(l, "0001");
  CHECK(isSongListEmpty(l));
}

TEST(delete_id_tidak_ada_tidak_mengubah_tree) {
  SongList l = rawList(balanced());
  std::string before = shape(l.node);
  long live = th::liveAllocs();
  deleteSong(l, "99");
  deleteSong(l, "45");
  deleteSong(l, "10");
  CHECK_EQ(shape(l.node), before);
  CHECK_EQ(th::liveAllocs(), live);
  rawFree(l);
}

TEST(delete_satu_satunya_node_membuat_tree_kosong) {
  long base = th::liveAllocs();
  SongList l;
  initializeSongList(l);
  addSong(l, "1", "judul yang panjang sekali supaya heap", "artis yang panjang sekali", 1);
  deleteSong(l, "1");
  CHECK(l.node == nullptr);
  CHECK(isSongListEmpty(l));
  CHECK_EQ(th::liveAllocs(), base);
}

TEST(delete_daun_kiri) {
  SongList l = rawList(balanced());
  deleteSong(l, "20");
  CHECK_EQ(shape(l.node), "(50 (30 . (40 . .)) (70 (60 . .) (80 . .)))");
  CHECK_BST(l.node);
  rawFree(l);
}

TEST(delete_daun_kanan) {
  SongList l = rawList(balanced());
  deleteSong(l, "80");
  CHECK_EQ(shape(l.node), "(50 (30 (20 . .) (40 . .)) (70 (60 . .) .))");
  CHECK_BST(l.node);
  rawFree(l);
}

TEST(delete_node_dengan_satu_anak_kiri) {
  SongList l = rawList({"50", "30", "20"});
  deleteSong(l, "30");
  CHECK_EQ(shape(l.node), "(50 (20 . .) .)");
  CHECK_BST(l.node);
  rawFree(l);
}

TEST(delete_node_dengan_satu_anak_kanan) {
  SongList l = rawList({"50", "70", "80"});
  deleteSong(l, "70");
  CHECK_EQ(shape(l.node), "(50 . (80 . .))");
  CHECK_BST(l.node);
  rawFree(l);
}

TEST(delete_node_dengan_dua_anak_pada_tengah_tree) {
  SongList l = rawList(balanced());
  deleteSong(l, "30");
  CHECK_IDS(l.node, "20,40,50,60,70,80");
  CHECK_BST(l.node);
  CHECK(othersUntouched(l.node, ""));
  ASSUME_EQ(shape(l.node), "(50 (40 (20 . .) .) (70 (60 . .) (80 . .)))");  // pakai suksesor in-order
  rawFree(l);
}

TEST(delete_root_dengan_dua_anak) {
  SongList l = rawList(balanced());
  deleteSong(l, "50");
  CHECK_IDS(l.node, "20,30,40,60,70,80");
  CHECK_BST(l.node);
  CHECK(l.node != nullptr);
  if (l.node != nullptr) CHECK(l.node->info.id != "50");
  CHECK(othersUntouched(l.node, ""));
  rawFree(l);
}

TEST(delete_root_hanya_punya_anak_kiri_root_pindah_ke_anak) {
  SongList l = rawList({"50", "30", "20"});
  deleteSong(l, "50");
  CHECK_EQ(shape(l.node), "(30 (20 . .) .)");
  rawFree(l);
}

TEST(delete_root_hanya_punya_anak_kanan_root_pindah_ke_anak) {
  SongList l = rawList({"50", "70", "80"});
  deleteSong(l, "50");
  CHECK_EQ(shape(l.node), "(70 . (80 . .))");
  rawFree(l);
}

TEST(delete_suksesor_adalah_anak_kanan_langsung) {
  SongList l = rawList({"50", "30", "70", "80"});  // 70 tidak punya anak kiri
  deleteSong(l, "50");
  CHECK_IDS(l.node, "30,70,80");
  CHECK_BST(l.node);
  ASSUME_EQ(shape(l.node), "(70 (30 . .) (80 . .))");
  rawFree(l);
}

TEST(delete_suksesor_punya_anak_kanan_tidak_boleh_hilang) {
  // 50 -> kanan 70 -> kiri 60 -> kanan 65. Hapus 50: suksesor = 60 yang punya anak 65.
  SongList l = rawList({"50", "30", "70", "60", "65"});
  deleteSong(l, "50");
  CHECK_IDS(l.node, "30,60,65,70");
  CHECK_EQ(countNodes(l.node), 4);
  CHECK(rawFind(l.node, "65") != nullptr);
  CHECK_BST(l.node);
  ASSUME_EQ(shape(l.node), "(60 (30 . .) (70 (65 . .) .))");
  rawFree(l);
}

TEST(delete_suksesor_dalam_di_kiri_kiri) {
  SongList l = rawList({"50", "30", "90", "70", "60", "55", "80"});
  deleteSong(l, "50");  // suksesor: 55
  CHECK_IDS(l.node, "30,55,60,70,80,90");
  CHECK_BST(l.node);
  CHECK(othersUntouched(l.node, ""));
  rawFree(l);
}

TEST(delete_data_node_lain_tetap_utuh) {
  SongList l = rawList(balanced());
  deleteSong(l, "30");
  deleteSong(l, "70");
  CHECK(othersUntouched(l.node, ""));
  CHECK_BST(l.node);
  rawFree(l);
}

TEST(delete_id_yang_sama_dua_kali_kedua_tidak_berpengaruh) {
  SongList l = rawList(balanced());
  deleteSong(l, "40");
  std::string after_first = shape(l.node);
  deleteSong(l, "40");
  CHECK_EQ(shape(l.node), after_first);
  rawFree(l);
}

TEST(delete_hilang_dari_pencarian_lainnya_tetap_ada) {
  SongList l = rawList(balanced());
  deleteSong(l, "30");
  CHECK(searchSong(l, "30") == nullptr);
  CHECK(!doesSongExist(l, "30"));
  CHECK(doesSongExist(l, "20"));
  CHECK(doesSongExist(l, "40"));
  CHECK(doesSongExist(l, "50"));
  rawFree(l);
}

TEST(delete_menghancurkan_node_tidak_bocor) {
  long base = th::liveAllocs();
  {
    SongList l;
    initializeSongList(l);
    std::vector<std::string> v = balanced();
    for (size_t i = 0; i < v.size(); ++i)
      addSong(l, v[i], "judul yang cukup panjang untuk alokasi heap", "artis yang cukup panjang untuk heap", 1);
    deleteSong(l, "20");  // daun
    deleteSong(l, "30");  // satu anak
    deleteSong(l, "70");  // dua anak
    deleteSong(l, "50");  // root
    deleteSong(l, "40");
    deleteSong(l, "60");
    deleteSong(l, "80");
    CHECK(l.node == nullptr);
  }
  CHECK_EQ(th::liveAllocs(), base);
}

TEST(delete_semua_node_dengan_banyak_urutan_acak) {
  std::mt19937 rng(2024);
  for (int round = 0; round < 25; ++round) {
    std::vector<int> nums;
    for (int i = 0; i < 31; ++i) nums.push_back(i);
    std::vector<int> remaining(nums);  // sudah terurut
    long base = th::liveAllocs();      // diambil setelah vector milik test dibuat
    std::shuffle(nums.begin(), nums.end(), rng);
    SongList l;
    initializeSongList(l);
    for (size_t i = 0; i < nums.size(); ++i) addSong(l, sid(nums[i]), "judul yang panjang sekali", "artis yang panjang sekali", 1);
    std::shuffle(nums.begin(), nums.end(), rng);
    bool ok = true;
    for (size_t i = 0; i < nums.size() && ok; ++i) {
      deleteSong(l, sid(nums[i]));
      remaining.erase(std::find(remaining.begin(), remaining.end(), nums[i]));
      std::vector<std::string> expected;
      for (size_t k = 0; k < remaining.size(); ++k) expected.push_back(sid(remaining[k]));
      std::string why;
      if (!bstValid(l.node, nullptr, nullptr, why) || ids(l.node) != expected) {
        th::report(false, false, "tree == model setelah delete", __FILE__, __LINE__,
                   "ronde " + std::to_string(round) + ", hapus id " + sid(nums[i]) + " " + why);
        ok = false;
      }
    }
    if (!ok) { rawFree(l); return; }
    CHECK(l.node == nullptr);
    CHECK_EQ(th::liveAllocs(), base);
  }
}

TEST(deleteSongNode_langsung_pada_subtree) {
  SongList l = rawList(balanced());
  deleteSongNode(l.node->left, "30");  // hapus root subtree kiri lewat reference anak
  CHECK_IDS(l.node, "20,40,50,60,70,80");
  CHECK_BST(l.node);
  CHECK_EQ(idOf(l.node), "50");
  rawFree(l);
}

// ===========================================================================
// updateSongArtist / updateSongTitle / updateSongDuration
// ===========================================================================

TEST(updateTitle_hanya_target_yang_berubah) {
  SongList l = rawList(balanced());
  updateSongTitle(l, "40", "Judul Baru");
  SongAddress p = rawFind(l.node, "40");
  CHECK(p != nullptr);
  if (p == nullptr) { rawFree(l); return; }
  CHECK_EQ(p->info.title, "Judul Baru");
  CHECK_EQ(p->info.artist, "Artis40");
  CHECK_EQ(p->info.duration, expectedDuration("40"));
  CHECK_EQ(p->info.id, "40");
  CHECK(othersUntouched(l.node, "40"));
  rawFree(l);
}

TEST(updateArtist_hanya_target_yang_berubah) {
  SongList l = rawList(balanced());
  updateSongArtist(l, "60", "Artis Baru");
  SongAddress p = rawFind(l.node, "60");
  CHECK(p != nullptr);
  if (p == nullptr) { rawFree(l); return; }
  CHECK_EQ(p->info.artist, "Artis Baru");
  CHECK_EQ(p->info.title, "Judul60");
  CHECK_EQ(p->info.duration, expectedDuration("60"));
  CHECK(othersUntouched(l.node, "60"));
  rawFree(l);
}

TEST(updateDuration_hanya_target_yang_berubah) {
  SongList l = rawList(balanced());
  updateSongDuration(l, "80", 999);
  SongAddress p = rawFind(l.node, "80");
  CHECK(p != nullptr);
  if (p == nullptr) { rawFree(l); return; }
  CHECK_EQ(p->info.duration, 999);
  CHECK_EQ(p->info.title, "Judul80");
  CHECK_EQ(p->info.artist, "Artis80");
  CHECK(othersUntouched(l.node, "80"));
  rawFree(l);
}

TEST(update_pada_root_daun_dan_tengah) {
  SongList l = rawList(balanced());
  updateSongTitle(l, "50", "T-root");
  updateSongTitle(l, "20", "T-daun");
  updateSongTitle(l, "70", "T-tengah");
  CHECK_EQ(rawFind(l.node, "50")->info.title, "T-root");
  CHECK_EQ(rawFind(l.node, "20")->info.title, "T-daun");
  CHECK_EQ(rawFind(l.node, "70")->info.title, "T-tengah");
  CHECK_EQ(rawFind(l.node, "30")->info.title, "Judul30");
  rawFree(l);
}

TEST(update_tidak_mengubah_bentuk_tree) {
  SongList l = rawList(balanced());
  std::string before = shape(l.node);
  updateSongTitle(l, "30", "x");
  updateSongArtist(l, "30", "y");
  updateSongDuration(l, "30", 1);
  CHECK_EQ(shape(l.node), before);
  rawFree(l);
}

TEST(update_id_tidak_ada_tidak_mengubah_apa_apa) {
  SongList l = rawList(balanced());
  updateSongTitle(l, "99", "x");
  updateSongArtist(l, "99", "y");
  updateSongDuration(l, "99", 1);
  CHECK(othersUntouched(l.node, ""));
  CHECK_EQ(countNodes(l.node), 7);
  rawFree(l);
}

TEST(update_pada_tree_kosong_tidak_crash) {
  SongList l;
  initializeSongList(l);
  updateSongTitle(l, "1", "x");
  updateSongArtist(l, "1", "y");
  updateSongDuration(l, "1", 5);
  CHECK(isSongListEmpty(l));
}

TEST(update_judul_sama_tidak_salah_target_berdasarkan_judul) {
  // Seperti test lama: dua lagu berjudul sama ("Lagu A2"), update harus pakai id.
  SongList l;
  initializeSongList(l);
  addSong(l, "1", "Lagu A1", "A", 3);
  addSong(l, "3", "Lagu A2", "A", 4);
  addSong(l, "2", "Lagu A2", "A", 2);
  updateSongTitle(l, "3", "Berubah");
  CHECK_EQ(rawFind(l.node, "3")->info.title, "Berubah");
  CHECK_EQ(rawFind(l.node, "2")->info.title, "Lagu A2");
  CHECK_EQ(rawFind(l.node, "1")->info.title, "Lagu A1");
  rawFree(l);
}

TEST(update_skenario_test_lama_judul_1_jadi_A_keren) {
  SongList l;
  initializeSongList(l);
  addSong(l, "1", "Lagu A1", "A", 3);
  addSong(l, "3", "Lagu A2", "A", 4);
  addSong(l, "2", "Lagu A2", "A", 2);
  addSong(l, "4", "Lagu B1", "B", 1);
  deleteSong(l, "4");
  updateSongTitle(l, "1", "A keren");
  CHECK_EQ(rawFind(l.node, "1")->info.title, "A keren");
  CHECK_EQ(rawFind(l.node, "1")->info.artist, "A");
  CHECK_EQ(rawFind(l.node, "1")->info.duration, 3);
  CHECK(rawFind(l.node, "4") == nullptr);
  CHECK_IDS(l.node, "1,2,3");
  rawFree(l);
}

TEST(update_berulang_dan_nilai_batas) {
  SongList l = rawList({"0001"});
  for (int i = 0; i < 20; ++i) updateSongTitle(l, "0001", "judul versi " + std::to_string(i) + " yang cukup panjang");
  CHECK_EQ(l.node->info.title, "judul versi 19 yang cukup panjang");
  updateSongDuration(l, "0001", 0);
  CHECK_EQ(l.node->info.duration, 0);
  updateSongTitle(l, "0001", "");
  CHECK_EQ(l.node->info.title, "");
  rawFree(l);
}

TEST(update_tidak_bocor_memori) {
  SongList l = rawList({"0001"});
  updateSongTitle(l, "0001", "judul awal yang panjang sekali untuk heap");
  updateSongArtist(l, "0001", "artis awal yang panjang sekali untuk heap");
  long live = th::liveAllocs();
  for (int i = 0; i < 20; ++i) {
    updateSongTitle(l, "0001", "judul berikutnya yang panjang sekali untuk heap");
    updateSongArtist(l, "0001", "artis berikutnya yang panjang sekali untuk heap");
  }
  CHECK_EQ(th::liveAllocs(), live);
  rawFree(l);
}

// ===========================================================================
// destroyAllSongNode / destroySongList
// ===========================================================================

TEST(destroyAll_node_nullptr_tidak_crash) {
  SongAddress n = nullptr;
  destroyAllSongNode(n);
  CHECK(n == nullptr);
}

TEST(destroyAll_membebaskan_semua_node_subtree) {
  std::vector<std::string> v = balanced();
  long base = th::liveAllocs();  // setelah vector milik test dibuat
  SongAddress root = nullptr;
  for (size_t i = 0; i < v.size(); ++i) insertSongNode(root, createSongNode(v[i], "judul panjang untuk heap sekali", "artis panjang untuk heap sekali", 1));
  destroyAllSongNode(root);
  CHECK_EQ(th::liveAllocs(), base);
  ASSUME(root == nullptr);
}

TEST(destroyAll_hanya_subtree_yang_dihancurkan) {
  long base = th::liveAllocs();
  SongList l = rawList(balanced());  // 7 node
  long perNode = (th::liveAllocs() - base) / 7;
  long full = th::liveAllocs();
  SongAddress left = l.node->left;  // subtree: 30, 20, 40
  destroyAllSongNode(left);
  CHECK_EQ(th::liveAllocs(), full - 3 * perNode);
  l.node->left = nullptr;  // buang pointer menggantung sebelum cleanup
  CHECK_IDS(l.node, "50,60,70,80");
  rawFree(l);
}

TEST(destroyList_pada_list_kosong_tidak_crash) {
  SongList l;
  initializeSongList(l);
  destroySongList(l);
  CHECK(l.node == nullptr);
}

TEST(destroyList_membebaskan_semua) {
  long base = th::liveAllocs();
  SongList l;
  initializeSongList(l);
  for (int i = 0; i < 50; ++i) addSong(l, sid((i * 7) % 50), "judul panjang untuk heap sekali", "artis panjang untuk heap sekali", i);
  destroySongList(l);
  CHECK_EQ(th::liveAllocs(), base);
  ASSUME(l.node == nullptr);
}

TEST(destroyList_pohon_rantai_dalam_2000_node) {
  long base = th::liveAllocs();
  SongList l;
  initializeSongList(l);
  for (int i = 0; i < 2000; ++i) addSong(l, sid(i), "t", "a", 1);
  destroySongList(l);
  CHECK_EQ(th::liveAllocs(), base);
}

TEST(destroyList_lalu_pakai_ulang) {
  SongList l;
  initializeSongList(l);
  addSong(l, "1", "a", "b", 1);
  addSong(l, "2", "a", "b", 1);
  destroySongList(l);
  initializeSongList(l);
  addSong(l, "5", "x", "y", 5);
  CHECK_IDS(l.node, "5");
  destroySongList(l);
}

// ===========================================================================
// dumpSongNode / dumpSongList
// ===========================================================================

TEST(dump_tree_kosong_tidak_crash) {
  SongList l;
  initializeSongList(l);
  th::CoutCapture cap;
  dumpSongList(l);
  dumpSongNode(nullptr);
}

TEST(dump_list_menampilkan_info_semua_lagu) {
  SongList l;
  initializeSongList(l);
  addSong(l, "0002", "JudulDua", "ArtisDua", 222);
  addSong(l, "0001", "JudulSatu", "ArtisSatu", 111);
  addSong(l, "0003", "JudulTiga", "ArtisTiga", 333);
  std::string out;
  {
    th::CoutCapture cap;
    dumpSongList(l);
    out = cap.str();
  }
  ASSUME(th::contains(out, "JudulDua"));
  ASSUME(th::contains(out, "JudulSatu"));
  ASSUME(th::contains(out, "JudulTiga"));
  ASSUME(th::contains(out, "ArtisSatu"));
  ASSUME(th::contains(out, "0003"));
  ASSUME(th::contains(out, "222"));
  rawFree(l);
}

TEST(dump_node_hanya_menampilkan_node_dan_turunannya) {
  SongList l;
  initializeSongList(l);
  addSong(l, "50", "JudulRoot", "a", 1);
  addSong(l, "30", "JudulKiri", "a", 1);
  addSong(l, "20", "JudulKiriKiri", "a", 1);
  addSong(l, "70", "JudulKanan", "a", 1);
  std::string out;
  {
    th::CoutCapture cap;
    dumpSongNode(l.node->left);  // subtree: 30 dan 20
    out = cap.str();
  }
  ASSUME(th::contains(out, "JudulKiri"));
  ASSUME(th::contains(out, "JudulKiriKiri"));
  CHECK(!th::contains(out, "JudulRoot"));
  CHECK(!th::contains(out, "JudulKanan"));
  rawFree(l);
}

TEST(dump_tidak_mengubah_tree) {
  SongList l = rawList(balanced());
  std::string before = shape(l.node);
  {
    th::CoutCapture cap;
    dumpSongList(l);
    dumpSongNode(l.node);
  }
  CHECK_EQ(shape(l.node), before);
  CHECK(othersUntouched(l.node, ""));
  rawFree(l);
}

// ===========================================================================
// Uji beban & pembandingan dengan model
// ===========================================================================

TEST(beban_tambah_1500_lagu_acak_lalu_hapus_separuh) {
  long base = th::liveAllocs();
  {
    std::mt19937 rng(99);
    std::vector<int> nums;
    for (int i = 0; i < 1500; ++i) nums.push_back(i);
    std::shuffle(nums.begin(), nums.end(), rng);
    SongList l;
    initializeSongList(l);
    for (size_t i = 0; i < nums.size(); ++i) addSong(l, sid(nums[i]), "t", "a", 1);
    CHECK_EQ(countNodes(l.node), 1500);
    for (int i = 0; i < 1500; i += 2) deleteSong(l, sid(i));
    CHECK_EQ(countNodes(l.node), 750);
    CHECK_BST(l.node);
    CHECK(!doesSongExist(l, sid(0)));
    CHECK(doesSongExist(l, sid(1)));
    destroySongList(l);
  }
  CHECK_EQ(th::liveAllocs(), base);
}

TEST(acak_add_delete_update_dibandingkan_model_map) {
  struct Rec {
    std::string title, artist;
    int duration;
  };
  std::mt19937 rng(31337);
  SongList l;
  initializeSongList(l);
  std::map<std::string, Rec> model;
  for (int step = 0; step < 4000; ++step) {
    std::string id = sid(static_cast<int>(rng() % 40));
    int op = static_cast<int>(rng() % 5);
    if (op == 0) {
      std::string t = "T" + std::to_string(step), a = "A" + std::to_string(step);
      addSong(l, id, t, a, step);
      if (!model.count(id)) { Rec r = {t, a, step}; model[id] = r; }
    } else if (op == 1) {
      deleteSong(l, id);
      model.erase(id);
    } else if (op == 2) {
      updateSongTitle(l, id, "TB" + std::to_string(step));
      if (model.count(id)) model[id].title = "TB" + std::to_string(step);
    } else if (op == 3) {
      updateSongArtist(l, id, "AB" + std::to_string(step));
      if (model.count(id)) model[id].artist = "AB" + std::to_string(step);
    } else {
      updateSongDuration(l, id, step + 1000);
      if (model.count(id)) model[id].duration = step + 1000;
    }

    std::string why;
    bool ok = bstValid(l.node, nullptr, nullptr, why);
    std::vector<std::string> expectedIds;
    for (std::map<std::string, Rec>::iterator it = model.begin(); it != model.end(); ++it) expectedIds.push_back(it->first);
    if (ok && ids(l.node) != expectedIds) { ok = false; why = "kumpulan id berbeda: [" + th::join(ids(l.node)) + "] vs model [" + th::join(expectedIds) + "]"; }
    for (std::map<std::string, Rec>::iterator it = model.begin(); ok && it != model.end(); ++it) {
      SongAddress p = rawFind(l.node, it->first);
      if (p == nullptr) { ok = false; why = "id hilang: " + it->first; }
      else if (p->info.title != it->second.title) { ok = false; why = "judul salah untuk " + it->first; }
      else if (p->info.artist != it->second.artist) { ok = false; why = "artis salah untuk " + it->first; }
      else if (p->info.duration != it->second.duration) { ok = false; why = "durasi salah untuk " + it->first; }
    }
    if (!ok) {
      th::report(false, false, "tree == model", __FILE__, __LINE__,
                 "langkah " + std::to_string(step) + " operasi " + std::to_string(op) + ": " + why);
      break;
    }
    // searchSong / doesSongExist harus sesuai model
    CHECK_EQ(doesSongExist(l, id), model.count(id) > 0);
  }
  destroySongList(l);
}

// ===========================================================================
// Ketahanan terhadap argumen nullptr (rawan crash, sengaja di paling akhir)
// ===========================================================================

TEST(robust_findMin_nullptr) {
  ASSUME(findMinSongNode(nullptr) == nullptr);
}

TEST(robust_insertNode_nullptr_tidak_mengubah_tree) {
  SongList l = rawList(balanced());
  std::string before = shape(l.node);
  insertSongNode(l.node, nullptr);
  ASSUME_EQ(shape(l.node), before);
  rawFree(l);
}

TEST_MAIN()
