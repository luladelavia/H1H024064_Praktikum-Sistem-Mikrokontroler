# Laporan Praktikum - Jawaban Pertanyaan

* **Nama:** Lula Khaisha Delavia
* **NIM:** H1H024064
* **Shift Awal:** D
* **Shift Akhir:** D

---

### 📌 Pertanyaan & Jawaban Praktikum 6A

#### **Soal 1**

**Bagaimana mekanisme sebuah tombol/sakelar dapat mengubah status menyala/mati LED melalui jalur *interrupt*?**

**Jawaban:**

1. **Perubahan Sinyal Input:** Ketika tombol ditekan, pin digital 2 yang dikonfigurasi sebagai `INPUT_PULLUP` akan mendeteksi perubahan logika dari **HIGH ke LOW**.
2. **Pemicuan Interrupt:** Perubahan level tegangan yang drastis ini langsung memicu interupsi eksternal (`INT0`) pada mikrokontroler ATmega328P (Arduino Uno).
3. **Pemberhentian Sementara:** CPU Arduino seketika menghentikan eksekusi baris kode pada program utama (`void loop()`) dan menyimpan posisi terakhirnya di memori.
4. **Eksekusi ISR:** Kendali program dialihkan untuk mengeksekusi Fungsi Layanan Interupsi atau *Interrupt Service Routine* (ISR) bernama `tombolInterrupt()`. Di dalam fungsi ini, nilai logika variabel status LED (`ledState`) dibalik (*di-toggle*).
5. **Kembali ke Program Utama:** Setelah ISR selesai, CPU melanjutkan program utama dan memperbarui kondisi fisik LED via perintah `digitalWrite()`.

Melalui metode ini, sistem tidak perlu membuang daya komputasi untuk memeriksa kondisi tombol berulang kali (*polling*).

#### **Soal 2**

**Uraikan fungsi dan kegunaan dari perintah `attachInterrupt()` di dalam pemrograman Arduino!**

**Jawaban:**
Fungsi `attachInterrupt()` bertugas untuk mendaftarkan dan mengaitkan pin fisik *interrupt* tertentu dengan fungsi penangan (ISR) yang kita buat, serta menentukan jenis translasi sinyal yang memicunya.

**Struktur Kode:**

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, FALLING);

```

**Penjelasan Parameter:**

* `digitalPinToInterrupt(2)`: Memetakan Pin Digital 2 secara spesifik ke jalur *Hardware Interrupt* 0 (`INT0`).
* `tombolInterrupt`: Menunjuk nama fungsi kustom (ISR) yang wajib dipanggil saat interupsi terjadi.
* `FALLING`: Menetapkan bahwa interupsi hanya akan aktif saat terjadi transisi sinyal dari logika **HIGH menuju LOW** (tekanan tombol).

*Tanpa inisialisasi ini, mikrokontroler tidak akan pernah merespons perubahan sinyal eksternal pada pin tersebut.*

#### **Soal 3**

**Mengapa penggunaan fungsi `delay()` dan `Serial.print()` sangat dilarang di dalam blok fungsi ISR?**

**Jawaban:**
Fungsi ISR dituntut untuk bekerja sekejap mungkin demi menjaga stabilitas sistem.

* **Masalah `delay()`:** Fungsi penunda waktu ini bergantung pada interupsi internal dari *Timer0*. Karena seluruh interupsi global otomatis dinonaktifkan saat CPU berada di dalam ISR, nilai timer tidak akan bertambah. Akibatnya, `delay()` akan terjebak selamanya dan membuat mikrokontroler mengalami *freeze* (hang).
* **Masalah `Serial.print()`:** Komunikasi serial membutuhkan interupsi *hardware* UART dan proses *buffering* data. Memanggil fungsi ini di dalam ISR yang sedang mengunci interupsi lain akan memicu kondisi saling tunggu (*deadlock*) atau menyebabkan data yang dikirim menjadi rusak (*corrupt*).

**Solusi Terbaik:** Di dalam ISR, cukup lakukan operasi berbasis data minimal seperti mengubah nilai variabel bendera (*flag*), membalikkan status boolean, atau memperbarui variabel pencatat.

#### **Soal 4**

**Jelaskan urgensi dan fungsi dari penyematan kata kunci `volatile` pada deklarasi variabel seperti `ledState`!**

**Jawaban:**
Keyword `volatile` bertindak sebagai instruksi khusus kepada *compiler* agar tidak melakukan optimasi penempatan memori pada variabel tersebut.

**Contoh Deklarasi:**

```cpp
volatile bool ledState = false;

