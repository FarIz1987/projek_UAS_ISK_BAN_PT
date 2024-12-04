#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <limits>

// Fungsi validasi umum
template <typename T>
T inputValidasi(const std::string& prompt, T minValue, T maxValue) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;

        // Periksa apakah input valid
        if (std::cin.fail() || value < minValue || value > maxValue) {
            std::cin.clear(); // Hapus status error
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Abaikan sisa input
            std::cout << "Input tidak valid. Harap masukkan nilai antara " 
                      << minValue << " dan " << maxValue << ".\n";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Abaikan sisa input
            return value; // Input valid
        }
    }
}

class ISKBanPTCalculator {
private:
    //Variabel data dosen
    double ndtps;  // Jumlah dosen tetap pengampu mata kuliah
    double ndgb;   // Jumlah dosen guru besar
    double ndlk;   // Jumlah dosen lektor kepala

    //variabel data kurikulum
    int kurikulumEvaluasiSkor;
    int kurikulumCapaianPembelajaranSkor;
    int kurikulumStrukturSkor;

    //Variabel Penjaminan Mutu
    int spmiAspekTerpenuhi;
    int indikator_terpenuhi; // Jumlah indikator kinerja tambahan yang terpenuhi
    bool melampaui_sn_dikti;

    //Variabel Tracer Study
    int tracerStudyAspekTerpenuhi;
    double persentaseBidangKerja;
    double jumlahLulusan;
    double lulusanTerlacak;

    //Variabel Publikasi Mahasiswa
    double publikasiNasionalTidakTerakreditasi;
    double publikasiNasionalTerakreditasi;
    double publikasiInternasional;
    double publikasiInternasionalReputasi;
    double seminarWilayah;
    double seminarNasional;
    double seminarInternasional;
    double mediaMassaWilayah;
    double mediaMassaNasional;
    double mediaMassaInternasional;
    double jumlahMahasiswa;

    // --- Atribut untuk Kepuasan Pengguna ---
    struct AspekKepuasan {
        std::string nama;
        double sangat_baik;
        double baik;
        double cukup;
        double kurang;
    };

    std::vector<AspekKepuasan> aspek_kepuasan;
    double jumlah_lulusan;
    double jumlah_pengguna_responden;
    // Fungsi pendukung untuk menghitung TKi per aspek kepuasan
    double hitungTKi(const AspekKepuasan& aspek) {
        return (4 * aspek.sangat_baik) + 
               (3 * aspek.baik) + 
               (2 * aspek.cukup) + 
               (1 * aspek.kurang);
    }

    // Fungsi validasi input persentase
    bool validasiPersentase(double sangat_baik, double baik, double cukup, double kurang) {
        double total = sangat_baik + baik + cukup + kurang;
        if (total > 100.0) {
            std::cout << "Error: Total persentase tidak boleh lebih dari 100% (Saat ini: " 
                      << total << "%).\n";
            return false;
        }
        return true;
    }

    // Metode perhitungan skor (sama seperti sebelumnya)
    double hitungSkorDosenTetap() {
        if (ndtps >= 6) return 4.0;
        if (ndtps >= 3) return (2.0 * ndtps) / 3.0;
        return 0.0;
    }

    double hitungSkorJabatanAkademik() {
        double persentaseGuruBesarLK = ((ndgb + ndlk) / ndtps) * 100.0;
        
        if (persentaseGuruBesarLK >= 70.0) return 4.0;
        if (persentaseGuruBesarLK > 0) {
            return 2.0 + ((20.0 * persentaseGuruBesarLK) / 7.0);
        }
        return 0.0;
    }

    double hitungSkorKurikulum() {
        return (kurikulumEvaluasiSkor + 
                (2 * kurikulumCapaianPembelajaranSkor) + 
                (2 * kurikulumStrukturSkor)) / 5.0;
    }

    double hitungSkorPenjaminanMutu() {
        // Sesuai dengan kriteria pada dokumen
        if (spmiAspekTerpenuhi == 5) return 4.0;
        if (spmiAspekTerpenuhi == 4) return 3.0;
        if (spmiAspekTerpenuhi == 3) return 2.0;
        if (spmiAspekTerpenuhi == 2) return 1.0;
        return 0.0;
    }

