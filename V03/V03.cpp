
#include <algorithm>
#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// ======= bendri tipai/pagalbinės =======
struct AggV1 { std::string vardas, pavarde; double gavg = 0, gmed = 0; };

static inline double avgVec(const std::vector<int>& v) {
    if (v.empty()) return 0.0;
    long long s = 0; for (int x : v) s += x; return double(s) / v.size();
}
static inline double medVec(std::vector<int> v) {
    if (v.empty()) return 0.0;
    std::sort(v.begin(), v.end());
    size_t n = v.size();
    return (n % 2) ? double(v[n / 2]) : (double(v[n / 2 - 1]) + v[n / 2]) / 2.0;
}

// nuskaitymas į bet kurį std konteinerį su push_back
template<class C>
static void readAggFile(const std::string& path, C& cont) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Nepavyko atidaryti: " + path);
    std::string header; std::getline(in, header);
    std::string line, v, p; int x;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        if (!(iss >> v >> p)) continue;
        std::vector<int> vals;
        while (iss >> x) vals.push_back(x);
        if (vals.empty()) continue;
        int egz = vals.back(); vals.pop_back();
        double a = avgVec(vals), m = medVec(vals);
        cont.push_back(AggV1{ v,p, 0.4 * a + 0.6 * egz, 0.4 * m + 0.6 * egz });
    }
}

static void writeHeader(std::ofstream& o, const std::string& title) {
    o << std::left << std::setw(12) << "Pavardė"
        << std::left << std::setw(12) << "Vardas"
        << std::right << std::setw(12) << title << "\n";
    o << std::string(12 + 12 + 12, '-') << "\n";
}

// spausdinam dvi grupes
template<class C>
static void writeTwo(const std::string& base, const C& low, const C& high, bool useMed) {
    std::ofstream a(base + "_vargsiukai.txt"), b(base + "_kietiakiai.txt");
    writeHeader(a, "Galutinis");
    writeHeader(b, "Galutinis");
    auto G = [&](const AggV1& s) { return useMed ? s.gmed : s.gavg; };
    for (const auto& s : low)
        a << std::left << std::setw(12) << s.pavarde << std::left << std::setw(12) << s.vardas
        << std::right << std::setw(12) << std::fixed << std::setprecision(2) << G(s) << "\n";
    for (const auto& s : high)
        b << std::left << std::setw(12) << s.pavarde << std::left << std::setw(12) << s.vardas
        << std::right << std::setw(12) << std::fixed << std::setprecision(2) << G(s) << "\n";
}

struct Timings { double tRead = 0, tSplit = 0, tWrite = 0, tAll = 0; };
static void printT(const Timings& T) {
    std::cerr << "Nuskaitymas: " << T.tRead << " s\n";
    std::cerr << "Skirstymas : " << T.tSplit << " s\n";
    if (T.tWrite > 0) std::cerr << "Naujų failų sukūrimas: " << T.tWrite << " s\n";
    std::cerr << "Viskas kartu: " << T.tAll << " s\n";
}

