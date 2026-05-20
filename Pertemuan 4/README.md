# Laporan Praktikum - Jawaban Pertanyaan

* **Nama:** Lula Khaisha Delavia
* **NIM:** H1H024064
* **Shift Awal:** D
* **Shift Akhir:** D

---

# Laporan Praktikum: Kontrol Servo dan LED Menggunakan ADC & PWM Arduino

Dokumen ini berisi dokumentasi dan jawaban analisis mengenai implementasi *Analog-to-Digital Converter* (ADC) dan *Pulse Width Modulation* (PWM) untuk mengendalikan motor servo dan intensitas cahaya LED.

---

## 📋 Bagian 1: Praktikum Kendali Motor Servo via Potensiometer

### 1. Fungsi Perintah `analogRead()`

Fungsi `analogRead(pin)` berperan sebagai jembatan untuk menangkap level tegangan analog yang dikirimkan oleh komponen input (seperti potensiometer) melalui pin analog Arduino.

* **Mekanisme Kerja:** Chip mikrokontroler pada Arduino Uno dilengkapi dengan fitur ADC beresolusi 10-bit. Fitur ini mengubah tegangan analog luar (skala 0V hingga 5V) menjadi representasi angka digital berbasis data integer dari **0 sampai 1023**.
* **Peran di Rangkaian:** Perintah ini mendeteksi seberapa jauh poros potensiometer diputar melalui fluktuasi tegangan yang masuk. Angka digital hasil konversi inilah yang nantinya diolah sistem untuk mengatur posisi lengan servo.

```cpp
val = analogRead(potensioPin); // Mengambil data digital (0-1023) berdasarkan tegangan input

```

### 2. Urgensi Penggunaan Fungsi `map()`

Fungsi `map()` mutlak diperlukan karena adanya **perbedaan skala metrik** antara subsistem input dan subsistem output.

* Data masuk dari `analogRead()` memiliki jangkauan **0 hingga 1023**.
* Data keluar untuk menggerakkan motor servo standar hanya menerima input sudut fisik antara **0° hingga 180°**.

Tanpa penyelarasan linear menggunakan fungsi `map()`, nilai ADC yang terlalu besar akan langsung dikirim ke motor servo. Hal ini dapat memaksa servo berputar melampaui batas mekanis aslinya dan berisiko merusak struktur roda gigi internal (*gearbox*).

```cpp
pos = map(val, 0, 1023, 0, 180);
// Mengonversi secara proporsional: 0->0°, 511->90°, 1023->180°

```

### 3. Modifikasi Kode Program: Pembatasan Sudut Servo (30°–150°)

Kode di bawah ini telah dimodifikasi agar gerakan servo menjadi terbatas pada rentang **30° hingga 150°**, meskipun tuas potensiometer tetap diputar penuh dari ujung ke ujung (ADC 0–1023).

```cpp
#include <Servo.h> // library untuk servo motor

Servo myservo; // membuat objek servo

// ===================== PIN SETUP =====================
// Tentukan pin yang digunakan untuk potensiometer dan servo
const int potensioPin = A0;  // isi pin analog input (contoh A0)
const int servoPin = 9;      // isi pin digital untuk servo (PWM)

// ===================== VARIABEL =====================
// Variabel untuk menyimpan data ADC dan sudut servo
int pos = 0; // isi dengan tipe data dan inisialisasi awal
int val = 0; // isi dengan tipe data dan inisialisasi awal

void setup() {

  // Hubungkan servo ke pin yang sudah ditentukan
  myservo.attach(servoPin); // isi dengan servoPin

  // Aktifkan komunikasi serial untuk monitoring
  Serial.begin(9600); // isi baud rate (contoh 9600)

}

void loop() {

  // ===================== PEMBACAAN ADC =====================
  // Baca nilai dari potensiometer (rentang 0–1023)
  val = analogRead(); // isi dengan potensioPin

  // ===================== KONVERSI DATA =====================
  // Ubah nilai ADC menjadi sudut servo (30–150 derajat)
  pos = map(val,
             0,   	// isi nilai minimum ADC
             1023,  // isi nilai maksimum ADC
             30,   	// isi sudut minimum servo
             150);  // isi sudut maksimum servo

  // ===================== OUTPUT SERVO =====================
  // Gerakkan servo sesuai hasil mapping
  myservo.write(); // isi dengan variabel sudut

  // ===================== MONITORING DATA =====================
  // Tampilkan data ADC dan sudut servo ke Serial Monitor
  Serial.print("ADC Potensio: ");
  Serial.print(); // isi variabel ADC

  Serial.print(" | Sudut Servo: ");
  Serial.println(); // isi variabel sudut

  // ===================== STABILISASI =====================
  // Delay untuk memberi waktu servo bergerak stabil
  delay(100); // isi dalam milidetik
}

```

