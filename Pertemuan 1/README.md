# Laporan Praktikum - Jawaban Pertanyaan

* **Nama:** Lula Khaisha Delavia
* **NIM:** H1H024048
* **Shift Awal:** D
* **Shift Akhir:** D


## 📌 Bagian 1: Analisis Percobaan 1A (LED Blink dengan Kecepatan Dinamis)

### 1. Kondisi Masuk ke Blok `if`
Program akan mengeksekusi blok kode di dalam perintah `if` ketika nilai dari variabel `timeDelay` bernilai **kurang dari atau sama dengan 100 (`timeDelay <= 100`)**. 
* **Artinya:** Setelah LED berkedip secara bertahap semakin cepat (durasi delay berkurang dari 1000 ms hingga mencapai batas 100 ms), program mendeteksi bahwa batas kecepatan maksimal telah tercapai. Pada program awal, kondisi ini memicu jeda statis selama 3 detik dan mereset `timeDelay` kembali ke 1000 ms.

### 2. Kondisi Masuk ke Blok `else`
Program akan masuk ke dalam blok `else` apabila kondisi pada `if` tidak terpenuhi, yaitu ketika nilai `timeDelay` masih **lebih besar dari 100 (`timeDelay > 100`)**.
* **Artinya:** Selama durasi kedipan belum mencapai batas tercepat (100 ms), program akan terus mengurangi nilai `timeDelay` sebesar 100 ms di setiap akhir siklus loop. Hal ini menyebabkan jeda hidup-mati LED semakin pendek dan menghasilkan efek kedipan yang kian cepat.

### 3. Fungsi Perintah `delay(timeDelay)`
Perintah `delay(timeDelay)` berfungsi untuk menghentikan sementara (*suspend*) eksekusi baris program berikutnya selama nilai milidetik yang ditentukan dalam variabel `timeDelay` (1 detik = 1000 milidetik).
Dalam satu siklus kedipan, perintah ini dipanggil sebanyak **dua kali**:
1. Setelah perintah `digitalWrite(ledPin, HIGH)` (Menahan LED dalam kondisi menyala).
2. Setelah perintah `digitalWrite(ledPin, LOW)` (Menahan LED dalam kondisi mati).
* **Tujuan:** Memastikan durasi aktif (ON) dan nonaktif (OFF) LED seimbang, sehingga frekuensi kedipan terlihat konstan di setiap fasenya.

---

## 🛠️ Bagian 2: Modifikasi Percobaan 1A (Alur Lambat ↔ Cepat ↔ Mati)

### Deskripsi Modifikasi
Alur program diubah dari yang semula **Mati → Lambat → Cepat → Reset (Mati)** menjadi alur bolak-balik yang lebih halus: **Lambat → Cepat → Sedang → Mati selama 3 detik → Reset**. Perubahan ini memanfaatkan variabel logika penentu arah perkalian (`arah`).

### Kode Sumber dan Penjelasan Baris demi Baris

```cpp
// Modifikasi Percobaan 1A: LED cepat -> sedang -> mati
const int ledPin = 6;        // LED terhubung ke pin digital 6
int timeDelay = 1000;        // waktu delay awal (lambat)
int arah = -1;               // -1 = percepat, 1 = perlambat

void setup() {
    pinMode(ledPin, OUTPUT); // set pin 6 sebagai output
}

void loop() {
    digitalWrite(ledPin, HIGH);  // nyalakan LED
    delay(timeDelay);            // tahan sesuai timeDelay
    digitalWrite(ledPin, LOW);   // matikan LED
    delay(timeDelay);            // tahan sesuai timeDelay

    // Jika kecepatan sudah mencapai batas cepat (delay <= 100)
    if (timeDelay <= 100) {
        arah = 1;                // ubah arah menjadi perlambat
    }
    // Jika kecepatan sudah mencapai batas lambat (delay >= 1000)
    else if (timeDelay >= 1000) {
        arah = -1;               // ubah arah menjadi percepat
    }

    // Ubah timeDelay: percepat jika arah=-1, perlambat jika arah=1
    timeDelay += arah * 100;

    // Jika delay mencapai 1000 dan sedang dalam mode perlambat (arah=1)
    if (timeDelay == 1000 && arah == 1) {
        digitalWrite(ledPin, LOW);  // pastikan LED mati
        delay(3000);                // mati total selama 3 detik
        timeDelay = 1000;           // reset ke delay awal
        arah = -1;                  // reset arah ke percepatan
   }
 }

```

---

## 📌 Bagian 3: Analisis Percobaan 2A (Efek Perulangan LED Berjalan)

### 1. Efek LED Berjalan dari Kiri ke Kanan

Efek ini diciptakan melalui pemanfaatan struktur perulangan `for` menaik (*increment*). Perulangan diatur mulai dari indeks pin terkecil hingga pin terbesar (`for (int ledPin = 2; ledPin <= 7; ledPin++)`). Di dalam perulangan tersebut, setiap pin diaktifkan (`HIGH`), diberi jeda sejenak, lalu dinonaktifkan (`LOW`) secara bergantian. Karena eksekusi program berurutan secara linier dari pin 2 ke pin 7, mata manusia menangkapnya sebagai pergerakan cahaya dari kiri ke kanan.

