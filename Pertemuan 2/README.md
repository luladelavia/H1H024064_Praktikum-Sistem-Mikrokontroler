
```markdown
# Laporan Praktikum Mikrokomputer: Seven Segment & Push Button

* **Nama** : Lula Khaisha Delavia
* **NIM** : H1H024064  
* **Shift Awal** : D  
* **Shift Akhir** : D 

---

## 2.5 Tugas Praktikum 1: Seven Segment Otomatis (F ke 0)

### 1. Skematik Rangkaian
![Schematic](https://github.com/user-attachments/assets/5364bd69-553a-42a6-8993-76464487624b)

### 2. Efek Jika `num` > 15
Program akan mengalami *array out-of-bounds* karena membaca memori di luar indeks `digitPattern` (0–15). Dampaknya, tampilan Seven Segment menjadi acak/tidak valid, dan berisiko membuat program *crash*.

### 3. Jenis Konfigurasi Display
Menggunakan **Common Cathode**. Karena semua katoda LED terhubung ke *Ground* (GND), segmen hanya akan menyala saat pin Arduino mengirimkan sinyal **HIGH (1)** untuk mengalirkan arus.

### 4. Kode Program (Hitung Mundur F ke 0)
```cpp
#include <Arduino.h>

// Pemetaan pin Arduino ke segmen: {a, b, c, d, e, f, g, dp}
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

// Pola bit katoda untuk karakter 0–F (1 = ON, 0 = OFF)
byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0}, {0,1,1,0,0,0,0,0}, {1,1,0,1,1,0,1,0}, {1,1,1,1,0,0,1,0}, // 0, 1, 2, 3
  {0,1,1,0,0,1,1,0}, {1,0,1,1,0,1,1,0}, {1,0,1,1,1,1,1,0}, {1,1,1,0,0,0,0,0}, // 4, 5, 6, 7
  {1,1,1,1,1,1,1,0}, {1,1,1,1,0,1,1,0}, {1,1,1,0,1,1,1,0}, {0,0,1,1,1,1,1,0}, // 8, 9, A, b
  {1,0,0,1,1,1,0,0}, {0,1,1,1,1,0,1,0}, {1,0,0,1,1,1,1,0}, {1,0,0,0,1,1,1,0}  // C, d, E, F
};

void displayDigit(int num) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i], digitPattern[num][i]);
  }
}

void setup() {
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
}

void loop() {
  // Loop counter bergerak turun dari indeks 15 (F) ke 0
  for (int i = 15; i >= 0; i--) {
    displayDigit(i);
    delay(1000);
  }
}

```

---

## 2.6 Tugas Praktikum 2: Up/Down Counter Manual

### 1. Skematik Rangkaian

### 2. Fungsi & Keuntungan `INPUT_PULLUP`

Mengaktifkan resistor internal Arduino untuk menjaga pin tetap **HIGH** saat tombol terbuka, dan menjadi **LOW** saat ditekan ke GND.

* **Keuntungan:** Menghemat komponen (tidak butuh resistor eksternal), menyederhanakan *wiring*, dan meredam *noise* sinyal.

### 3. Analisis Kegagalan Segmen (Mati)

* **Hardware:** Kabel kendur/salah pin, resistor rusak, LED segmen putus, atau jalur GND utama lepas.
* **Software:** Kesalahan pemetaan pin pada array `segmentPins`, eror bit pada kode `digitPattern`, atau lupa mengeset `pinMode` ke `OUTPUT`.

### 4. Kode Program (Increment/Decrement Counter)

```cpp
#include <Arduino.h>

const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4}; // Pin Seven Segment
const int btnUp   = 2;                                // Tombol Tambah (Increment)
const int btnDown = 3;                                // Tombol Kurang (Decrement)

byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0}, {0,1,1,0,0,0,0,0}, {1,1,0,1,1,0,1,0}, {1,1,1,1,0,0,1,0},
  {0,1,1,0,0,1,1,0}, {1,0,1,1,0,1,1,0}, {1,0,1,1,1,1,1,0}, {1,1,1,0,0,0,0,0},
  {1,1,1,1,1,1,1,0}, {1,1,1,1,0,1,1,0}, {1,1,1,0,1,1,1,0}, {0,0,1,1,1,1,1,0},
  {1,0,0,1,1,1,0,0}, {0,1,1,1,1,0,1,0}, {1,0,0,1,1,1,1,0}, {1,0,0,0,1,1,1,0}
};

int currentDigit   = 0;    // Menyimpan nilai hitungan aktif
bool lastUpState   = HIGH; // Status tombol Up sebelumnya
bool lastDownState = HIGH; // Status tombol Down sebelumnya

void displayDigit(int num) {
  for (int i = 0; i < 8; i++) {
    digitalWrite(segmentPins[i], digitPattern[num][i]);
  }
}

void setup() {
  for (int i = 0; i < 8; i++) pinMode(segmentPins[i], OUTPUT);
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  displayDigit(currentDigit); // Tampilan awal angka 0
}

void loop() {
  bool upState   = digitalRead(btnUp);   // Baca tombol tambah
  bool downState = digitalRead(btnDown); // Baca tombol kurang

  // Deteksi pencetan tombol Up (Transisi HIGH ke LOW)
  if (lastUpState == HIGH && upState == LOW) {
    currentDigit++;
    if (currentDigit > 15) currentDigit = 0; // Kembali ke 0 jika lewat dari F
    displayDigit(currentDigit);
    delay(50); // Debounce singkat
  }

  // Deteksi pencetan tombol Down (Transisi HIGH ke LOW)
  if (lastDownState == HIGH && downState == LOW) {
    currentDigit--;
    if (currentDigit < 0) currentDigit = 15; // Kembali ke F jika kurang dari 0
    displayDigit(currentDigit);
    delay(50); // Debounce singkat
  }

  lastUpState   = upState;
  lastDownState = downState;
}

```

---

## 2.7 Analisis & Prinsip Kerja

### 1. Resume Hasil Percobaan

* **Percobaan 2A:** Display berhasil melakukan *looping* otomatis memunculkan sekuens karakter heksadesimal (0–F) dengan jeda konstan 1 detik.
* **Percobaan 2B:** Kontrol beralih ke *user*. Sistem sukses melakukan *update* karakter secara presisi berkat implementasi *edge detection* yang mencegah pembacaan ganda akibat efek *bouncing* tombol.

### 2. Mekanisme Kerja Seven Segment

Display tersusun dari 7 LED batang (a-g) dan 1 LED titik (dp). Karakter terbentuk lewat kombinasi nyala LED spesifik yang polanya dikodekan dalam bentuk logika biner lewat array `digitPattern`. Mengingat jenis yang dipakai adalah *common cathode*, pin Arduino harus mengirimkan tegangan positif (HIGH) ke segmen terkait agar arus mengalir melewati LED menuju *ground* bersama dan menciptakan visual karakter.

### 3. Cara Kerja Sistem Counter

* **Sistem Otomatis (2A):** Mengandalkan struktur internal perulangan `for` bawaan kode yang berjalan sequensial dari batas bawah ke batas atas (atau sebaliknya) terikat interupsi waktu via `delay()`.
* **Sistem Manual (2B):** Logika bertumpu pada pendeteksian pulsa turun sinyal (*falling edge* dari HIGH ke LOW) saat tombol dipencet. Begitu pemicu terbaca, variabel dinamis `currentDigit` dimodifikasi (`++` atau `--`) dengan proteksi sirkular (*wrap-around* batas nilai 0 dan 15) sebelum dilempar kembali ke fungsi display.

```

```
