#include <iostream>
#include <string>
using namespace std;

struct PtbNode {
    int tahunTerbit;
    string pengarang;
    string judul;
    PtbNode *left;
    PtbNode *right;

    PtbNode(int tahun, string pengarang, string judul) {
        this->tahunTerbit = tahun;
        this->pengarang = pengarang;
        this->judul = judul;
        left = right = nullptr;
        // this merujuk pada variabel anggota (milik struct PtbNode)
    }
};

// Node Stack untuk log undo
struct StackNode {
    string aksi; // "tambah" atau "hapus"
    PtbNode *buku; // data buku
    StackNode *next;

    StackNode(string aksi, PtbNode *buku) {
        this->aksi = aksi;
        this->buku = buku;
        next = nullptr;
    }
};

// Stack untuk undo
struct Stack {
    StackNode *top = nullptr;

    void push(string aksi, PtbNode *buku) {
        StackNode *baru = new StackNode(aksi, new PtbNode(buku->tahunTerbit, buku->pengarang, buku->judul));
        baru->next = top;
        top = baru;
    }

    bool pop(string &aksi, PtbNode *&buku) {
        if (top == nullptr) return false;
        StackNode *hapus = top;
        aksi = hapus->aksi;
        buku = hapus->buku;
        top = top->next;
        delete hapus;
        return true;
    }

    bool isEmpty() {
        return top == nullptr;
    }
};

// BST diurutkan berdasarkan judul
void insert(PtbNode *&root, int tahun, string pengarang, string judul, Stack &logUndo, bool isUndo = false) {
    if (root == nullptr) {
        root = new PtbNode(tahun, pengarang, judul);
        if (!isUndo) {
            logUndo.push("tambah", root);
        }
        cout << "Buku \"" << judul << "\" berhasil ditambahkan.\n";
        return;
    }

    if (judul < root->judul) {
        insert(root->left, tahun, pengarang, judul, logUndo, isUndo);
    } else if (judul > root->judul) {
        insert(root->right, tahun, pengarang, judul, logUndo, isUndo);
    } else {
        cout << "Buku dengan judul \"" << judul << "\" sudah ada.\n";
    }
}

void inorder(PtbNode *root) {
    if (root != nullptr) {
        inorder(root->left);
        cout << "Judul: " << root->judul << "\n";
        cout << "Pengarang: " << root->pengarang << "\n";
        cout << "Tahun Terbit: " << root->tahunTerbit << "\n";
        cout << "-----------------------------\n";
        inorder(root->right);
    }
}

// Mencari node minimum di subtree (untuk membantu penghapusan)
PtbNode* findMin(PtbNode *node) {
    while (node->left != nullptr) node = node->left;
    return node;
}

// Menghapus node berdasarkan judul
PtbNode* hapusBuku(PtbNode *root, string judul, Stack &logUndo, bool isUndo = false) {
    if (root == nullptr) {
        cout << "Buku dengan judul \"" << judul << "\" tidak ditemukan.\n";
        return nullptr;
    }

    if (judul < root->judul) {
        root->left = hapusBuku(root->left, judul, logUndo, isUndo);
    } else if (judul > root->judul) {
        root->right = hapusBuku(root->right, judul, logUndo, isUndo);
    } else {
        // Buku ditemukan
        if (!isUndo) {
            // Catat ke stack undo
            logUndo.push("hapus", root);
        }

        if (root->left == nullptr) {
            PtbNode *temp = root->right;
            delete root;
            cout << "Buku \"" << judul << "\" berhasil dihapus.\n";
            return temp;
        } else if (root->right == nullptr) {
            PtbNode *temp = root->left;
            delete root;
            cout << "Buku \"" << judul << "\" berhasil dihapus.\n";
            return temp;
        } else {
            // Dua anak
            PtbNode *minNode = findMin(root->right);
            root->judul = minNode->judul;
            root->pengarang = minNode->pengarang;
            root->tahunTerbit = minNode->tahunTerbit;
            root->right = hapusBuku(root->right, minNode->judul, logUndo, true); // true: agar tidak catat lagi di undo
        }
    }
    return root;
}

// Fungsi undo
void undo(PtbNode *&root, Stack &logUndo) {
    if (logUndo.isEmpty()) {
        cout << "Tidak ada aksi untuk di-undo.\n";
        return;
    }

    string aksi;
    PtbNode *buku;
    logUndo.pop(aksi, buku);

    if (aksi == "tambah") {
        // Undo tambah = hapus buku
        root = hapusBuku(root, buku->judul, logUndo, true); // true = supaya undo tidak dicatat ulang
        cout << "Undo: Buku \"" << buku->judul << "\" dihapus.\n";
    } else if (aksi == "hapus") {
        // Undo hapus = tambah buku
        insert(root, buku->tahunTerbit, buku->pengarang, buku->judul, logUndo, true);
        cout << "Undo: Buku \"" << buku->judul << "\" dikembalikan.\n";
    }

    delete buku;
}

int main() {
    PtbNode *root = nullptr;
    Stack logUndo;
    int pilihan;

    do {
        cout << "\nMenu:\n";
        cout << "1. Tambah Buku\n";
        cout << "2. Tampilkan Semua Buku\n";
        cout << "3. Hapus Buku\n";
        cout << "4. Undo Aksi Terakhir\n";
        cout << "5. Keluar\n";
        cout << "Pilihan: ";
        cin >> pilihan;
        cin.ignore();

        if (pilihan == 1) {
            string judul, pengarang;
            int tahun;
            cout << "Masukkan Judul: ";
            getline(cin, judul);
            cout << "Masukkan Pengarang: ";
            getline(cin, pengarang);
            cout << "Masukkan Tahun Terbit: ";
            cin >> tahun;
            cin.ignore();
            insert(root, tahun, pengarang, judul, logUndo);
        } else if (pilihan == 2) {
            cout << "\nDaftar Buku:\n";
            inorder(root);
        } else if (pilihan == 3) {
            string judul;
            cout << "Masukkan Judul Buku yang akan dihapus: ";
            getline(cin, judul);
            root = hapusBuku(root, judul, logUndo);
        } else if (pilihan == 4) {
            undo(root, logUndo);
        }

    } while (pilihan != 5);

    return 0;
}
