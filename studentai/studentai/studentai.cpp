#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>  // _getcwd
static void enableUtf8Console() { SetConsoleOutputCP(CP_UTF8); }
#else
static void enableUtf8Console() {}
#endif

using namespace std;

struct Student {
    string vardas, pavarde;
    vector<int> nd;
    int egz = 0;
    double galutinis_avg = 0, galutinis_med = 0;
};

double average(const vector<int>& v) {
    if (v.empty()) return 0;
    double sum = 0;
    for (int x : v) sum += x;
    return sum / v.size();
}

double median(vector<int> v) {
    if (v.empty()) return 0;
    sort(v.begin(), v.end());
    size_t n = v.size();
    return n % 2 ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2.0;
}

void computeFinal(Student& s) {
    s.galutinis_avg = 0.4 * average(s.nd) + 0.6 * s.egz;
    s.galutinis_med = 0.4 * median(s.nd) + 0.6 * s.egz;
}

void printHeader() {
    cout << left << setw(12) << "Pavardė" << left << setw(12) << "Vardas"
        << right << setw(16) << "Galutinis (Vid.)" << right << setw(18)
        << "Galutinis (Med.)" << "\n";
    cout << string(12 + 12 + 16 + 18, '-') << "\n";
}

// Tvirtesnis nuskaitymas: skaitome eilutėmis
void readFromFile(const string& filename, vector<Student>& studs) {
    ifstream in(filename);
    if (!in) {
#ifdef _WIN32
        {
            char cwd[MAX_PATH]{ 0 };
            if (_getcwd(cwd, MAX_PATH)) {
                cerr << "[DEBUG] current_path = " << cwd << "\n";
            }
            else {
                cerr << "[DEBUG] current_path = (nepavyko gauti)\n";
            }
        }
#else
        cerr << "[DEBUG] current_path = (Windows kodas – praleista)\n";
#endif
    }
    string header; getline(in, header); // praleidžiame antraštę

    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        Student s;
        if (!(iss >> s.vardas >> s.pavarde)) continue;
        vector<int> vals; int x;
        while (iss >> x) vals.push_back(x);
        if (vals.empty()) continue;
        s.egz = vals.back();
        vals.pop_back();
        s.nd = std::move(vals);
        computeFinal(s);
        studs.push_back(std::move(s));
    }
}

void genRandom(vector<Student>& studs, int n) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(0, 10);
    for (int i = 1; i <= n; ++i) {
        Student s;
        s.vardas = "Vardas" + to_string(i);
        s.pavarde = "Pavarde" + to_string(i);
        int ndCount = 5;
        for (int j = 0; j < ndCount; ++j) s.nd.push_back(dist(gen));
        s.egz = dist(gen);
        computeFinal(s);
        studs.push_back(std::move(s));
    }
}

int main() {
    enableUtf8Console();
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<Student> studs;
    cout << "Pasirinkite režimą:\n1 - skaityti iš failo (studentai10000.txt)\n2 - atsitiktinai sugeneruoti 5 studentus\n> ";
    int mode;
    cin >> mode;
    try {
        if (mode == 1) {
            cout << "[INFO] Bandoma skaityti: studentai10000.txt\n";
            readFromFile("studentai10000.txt", studs);
        }
        else {
            genRandom(studs, 5);
        }
    }
    catch (exception& e) {
        cerr << "Klaida: " << e.what() << "\n";
        cerr << "(Jei failas vietoje – pažymėk jį Project'e, Properties → 'Copy to Output Directory' = 'Copy always')\n";
        return 1;
    }


    sort(studs.begin(), studs.end(), [](const Student& a, const Student& b) {
        return a.pavarde < b.pavarde;
        });

    printHeader();
    for (auto& s : studs) {
        cout << left << setw(12) << s.pavarde << left << setw(12) << s.vardas
            << right << setw(16) << fixed << setprecision(2) << s.galutinis_avg
            << right << setw(18) << fixed << setprecision(2) << s.galutinis_med
            << '\n';
    }
}