    double hitungSkorPelampauan_SN_DIKTI() {
        switch (indikator_terpenuhi) {
            case 5: return 4.0; // Semua indikator terpenuhi, dengan daya saing internasional
            case 4: return 3.0; // Sebagian besar indikator terpenuhi, dengan daya saing nasional
            case 3: return 2.5; // Indikator cukup terpenuhi
            case 2: return 2.0; // Indikator minimum terpenuhi
            default: return 2.0; // Tidak ada indikator tambahan yang terpenuhi
        }
    }

    double hitungSkorTracerStudy() {
        // Sesuai kriteria aspek terpenuhi
        if (tracerStudyAspekTerpenuhi == 5) return 4.0;
        if (tracerStudyAspekTerpenuhi == 4) return 3.0;
        if (tracerStudyAspekTerpenuhi == 3) return 2.0;
        if (tracerStudyAspekTerpenuhi == 2) return 1.0;
        return 0.0;
    }

    double hitungSkorKesesuaianBidangKerja() {
        if (jumlahLulusan == 0) {
            std::cout << "Error: Jumlah lulusan tidak valid.\n";
            return 0.0;
        }

        double prmin = (jumlahLulusan >= 300) ? 30.0 : (50.0 - ((jumlahLulusan / 300.0) * 20.0));
        double persentaseRespon = (lulusanTerlacak / jumlahLulusan) * 100.0;

        if (persentaseBidangKerja >= 60.0) {
            return 4.0; // Maksimum skor jika PBS >= 60%
        }

        // Hitung skor awal dan sesuaikan dengan persentase responden
        double skorAwal = (20.0 * persentaseBidangKerja) / 3.0;
        double skorAkhir = (persentaseRespon / prmin) * skorAwal;

        return std::min(skorAkhir, 4.0); // Maksimum skor adalah 4.0
    }

    double hitungSkorKepuasanPengguna() {
        if (jumlah_pengguna_responden == 0 || jumlahLulusan == 0) {
            std::cout << "Error: Tidak ada data pengguna atau lulusan untuk dihitung.\n";
            return 0.0;
        }

        // Hitung total TKi untuk semua aspek
        double total_TKi = 0.0;
            for (const auto& aspek : aspek_kepuasan) {
                total_TKi += hitungTKi(aspek);
            }

        // Hitung rata-rata TKi
        double rata_rata_TKI = total_TKi / aspek_kepuasan.size();

        // Validasi prmin
        double prmin = (jumlahLulusan >= 300) ? 30.0 : (50.0 - ((jumlahLulusan / 300.0) * 20.0));
        double persentase_respon = (jumlah_pengguna_responden / jumlahLulusan) * 100.0;

        if (persentase_respon < prmin) {
            return std::min((persentase_respon / prmin) * rata_rata_TKI, 4.0);
        }

        return std::min(rata_rata_TKI, 4.0); // Maksimum skor adalah 4.0
    }

    double hitungSkorPublikasiMahasiswa() {
        if (jumlahMahasiswa == 0) {
            std::cout << "Error: Tidak ada data jumlah mahasiswa untuk dihitung.\n";
            return 0.0;
        }

        // Hitung rasio level publikasi
        double RL = ((publikasiNasionalTidakTerakreditasi + seminarWilayah + mediaMassaWilayah) / jumlahMahasiswa) * 100.0;
        double RN = ((publikasiNasionalTerakreditasi + publikasiInternasional + seminarNasional + mediaMassaNasional) / jumlahMahasiswa) * 100.0;
        double RI = ((publikasiInternasionalReputasi + seminarInternasional + mediaMassaInternasional) / jumlahMahasiswa) * 100.0;

        // Kriteria dari dokumen
        const double a = 2.0;   // Internasional bereputasi
        const double b = 20.0;  // Nasional/internasional
        const double c = 70.0;  // Wilayah/lokal

        if (RI >= a) return 4.0;
        if (RI < a && RN >= b) return 3.0 + (RI / a);
        if (RI == 0 && RN == 0 && RL >= c) return 2.0;
        if (RI == 0 && RN == 0 && RL < c) return (2.0 * RL) / c;

        // Kasus kompleks
        return 2.0 + (2.0 * (RI / a)) + (RN / b) - ((RI * RN) / (a * b));
    }

public:
    ISKBanPTCalculator() {
        // Inisialisasi default
        ndtps = 0; ndgb = 0; ndlk = 0;
        kurikulumEvaluasiSkor = 0;
        kurikulumCapaianPembelajaranSkor = 0;
        kurikulumStrukturSkor = 0;
        spmiAspekTerpenuhi = 0;
        melampaui_sn_dikti = false;
        tracerStudyAspekTerpenuhi = 0;
        persentaseBidangKerja = 0;
        jumlahLulusan = 0;
        lulusanTerlacak = 0;
        
        publikasiNasionalTidakTerakreditasi = 0;
        publikasiNasionalTerakreditasi = 0;
        publikasiInternasional = 0;
        publikasiInternasionalReputasi = 0;
        seminarWilayah = 0;
        seminarNasional = 0;
        seminarInternasional = 0;
        mediaMassaWilayah = 0;
        mediaMassaNasional = 0;
        mediaMassaInternasional = 0;
        jumlahMahasiswa = 0;

        // Inisialisasi aspek kepuasan
        std::vector<std::string> nama_aspek = {
            "Etika (Total Keseluruhan Kriteria <= 100)",
            "Keahlian pada bidang ilmu (Total Keseluruhan Kriteria <= 100)",
            "Kemampuan berbahasa asing (Total Keseluruhan Kriteria <= 100)",
            "Penggunaan teknologi informasi (Total Keseluruhan Kriteria <= 100)",
            "Kemampuan berkomunikasi (Total KeseluruhanKriteria <= 100)",
            "Kerjasama (Total Keseluruhan Kriteria <= 100)",
            "Pengembangan diri (Total Keseluruhan Kriteria <= 100)"
        };

        for (const auto& nama : nama_aspek) {
            aspek_kepuasan.push_back({nama, 0, 0, 0, 0});
        }
    }      
    
