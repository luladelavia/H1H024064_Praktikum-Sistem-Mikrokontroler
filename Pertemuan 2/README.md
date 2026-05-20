# Laporan Praktikum - Jawaban Pertanyaan

* **Nama:** Lula Khaisha Delavia
* **NIM:** H1H024064
* **Shift Awal:** D
* **Shift Akhir:** D


---

## Tugas Praktikum 1: Seven Segment Otomatis (F ke 0)

### 1. Skematik Rangkaian
![Schematic](https://github.com/user-attachments/assets/5364bd69-553a-42a6-8993-76464487624b)

### 2. Efek Jika `num` > 15
Program akan mengalami *array out-of-bounds* karena membaca memori di luar indeks `digitPattern` (0–15). Dampaknya, tampilan Seven Segment menjadi acak/tidak valid, dan berisiko membuat program *crash*.

### 3. Jenis Konfigurasi Display
Menggunakan **Common Cathode**. Karena semua katoda LED terhubung ke *Ground* (GND), segmen hanya akan menyala saat pin Arduino mengirimkan sinyal **HIGH (1)** untuk mengalirkan arus.

### 4. Kode Program (Hitung Mundur F ke 0)
```cpp
#include <Arduino.h>
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

const int buttonPin = 3;

int counter = 0;

bool lastButtonState = HIGH;

byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0}, //0
  {0,1,1,0,0,0,0,0}, //1
  {1,1,0,1,1,0,1,0}, //2
  {1,1,1,1,0,0,1,0}, //3
  {0,1,1,0,0,1,1,0}, //4
  {1,0,1,1,0,1,1,0}, //5 
  {1,0,1,1,1,1,1,0}, //6
  {1,1,1,0,0,0,0,0}, //7
  {1,1,1,1,1,1,1,0}, //8
  {1,1,1,1,0,1,1,0}, //9
  {1,1,1,0,1,1,1,0}, //A
  {0,0,1,1,1,1,1,0}, //b
  {1,0,0,1,1,1,0,0}, //C
  {0,1,1,1,1,0,1,0}, //d
  {1,0,0,1,1,1,1,0}, //E
  {1,0,0,0,1,1,1,0}  //F
};

void displayDigit(int num)
{
  for(int i=0; i<8; i++)
  {
    digitalWrite(segmentPins[i], !digitPattern[num][i]);
  }
}

void setup()
{
  for(int i=0; i<8; i++)
  {
    pinMode(segmentPins[i], OUTPUT);
  }

  pinMode(buttonPin, INPUT_PULLUP);

  displayDigit(counter);
}

void loop()
{
  bool currentButtonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && currentButtonState == LOW)
  {
    counter++;
    if(counter > 15) counter = 0;

    displayDigit(counter); 

    delay(200); 
  }

  lastButtonState = currentButtonState;
}

```

---

## Tugas Praktikum 2: Up/Down Counter Manual

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
const int segmentPins[8] = {7, 6, 5, 11, 10, 8, 9, 4};

const int buttonUp = 3;   
const int buttonDown = 2; 
int counter = 0;

bool lastUpState = HIGH;
bool lastDownState = HIGH;

byte digitPattern[16][8] = {
  {1,1,1,1,1,1,0,0}, //0
  {0,1,1,0,0,0,0,0}, //1
  {1,1,0,1,1,0,1,0}, //2
  {1,1,1,1,0,0,1,0}, //3
  {0,1,1,0,0,1,1,0}, //4
  {1,0,1,1,0,1,1,0}, //5 
  {1,0,1,1,1,1,1,0}, //6
  {1,1,1,0,0,0,0,0}, //7
  {1,1,1,1,1,1,1,0}, //8
  {1,1,1,1,0,1,1,0}, //9
  {1,1,1,0,1,1,1,0}, //A
  {0,0,1,1,1,1,1,0}, //b
  {1,0,0,1,1,1,0,0}, //C
  {0,1,1,1,1,0,1,0}, //d
  {1,0,0,1,1,1,1,0}, //E
  {1,0,0,0,1,1,1,0}  //F
};

void displayDigit(int num) {
  for(int i=0; i<8; i++) {
    digitalWrite(segmentPins[i], !digitPattern[num][i]);
  }
}

void setup() {
  for(int i=0; i<8; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }

  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);

  displayDigit(counter);
}

void loop() {
  bool currentUpState = digitalRead(buttonUp);
  bool currentDownState = digitalRead(buttonDown);


  if (lastUpState == HIGH && currentUpState == LOW) {
    counter++;
    if(counter > 15) counter = 0; 
    displayDigit(counter);
    delay(200); 
  }

  if (lastDownState == HIGH && currentDownState == LOW) {
    counter--;
    if(counter < 0) counter = 15; 
    displayDigit(counter);
    delay(200); 
  }

  lastUpState = currentUpState;
  lastDownState = currentDownState;
}
```

---

## Analisis & Prinsip Kerja

### 1. Resume Hasil Percobaan

* **Percobaan 2A:** Display berhasil melakukan *looping* otomatis memunculkan sekuens karakter heksadesimal (0–F) dengan jeda konstan 1 detik.
* **Percobaan 2B:** Kontrol beralih ke *user*. Sistem sukses melakukan *update* karakter secara presisi berkat implementasi *edge detection* yang mencegah pembacaan ganda akibat efek *bouncing* tombol.

### 2. Mekanisme Kerja Seven Segment

Display tersusun dari 7 LED batang (a-g) dan 1 LED titik (dp). Karakter terbentuk lewat kombinasi nyala LED spesifik yang polanya dikodekan dalam bentuk logika biner lewat array `digitPattern`. Mengingat jenis yang dipakai adalah *common cathode*, pin Arduino harus mengirimkan tegangan positif (HIGH) ke segmen terkait agar arus mengalir melewati LED menuju *ground* bersama dan menciptakan visual karakter.

### 3. Cara Kerja Sistem Counter

* **Sistem Otomatis (2A):** Mengandalkan struktur internal perulangan `for` bawaan kode yang berjalan sequensial dari batas bawah ke batas atas (atau sebaliknya) terikat interupsi waktu via `delay()`.
* **Sistem Manual (2B):** Logika bertumpu pada pendeteksian pulsa turun sinyal (*falling edge* dari HIGH ke LOW) saat tombol dipencet. Begitu pemicu terbaca, variabel dinamis `currentDigit` dimodifikasi (`++` atau `--`) dengan proteksi sirkular (*wrap-around* batas nilai 0 dan 15) sebelum dilempar kembali ke fungsi display.
