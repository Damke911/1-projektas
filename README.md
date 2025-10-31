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

▶️ Naudojimas
Studentų duomenų failas

Failas (students_100000.txt) turi būti formato:

Pavardė     Vardas      ND1 ND2 ND3 ND4 ND5 Egzaminas
Pavarde1    Vardas1     5   8   9   10  7   8
Pavarde2    Vardas2     3   5   6   4   7   6
...

Paleidimo pavyzdžiai
1 strategija – du nauji konteineriai:

build\v10_bench.exe bench1 --input=data\students_100000.txt --container=vector --mode=avg --out=rez_s1

| Strategija  | Konteineris | 100k įrašų (s) | 1M įrašų (s) | Pastabos              |
| ----------- | ----------- | -------------- | ------------ | --------------------- |
| 1           | `vector`    | 0.23           | 2.84         | greičiausias          |
| 1           | `list`      | 0.45           | 5.22         | lėtesnis              |
| 2           | `deque`     | 0.38           | 3.40         | atminties efektyviau  |
| Optimizuota | `vector`    | 0.18           | 2.12         | geriausias rezultatas |

Autorius: Tomas Damkauskas
Data: 2025-10-31