    void inputDataDosen() {
        std::cout << "\n=== Input Data Dosen ===\n";
        ndtps = inputValidasi<double>("Jumlah total dosen tetap (0-100): ", 0.0, 100.0);
        ndgb = inputValidasi<double>("Jumlah dosen guru besar (0-100): ", 0.0, 100.0);
        ndlk = inputValidasi<double>("Jumlah dosen lektor kepala (0-100): ", 0.0, 100.0);
    }

    void inputDataKurikulum() {
        std::cout << "\n=== Input Data Kurikulum ===\n";
        kurikulumEvaluasiSkor = inputValidasi<int>("Skor Evaluasi Kurikulum (0-4): ", 0, 4);
        kurikulumCapaianPembelajaranSkor = inputValidasi<int>("Skor Capaian Pembelajaran (0-4): ", 0, 4);
        kurikulumStrukturSkor = inputValidasi<int>("Skor Struktur Kurikulum (0-4): ", 0, 4);
    }

    void inputPenjaminanMutu() {
        std::cout << "\n=== Input Penjaminan Mutu ===\n";
        spmiAspekTerpenuhi = inputValidasi<int>("Jumlah aspek SPMI terpenuhi (0-5): ", 0, 5);
        int melampaui = inputValidasi<int>("Apakah melampaui SN-DIKTI? (1=Ya, 0=Tidak): ", 0, 1);
        melampaui_sn_dikti = (melampaui == 1);
    }

    void inputPelampauanSNDIKTI() {
        std::cout << "\n=== Input Pelampauan SN-DIKTI ===\n";
        indikator_terpenuhi = inputValidasi<int>("Jumlah indikator kinerja tambahan yang terpenuhi (0-5): ", 0, 5);
    }

    void inputPelacakanLulusan() {
        std::cout << "\n=== Input Pelacakan Lulusan ===\n";
        tracerStudyAspekTerpenuhi = inputValidasi<int>("Jumlah aspek Tracer Study terpenuhi (0-5): ", 0, 5);
        persentaseBidangKerja = inputValidasi<double>("Persentase kesesuaian bidang kerja (0-100): ", 0.0, 100.0);
        jumlahLulusan = inputValidasi<double>("Jumlah total lulusan dalam 3 tahun terakhir (0-400): ", 0.0, 400.0);
        lulusanTerlacak = inputValidasi<double>("Jumlah lulusan yang terlacak (0-<=jumlah_lulusan): ", 0.0, jumlahLulusan);
    }

