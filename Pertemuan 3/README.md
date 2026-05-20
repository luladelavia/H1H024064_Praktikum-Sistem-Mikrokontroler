# Laporan Praktikum - Jawaban Pertanyaan

* **Nama:** Lula Khaisha Delavia
* **NIM:** H1H024064
* **Shift Awal:** D
* **Shift Akhir:** D

---

# Laporan Praktikum: Komunikasi Serial (UART) & Inter-Integrated Circuit (I2C)

Dokumen ini berisi dokumentasi, analisis, dan modifikasi kode untuk Percobaan 3A (UART) dan Percobaan 3B (I2C) menggunakan Arduino.

---

## 3.5 Percobaan 3A: Komunikasi Serial (UART)

### 1. Alur Proses: Input Keyboard hingga Modifikasi Status LED
Proses transfer data dan eksekusi perintah berjalan melalui tahapan berikut:

[Keyboard] -> (Ketik & Enter di Serial Monitor)
│
▼
[Komputer] -> (Kirim data via Kabel USB)
│
▼
[Arduino: Serial Buffer] -> (Dideteksi oleh Serial.available() > 0)
│
▼
[Arduino: Serial.read()] -> (Karakter disimpan ke variabel 'data')
│
▼
[Logika Percabangan] -> (Evaluasi karakter '1' atau '0')
│
├─► Jika '1': digitalWrite(PIN_LED, HIGH) -> LED Menyala
└─► Jika '0': digitalWrite(PIN_LED, LOW)  -> LED Mati
│
▼
[Konfirmasi] -> (Arduino mengirim teks "LED ON"/"LED OFF" kembali ke Serial Monitor)

### 2. Fungsi dan Urgensi `Serial.available()`
* **Fungsi Utama:** Berperan sebagai validasi untuk memastikan bahwa antrean (*buffer*) penerimaan Serial telah terisi oleh data masuk sebelum proses pembacaan dilakukan.
* **Dampak Jika Dihilangkan:** Program akan terus memaksa melakukan pembacaan melalui `Serial.read()` secara berulang-ulang (*polling* tanpa henti). Karena *buffer* kosong, fungsi tersebut akan mengembalikan nilai `-1`. Hal ini memicu pemborosan siklus prosesor (tidak efisien) dan berpotensi menimbulkan *error* logika pada pembacaan karakter.

3. Modifikasi program agar LED berkedip (blink) ketika menerima input '2' dengan kondisi jika ‘2’ aktif maka LED akan terus berkedip sampai perintah selanjutnya diberikan!

```c++
// Modifikasi 3A: LED berkedip saat input '2', berhenti saat input '0'
#include <Arduino.h>

const int PIN_LED = 12;
bool blinkMode = false;   // status mode berkedip
unsigned long previousMillis = 0;
const long interval = 500; // jeda 500ms
bool ledState = false;

void setup() {
    Serial.begin(9600);
    Serial.println("Ketik '1' (ON), '0' (OFF), '2' (BLINK)");
    pinMode(PIN_LED, OUTPUT);
}

void loop() {
    // Baca perintah dari serial
    if (Serial.available() > 0) {
        char data = Serial.read();
        if (data == '1') {
            blinkMode = false;
            digitalWrite(PIN_LED, HIGH);
            Serial.println("LED ON");
        }
        else if (data == '0') {
            blinkMode = false;
            digitalWrite(PIN_LED, LOW);
            Serial.println("LED OFF");
        }
        else if (data == '2') {
            blinkMode = true;
            Serial.println("BLINK MODE ON");
        }
        else if (data != '\n' && data != '\r') {
            Serial.println("Perintah tidak dikenal");
        }
    }
    
    // Jika mode blink aktif, lakukan kedip tanpa delay() blocking
    if (blinkMode) {
        unsigned long currentMillis = millis();
        if (currentMillis - previousMillis >= interval) {
            previousMillis = currentMillis;
            ledState = !ledState;
            digitalWrite(PIN_LED, ledState);
        }
    }
}
```

4. Tentukan apakah menggunakan delay() atau milis()! Jelaskan pengaruhnya terhadap sistem!

> Pada modifikasi ini digunakan millis(), bukan delay(). Alasannya dikarenakan sistem tesrebut bersifat non-blocking. Program tetap bisa membaca input serial sambil menghitung waktu yang berlalu. Setiap loop, program mengecek apakah sudah waktunya mengganti status LED tanpa menghentikan eksekusi. Sehingga menjadi lebih responsif terhadap input dari pengguna. Cocok untuk aplikasi yang memerlukan pemrosesan paralel seperti membaca sensor, menerima perintah, dan menggerakkan aktuator secara bersamaan.