// ======= STRATEGIJA 1 (bench1): kopija į du konteinerius =======
int cmd_bench1(int argc, char** argv) {
    std::string input, container = "vector", mode = "avg", out = "rez_s1";
    for (int i = 1;i < argc;++i) {
        std::string a = argv[i];
        if (a.rfind("--input=", 0) == 0) input = a.substr(8);
        else if (a.rfind("--container=", 0) == 0) container = a.substr(12);
        else if (a.rfind("--mode=", 0) == 0) mode = a.substr(7);
        else if (a.rfind("--out=", 0) == 0) out = a.substr(6);
    }
    if (input.empty()) { std::cerr << "Naudojimas: bench1 --input=... --container=vector|list|deque --mode=avg|med --out=baze\n"; return 2; }
    bool useMed = (mode == "med");

    Timings T; auto t0 = std::chrono::high_resolution_clock::now();

    if (container == "vector") {
        std::vector<AggV1> all; auto tA = std::chrono::high_resolution_clock::now(); readAggFile(input, all); auto tB = std::chrono::high_resolution_clock::now();
        std::vector<AggV1> low, high; low.reserve(all.size()); high.reserve(all.size());
        auto G = [&](const AggV1& s) { return useMed ? s.gmed : s.gavg; };
        for (const auto& s : all) (G(s) < 5.0 ? low : high).push_back(s);
        auto tC = std::chrono::high_resolution_clock::now();
        auto tD = std::chrono::high_resolution_clock::now(); writeTwo(out, low, high, useMed); auto tE = std::chrono::high_resolution_clock::now();
        T.tRead = std::chrono::duration<double>(tB - tA).count(); T.tSplit = std::chrono::duration<double>(tC - tB).count(); T.tWrite = std::chrono::duration<double>(tE - tD).count();
    }
    else if (container == "list") {
        std::list<AggV1> all; auto tA = std::chrono::high_resolution_clock::now(); readAggFile(input, all); auto tB = std::chrono::high_resolution_clock::now();
        std::list<AggV1> low, high; auto G = [&](const AggV1& s) { return useMed ? s.gmed : s.gavg; };
        for (const auto& s : all) (G(s) < 5.0 ? low : high).push_back(s);
        auto tC = std::chrono::high_resolution_clock::now();
        auto tD = std::chrono::high_resolution_clock::now(); writeTwo(out, low, high, useMed); auto tE = std::chrono::high_resolution_clock::now();
        T.tRead = std::chrono::duration<double>(tB - tA).count(); T.tSplit = std::chrono::duration<double>(tC - tB).count(); T.tWrite = std::chrono::duration<double>(tE - tD).count();
    }
    else if (container == "deque") {
        std::deque<AggV1> all; auto tA = std::chrono::high_resolution_clock::now(); readAggFile(input, all); auto tB = std::chrono::high_resolution_clock::now();
        std::deque<AggV1> low, high; auto G = [&](const AggV1& s) { return useMed ? s.gmed : s.gavg; };
        for (const auto& s : all) (G(s) < 5.0 ? low : high).push_back(s);
        auto tC = std::chrono::high_resolution_clock::now();
        auto tD = std::chrono::high_resolution_clock::now(); writeTwo(out, low, high, useMed); auto tE = std::chrono::high_resolution_clock::now();
        T.tRead = std::chrono::duration<double>(tB - tA).count(); T.tSplit = std::chrono::duration<double>(tC - tB).count(); T.tWrite = std::chrono::duration<double>(tE - tD).count();
    }
    else { std::cerr << "Nežinomas konteineris\n"; return 3; }

    auto t1 = std::chrono::high_resolution_clock::now(); T.tAll = std::chrono::duration<double>(t1 - t0).count(); printT(T); return 0;
}

// ======= STRATEGIJA 2 (bench2): tik „vargšiukai“, o all lieka „kietiakiai“ =======
int cmd_bench2(int argc, char** argv) {
    std::string input, container = "vector", mode = "avg", out = "rez_s2";
    for (int i = 1;i < argc;++i) {
        std::string a = argv[i];
        if (a.rfind("--input=", 0) == 0) input = a.substr(8);
        else if (a.rfind("--container=", 0) == 0) container = a.substr(12);
        else if (a.rfind("--mode=", 0) == 0) mode = a.substr(7);
        else if (a.rfind("--out=", 0) == 0) out = a.substr(6);
    }
    if (input.empty()) { std::cerr << "Naudojimas: bench2 --input=... --container=vector|list|deque --mode=avg|med --out=baze\n"; return 2; }
    bool useMed = (mode == "med");

    Timings T; auto t0 = std::chrono::high_resolution_clock::now();

    if (container == "vector") {
        std::vector<AggV1> all; auto tA = std::chrono::high_resolution_clock::now(); readAggFile(input, all); auto tB = std::chrono::high_resolution_clock::now();
        std::vector<AggV1> low; auto isHigh = [&](const AggV1& s) { return (useMed ? s.gmed : s.gavg) >= 5.0; };
        auto it = std::stable_partition(all.begin(), all.end(), isHigh); // [it,end) – vargšiukai
        low.assign(it, all.end()); all.erase(it, all.end()); // all – „kietiakiai“
        auto tC = std::chrono::high_resolution_clock::now();
        auto tD = std::chrono::high_resolution_clock::now(); writeTwo(out, low, all, useMed); auto tE = std::chrono::high_resolution_clock::now();
        T.tRead = std::chrono::duration<double>(tB - tA).count(); T.tSplit = std::chrono::duration<double>(tC - tB).count(); T.tWrite = std::chrono::duration<double>(tE - tD).count();
    }
    else if (container == "list") {
        std::list<AggV1> all; auto tA = std::chrono::high_resolution_clock::now(); readAggFile(input, all); auto tB = std::chrono::high_resolution_clock::now();
        std::list<AggV1> low;
        for (auto it = all.begin(); it != all.end(); ) { double g = useMed ? it->gmed : it->gavg; if (g < 5.0) { low.push_back(*it); it = all.erase(it); } else ++it; }
        auto tC = std::chrono::high_resolution_clock::now();
        auto tD = std::chrono::high_resolution_clock::now(); writeTwo(out, low, all, useMed); auto tE = std::chrono::high_resolution_clock::now();
        T.tRead = std::chrono::duration<double>(tB - tA).count(); T.tSplit = std::chrono::duration<double>(tC - tB).count(); T.tWrite = std::chrono::duration<double>(tE - tD).count();
    }
    else if (container == "deque") {
        std::deque<AggV1> all; auto tA = std::chrono::high_resolution_clock::now(); readAggFile(input, all); auto tB = std::chrono::high_resolution_clock::now();
        std::deque<AggV1> low;
        for (auto it = all.begin(); it != all.end(); ) { double g = useMed ? it->gmed : it->gavg; if (g < 5.0) { low.push_back(*it); it = all.erase(it); } else ++it; }
        auto tC = std::chrono::high_resolution_clock::now();
        auto tD = std::chrono::high_resolution_clock::now(); writeTwo(out, low, all, useMed); auto tE = std::chrono::high_resolution_clock::now();
        T.tRead = std::chrono::duration<double>(tB - tA).count(); T.tSplit = std::chrono::duration<double>(tC - tB).count(); T.tWrite = std::chrono::duration<double>(tE - tD).count();
    }
    else { std::cerr << "Nežinomas konteineris\n"; return 3; }

    auto t1 = std::chrono::high_resolution_clock::now(); T.tAll = std::chrono::duration<double>(t1 - t0).count(); printT(T); return 0;
}

