// test_queue.cpp
// Test untuk ADT queue (FIFO, double linked list dengan head & tail) pada queue.h
//
// Compile (sesuaikan path):
//   g++ -std=c++11 -g -Wall -Wextra -fsanitize=address,undefined test_queue.cpp <file implementasi queue>.cpp -I<folder queue.h> -o test_queue
// Jalankan:
//   ./test_queue             (atau ./test_queue --isolate)
//
// Catatan tentang info type:
//   QueueInfoType saat ini kosong, jadi elemen dibedakan lewat ALAMAT-nya dan
//   diberi nomor int lewat makeElem()/idOf() di bawah. Saat QueueInfoType sudah
//   punya field (mis. `int value;`), cukup ubah DUA fungsi itu:
//       makeElem(): isi e->info.value = id;
//       idOf()    : return p->info.value;
//   Seluruh test lain tidak perlu diubah.
//
//   ADT queue tidak punya destroyQueue(), jadi test membebaskan elemen sendiri.
//   Elemen yang keluar lewat dequeue() TIDAK boleh sudah di-delete oleh ADT
//   (header: "tidak menghancurkannya"), jadi test yang meng-delete-nya sendiri.
//
//   CHECK = wajib benar. ASSUME = hal yang tidak ditegaskan header (jadi peringatan).

#include <deque>
#include <map>
#include <random>

#include "queue.h"
#include "test_helper.h"

namespace {

const int kGuard = 100000;

// ----------------------------- id elemen (int) -----------------------------

std::map<QueueAddress, int>& idTable() {
  static std::map<QueueAddress, int> t;
  return t;
}

QueueAddress makeElem(int id) {
  QueueAddress e = new QueueElement();
  e->next = nullptr;
  e->prev = nullptr;
  idTable()[e] = id;  // <-- ganti jadi `e->info.value = id;` bila infotype sudah ada
  return e;
}

int idOf(QueueAddress p) {  // <-- ganti jadi `return p->info.value;` bila infotype sudah ada
  std::map<QueueAddress, int>::iterator it = idTable().find(p);
  return it == idTable().end() ? -1 : it->second;
}

// ----------------------------- helper struktur -----------------------------

std::vector<int> idsFwd(const Queue& q) {
  std::vector<int> v;
  int guard = 0;
  for (QueueAddress p = q.head; p != nullptr && guard < kGuard; p = p->next, ++guard) v.push_back(idOf(p));
  return v;
}

std::vector<int> idsBwd(const Queue& q) {
  std::vector<int> v;
  int guard = 0;
  for (QueueAddress p = q.tail; p != nullptr && guard < kGuard; p = p->prev, ++guard) v.push_back(idOf(p));
  return v;
}

// head/tail konsisten, prev/next simetris, tidak melingkar.
bool queueValid(const Queue& q, std::string& why) {
  if ((q.head == nullptr) != (q.tail == nullptr)) {
    why = "head dan tail tidak konsisten (salah satunya nullptr)";
    return false;
  }
  if (q.head == nullptr) return true;
  if (q.head->prev != nullptr) { why = "head->prev bukan nullptr"; return false; }
  if (q.tail->next != nullptr) { why = "tail->next bukan nullptr"; return false; }
  int n = 0;
  QueueAddress p = q.head, before = nullptr;
  while (p != nullptr) {
    if (++n > kGuard) { why = "queue melingkar / terlalu panjang"; return false; }
    if (p->prev != before) {
      why = "prev tidak cocok pada elemen ke-" + std::to_string(n);
      return false;
    }
    before = p;
    p = p->next;
  }
  if (before != q.tail) { why = "tail tidak menunjuk elemen terakhir"; return false; }
  return true;
}

#define CHECK_QUEUE(q)                                                          \
  do {                                                                          \
    std::string why_;                                                           \
    th::report(queueValid((q), why_), false, "struktur queue valid: " #q,       \
               __FILE__, __LINE__, why_);                                       \
  } while (0)

#define CHECK_IDS(q, expected) CHECK_EQ(th::joinInts(idsFwd(q)), expected)

// ----------------------------- fixture manual ------------------------------

Queue rawQueue(const std::vector<int>& v) {
  Queue q;
  q.head = nullptr;
  q.tail = nullptr;
  for (size_t i = 0; i < v.size(); ++i) {
    QueueAddress e = makeElem(v[i]);
    if (q.tail == nullptr) {
      q.head = q.tail = e;
    } else {
      q.tail->next = e;
      e->prev = q.tail;
      q.tail = e;
    }
  }
  return q;
}

void rawFree(Queue& q) {
  std::string why;
  if (queueValid(q, why)) {
    QueueAddress p = q.head;
    while (p != nullptr) {
      QueueAddress nx = p->next;
      delete p;
      p = nx;
    }
  }
  q.head = nullptr;
  q.tail = nullptr;
}

QueueAddress nth(const Queue& q, int i) {
  QueueAddress p = q.head;
  while (p != nullptr && i-- > 0) p = p->next;
  return p;
}

}  // namespace

