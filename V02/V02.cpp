#include <algorithm>
#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <direct.h> // _mkdir
#endif

namespace v02 {

    static double avg(const std::vector<int>& v) {
        if (v.empty()) return 0.0;
        long long s = 0; for (int x : v) s += x; return double(s) / v.size();
    }
    static double med(std::vector<int> v) {
        if (v.empty()) return 0.0;
        std::sort(v.begin(), v.end()); size_t n = v.size();
        return (n % 2) ? double(v[n / 2]) : (double(v[n / 2 - 1]) + v[n / 2]) / 2.0;
    }

    // ----------------- GEN -----------------
    static void ensureDirPortable(const std::string& path) {
#ifdef _WIN32
        _mkdir(path.c_str());
#else
        std::string cmd = std::string("mkdir -p ") + path; std::system(cmd.c_str());
#endif
    }

    static void generateOne(const std::string& path, long long n, int hwN) {
        std::ofstream out(path);
        if (!out) throw std::runtime_error("Nepavyko sukurti: " + path);
        out << std::left << std::setw(12) << "Pavardė" << std::left << std::setw(12) << "Vardas";
        for (int i = 1;i <= hwN;++i) out << std::right << std::setw(5) << (std::string("ND") + std::to_string(i));
        out << std::right << std::setw(12) << "Egzaminas" << "\n";
        std::mt19937 g(std::random_device{}()); std::uniform_int_distribution<int> d(0, 10);
        for (long long i = 1;i <= n;++i) {
            out << std::left << std::setw(12) << ("Vardas" + std::to_string(i))
                << std::left << std::setw(12) << ("Pavarde" + std::to_string(i));
            for (int k = 0;k < hwN;++k) out << std::right << std::setw(5) << d(g);
            out << std::right << std::setw(12) << d(g) << "\n";
        }
    }

    static int cmd_gen(int argc, char** argv) {
        std::string sizes = "1000,10000,100000"; int hw = 5; std::string outdir = "data";
        for (int i = 1;i < argc;++i) {
            std::string a = argv[i];
            if (a.rfind("--sizes=", 0) == 0) sizes = a.substr(8);
            else if (a.rfind("--hw=", 0) == 0) hw = std::stoi(a.substr(5));
            else if (a.rfind("--outdir=", 0) == 0) outdir = a.substr(9);
        }
        ensureDirPortable(outdir);
        std::stringstream ss(sizes); std::string tok;
        while (std::getline(ss, tok, ',')) {
            long long n = std::stoll(tok);
            std::string path = outdir + "/students_" + tok + ".txt";
            auto t0 = std::chrono::high_resolution_clock::now();
            generateOne(path, n, hw);
            auto t1 = std::chrono::high_resolution_clock::now();
            std::cerr << "[GEN] " << path << " per "
                << std::chrono::duration<double>(t1 - t0).count() << " s\n";
        }
        return 0;
    }

    // ----------------- BENCH -----------------
    struct Agg { std::string vardas, pavarde; double gavg, gmed; };

    template<class C>
    static void readFile(const std::string& path, C& cont) {
        std::ifstream in(path);
        if (!in) throw std::runtime_error("Nepavyko atidaryti: " + path);
        std::string header; std::getline(in, header);
        std::string line, v, p; int x;
        while (std::getline(in, line)) {
            if (line.empty()) continue; std::istringstream iss(line);
            if (!(iss >> v >> p)) continue; std::vector<int> vals; while (iss >> x) vals.push_back(x); if (vals.empty()) continue;
            int egz = vals.back(); vals.pop_back();
            double a = avg(vals); double m = med(vals);
            cont.push_back(Agg{ v,p, 0.4 * a + 0.6 * egz, 0.4 * m + 0.6 * egz });
        }
    }

    template<class C>
    static void splitTwo(const C& cont, C& low, C& high, bool useMed) {
        for (const auto& s : cont) { double g = useMed ? s.gmed : s.gavg; if (g < 5.0) low.push_back(s); else high.push_back(s); }
    }

    template<class C>
    static void writeTwo(const std::string& base, const C& low, const C& high) {
        std::ofstream a(base + "_vargsiukai.txt"), b(base + "_kietiakiai.txt");
        a << std::left << std::setw(12) << "Pavardė" << std::left << std::setw(12) << "Vardas" << std::right << std::setw(16) << "Galutinis" << "\n";
        a << std::string(40, '-') << "\n";
        b << std::left << std::setw(12) << "Pavardė" << std::left << std::setw(12) << "Vardas" << std::right << std::setw(16) << "Galutinis" << "\n";
        b << std::string(40, '-') << "\n";
        for (const auto& s : low)  a << std::left << std::setw(12) << s.pavarde << std::left << std::setw(12) << s.vardas << std::right << std::setw(16) << std::fixed << std::setprecision(2) << s.gavg << "\n";
        for (const auto& s : high) b << std::left << std::setw(12) << s.pavarde << std::left << std::setw(12) << s.vardas << std::right << std::setw(16) << std::fixed << std::setprecision(2) << s.gavg << "\n";
    }

