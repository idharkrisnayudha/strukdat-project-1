// test_helper.h
// Kerangka test mini (tanpa library eksternal) yang dipakai test_user.cpp,
// test_song.cpp, test_genre.cpp, dan test_queue.cpp.
//
// Isi:
//   - TEST(nama) { ... }       : mendaftarkan satu test case
//   - CHECK / CHECK_EQ         : pemeriksaan WAJIB (gagal = bug di ADT)
//   - ASSUME / ASSUME_EQ       : pemeriksaan ASUMSI. Dipakai untuk hal yang tidak
//                                ditulis tegas di header (mis. nilai `out` saat
//                                list kosong). Kalau gagal hanya jadi peringatan.
//   - th::liveAllocs()         : jumlah alokasi `new` yang belum di-`delete`
//                                (untuk mendeteksi memory leak / tidak dealokasi)
//   - CoutCapture              : menangkap output std::cout (untuk test dump*)
//
// Opsi saat menjalankan:
//   ./test_x                   jalankan semua test
//   ./test_x --list            tampilkan daftar nama test
//   ./test_x --only=teks       hanya test yang namanya mengandung "teks"
//   ./test_x --skip=teks       lewati test yang namanya mengandung "teks"
//   ./test_x --isolate         tiap test dijalankan di proses terpisah, jadi
//                              kalau satu test crash (segfault), test lain
//                              tetap jalan.
//
// Definisi operator new/delete di bawah dipakai untuk menghitung alokasi.
// Kalau bentrok dengan kodemu, compile dengan -DTH_NO_ALLOC_TRACKING
// (test yang mengecek memori akan selalu lolos).

#ifndef INCLUDE_TEST_HELPER_H
#define INCLUDE_TEST_HELPER_H

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <new>
#include <sstream>
#include <string>
#include <vector>

#if defined(__unix__) || defined(__APPLE__)
#include <sys/wait.h>
#include <unistd.h>
#endif

// ---------------------------------------------------------------------------
// Penghitung alokasi
// ---------------------------------------------------------------------------
namespace th {
inline long& allocCount() { static long n = 0; return n; }
inline long& freeCount() { static long n = 0; return n; }
inline long liveAllocs() { return allocCount() - freeCount(); }
}  // namespace th

#ifndef TH_NO_ALLOC_TRACKING
void* operator new(std::size_t n) {
  void* p = std::malloc(n ? n : 1);
  if (p == nullptr) throw std::bad_alloc();
  ++th::allocCount();
  return p;
}
void operator delete(void* p) noexcept {
  if (p == nullptr) return;
  ++th::freeCount();
  std::free(p);
}
void operator delete(void* p, std::size_t) noexcept { operator delete(p); }
#endif