```

**Analisis Kerja:**

* **Tanpa `volatile`:** *Compiler* mungkin akan menyimpan variabel di register CPU internal demi mempercepat akses program utama. Imbasnya, ketika fungsi ISR mengubah nilai variabel tersebut di RAM, program utama tidak menyadarinya karena masih membaca nilai lama dari register.
* **Dengan `volatile`:** *Compiler* dipaksa untuk selalu membaca dan menulis nilai variabel langsung ke alamat memori RAM fisik setiap kali dibutuhkan. Hal ini menjamin program utama selalu mendapatkan data paling mutakhir yang telah dimodifikasi oleh ISR.

#### **Soal 5**

**Lakukan modifikasi kode mengggunakan variasi mode interupsi selain `FALLING` (`RISING`, `CHANGE`, `LOW`) dan paparkan karakteristik masing-masing!**

**Jawaban:**

##### **1. Mode RISING (Transisi Naik)**

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, RISING);

```

* **Prinsip Kerja:** Interupsi terpicu tepat saat sinyal berubah dari **LOW ke HIGH**.
* **Respon LED:** Status LED baru akan berubah sesaat setelah tombol **dilepas** (bukan saat ditekan).
* **Aplikasi:** Kurang responsif untuk tombol mekanis, namun sangat baik untuk mendeteksi tepi naik pulsa sensor.

##### **2. Mode CHANGE (Perubahan Logika)**

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, CHANGE);

```

* **Prinsip Kerja:** Interupsi aktif pada setiap ada perubahan logika (baik dari **LOW ke HIGH** maupun **HIGH ke LOW**).
* **Respon LED:** LED mengalami *toggle* sebanyak dua kali: pertama saat tombol ditekan, dan kedua saat tombol dilepas.
* **Aplikasi:** Berguna untuk mendeteksi transisi penuh, namun jika digunakan pada tombol tanpa rangkaian *debouncing*, perubahan akan terjadi terlalu cepat sehingga kasat mata.

##### **3. Mode LOW (Level Rendah)**

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, LOW);

```

* **Prinsip Kerja:** Interupsi akan terus-menerus terpicu dan mengeksekusi ISR selama pin berada dalam kondisi tegangan rendah (**LOW**).
* **Respon LED:** Selama tombol ditahan dalam posisi ditekan, LED akan berkedip/berkelip tak beraturan dengan frekuensi yang sangat tinggi mengikuti siklus instruksi ISR.
* **Aplikasi:** Tidak ideal untuk sakelar *toggle* sederhana; biasanya dipakai untuk memicu mode hemat daya (*wake-on-low*).

---

### 📌 Pertanyaan & Jawaban Praktikum 6B

#### **Soal 1**

**Bagaimana prinsip kerja pencatatan waktu menggunakan fungsi `millis()` di dalam struktur program?**

**Jawaban:**
Fungsi `millis()` memanfaatkan *Hardware Timer* internal Arduino untuk menghitung durasi waktu (dalam mili-detik) yang telah terlampaui sejak papan mikrokontroler mulai mendapatkan daya (*booting*).

Mekanisme pengendalian waktu dilakukan dengan teknik selisih:

1. **Membaca Waktu Aktual:** Mengambil nilai milidetik terbaru melalui `millis()` dan menyimpannya ke variabel temporer (`currentMillis`).
2. **Kalkulasi Selisih:** Mengurangi `currentMillis` dengan variabel pencatat waktu sebelumnya (`previousMillis`).
3. **Validasi Ambang Batas:** Jika hasil pengurangan bernilai sama dengan atau lebih besar dari durasi jeda target (`interval`), maka kondisi terpenuhi. Program kemudian mengeksekusi perintah manipulasi LED dan memperbarui nilai `previousMillis` dengan nilai `currentMillis` agar siklus waktu berikutnya dapat dihitung kembali.