    static int cmd_bench(int argc, char** argv) {
        std::string input; std::string container = "vector"; std::string mode = "avg"; std::string outBase = "rez";
        for (int i = 1;i < argc;++i) {
            std::string a = argv[i];
            if (a.rfind("--input=", 0) == 0) input = a.substr(8);
            else if (a.rfind("--container=", 0) == 0) container = a.substr(12);
            else if (a.rfind("--mode=", 0) == 0) mode = a.substr(7);
            else if (a.rfind("--out=", 0) == 0) outBase = a.substr(6);
        }
        if (input.empty()) { std::cerr << "Naudojimas: bench --input=failas --container=vector|list|deque --mode=avg|med --out=baze\n"; return 2; }
        bool useMed = (mode == "med");

        auto tStart = std::chrono::high_resolution_clock::now();

        if (container == "vector") {
            std::vector<Agg> all; auto t0 = std::chrono::high_resolution_clock::now(); readFile(input, all); auto t1 = std::chrono::high_resolution_clock::now();
            std::vector<Agg> low, high; auto t2 = std::chrono::high_resolution_clock::now(); splitTwo(all, low, high, useMed); auto t3 = std::chrono::high_resolution_clock::now();
            auto t4 = std::chrono::high_resolution_clock::now(); writeTwo(outBase, low, high); auto t5 = std::chrono::high_resolution_clock::now();
            std::cerr << "Nuskaitymas: " << std::chrono::duration<double>(t1 - t0).count() << " s\n";
            std::cerr << "Skirstymas : " << std::chrono::duration<double>(t3 - t2).count() << " s\n";
            std::cerr << "Naujų failų sukūrimas: " << std::chrono::duration<double>(t5 - t4).count() << " s\n";
        }
        else if (container == "list") {
            std::list<Agg> all; auto t0 = std::chrono::high_resolution_clock::now(); readFile(input, all); auto t1 = std::chrono::high_resolution_clock::now();
            std::list<Agg> low, high; auto t2 = std::chrono::high_resolution_clock::now(); splitTwo(all, low, high, useMed); auto t3 = std::chrono::high_resolution_clock::now();
            auto t4 = std::chrono::high_resolution_clock::now(); writeTwo(outBase, low, high); auto t5 = std::chrono::high_resolution_clock::now();
            std::cerr << "Nuskaitymas: " << std::chrono::duration<double>(t1 - t0).count() << " s\n";
            std::cerr << "Skirstymas : " << std::chrono::duration<double>(t3 - t2).count() << " s\n";
            std::cerr << "Naujų failų sukūrimas: " << std::chrono::duration<double>(t5 - t4).count() << " s\n";
        }
        else if (container == "deque") {
            std::deque<Agg> all; auto t0 = std::chrono::high_resolution_clock::now(); readFile(input, all); auto t1 = std::chrono::high_resolution_clock::now();
            std::deque<Agg> low, high; auto t2 = std::chrono::high_resolution_clock::now(); splitTwo(all, low, high, useMed); auto t3 = std::chrono::high_resolution_clock::now();
            auto t4 = std::chrono::high_resolution_clock::now(); writeTwo(outBase, low, high); auto t5 = std::chrono::high_resolution_clock::now();
            std::cerr << "Nuskaitymas: " << std::chrono::duration<double>(t1 - t0).count() << " s\n";
            std::cerr << "Skirstymas : " << std::chrono::duration<double>(t3 - t2).count() << " s\n";
            std::cerr << "Naujų failų sukūrimas: " << std::chrono::duration<double>(t5 - t4).count() << " s\n";
        }
        else {
            std::cerr << "Nežinomas konteineris: " << container << " (naudokite vector|list|deque)\n"; return 3;
        }

        auto tEnd = std::chrono::high_resolution_clock::now();
        std::cerr << "Viskas kartu: " << std::chrono::duration<double>(tEnd - tStart).count() << " s\n";
        return 0;
    }

} // namespace v02

int main(int argc, char** argv) {
    // Vienas EXE – dvi komandos: gen / bench
    if (argc < 2) {
        std::cerr << "Naudojimas:\n  gen  --sizes=... --hw=5 --outdir=data\n  bench --input=failas --container=vector|list|deque --mode=avg|med --out=baze\n";
        return 1;
    }
    std::string cmd = argv[1];
    if (cmd == "gen")   return v02::cmd_gen(argc - 1, argv + 1);
    if (cmd == "bench") return v02::cmd_bench(argc - 1, argv + 1);
    std::cerr << "Nežinoma komanda: " << cmd << " (naudokite gen arba bench)\n"; return 1;
}
