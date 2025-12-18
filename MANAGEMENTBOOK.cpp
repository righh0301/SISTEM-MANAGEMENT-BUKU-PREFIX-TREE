#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <set>
#include <limits> 
#include <stdexcept> 
#include <map>

using namespace std;

const string CSV_FILENAME = "databuku.csv";

// --- Helper Function ---
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), 
        [](unsigned char c){ return tolower(c); });
    return s;
}

// --- 1. Struktur Data Buku ---
struct Buku {
    string Title;
    string Author;
    int Year_Published = 0; 
    string Genre;
    double Rating = 0.0;     
    int Ratings_Count = 0;  
    string Description;
    string Popularity;

    void displayDetail() const {
        cout << "\n==================================================" << endl;
        cout << "JUDUL: " << Title << endl; 
        cout << "==================================================" << endl;
        cout << "Penulis: " << Author << endl;
        cout << "Tahun Terbit: " << Year_Published << endl;
        cout << "Genre: " << Genre << endl;
        cout << fixed << setprecision(2);
        cout << "Rating: " << Rating << " (" << Ratings_Count << " ulasan)" << endl;
        cout << "Popularitas: " << Popularity << endl;
        cout << "Deskripsi: " << Description << endl;
        cout << "==================================================" << endl;
    }
};

// --- STRUKTUR DATA PREFIX TREE (TRIE) ---
struct TrieNode {
    map<char, TrieNode*> children;
    bool isEndOfWord = false;
};

void insertTrie(TrieNode* root, string key) {
    TrieNode* curr = root;
    for (char c : toLower(key)) {
        if (curr->children.find(c) == curr->children.end()) {
            curr->children[c] = new TrieNode();
        }
        curr = curr->children[c];
    }
    curr->isEndOfWord = true;
}

bool searchTrie(TrieNode* root, string key) {
    TrieNode* curr = root;
    for (char c : toLower(key)) {
        if (curr->children.find(c) == curr->children.end()) return false;
        curr = curr->children[c];
    }
    return curr->isEndOfWord;
}

// Global Variables
vector<Buku> semuaBuku; 
TrieNode* akarTrie = new TrieNode();

// --- 2. Fungsi Memuat Data ---
vector<Buku> loadData(const string& filename) {
    vector<Buku> daftarBuku;
    ifstream file(filename);
    string line;
    if (!file.is_open()) {
        throw runtime_error("FATAL ERROR: Gagal membuka file " + filename);
    }
    getline(file, line); // Skip header

    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string cell;
        Buku buku;
        try {
            getline(ss, buku.Title, ',');
            getline(ss, buku.Author, ',');
            getline(ss, cell, ','); buku.Year_Published = stoi(cell);
            getline(ss, buku.Genre, ',');
            getline(ss, cell, ','); buku.Rating = stod(cell);
            getline(ss, cell, ','); buku.Ratings_Count = stoi(cell);
            getline(ss, buku.Description, ',');
            getline(ss, buku.Popularity);

            daftarBuku.push_back(buku);
            insertTrie(akarTrie, buku.Title); // Masukkan ke Trie

        } catch (...) {}
    }
    return daftarBuku;
}

// --- 3. Implementasi Fungsi ---

/**
 * Fungsi 1: validasiJudulBuku() - MENGGUNAKAN PREFIX TREE
 * Mengecek keberadaan judul secara instan.
 */
void validasiJudulBuku() {
    string judul;
    cout << "Masukkan judul buku yang ingin divalidasi: ";
    getline(cin, judul);

    cout << "\n==================================================" << endl;
    if (searchTrie(akarTrie, judul)) {
        cout << "HASIL VALIDASI: Judul '" << judul << "' TERSEDIA." << endl;
        cout << "Judul ini ditemukan dalam sistem menggunakan Prefix Tree." << endl;
    } else {
        cout << "HASIL VALIDASI: Judul '" << judul << "' TIDAK DITEMUKAN." << endl;
        cout << "Pastikan ejaan dan huruf sudah benar." << endl;
    }
    cout << "==================================================" << endl;
}

void getSemuaBuku() {
    cout << "\n==================================================" << endl;
    cout << "            DAFTAR SELURUH BUKU (" << semuaBuku.size() << " total)" << endl;
    cout << "==================================================" << endl;
    for (size_t i = 0; i < semuaBuku.size(); ++i) {
        cout << i+1 << ". " << semuaBuku[i].Title << " - " << semuaBuku[i].Author << " (Genre: " << semuaBuku[i].Genre << ")" << endl;
    }
    cout << "==================================================" << endl;
}