## 3.6 Percobaan 3B: Inter-Integrated Circuit (I2C)

### 1. Mekanisme Komunikasi Arduino ke LCD via Bus I2C
Komunikasi ini memanfaatkan arsitektur sinkronos berbasis Master-Slave dengan detail sebagai berikut:
* **Jalur Fisik:** Hanya menggunakan dua kabel utama, yaitu **SDA** (jalur data) dan **SCL** (jalur detak/clock sebagai penyinkron yang diatur oleh Arduino).
* **Prosedur Komunikasi:** 1. Arduino mengirimkan alamat spesifik milik LCD (umumnya `0x27`) untuk membuka komunikasi.
    2. LCD merespons dengan sinyal *Acknowledge* (ACK).
    3. Arduino mengirimkan data atau instruksi (seperti inisialisasi teks atau posisi kursor).
* **Abstraksi Library:** Detail bit-per-bit di atas ditangani secara otomatis oleh *library* `<LiquidCrystal_I2C.h>`, sehingga pengguna cukup memanggil fungsi tingkat tinggi seperti `lcd.print()`.
* **Kelebihan Utama:** Sangat hemat pin. Cukup dengan 2 pin (SDA & SCL), Arduino dapat mengontrol banyak perangkat *slave* sekaligus, selama tiap perangkat memiliki alamat unik yang berbeda.

### 2. Analisis Konfigurasi Pin Potensiometer (Kontras LCD)
* **Efek Pin Kiri dan Kanan Tertukar:** Potensiometer tetap bekerja secara aman tanpa risiko kerusakan komponen. Namun, terjadi **inversi logika arah putaran**.
* **Detail Teknis:** Jika kondisi normal searah jarum jam meningkatkan tegangan (nilai ADC `0 → 1023`), maka saat pin tertukar, putaran searah jarum jam justru akan menurunkan tegangan (nilai ADC `1023 → 0`). Rentang nilai minimum dan maksimum tetap tidak berubah.

- Data yang ditampilkan pada Serial Monitor sesuai dengan table berikut:

   | ADC: 0 | Volt: 0.00V | Persen: 0% |
   | ------ | ----------- | ---------- |
  

## 3.7 Modifikasi Program: Integrasi UART & I2C (Dual Output)

Berikut adalah modifikasi kode program untuk menggabungkan komunikasi UART dan I2C. Data yang diterima dari Serial Monitor akan mengontrol LED, sekaligus menampilkan statusnya secara sinkron di **Serial Monitor** dan **Layar LCD**.
```c++

// Modifikasi 3B: Tampilan ke Serial Monitor dan LCD dengan format rapi

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
const int pinPot = A0;

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
}

void loop() {
    int nilaiADC = analogRead(pinPot);
    if (nilaiADC < 1) nilaiADC = 1;   // hindari pembagian nol (opsional)
    
    float tegangan = (nilaiADC / 1023.0) * 5.0;
    int persen = map(nilaiADC, 0, 1023, 0, 100);
    
    // --- Serial Monitor format sesuai tabel (UART) ---
    Serial.print("ADC: ");
    Serial.print(nilaiADC);
    Serial.print(" Volt: ");
    Serial.print(tegangan, 2);
    Serial.print(" V Persen: ");
    Serial.print(persen);
    Serial.println("%");
    
    // --- LCD baris 1: "ADC: nilai persen%" (I2C) ---
    lcd.setCursor(0, 0);
    lcd.print("ADC: ");
    lcd.print(nilaiADC);
    lcd.print(" ");
    lcd.print(persen);
    lcd.print("%   ");   // hapus sisa karakter
    
    // --- LCD baris 2: bar grafis (I2C) ---
    int panjangBar = map(nilaiADC, 0, 1023, 0, 16);
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++) {
        if (i < panjangBar) {
            lcd.print((char)255);   // blok
        } else {
            lcd.print(" ");
        }
    }
    
    delay(200);
}
```

4. Lengkapi tabel berikut berdasarkan pengamatan pada Serial Monitor


| ADC | Volt (V) | Persen (%) |
| --- | -------- | ---------- |
| 1   | 0.00     | 0%         |
| 21  | 0.10     | 2%         |
| 49  | 0.24     | 4%         |
| 74  | 0.36     | 7%         |
| 96  | 0.47     | 9%         |