#### **Soal 2**

**Komparasikan perbedaan fundamental antara fungsi penunda `delay()` dengan fungsi berbasis pewaktu `millis()`!**

**Jawaban:**

| Parameter Komparasi | Fungsi `delay()` | Fungsi `millis()` |
| --- | --- | --- |
| **Sifat Eksekusi** | *Blocking* (Memblokir baris kode) | *Non-blocking* (Tanpa memblokir) |
| **Kondisi CPU** | Pasif / Berhenti total menunggu waktu habis | Aktif / Tetap memproses instruksi lain |
| **Respon Input** | Mengabaikan interaksi luar (Apatis) | Sangat responsif terhadap input sensor/tombol |
| **Kemampuan** | Hanya dapat menangani tugas tunggal (*Single Task*) | Fondasi utama pengerjaan banyak tugas (*Multitasking*) |

**Ilustrasi Analogi:** `delay(1000)` memaksa CPU menutup mata dan tidur selama 1 detik penuh. Sebaliknya, pendekatan `millis()` membuat CPU terus bekerja sembari sesekali melirik jam dinding untuk memastikan apakah waktu target sudah tiba.
#### **Soal 3**

**Mengapa manajemen waktu menggunakan fungsi `millis()` dikategorikan sebagai metode *non-blocking*?**

**Jawaban:**
Dikatakan *non-blocking* karena fungsi `millis()` hanya bertindak sebagai pembaca register pencatat waktu tanpa menghentikan alur kerja program (*program counter*). Ketika instruksi pengecekan waktu belum mencapai interval yang diinginkan, CPU tidak dipaksa menunggu pasif, melainkan langsung melewati blok kondisi tersebut dan mengeksekusi baris perintah selanjutnya di dalam loop utama secara terus-menerus.

**Dampak Positif:**

* Sistem embedded dapat membaca data dari modul sensor secara *real-time*.
* Tombol kontrol tetap sensitif dan dapat mendeteksi input pengguna kapan saja.
* Dapat mengolah proses pengiriman data serial secara simultan tanpa interupsi jeda kosong.

#### **Soal 4**

**Rancanglah sebuah modifikasi program Arduino untuk mengedipkan dua buah LED secara independen (LED 1 berkedip tiap 1 detik, LED 2 berkedip tiap 500 ms) secara simultan tanpa fungsi `delay()`!**

**Jawaban:**
Berikut adalah implementasi struktur kode pemrograman berbasis *asynchronous timer*:

```cpp
#include <Arduino.h>

unsigned long previousMillis = 0; // waktu terakhir LED berubah

const long interval = 1000; // interval kedip: 1000 ms

bool ledState = false; // status LED saat ini

void setup() {
    pinMode(13, OUTPUT); // Pin 13 sebagai output
}

void loop() {
    // Ambil waktu saat ini
    unsigned long currentMillis = millis();

    // Cek apakah sudah melewati interval
    if(currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis; // simpan waktu terakhir
        ledState = !ledState; // toggle status LED
        digitalWrite(13, ledState);// tulis ke pin LED
    }
}

```
---

## 🧩 Pertanyaan Praktikum Umum

#### **Soal 1**

**Identifikasi dan jabarkan keunggulan mutlak pemanfaatan mekanisme *Interrupt* dibandingkan metode *Polling* konvensional!**

**Jawaban:**