void cariBuku() {
    string keyword;
    cout << "Masukkan keyword (judul atau penulis) untuk dicari: ";
    getline(cin, keyword); 
    string lowerKeyword = toLower(keyword);
    vector<const Buku*> hasil;
    for (const auto& b : semuaBuku) {
        if (toLower(b.Title).find(lowerKeyword) != string::npos || toLower(b.Author).find(lowerKeyword) != string::npos)
            hasil.push_back(&b);
    }
    cout << "\n==================================================" << endl;
    cout << "HASIL PENCARIAN KEYWORD: '" << keyword << "' (" << hasil.size() << " ditemukan)" << endl;
    cout << "==================================================" << endl;
    for (const auto* b : hasil) {
        cout << "* " << b->Title << " - " << b->Author << " (Rating: " << b->Rating << ")" << endl;
    }
    cout << "==================================================" << endl;
}

void getBukuPopuler() {
    vector<const Buku*> populer;
    for (const auto& b : semuaBuku) if (b.Popularity == "Populer") populer.push_back(&b);
    sort(populer.begin(), populer.end(), [](const Buku* a, const Buku* b) { return a->Rating > b->Rating; });
    cout << "\n==================================================" << endl;
    cout << "             DAFTAR BUKU POPULER (" << populer.size() << " total)" << endl;
    cout << "==================================================" << endl;
    for (const auto* b : populer) cout << "* " << b->Title << " (Rating: " << b->Rating << ")" << endl;
    cout << "==================================================" << endl;
}

void cariBukuByGenre() {
    set<string> genres;
    for (const auto& b : semuaBuku) genres.insert(b.Genre);
    cout << "\n==================================================" << endl;
    cout << "          DAFTAR GENRE YANG TERSEDIA" << endl;
    cout << "==================================================" << endl;
    int i = 1;
    for (const auto& g : genres) {
        cout << left << setw(15) << g;
        if (i++ % 4 == 0) cout << endl;
    }
    cout << "\nMasukkan nama Genre: ";
    string input; getline(cin, input);
    string lowInput = toLower(input);
    cout << "\n==================================================" << endl;
    for (const auto& b : semuaBuku) {
        if (toLower(b.Genre) == lowInput) cout << "* " << b.Title << " - " << b.Author << endl;
    }
    cout << "==================================================" << endl;
}

void getDetailBuku() {
    string input;
    cout << "\nMasukkan Judul Buku untuk detail: ";
    getline(cin, input);
    string lowInput = toLower(input);
    for (const auto& b : semuaBuku) {
        if (toLower(b.Title) == lowInput) { b.displayDetail(); return; }
    }
    cout << "Buku tidak ditemukan." << endl;
}



// --- 4. Main Menu ---
int main() {
    try {
        cout << "Memuat data dari '" << CSV_FILENAME << "'..." << endl;
        semuaBuku = loadData(CSV_FILENAME);
    } catch (const exception& e) {
        cerr << e.what() << endl;
        return 1;
    }
    cout << "Data berhasil dimuat. Total: " << semuaBuku.size() << " entri." << endl; 

    int pilihan;
    do {
        cout << "\n\n--- SISTEM MANAJEMEN BUKU (DATA LENGKAP) ---" << endl;
        cout << "1. validasiJudulBuku() - Cek Ketersediaan Judul (Prefix Tree)" << endl;
        cout << "2. getSemuaBuku() - Tampilkan Semua Buku" << endl;
        cout << "3. cariBuku() - Cari Buku (Judul/Penulis)" << endl;
        cout << "4. getBukuPopuler() - Tampilkan Buku Populer" << endl;
        cout << "5. cariBukuByGenre() - Cari Buku Berdasarkan Genre" << endl;
        cout << "6. getDetailBuku() - Tampilkan Detail Buku" << endl;
        cout << "0. Keluar" << endl;
        cout << "Masukkan pilihan: ";
        
        if (!(cin >> pilihan)) {
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
            pilihan = -1; continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        switch (pilihan) {
            case 1: validasiJudulBuku(); break;
            case 2: getSemuaBuku(); break;
            case 3: cariBuku(); break;
            case 4: getBukuPopuler(); break;
            case 5: cariBukuByGenre(); break;
            case 6: getDetailBuku(); break;
            case 0: cout << "Keluar..." << endl; break;
        }
    } while (pilihan != 0);
    return 0;

}