// ======= OPTIMIZACIJOS ANT VEKTORIAUS (benchopt) =======
int cmd_benchopt(int argc, char** argv) {
    std::string input, algo = "partition", mode = "avg", out = "rez_opt";
    for (int i = 1;i < argc;++i) {
        std::string a = argv[i];
        if (a.rfind("--input=", 0) == 0) input = a.substr(8);
        else if (a.rfind("--algo=", 0) == 0) algo = a.substr(7);
        else if (a.rfind("--mode=", 0) == 0) mode = a.substr(7);
        else if (a.rfind("--out=", 0) == 0) out = a.substr(6);
    }
    if (input.empty()) { std::cerr << "Naudojimas: benchopt --input=... --algo=partition|stable_part|remove_if|copy_if --mode=avg|med --out=baze\n"; return 2; }
    bool useMed = (mode == "med");

    Timings T; auto t0 = std::chrono::high_resolution_clock::now();

    std::vector<AggV1> all; auto tA = std::chrono::high_resolution_clock::now(); readAggFile(input, all); auto tB = std::chrono::high_resolution_clock::now();

    std::vector<AggV1> low, high;
    auto isLow = [&](const AggV1& s) { return (useMed ? s.gmed : s.gavg) < 5.0; };
    auto isHigh = [&](const AggV1& s) { return !isLow(s); };

    auto tC = std::chrono::high_resolution_clock::now();
    if (algo == "partition") {
        auto it = std::partition(all.begin(), all.end(), isHigh);
        low.assign(it, all.end()); all.erase(it, all.end());
    }
    else if (algo == "stable_part") {
        auto it = std::stable_partition(all.begin(), all.end(), isHigh);
        low.assign(it, all.end()); all.erase(it, all.end());
    }
    else if (algo == "remove_if") {
        std::copy_if(all.begin(), all.end(), std::back_inserter(low), isLow);
        all.erase(std::remove_if(all.begin(), all.end(), isLow), all.end());
    }
    else if (algo == "copy_if") {
        std::copy_if(all.begin(), all.end(), std::back_inserter(low), isLow);
        std::copy_if(all.begin(), all.end(), std::back_inserter(high), isHigh);
    }
    else { std::cerr << "Nežinomas --algo\n"; return 3; }
    auto tD = std::chrono::high_resolution_clock::now();

    auto tE = std::chrono::high_resolution_clock::now(); writeTwo(out, low, (high.empty() ? all : high), useMed); auto tF = std::chrono::high_resolution_clock::now();

    T.tRead = std::chrono::duration<double>(tB - tA).count();
    T.tSplit = std::chrono::duration<double>(tD - tC).count();
    T.tWrite = std::chrono::duration<double>(tF - tE).count();

    auto t1 = std::chrono::high_resolution_clock::now(); T.tAll = std::chrono::duration<double>(t1 - t0).count(); printT(T); return 0;
}

// ======= main: dispatcher =======
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Komandos:\n"
            "  bench1  --input=... --container=vector|list|deque --mode=avg|med --out=baze\n"
            "  bench2  --input=... --container=vector|list|deque --mode=avg|med --out=baze\n"
            "  benchopt --input=... --algo=partition|stable_part|remove_if|copy_if --mode=avg|med --out=baze\n";
        return 1;
    }
    std::string cmd = argv[1];
    if (cmd == "bench1")   return cmd_bench1(argc - 1, argv + 1);
    if (cmd == "bench2")   return cmd_bench2(argc - 1, argv + 1);
    if (cmd == "benchopt") return cmd_benchopt(argc - 1, argv + 1);
    std::cerr << "Nežinoma komanda\n"; return 1;
}