### 2. Efek LED Berjalan Kembali dari Kanan ke Kiri

Untuk membalikkan arah aliran cahaya, digunakan struktur perulangan `for` kedua yang bersifat menurun (*decrement*). Iterasi diatur terbalik, dimulai dari pin terbesar menuju pin terkecil (`for (int ledPin = 7; ledPin >= 2; ledPin--)`). Logika eksekusi di dalam blok tetap sama (nyala → delay → mati). Transisi dari perulangan pertama (naik) langsung menuju perulangan kedua (turun) menghasilkan efek animasi bolak-balik yang dinamis, menyerupai pola lampu mobil ikonik *Knight Rider*.

---

## 🛠️ Bagian 4: Modifikasi Percobaan 2A (Aktivasi Kelompok LED Kiri & Kanan)

### Deskripsi Modifikasi

Program ini dimodifikasi untuk memisahkan enam LED menjadi dua kelompok modular: Kelompok Kiri (Pin 2, 3, dan 4) serta Kelompok Kanan (Pin 5, 6, dan 7). Sistem bekerja dengan menyalakan satu kelompok secara utuh (tiga LED sekaligus) secara bergantian dengan kelompok lainnya, bukan berjalan satu per satu.

### Kode Sumber dan Penjelasan Baris demi Baris

```cpp
// Program LED: 3 kiri dan 3 kanan bergantian
int timer = 200;   // jeda perpindahan grup (ms)

void setup() {
    // Inisialisasi semua pin 2-7 sebagai OUTPUT
    for (int i = 2; i < 8; i++) {
        pinMode(i, OUTPUT);
    }
}

void loop() {
    // --- Grup Kiri (pin 2,3,4) menyala ---
    // Matikan semua LED terlebih dahulu (bersihkan kondisi)
    for (int i = 2; i < 8; i++) {
        digitalWrite(i, LOW);
    }
    // Nyalakan 3 LED kiri (pin 2,3,4) bersamaan
    for (int i = 2; i <= 4; i++) {
        digitalWrite(i, HIGH);
    }
    delay(timer);   // tahan selama timer ms

    // --- Grup Kanan (pin 5,6,7) menyala ---
    // Matikan semua LED lagi
    for (int i = 2; i < 8; i++) {
        digitalWrite(i, LOW);
    }
    // Nyalakan 3 LED kanan (pin 5,6,7) bersamaan
    for (int i = 5; i <= 7; i++) {
        digitalWrite(i, HIGH);
    }
    delay(timer);   // tahan selama timer ms
}

---

## 📊 Bagian 5: Pertanyaan Analisis & Kesimpulan Praktikum

### 1. Uraian Hasil Setiap Percobaan

* **Percobaan 1 (LED Blink Dinamis):** Berhasil mensimulasikan kontrol akselerasi kedipan LED tunggal menggunakan manipulasi nilai variabel waktu. Melalui fungsi modifikasi matematika, program mampu membalikkan status percepatan menjadi perlambatan secara dinamis tanpa interupsi eksternal.
* **Percobaan 2 (LED Running):** Berhasil mengontrol array LED multivariabel. Modifikasi yang dilakukan membuktikan bahwa perulangan `for` dapat dimanfaatkan untuk memanipulasi sekelompok pin logika mikroprosesor secara kolektif untuk membentuk pola pencahayaan spasial (Kiri vs Kanan).

### 2. Pengaruh Struktur Perulangan (`for` / `while`) terhadap Jalannya Program

Struktur perulangan memegang peranan krusial dalam efisiensi penulisan sintaksis program:

* Perulangan `for` sangat optimal digunakan ketika **jumlah iterasi atau target rentang pin sudah diketahui pasti** sejak awal (seperti proses inisialisasi pin array 2-7 atau penargetan kelompok pin terisolasi).
* Struktur perulangan mendikte urutan eksekusi instruksi temporal berdasarkan arah pergerakan indeks (*increment/decrement*), yang secara langsung mempengaruhi arah pergerakan visual LED (Kiri ke Kanan atau Kanan ke Kiri).

### 3. Cara Kerja Struktur Percabangan (`if-else`)

Struktur percabangan berperan sebagai otak pengambil keputusan (*decision maker*) logika sistem berdasarkan evaluasi kondisi real-time:

* Mikrokontroler mengevaluasi kondisi logis (misalnya, membandingkan isi `timeDelay` dengan angka batas threshold). Jika bernilai `true`, blok aksi spesifik dijalankan; jika `false`, alur dilempar ke blok alternatif (`else` atau `else if`).
* Dalam konteks praktikum ini, percabangan memastikan batasan fisik (waktu tunggu minimum dan maksimum) tidak terlampaui sehingga kestabilan siklus program tetap terjaga.

### 4. Kombinasi Perulangan dan Percabangan untuk Pengembangan Sistem

Kombinasi antara perulangan (`for`/`while`) dan percabangan (`if-else`) menciptakan pondasi automasi yang kuat: Perulangan bertanggung jawab menangani rutinitas penataan eksekusi pin secara massal, sedangkan percabangan bertugas menyuntikkan sifat adaptif terhadap perubahan parameter di dalam rutinitas tersebut (seperti perubahan arah atau kecepatan).
