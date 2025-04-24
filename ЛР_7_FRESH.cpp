#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <locale>
#include <limits>
#include <deque>
#include <algorithm>

using namespace std;

// Класс Man
class Man {
public:
    wstring surname;
    wstring name;

    Man() : surname(L""), name(L"") {}
    Man(wstring s, wstring n) : surname(s), name(n) {}

    friend bool operator==(const Man& m1, const Man& m2) {
        return (m1.surname == m2.surname && m1.name == m2.name);
    }
};

// Перечисления для специальности и квалификации
enum Specialty { THERAPIST = 1, CARDIOLOGIST, DENTIST };
enum Qualification { HIGHEST = 0, FIRST, SECOND };

// Класс Doctor
class Doctor {
private:
    Man man;
    Specialty specialty;
    Qualification qualification;

public:
    Doctor() : man(), specialty(THERAPIST), qualification(HIGHEST) {}
    Doctor(Man m, Specialty s, Qualification q) : man(m), specialty(s), qualification(q) {}

    friend wistream& operator>>(wistream& in, Doctor& d) {
        wcout << L"Enter surname: ";
        in >> d.man.surname;
        if (in.fail()) {
            throw runtime_error("Error reading surname.");
        }

        wcout << L"Enter name: ";
        in >> d.man.name;
        if (in.fail()) {
            throw runtime_error("Error reading name.");
        }

        d.specialty = static_cast<Specialty>(getValidatedInput(L"Enter speciality (1-therapist, 2-cardiologist, 3-dentist): ", 1, 3));
        d.qualification = static_cast<Qualification>(getValidatedInput(L"Enter qualification (0-highest, 1-first, 2-second): ", 0, 2));

        return in;
    }

    void print() const {
        wcout << left << setw(20) << man.surname << setw(20) << man.name
            << setw(20) << specialtyToString(specialty) << setw(20) << qualificationToString(qualification) << endl;
    }

    Specialty getSpecialty() const { return specialty; }
    Qualification getQualification() const { return qualification; }
    const Man& getMan() const { return man; }

    static int getValidatedInput(const wstring& prompt, int min, int max) {
        int value;
        while (true) {
            try {
                wcout << prompt;
                wcin >> value;
                if (wcin.fail() || value < min || value > max) {
                    throw invalid_argument("Error: incorrect number. Try again.");
                }
                wcin.ignore(numeric_limits<streamsize>::max(), L'\n');
                break;
            }
            catch (const invalid_argument& e) {
                wcout << e.what() << endl;
                wcin.clear();
                wcin.ignore(numeric_limits<streamsize>::max(), L'\n');
            }
        }
        return value;
    }

    static wstring specialtyToString(Specialty s) {
        switch (s) {
        case THERAPIST: return L"Therapist";
        case CARDIOLOGIST: return L"Cardiologist";
        case DENTIST: return L"Dentist";
        default: return L"Unknown";
        }
    }

    static wstring qualificationToString(Qualification q) {
        switch (q) {
        case HIGHEST: return L"Highest";
        case FIRST: return L"First";
        case SECOND: return L"Second";
        default: return L"Unknown";
        }
    }
};

// Класс Clinic
class Clinic {
private:
    deque<Doctor> doctors;

public:
    void addDoctorToFront(const Doctor& doctor) {
        doctors.push_front(doctor);
    }

    void addDoctorToBack(const Doctor& doctor) {
        doctors.push_back(doctor);
    }

    void removeDoctorAtPosition(size_t position) {
        if (position < doctors.size()) {
            doctors.erase(doctors.begin() + position);
        }
        else {
            throw out_of_range("Position out of range.");
        }
    }

    Doctor* findDoctorByMan(const Man& man) {
        auto it = find_if(doctors.begin(), doctors.end(), [&man](const Doctor& d) {
            return d.getMan() == man;
            });
        return it != doctors.end() ? &(*it) : nullptr;
    }

    deque<Doctor> findDoctorsBySpecialty(Specialty specialty) {
        deque<Doctor> result;
        copy_if(doctors.begin(), doctors.end(), back_inserter(result), [specialty](const Doctor& d) {
            return d.getSpecialty() == specialty;
            });
        return result;
    }

    void sortDoctorsBySurname() {
        sort(doctors.begin(), doctors.end(), [](const Doctor& a, const Doctor& b) {
            return a.getMan().surname < b.getMan().surname;
            });
    }

    void printDoctors() const {
        wcout << left << setw(20) << L"Surname" << setw(20) << L"Name"
            << setw(20) << L"Speciality" << setw(20) << L"Qualification" << endl;
        wcout << wstring(80, L'-') << endl;
        for (const auto& doctor : doctors) {
            doctor.print();
        }
    }

