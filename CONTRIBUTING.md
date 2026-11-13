# Panduan Kontribusi (Contributing Guidelines)

Panduan ini dibuat untuk menyamakan standar penulisan kode C++ dalam pengerjaan tugas besar agar bebas dari ambiguitas.

## 1. Struktur Folder Proyek
* `include/` : Berisi seluruh file header (`*.h`).
* `src/` : Berisi seluruh file implementasi (`*.cpp`), termasuk `main.cpp`.

## 2. Aturan Format & Editor
* **Indentasi**: Wajib menggunakan **2 spasi** (bukan karakter tab ukuran 4).
* **Komentar Kode**: **Penulisan komentar hanya dilakukan untuk menghilangkan keambiguan** (`*.h`, `*.cpp`, `CMakeLists.txt`). Kode harus ditulis bersih dan *self-explanatory* melalui penamaan yang tepat.

## 3. Aturan Penamaan (Naming Conventions)

### a. Tipe Data (`struct`, `typedef`, `using`, `class`, `enum`)
Menggunakan **PascalCase** (huruf kapital di awal setiap kata tanpa pemisah spasi atau garis bawah).
* Benar: `InfoType`, `UserAddress`, `TreeNode`, `ChildList`
* Salah: `infotype`, `user_address`, `userAddress`

### b. Variabel & Parameter
Menggunakan **snake_case** (huruf kecil semua, setiap kata dipisahkan dengan garis bawah `_`).
* Benar: `user_address`, `root_node`, `first_child`, `info_data`
* Salah: `userAddress`, `UserAddress`, `useraddress`

### c. Fungsi & Prosedur
Menggunakan **camelCase** (sama seperti penamaan tipe, tetapi huruf paling depan wajib huruf kecil).
* Benar: `createList`, `insertFirstParent`, `findNodeBst`, `deleteUserAddress`
* Salah: `CreateList`, `create_list`, `createlist`

## 4. Penggunaan Macro Header Guard
* Penggunaan `#pragma once` **dilarang keras**.
* Wajib menggunakan `#ifndef`, `#define`, dan `#endif` dengan format `<PATH>_<NAMA_FILE>_H_`.
* Sesuai aturan tanpa komentar di dalam skrip kode, jangan tambahkan komentar setelah `#endif`.

Contoh penulisan untuk file `include/user.h`:
```cpp
#ifndef INCLUDE_USER_H_
#define INCLUDE_USER_H_

struct InfoType {
  int user_id;
};

typedef struct ElementUser* UserAddress;

void createUser(UserAddress &user_address);

#endif

```

Contoh penulisan untuk file di dalam subfolder `include/bst/tree_node.h`:

```cpp
#ifndef INCLUDE_BST_TREE_NODE_H_
#define INCLUDE_BST_TREE_NODE_H_

struct TreeNode {
  int key_value;
  TreeNode* left_child;
  TreeNode* right_child;
};

void initTree(TreeNode* &root_node);

#endif

```

## 5. Cara Build Proyek

### Menggunakan CMake (VS Code / Terminal)

1. Buat direktori build: `mkdir build && cd build`
2. Konfigurasi CMake: `cmake ..`
3. Kompilasi: `cmake --build .`

### Menggunakan Code::Blocks

1. Buka file `Project 1.cbp`.
2. Pastikan pengaturan editor menggunakan ukuran tab/spasi **2**.
3. Tekan **F9** (Build and Run).