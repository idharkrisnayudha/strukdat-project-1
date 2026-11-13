# Topik 6 — Playlist Layanan Streaming Musik (MLL-01)
## Deskripsi
Pengguna dapat menyimpan banyak lagu ke playlist-nya, dan satu lagu dapat disimpan oleh banyak pengguna.

Struktur data. Parent: Pengguna (`username`, `nama`, `tipe akun`). Child: Lagu (`ID`, `judul`, `penyanyi`, `genre`, `durasi`). Relasi: list relasi di dalam parent yang menunjuk ke lagu. BST: Katalog lagu dengan key ID lagu (menyimpan jumlah diputar).

### Fungsionalitas khusus.
- Memutar lagu (menambah jumlah diputar di BST).
- Menampilkan lagu yang paling banyak disimpan pengguna.
- Rekomendasi sederhana: menampilkan lagu bergenre sama dengan lagu terbanyak di playlist pengguna yang belum ia simpan.
- Menampilkan total durasi playlist seorang pengguna.

### Fungsionalitas saran (opsional).
- Mode acak (shuffle) saat memutar playlist.
- Daftar 5 lagu trending berdasarkan jumlah diputar.
- Menampilkan lagu yang sama-sama disimpan oleh dua pengguna.