// ===========================================================================
// initializeQueue / isQueueEmpty / sizeQueue / frontQueue pada queue kosong
// ===========================================================================

TEST(init_menjadikan_head_dan_tail_nullptr) {
  Queue q;
  q.head = reinterpret_cast<QueueAddress>(0x10);  // sengaja berisi sampah
  q.tail = reinterpret_cast<QueueAddress>(0x20);
  initializeQueue(q);
  CHECK(q.head == nullptr);
  CHECK(q.tail == nullptr);
}

TEST(queue_baru_kosong_ukuran_nol_dan_front_nullptr) {
  Queue q;
  initializeQueue(q);
  CHECK(isQueueEmpty(q));
  CHECK_EQ(sizeQueue(q), 0);
  CHECK(frontQueue(q) == nullptr);
}

// ===========================================================================
// enqueue
// ===========================================================================

TEST(enqueue_ke_queue_kosong) {
  Queue q;
  initializeQueue(q);
  QueueAddress a = makeElem(1);
  enqueue(q, a);
  CHECK(q.head == a);
  CHECK(q.tail == a);
  CHECK(a->next == nullptr);
  CHECK(a->prev == nullptr);
  CHECK(!isQueueEmpty(q));
  CHECK_EQ(sizeQueue(q), 1);
  CHECK(frontQueue(q) == a);
  CHECK_QUEUE(q);
  rawFree(q);
}

TEST(enqueue_kedua_ditaruh_di_belakang_head_tetap) {
  Queue q;
  initializeQueue(q);
  QueueAddress a = makeElem(1), b = makeElem(2);
  enqueue(q, a);
  enqueue(q, b);
  CHECK(q.head == a);
  CHECK(q.tail == b);
  CHECK(a->next == b);
  CHECK(b->prev == a);
  CHECK(b->next == nullptr);
  CHECK(a->prev == nullptr);
  CHECK(frontQueue(q) == a);  // FIFO: yang pertama masuk tetap di depan
  CHECK_EQ(sizeQueue(q), 2);
  CHECK_QUEUE(q);
  rawFree(q);
}

TEST(enqueue_tiga_elemen_urutan_benar) {
  Queue q;
  initializeQueue(q);
  enqueue(q, makeElem(10));
  enqueue(q, makeElem(20));
  enqueue(q, makeElem(30));
  CHECK_IDS(q, "10,20,30");
  CHECK_EQ(th::joinInts(idsBwd(q)), "30,20,10");  // prev juga benar
  CHECK_QUEUE(q);
  rawFree(q);
}

TEST(enqueue_ke_queue_buatan_manual_berisi) {
  Queue q = rawQueue({1, 2});
  QueueAddress tailBefore = q.tail;
  QueueAddress c = makeElem(3);
  enqueue(q, c);
  CHECK(q.tail == c);
  CHECK(tailBefore->next == c);
  CHECK(c->prev == tailBefore);
  CHECK_IDS(q, "1,2,3");
  CHECK_QUEUE(q);
  rawFree(q);
}

TEST(enqueue_tidak_menyalin_atau_mengalokasi_elemen_baru) {
  Queue q;
  initializeQueue(q);
  QueueAddress a = makeElem(1);
  QueueAddress b = makeElem(2);
  long live = th::liveAllocs();  // diukur setelah semua elemen milik test dibuat
  enqueue(q, a);
  enqueue(q, b);
  CHECK_EQ(th::liveAllocs(), live);  // enqueue tidak boleh menyalin / mengalokasi
  CHECK(q.head == a);                // alamat yang sama, bukan salinan
  CHECK(q.tail == b);
  rawFree(q);
}

// ===========================================================================
// frontQueue
// ===========================================================================