// ---------------------------------------------------------------------------
// Registrasi & pelaporan
// ---------------------------------------------------------------------------
namespace th {

struct Entry {
  const char* name;
  void (*fn)();
};

inline std::vector<Entry>& registry() {
  static std::vector<Entry> r;
  return r;
}

struct State {
  const char* current;
  int checks;
  int hardFails;
  int assumeFails;
  int testHardFails;
  int testAssumeFails;
};

inline State& state() {
  static State s = {"", 0, 0, 0, 0, 0};
  return s;
}

struct Registrar {
  Registrar(const char* name, void (*fn)()) {
    Entry e = {name, fn};
    registry().push_back(e);
  }
};

inline void report(bool ok, bool soft, const char* expr, const char* file,
                   int line, const std::string& detail) {
  State& s = state();
  s.checks++;
  if (ok) return;
  if (soft) {
    s.assumeFails++;
    s.testAssumeFails++;
    std::printf("    [ASUMSI?] %s:%d  %s", file, line, expr);
  } else {
    s.hardFails++;
    s.testHardFails++;
    std::printf("    [GAGAL]   %s:%d  %s", file, line, expr);
  }
  if (!detail.empty()) std::printf("\n              -> %s", detail.c_str());
  std::printf("\n");
  std::fflush(stdout);
}

template <class A, class B>
inline void reportEq(const A& a, const B& b, const char* expr, const char* file,
                     int line, bool soft) {
  bool ok = (a == b);
  std::string detail;
  if (!ok) {
    std::ostringstream os;
    os << "didapat: \"" << a << "\", seharusnya: \"" << b << "\"";
    detail = os.str();
  }
  report(ok, soft, expr, file, line, detail);
}

inline std::string join(const std::vector<std::string>& v) {
  std::string s;
  for (size_t i = 0; i < v.size(); ++i) {
    if (i) s += ",";
    s += v[i];
  }
  return s;
}

inline std::string joinInts(const std::vector<int>& v) {
  std::string s;
  for (size_t i = 0; i < v.size(); ++i) {
    if (i) s += ",";
    s += std::to_string(v[i]);
  }
  return s;
}

inline bool contains(const std::string& hay, const std::string& needle) {
  return hay.find(needle) != std::string::npos;
}

// Menangkap semua yang ditulis ke std::cout selama objek ini hidup.
class CoutCapture {
 public:
  CoutCapture() : old_(std::cout.rdbuf(buf_.rdbuf())) {}
  ~CoutCapture() { std::cout.rdbuf(old_); }
  std::string str() const { return buf_.str(); }

 private:
  std::ostringstream buf_;
  std::streambuf* old_;
};

// ---------------------------------------------------------------------------
// Penangkap crash
// ---------------------------------------------------------------------------
inline void onSignal(int sig) {
  std::fflush(stdout);
  const char* why = "sinyal tidak dikenal";
  if (sig == SIGSEGV)
    why = "SIGSEGV (akses memori ilegal: nullptr / pointer menggantung / use-after-free)";
  else if (sig == SIGABRT)
    why = "SIGABRT (double free / abort / exception tidak tertangani)";
  else if (sig == SIGFPE)
    why = "SIGFPE";
  else if (sig == SIGILL)
    why = "SIGILL";
#ifdef SIGALRM
  else if (sig == SIGALRM)
    why = "TIMEOUT (kemungkinan infinite loop / list melingkar)";
#endif
  std::fprintf(stderr, "\n[CRASH] test '%s' berhenti: %s\n", state().current, why);
  std::fprintf(stderr, "        Jalankan dengan --isolate agar test lain tetap berjalan.\n");
  std::fflush(stderr);
  std::_Exit(3);
}

inline void armAlarm(unsigned secs) {
#ifdef SIGALRM
  alarm(secs);
#else
  (void)secs;
#endif
}

// ---------------------------------------------------------------------------
// Runner
// ---------------------------------------------------------------------------
inline int runAll(int argc, char** argv) {
  std::string only, skip, runExact;
  bool list = false, isolate = false;
  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    if (a == "--list") list = true;
    else if (a == "--isolate") isolate = true;
    else if (a.compare(0, 7, "--only=") == 0) only = a.substr(7);
    else if (a.compare(0, 7, "--skip=") == 0) skip = a.substr(7);
    else if (a.compare(0, 6, "--run=") == 0) runExact = a.substr(6);
    else {
      std::printf("Argumen tidak dikenal: %s\n", argv[i]);
      std::printf("Pakai: --list | --only=teks | --skip=teks | --isolate\n");
      return 64;
    }
  }

  std::vector<Entry> chosen;
  for (size_t i = 0; i < registry().size(); ++i) {
    const Entry& e = registry()[i];
    std::string name = e.name;
    if (!runExact.empty()) {
      if (name != runExact) continue;
    } else {
      if (!only.empty() && !contains(name, only)) continue;
      if (!skip.empty() && contains(name, skip)) continue;
    }
    chosen.push_back(e);
  }

  if (list) {
    for (size_t i = 0; i < chosen.size(); ++i) std::printf("%s\n", chosen[i].name);
    return 0;
  }

