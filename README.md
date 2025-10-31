# Studentų vertinimo sistema

Projektas sukurtas pagal užduočių reikalavimus (v0.1 → v0.2 → v1.0).  
Sistema leidžia:
- nuskaityti studentų duomenis iš failo;
- generuoti atsitiktinius studentus;
- suskaičiuoti galutinius pažymius (pagal vidurkį ir medianą);
- rūšiuoti ir skirstyti studentus į dvi kategorijas („vargšiukai“ ir „kietiakiai“);
- matuoti programos veikimo spartą skirtinguose konteineriuose (`vector`, `list`, `deque`).

---

## 💾 Versijos istorija

### ✅ v0.1
- Įgyvendinta `Student` klasė (Rule of Three, operator>> ir operator<<).
- Galutinio balo skaičiavimas pagal **vidurkį** ir **medianą**.
- Įvedimas iš konsolės, išvedimas į konsolę ar failą.

### ✅ v0.2
- Įdėta galimybė skaityti/išvesti didelius failus (iki 10⁷ įrašų).
- Pridėtas atsitiktinių studentų generavimas.
- Įdiegta konteinerių (vector, list, deque) spartos analizė.
- Skirstymas į dvi grupes pagal galutinį balą.

### 🚀 v1.0
- Įgyvendintos **dvi skirstymo strategijos**:
  1. **Strategija 1:** Sukuriami du nauji konteineriai („vargšiukai“ ir „kietiakiai“).  
     Bendrame konteineryje studentai lieka — užimama daugiau atminties.
  2. **Strategija 2:** Kuriamas tik „vargšiukų“ konteineris, studentai šalinami iš bendro sąrašo.  
     Efektyviau atminties atžvilgiu, bet lėčiau kai kuriuose konteineriuose.

- Palyginta veikimo sparta tarp:
  - `std::vector`
  - `std::list`
  - `std::deque`

- Optimizuota su `std::partition`, `std::stable_partition`, `std::remove_if`, `std::copy_if`.

---

## ⚙️ Įdiegimas

### 1️⃣ Reikalavimai
- **Windows 10+**  
- **Visual Studio 2022** su `C++ Desktop Development`
- **CMake ≥ 3.16**

### 2️⃣ Projekto sukūrimas

Atidaryk **x64 Native Tools Command Prompt for VS 2022** ir paleisk:

```bat
cd C:\Users\admin\source\repos\darbas\studentai
cmake -S . -B build -G "NMake Makefiles"
cmake --build build