---

## 💡 Bagian 2: Praktikum Modulasi Kecerahan LED (PWM)

### 1. Prinsip Kerja `analogWrite()` dalam Mengatur Kecerahan LED

Secara fisik, lampu LED adalah komponen digital yang hanya mengenal status aktif (ON) dan nonaktif (OFF). Namun, dengan teknik **PWM (Pulse Width Modulation)**, kita bisa memanipulasi persepsi intensitas cahayanya.

* **Cara Kerja:** Fungsi `analogWrite()` memicu pin digital untuk melakukan sakelar (saklar ON/OFF) secara berulang-ulang dengan frekuensi yang sangat tinggi.
* **Duty Cycle:** Rasio durasi kondisi HIGH terhadap satu siklus penuh dinamakan *duty cycle*. Karena proses *switching* ini berjalan sangat cepat melampaui batas tangkapan mata manusia (*persistence of vision*), efek yang terlihat adalah redup atau terangnya cahaya LED secara halus.

```cpp
analogWrite(LED_PIN, 127); // Duty cycle ~50%: LED menyala dengan kecerahan sedang

```

### 2. Korelasi Nilai ADC (0–1023) dengan Nilai PWM (0–255)

Hubungan keduanya adalah hubungan **linear proporsional dengan rasio perbandingan 4:1**. Hal ini dipicu oleh perbedaan spesifikasi arsitektur mikrokontroler:

* Resolusi membaca input (ADC) sebesar **10-bit** ($2^{10} = 1024$ kombinasi nilai).
* Resolusi menghasilkan output (PWM) sebesar **8-bit** ($2^8 = 256$ kombinasi nilai).

Melalui fungsi pemetaan `map()`, setiap lonjakan sebesar ~4 angka pada nilai ADC akan dikonversi menjadi kenaikan 1 angka pada nilai PWM. Penyelarasan ini menjamin pergeseran putaran potensiometer berbanding lurus secara mulus dengan tingkat kecerahan LED.

### 3. Modifikasi Kode Program: Pembatasan Kecerahan LED (PWM 50–200)

Kode berikut dimodifikasi menggunakan logika kondisional (`if-else`) agar LED hanya menyala pada rentang intensitas menengah (**PWM 50 hingga 200**). Jika di luar rentang tersebut, LED dipaksa mati total (OFF).

```cpp
#include <Arduino.h> // library dasar Arduino (tidak wajib diubah)

// ===================== PIN SETUP =====================
// Tentukan pin yang digunakan untuk potensiometer dan LED PWM
const int potPin = A0;   // isi dengan pin analog (contoh A0)
const int ledPin = 9;   // isi dengan pin digital PWM (contoh 9)

// ===================== VARIABEL =====================
// Variabel untuk menyimpan hasil pembacaan dan konversi PWM
int nilaiADC = 0;  // isi dengan nilai awal (default 0)
int pwm = 0;       // isi dengan nilai awal (default 0)

void setup() {

  // ===================== OUTPUT SETUP =====================
  // Atur pin LED sebagai output
  pinMode(ledPin, );

  // ===================== SERIAL MONITOR =====================
  // Aktifkan komunikasi serial untuk melihat data pembacaan
  Serial.begin(9600); // isi baud rate (contoh 9600)
}

void loop() {

  // ===================== PEMBACAAN SENSOR =====================
  // Baca nilai analog dari potensiometer (rentang 0–1023)
  nilaiADC = analogRead(potPin); // isi dengan potPin

  // ===================== PEMROSESAN DATA (SCALING) =====================
  // Ubah nilai ADC (0–1023) menjadi nilai PWM (0–255)
  pwm = map(nilaiADC,
            0,   // isi nilai minimum ADC
            1023,   // isi nilai maksimum ADC
            0,   // isi PWM minimum
            255);  // isi PWM maksimum

  // ===================== OUTPUT PWM =====================
  if (pwm >= 50 && pwm <= 200){ //ketika nilai pwm 50-200
  	analogWrite(ledPin, pwm);   // led menyala sesuai pwm
  } else {
	analogWrite(ledPin, 0);       // led mati 
  }

  // ===================== MONITORING DATA =====================
  // Tampilkan data ADC dan PWM ke Serial Monitor
  Serial.print("ADC: ");
  Serial.print(nilaiADC); // isi variabel ADC

  Serial.print(" | PWM: ");
  Serial.println(pwm); // isi variabel PWM

  // ===================== STABILISASI SISTEM =====================
  // Delay untuk menstabilkan pembacaan dan tampilan data
  delay(50); // isi dalam milidetik (contoh 50)
}
```