TEST(front_memberi_elemen_terdepan_tanpa_mengurangi_isi) {
  Queue q = rawQueue({1, 2, 3});
  QueueAddress a = q.head;
  CHECK(frontQueue(q) == a);
  CHECK(frontQueue(q) == a);  // dipanggil berulang tetap sama
  CHECK_EQ(sizeQueue(q), 3);
  CHECK_IDS(q, "1,2,3");
  CHECK_QUEUE(q);
  rawFree(q);
}

TEST(front_mengikuti_perubahan_setelah_dequeue) {
  Queue q = rawQueue({1, 2, 3});
  QueueAddress b = nth(q, 1);
  QueueAddress out = dequeue(q);
  CHECK(frontQueue(q) == b);
  delete out;
  rawFree(q);
}

TEST(front_queue_satu_elemen) {
  Queue q = rawQueue({7});
  CHECK(frontQueue(q) == q.head);
  CHECK_EQ(idOf(frontQueue(q)), 7);
  rawFree(q);
}

// ===========================================================================
// sizeQueue / isQueueEmpty
// ===========================================================================

TEST(size_bertambah_saat_enqueue_berkurang_saat_dequeue) {
  Queue q;
  initializeQueue(q);
  for (int i = 1; i <= 5; ++i) {
    enqueue(q, makeElem(i));
    CHECK_EQ(sizeQueue(q), i);
  }
  for (int i = 4; i >= 0; --i) {
    QueueAddress out = dequeue(q);
    delete out;
    CHECK_EQ(sizeQueue(q), i);
  }
  CHECK(isQueueEmpty(q));
}

TEST(size_dan_isEmpty_tidak_mengubah_queue) {
  Queue q = rawQueue({1, 2, 3});
  sizeQueue(q);
  isQueueEmpty(q);
  frontQueue(q);
  CHECK_IDS(q, "1,2,3");
  CHECK_QUEUE(q);
  rawFree(q);
}

TEST(isEmpty_false_jika_ada_isi_true_setelah_kosong_lagi) {
  Queue q;
  initializeQueue(q);
  CHECK(isQueueEmpty(q));
  enqueue(q, makeElem(1));
  CHECK(!isQueueEmpty(q));
  QueueAddress out = dequeue(q);
  delete out;
  CHECK(isQueueEmpty(q));
}

// ===========================================================================
// dequeue
// ===========================================================================

TEST(dequeue_queue_kosong_memberi_nullptr_dan_tetap_kosong) {
  Queue q;
  initializeQueue(q);
  QueueAddress out = dequeue(q);
  CHECK(out == nullptr);
  CHECK(q.head == nullptr);
  CHECK(q.tail == nullptr);
  CHECK(isQueueEmpty(q));
  CHECK_EQ(sizeQueue(q), 0);
}

TEST(dequeue_satu_elemen_membuat_queue_kosong) {
  Queue q = rawQueue({1});
  QueueAddress a = q.head;
  long live = th::liveAllocs();
  QueueAddress out = dequeue(q);
  CHECK(out == a);
  CHECK(q.head == nullptr);
  CHECK(q.tail == nullptr);  // kesalahan umum: tail lupa di-null-kan
  CHECK(isQueueEmpty(q));
  CHECK_EQ(th::liveAllocs(), live);  // hanya dikeluarkan, tidak dihancurkan
  FREE_IF_ALIVE(a, live);
}

TEST(dequeue_dua_elemen) {
  Queue q = rawQueue({1, 2});
  QueueAddress a = q.head, b = q.tail;
  long live = th::liveAllocs();
  QueueAddress out = dequeue(q);
  CHECK(out == a);
  CHECK(q.head == b);
  CHECK(q.tail == b);
  CHECK(b->prev == nullptr);
  CHECK(b->next == nullptr);
  CHECK_QUEUE(q);
  FREE_IF_ALIVE(a, live);
  rawFree(q);
}

TEST(dequeue_banyak_elemen_mengeluarkan_yang_terdepan) {
  Queue q = rawQueue({1, 2, 3});
  QueueAddress a = q.head, b = nth(q, 1), c = q.tail;
  long live = th::liveAllocs();
  QueueAddress out = dequeue(q);
  CHECK(out == a);
  CHECK(q.head == b);
  CHECK(q.tail == c);
  CHECK(b->prev == nullptr);
  CHECK_IDS(q, "2,3");
  CHECK_QUEUE(q);
  CHECK_EQ(th::liveAllocs(), live);
  if (th::liveAllocs() == live) {
    CHECK_EQ(idOf(a), 1);  // data elemen yang keluar masih utuh
    ASSUME(a->next == nullptr);  // elemen keluar sebaiknya terlepas
    ASSUME(a->prev == nullptr);
  }
  FREE_IF_ALIVE(a, live);
  rawFree(q);
}