  if (isolate) {
    std::string prog = argv[0];
    if (prog.find(' ') != std::string::npos) prog = "\"" + prog + "\"";
    int passed = 0, failed = 0, crashed = 0, warned = 0;
    for (size_t i = 0; i < chosen.size(); ++i) {
      std::string cmd = prog + " --run=" + chosen[i].name;
      std::fflush(stdout);
      int st = std::system(cmd.c_str());
      int code = st;
#if defined(WIFEXITED)
      code = WIFEXITED(st) ? WEXITSTATUS(st) : 3;
#endif
      if (code == 0) ++passed;
      else if (code == 2) { ++passed; ++warned; }
      else if (code == 1) ++failed;
      else ++crashed;
    }
    std::printf("\n=== RINGKASAN (isolate) ===\n");
    std::printf("lolos: %d (dengan peringatan asumsi: %d), gagal: %d, crash: %d, total: %d\n",
                passed, warned, failed, crashed, (int)chosen.size());
    return (failed + crashed) ? 1 : 0;
  }

  std::signal(SIGSEGV, onSignal);
  std::signal(SIGABRT, onSignal);
  std::signal(SIGFPE, onSignal);
  std::signal(SIGILL, onSignal);
#ifdef SIGALRM
  std::signal(SIGALRM, onSignal);
#endif

  int passed = 0, failed = 0, warned = 0;
  for (size_t i = 0; i < chosen.size(); ++i) {
    State& s = state();
    s.current = chosen[i].name;
    s.testHardFails = 0;
    s.testAssumeFails = 0;
    std::printf("[ RUN    ] %s\n", chosen[i].name);
    std::fflush(stdout);
    armAlarm(30);
    chosen[i].fn();
    armAlarm(0);
    if (s.testHardFails == 0) {
      ++passed;
      if (s.testAssumeFails) {
        ++warned;
        std::printf("[     OK ] %s  (%d asumsi tidak terpenuhi)\n", chosen[i].name, s.testAssumeFails);
      } else {
        std::printf("[     OK ] %s\n", chosen[i].name);
      }
    } else {
      ++failed;
      std::printf("[ GAGAL  ] %s  (%d pemeriksaan gagal)\n", chosen[i].name, s.testHardFails);
    }
  }

  if (!runExact.empty()) {
    if (failed) return 1;
    return warned ? 2 : 0;
  }
  std::printf("\n=== RINGKASAN ===\n");
  std::printf("test lolos : %d (%d di antaranya punya asumsi yang tidak terpenuhi)\n", passed, warned);
  std::printf("test gagal : %d\n", failed);
  std::printf("pemeriksaan: %d, gagal wajib: %d, asumsi tidak terpenuhi: %d\n",
              state().checks, state().hardFails, state().assumeFails);
  return failed ? 1 : 0;
}

}  // namespace th

#define TEST(name)                                          \
  static void name();                                       \
  static th::Registrar registrar_##name(#name, name);       \
  static void name()

#define TEST_MAIN() \
  int main(int argc, char** argv) { return th::runAll(argc, argv); }

#define CHECK(cond) th::report((cond), false, #cond, __FILE__, __LINE__, "")
#define ASSUME(cond) th::report((cond), true, #cond, __FILE__, __LINE__, "")
#define CHECK_EQ(actual, expected) \
  th::reportEq((actual), (expected), #actual " == " #expected, __FILE__, __LINE__, false)
#define ASSUME_EQ(actual, expected) \
  th::reportEq((actual), (expected), #actual " == " #expected, __FILE__, __LINE__, true)

// Membebaskan `ptr` hanya kalau jumlah alokasi tidak berubah sejak `live`
// diambil. Kalau ADT ternyata sudah menghancurkan elemen yang seharusnya
// hanya dikeluarkan, kita tidak ikut double-free.
#define FREE_IF_ALIVE(ptr, live) \
  do { if (th::liveAllocs() == (live)) delete (ptr); } while (0)

#endif  // INCLUDE_TEST_HELPER_H