    void inputDataKepuasanPengguna() {
        std::cout << "\n=== Input Data Kepuasan Pengguna ===\n";
        jumlahLulusan = inputValidasi<double>("Jumlah lulusan dalam 3 tahun terakhir (0-400): ", 0.0, 400.0);
        jumlah_pengguna_responden = inputValidasi<double>("Jumlah pengguna lulusan yang merespons (0-<=jumlahLulusan): ", 0.0, jumlahLulusan);

        for (auto& aspek : aspek_kepuasan) {
            do {
                std::cout << "\nAspek: " << aspek.nama << std::endl;
                aspek.sangat_baik = inputValidasi<double>("Persentase Sangat Baik (0-100): ", 0.0, 100.0);
                aspek.baik = inputValidasi<double>("Persentase Baik (0-100): ", 0.0, 100.0);
                aspek.cukup = inputValidasi<double>("Persentase Cukup (0-100): ", 0.0, 100.0);
                aspek.kurang = inputValidasi<double>("Persentase Kurang (0-100): ", 0.0, 100.0);
            } while (!validasiPersentase(aspek.sangat_baik, aspek.baik, aspek.cukup, aspek.kurang));
        }
    }
    

    void inputPublikasiMahasiswa() {
        jumlahMahasiswa = inputValidasi<double>("Jumlah mahasiswa saat ini dalam 3 tahun terakhir (0-400): ", 0.0, 400.0);
    
        publikasiNasionalTidakTerakreditasi = inputValidasi<double>("Publikasi Nasional Tidak Terakreditasi (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);
        publikasiNasionalTerakreditasi = inputValidasi<double>("Publikasi Nasional Terakreditasi (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);
        publikasiInternasional = inputValidasi<double>("Publikasi Internasional (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);
        publikasiInternasionalReputasi = inputValidasi<double>("Publikasi Internasional Bereputasi (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);

        seminarWilayah = inputValidasi<double>("Seminar Wilayah/Lokal (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);
        seminarNasional = inputValidasi<double>("Seminar Nasional (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);
        seminarInternasional = inputValidasi<double>("Seminar Internasional (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);

        mediaMassaWilayah = inputValidasi<double>("Media Massa Wilayah (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);
        mediaMassaNasional = inputValidasi<double>("Media Massa Nasional (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);
        mediaMassaInternasional = inputValidasi<double>("Media Massa Internasional (0-<= Jumlah Mahasiswa saat ini): ", 0.0, jumlahMahasiswa);
    }

    void hitungDanTampilkanSkor() {
        std::cout << "\n=== Hasil Perhitungan Skor ===\n";
        std::cout << "1. Skor Dosen Tetap: " 
                  << std::fixed << std::setprecision(2) 
                  << hitungSkorDosenTetap() << std::endl;
        std::cout << "2. Skor Jabatan Akademik: " 
                  << hitungSkorJabatanAkademik() << std::endl;
        std::cout << "3. Skor Kurikulum: "
                  << hitungSkorKurikulum() << std::endl;
        std::cout << "4. Skor Penjaminan Mutu: "
                  << hitungSkorPenjaminanMutu() << std::endl;
        std::cout << "5. Skor Pelampauan SN-DIKTI: "
                  << hitungSkorPelampauan_SN_DIKTI() << std::endl;
        std::cout << "6. Skor Tracer Study: "
                  << hitungSkorTracerStudy() << std::endl;
        std::cout << "7. Skor Kesesuaian Bidang Kerja: "
                  << hitungSkorKesesuaianBidangKerja() << std::endl;
        std::cout << "8. Skor Kepuasan Pengguna: "
                  << hitungSkorKepuasanPengguna() << std::endl;
        std::cout << "9. Skor Publikasi Mahasiswa: "
                  << hitungSkorPublikasiMahasiswa() << std::endl;  
    }

};

int main() {
    char lanjut;

    do {
        ISKBanPTCalculator kalkulator;

        std::cout << "=== Kalkulator ISK BAN-PT Program Magister ===\n";
        kalkulator.inputDataDosen();
        kalkulator.inputDataKurikulum();
        kalkulator.inputPenjaminanMutu();
        kalkulator.inputPelampauanSNDIKTI();
        kalkulator.inputPelacakanLulusan();
        kalkulator.inputPublikasiMahasiswa();
        kalkulator.inputDataKepuasanPengguna();
        kalkulator.hitungDanTampilkanSkor();

        std::cout << "\nIngin memasukkan data lagi? (y/n): ";
        std::cin >> lanjut;

        if (lanjut == 'N' || lanjut == 'n') {
            std::cout << "-Terimakasih Telah Memakai Program Ini-" << std::endl;
            std::cout << "--Program Selesai--" << std::endl;
        } else {
            std::cout << "Input Tidak Valid. Silahkan Masukkan (Y/N)" << std::endl;
        }
    } while (lanjut == 'y' || lanjut == 'Y');

    return 0;
}