TEST(dequeue_berulang_urutan_FIFO_sampai_kosong) {
  Queue q;
  initializeQueue(q);
  for (int i = 1; i <= 10; ++i) enqueue(q, makeElem(i));
  std::vector<int> got;
  for (int i = 0; i < 10; ++i) {
    long live = th::liveAllocs();
    QueueAddress out = dequeue(q);
    bool intact = (th::liveAllocs() == live);  // dequeue tidak boleh dealokasi
    CHECK(out != nullptr);
    if (out == nullptr) break;
    got.push_back(idOf(out));
    CHECK_QUEUE(q);
    if (intact) delete out;
  }
  CHECK_EQ(th::joinInts(got), "1,2,3,4,5,6,7,8,9,10");
  CHECK(isQueueEmpty(q));
  CHECK(dequeue(q) == nullptr);  // sekali lagi pada queue yang sudah kosong
}

TEST(dequeue_tidak_dealokasi_elemen_yang_keluar) {
  Queue q = rawQueue({1, 2, 3});
  long live = th::liveAllocs();
  QueueAddress a = dequeue(q);
  QueueAddress b = dequeue(q);
  bool intact = (th::liveAllocs() == live);
  CHECK(intact);
  if (intact) {
    delete a;
    delete b;
  }
  rawFree(q);
}

// ===========================================================================
// Pemakaian ulang setelah kosong / elemen dipakai ulang
// ===========================================================================

TEST(enqueue_setelah_queue_dikosongkan_head_tail_benar) {
  Queue q;
  initializeQueue(q);
  QueueAddress a = makeElem(1);
  enqueue(q, a);
  QueueAddress out = dequeue(q);
  CHECK(out == a);
  QueueAddress b = makeElem(2);
  enqueue(q, b);
  CHECK(q.head == b);
  CHECK(q.tail == b);
  CHECK(b->prev == nullptr);  // kesalahan umum: prev menunjuk elemen lama
  CHECK(b->next == nullptr);
  CHECK_EQ(sizeQueue(q), 1);
  CHECK_QUEUE(q);
  delete a;
  rawFree(q);
}

TEST(elemen_hasil_dequeue_bisa_dienqueue_lagi) {
  Queue q = rawQueue({1, 2, 3});
  QueueAddress a = dequeue(q);
  CHECK(a != nullptr);
  if (a == nullptr) { rawFree(q); return; }
  enqueue(q, a);
  CHECK_IDS(q, "2,3,1");
  CHECK(q.tail == a);
  CHECK(a->next == nullptr);  // kalau gagal: pointer lama tidak dibersihkan -> melingkar
  CHECK_QUEUE(q);
  rawFree(q);
}

TEST(elemen_dequeue_dari_queue_satu_elemen_bisa_dienqueue_ke_queue_lain) {
  Queue q1 = rawQueue({1});
  Queue q2 = rawQueue({2, 3});
  QueueAddress a = dequeue(q1);
  enqueue(q2, a);
  CHECK_IDS(q2, "2,3,1");
  CHECK(isQueueEmpty(q1));
  CHECK_QUEUE(q1);
  CHECK_QUEUE(q2);
  rawFree(q2);
}

TEST(dua_queue_terpisah_tidak_saling_memengaruhi) {
  Queue a, b;
  initializeQueue(a);
  initializeQueue(b);
  enqueue(a, makeElem(1));
  enqueue(a, makeElem(2));
  enqueue(b, makeElem(10));
  QueueAddress x = dequeue(a);
  delete x;
  CHECK_IDS(a, "2");
  CHECK_IDS(b, "10");
  CHECK_EQ(sizeQueue(a), 1);
  CHECK_EQ(sizeQueue(b), 1);
  rawFree(a);
  rawFree(b);
}