---

## 🔬 Bagian 3: Analisis Mendalam & Teoretis

### 1. Tahapan Mekanisme Kerja ADC pada Arduino

Proses transformasi dari tegangan fisik nyata (sinyal analog kontinu) menjadi data angka digital (sinyal diskrit) melibatkan empat fase utama di dalam chip mikrokontroler:

1. **Sampling (Pencuplikan):** Arduino mengisolasi dan mengambil sampel sinyal tegangan dari pin analog pada rentang waktu berkala yang sangat cepat.
2. **Holding (Penahanan):** Sinyal cuplikan tersebut disimpan sementara pada komponen kapasitor internal agar level tegangannya konstan dan tidak berubah selama proses pembacaan.
3. **Quantization (Kuantisasi):** Tegangan yang ditahan tersebut dikomparasikan dengan tegangan acuan standar (VCC 5V). Karena resolusinya 10-bit, rentang 0V–5V dipisahkan menjadi 1024 tingkatan.
> **Formulasi Matematis:** > 
> $$\text{Nilai ADC} = \frac{V_{\text{in}}}{V_{\text{ref}}} \times 1023$$
> 
> 
> 
> Jika tegangan masuk sebesar 2.5V, maka perhitungannya adalah $(2.5 / 5.0) \times 1023 = 511$.


4. **Coding (Penyandian):** Hasil pembulatan dari tahap kuantisasi diterjemahkan menjadi deretan bilangan biner yang dapat dibaca sebagai tipe data *integer* oleh baris program.

### 2. Faktor-Faktor Penentu Akurasi Pembacaan ADC

Akurasi data digital yang dihasilkan oleh fitur ADC sangat dipengaruhi oleh variabel internal maupun eksternal berikut:

* **Stabilitas Tegangan Referensi ($V_{\text{ref}}$):** Jika pasokan daya utama Arduino naik-turun, titik acuan komparasi ADC ikut bergeser, memicu deviasi data output.
* **Derau Elektromagnetik (*Noise*):** Sinyal frekuensi tinggi atau riak daya (*ripple*) dari komponen aktif di sekitar papan sirkuit dapat menginterfere jalur analog.
* **Keterbatasan Resolusi Perangkat:** Akibat batas resolusi 10-bit, perubahan tegangan terkecil yang mampu dirasakan oleh sistem terbatas pada nilai $\pm 4.9 \text{ mV}$ per tingkatan (*step*).
* **Impedansi Internal Sumber Sinyal:** Jika hambatan internal komponen input terlalu besar, pengisian daya pada kapasitor *sample-and-hold* internal akan lambat dan menghasilkan data bias.
* **Kondisi Fisik Komponen:** Komponen mekanis seperti potensiometer yang aus atau kotor memicu lonjakan hambatan sesaat yang membuat data ADC fluktuatif.
* **Termal (Suhu):** Perubahan suhu ekstrem berpotensi mengubah karakteristik resistansi internal sirkuit analog mikro.

### 3. Kendala Integrasi ADC dan PWM dalam Satu Sistem Berkelanjutan

Menggabungkan sistem pembacaan analog (ADC) dan sistem modulasi daya (PWM) secara simultan berpotensi memunculkan beberapa konflik teknis:

* **Interferensi Arus Balik (*Cross-talk*):** Sinyal PWM bekerja dengan teknik pemutusan arus secara cepat (*high-speed switching*). Aktivitas ini memicu radiasi elektromagnetik dan fluktuasi arus (*noise*) pada jalur *ground* yang dapat mengganggu keaslian tegangan analog pada pin ADC di dekatnya.
* **Dilema Pengaturan Waktu (*Timing Dilemma*):** Penggunaan fungsi `delay()` yang besar membuat respons sistem terhadap perubahan potensiometer menjadi lambat. Sebaliknya, jika durasi *delay* terlalu singkat, pembacaan ADC menjadi tidak stabil karena tidak ada waktu yang cukup untuk proses pengosongan muatan kapasitor internal.
* **Ketidaksinambungan Resolusi:** Ketidaksamaan antara ruang data ADC (10-bit) dan ruang perintah PWM (8-bit) menuntut adanya kalkulasi pembagian. Kesalahan pembulatan dalam proses matematika ini dapat menyebabkan perubahan output tidak berjalan linear.
* **Gejala Getaran Mekanis (*Jitter*):** Fluktuasi minor pada hasil ADC akibat *noise* sirkuit akan langsung diteruskan ke output PWM. Hal ini berdampak buruk pada komponen output, seperti lampu LED yang terlihat berkedip tipis (*flicker*) atau motor servo yang bergetar kecil (*jitter*) saat posisi potensio diam.