    friend wostream& operator<<(wostream& out, const Clinic& clinic) {
        clinic.printDoctors();
        return out;
    }

    bool operator>(Specialty specialty) const {
        return any_of(doctors.begin(), doctors.end(), [specialty](const Doctor& d) {
            return d.getSpecialty() == specialty;
            });
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            string surname, name;
            int spec, qual;
            while (file >> surname >> name >> spec >> qual) {
                Doctor doctor(Man(wstring(surname.begin(), surname.end()), wstring(name.begin(), name.end())),
                    static_cast<Specialty>(spec), static_cast<Qualification>(qual));
                doctors.push_back(doctor);
            }
            file.close();
        }
    }

    void saveToFile(const string& filename) const {
        ofstream file(filename);
        if (file.is_open()) {
            for (const auto& doctor : doctors) {
                file << string(doctor.getMan().surname.begin(), doctor.getMan().surname.end()) << " "
                    << string(doctor.getMan().name.begin(), doctor.getMan().name.end()) << " "
                    << doctor.getSpecialty() << " " << doctor.getQualification() << endl;
            }
            file.close();
        }
    }
};

int main() {
    Clinic clinic;
    string filename = "polyclinic.csv";

    // Загрузка данных из файла
    clinic.loadFromFile(filename);

    int choice;
    do {
        wcout << L"\nMenu:\n"
            << L"1. Add new doctor to front\n"
            << L"2. Add new doctor to back\n"
            << L"3. Show all doctors\n"
            << L"4. Remove doctor at position\n"
            << L"5. Find doctor by name and surname\n"
            << L"6. Find doctors by specialty\n"
            << L"7. Sort doctors by surname\n"
            << L"8. Exit\n"
            << L"Choose action: ";
        wcin >> choice;

        if (wcin.fail()) {
            wcout << L"Error: incorrect input. Try again.\n";
            wcin.clear();
            wcin.ignore(numeric_limits<streamsize>::max(), L'\n');
            continue;
        }

        switch (choice) {
        case 1: {
            Doctor newDoctor;
            try {
                wcin >> newDoctor;
                clinic.addDoctorToFront(newDoctor);
            }
            catch (const runtime_error& e) {
                wcout << e.what() << endl;
            }
            clinic.printDoctors(); // Выводим таблицу после добавления
            break;
        }
        case 2: {
            Doctor newDoctor;
            try {
                wcin >> newDoctor;
                clinic.addDoctorToBack(newDoctor);
            }
            catch (const runtime_error& e) {
                wcout << e.what() << endl;
            }
            clinic.printDoctors(); // Выводим таблицу после добавления
            break;
        }
        case 3: {
            clinic.printDoctors(); // Выводим таблицу всех докторов
            break;
        }
        case 4: {
            size_t position;
            wcout << L"Enter position to remove: ";
            wcin >> position;
            try {
                clinic.removeDoctorAtPosition(position);
            }
            catch (const out_of_range& e) {
                wcout << e.what() << endl;
            }
            clinic.printDoctors(); // Выводим таблицу после удаления
            break;
        }
        case 5: {
            Man find;
            wcout << L"Enter surname for searching: ";
            wcin >> find.surname;
            wcout << L"Enter name for searching: ";
            wcin >> find.name;
            Doctor* foundDoctor = clinic.findDoctorByMan(find);
            if (foundDoctor) {
                wcout << L"The doctor was found: ";
                foundDoctor->print();
            }
            else {
                wcout << L"The doctor was not found.\n";
            }
            break;
        }
        case 6: {
            int spec = Doctor::getValidatedInput(L"Enter speciality (1-therapist, 2-cardiologist, 3-dentist): ", 1, 3);
            deque<Doctor> foundDoctors = clinic.findDoctorsBySpecialty(static_cast<Specialty>(spec));
            if (!foundDoctors.empty()) {
                wcout << L"Doctors with the specified specialty:\n";
                wcout << left << setw(20) << L"Surname" << setw(20) << L"Name"
                    << setw(20) << L"Speciality" << setw(20) << L"Qualification" << endl;
                wcout << wstring(80, L'-') << endl;
                for (const auto& doctor : foundDoctors) {
                    doctor.print();
                }
            }
            else {
                wcout << L"No doctors found with the specified specialty.\n";
            }
            break;
        }
        case 7: {
            clinic.sortDoctorsBySurname();
            wcout << L"Doctors were sorted by surname.\n";
            clinic.printDoctors(); // Выводим таблицу после сортировки
            break;
        }
        case 8: {
            clinic.saveToFile(filename);
            wcout << L"The data was saved. Exit from the program.\n";
            break;
        }
        default:
            wcout << L"Incorrect choice. Try again.\n";
        }
    } while (choice != 8);

    return 0;
}