TEST(enqueue_dequeue_bergantian_ukuran_selalu_satu) {
  Queue q;
  initializeQueue(q);
  for (int i = 0; i < 1000; ++i) {
    QueueAddress e = makeElem(i);
    enqueue(q, e);
    CHECK_EQ(sizeQueue(q), 1);
    QueueAddress out = dequeue(q);
    CHECK(out == e);
    delete out;
    CHECK(isQueueEmpty(q));
    CHECK(q.head == nullptr);
    CHECK(q.tail == nullptr);
  }
}

TEST(pola_dua_enqueue_satu_dequeue_berulang) {
  Queue q;
  initializeQueue(q);
  std::deque<int> model;
  int next = 0;
  for (int round = 0; round < 200; ++round) {
    enqueue(q, makeElem(next)); model.push_back(next++);
    enqueue(q, makeElem(next)); model.push_back(next++);
    QueueAddress out = dequeue(q);
    CHECK(out != nullptr);
    if (out != nullptr) {
      CHECK_EQ(idOf(out), model.front());
      delete out;
    }
    model.pop_front();
    CHECK_EQ(sizeQueue(q), static_cast<int>(model.size()));
  }
  CHECK_QUEUE(q);
  rawFree(q);
}

// ===========================================================================
// Uji beban & pembandingan dengan model
// ===========================================================================

TEST(beban_enqueue_5000_lalu_dequeue_semua_urutan_FIFO) {
  Queue q;
  initializeQueue(q);
  for (int i = 0; i < 5000; ++i) enqueue(q, makeElem(i));
  CHECK_EQ(sizeQueue(q), 5000);
  CHECK_QUEUE(q);
  bool ok = true;
  for (int i = 0; i < 5000 && ok; ++i) {
    QueueAddress f = frontQueue(q);
    QueueAddress out = dequeue(q);
    if (out == nullptr || out != f || idOf(out) != i) {
      th::report(false, false, "dequeue ke-i FIFO", __FILE__, __LINE__, "i = " + std::to_string(i));
      ok = false;
    }
    delete out;
  }
  CHECK(isQueueEmpty(q));
  CHECK_EQ(sizeQueue(q), 0);
}

TEST(acak_operasi_dibandingkan_model_deque) {
  std::mt19937 rng(4242);
  Queue q;
  initializeQueue(q);
  std::deque<QueueAddress> model;
  int next = 0;
  for (int step = 0; step < 5000; ++step) {
    int op = static_cast<int>(rng() % 5);
    if (op <= 1) {  // enqueue (lebih sering)
      QueueAddress e = makeElem(next++);
      enqueue(q, e);
      model.push_back(e);
    } else if (op == 2) {  // dequeue
      QueueAddress out = dequeue(q);
      if (model.empty()) {
        CHECK(out == nullptr);
      } else {
        CHECK(out == model.front());
        model.pop_front();
        delete out;
      }
    } else if (op == 3) {  // front
      CHECK(frontQueue(q) == (model.empty() ? nullptr : model.front()));
    } else {  // size & empty
      CHECK_EQ(sizeQueue(q), static_cast<int>(model.size()));
      CHECK_EQ(isQueueEmpty(q), model.empty());
    }
    std::string why;
    bool ok = queueValid(q, why);
    if (ok && sizeQueue(q) != static_cast<int>(model.size())) {
      ok = false;
      why = "ukuran " + std::to_string(sizeQueue(q)) + " != model " + std::to_string(model.size());
    }
    if (ok && q.head != (model.empty() ? nullptr : model.front())) { ok = false; why = "head != model.front"; }
    if (ok && q.tail != (model.empty() ? nullptr : model.back())) { ok = false; why = "tail != model.back"; }
    if (!ok) {
      th::report(false, false, "queue == model", __FILE__, __LINE__,
                 "langkah " + std::to_string(step) + " operasi " + std::to_string(op) + ": " + why);
      break;
    }
  }
  rawFree(q);
}

// ===========================================================================
// Ketahanan terhadap argumen nullptr (rawan crash, sengaja di paling akhir)
// ===========================================================================

TEST(robust_enqueue_nullptr_tidak_mengubah_queue) {
  Queue q;
  initializeQueue(q);
  enqueue(q, nullptr);
  ASSUME(isQueueEmpty(q));
  CHECK_QUEUE(q);
  enqueue(q, makeElem(1));
  enqueue(q, nullptr);
  ASSUME_EQ(sizeQueue(q), 1);
  CHECK_QUEUE(q);
  rawFree(q);
}

TEST_MAIN()