1. **Efisiensi Daya Komputasi Tinggi:** CPU tidak perlu menghabiskan siklus clock hanya untuk mengecek status pin input secara berulang-ulang di dalam loop.
2. **Kecepatan Respon Instan (Low Latency):** Begitu sinyal interupsi masuk, sistem langsung merespon dalam hitungan nanodetik, meminimalkan jeda waktu respons.
3. **Optimalisasi Konsumsi Energi:** Memungkinkan mikrokontroler masuk ke mode tidur (*Sleep Mode*) untuk menghemat baterai, dan hanya terjaga (*wake up*) ketika ada interupsi dari luar.
4. **Keandalan Sistem Real-Time:** Menjamin sinyal atau peristiwa kritis yang durasinya sangat pendek (seperti pulsa sensor kecepatan) tidak terlewatkan oleh program.


#### **Soal 2**

**Mengapa keberadaan komponen *Timer* sangat vital pada arsitektur sistem tertanam (*embedded system*) dan aplikasi *real-time*?**

**Jawaban:**
*Timer* internal bertindak sebagai "jantung" sinkronisasi data yang berfungsi untuk:

* Membangkitkan sinyal PWM (*Pulse Width Modulation*) untuk kontrol motor dan peredupan lampu.
* Mengatur periodisitas eksekusi algoritma kontrol (seperti kalkulasi PID).
* Menjalankan penjadwalan tugas (*task scheduler*) dalam sistem operasi waktu nyata (RTOS).
* Melakukan kalkulasi presisi tinggi terkait pembacaan durasi pulsa frekuensi ataupun sensor ultrasonik.


#### **Soal 3**

**Gambarkan bagaimana alur integrasi kerja sistem saat fitur *Interrupt* dan *Timer* diaktifkan bersamaan dalam satu aplikasi!**

**Jawaban:**

1. Di bawah kondisi normal, CPU secara kontinu memproses logika berkala (seperti kalkulasi berbasis `millis()`) di dalam fungsi `loop()`.
2. Secara bersamaan, *Hardware Timer* di latar belakang terus menghitung detak waktu secara independen.
3. Ketika sebuah peristiwa luar terjadi (misal, tombol ditekan), jalur *Interrupt* eksternal mengirim sinyal interupsi ke CPU.
4. CPU menghentikan jalannya pengolahan *Timer* dan fungsi `loop()`, beralih mengeksekusi fungsi ISR untuk mengatasi kejadian darurat tersebut.
5. Setelah penanganan ISR selesai, konteks register dipulihkan, dan CPU kembali melanjutkan perhitungan waktu *Timer* serta kelanjutan baris kode utama tepat dari titik terakhir ia ditinggalkan.


#### **Soal 4**

**Analisis dampak buruk yang akan terjadi apabila blok fungsi ISR dirancang terlalu panjang, kompleks, atau sarat komputasi!**

**Jawaban:**
Penulisan ISR yang buruk akan memicu masalah sistemik serius, antara lain:

* **Jeda Siklus Utama:** Program utama dalam `loop()` akan tertahan lama, menyebabkan sistem tampak tidak responsif (*laggy*).
* **Kehilangan Data Interupsi (*Missed Interrupt*):** Interupsi eksternal lain yang terjadi saat ISR sedang berjalan berisiko diabaikan atau terlambat diproses.
* **Kekacauan Waktu Sistem:** Fungsi berbasis waktu seperti `millis()` dan `micros()` kehilangan akurasinya (waktu berjalan melambat).
* **Stack Overflow:** Jika ISR memicu fungsi lain secara berantai, memori RAM (bagian *stack*) dapat penuh dan mengakibatkan mikrokontroler melakukan *crash* atau *reboot* mendadak.

---

## 🎯 Ringkasan Konseptual Modul

Melalui restrukturisasi ini, dipahami bahwa **Interrupt Eksternal** digunakan sebagai mekanisme penanganan peristiwa asinkron/darurat yang membutuhkan tindakan instan (seperti tombol kontrol atau sensor keamanan). Di sisi lain, **Timer (`millis()`)** diandalkan untuk mengelola tugas-tugas periodik terencana tanpa mengorbankan fungsionalitas sistem secara keseluruhan (*non-blocking multitasking*). Kombinasi sinergis keduanya merupakan pondasi wajib dalam merancang arsitektur perangkat keras tertanam yang responsif, efisien, dan andal.
