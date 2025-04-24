#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept> // Для использования исключений

using namespace std;

// Базовый класс Comon
class Comon {
protected:
    string surname;
    string name;

public:
    Comon() : surname(""), name("") {}
    Comon(string s, string n) : surname(s), name(n) {}

    friend bool operator==(const Comon& m1, const Comon& m2) {
        return (m1.surname == m2.surname && m1.name == m2.name);
    }

    virtual void print() const {
        cout << left << setw(20) << surname << setw(20) << name;
    }

    void setSurname(const string& s) { surname = s; }
    void setName(const string& n) { name = n; }
    string getSurname() const { return surname; }
    string getName() const { return name; }
};

// Класс Date
class Date {
private:
    int day;
    int month;
    int year;

public:
    Date() : day(1), month(1), year(1) {}

    void readDate() {
        while (true) {
            try {
                cout << "Enter date with space in between (day month year): ";
                cin >> day >> month >> year;
                if (cin.fail() || day < 1 || day > 31 || month < 1 || month > 12 || year < 1) {
                    throw invalid_argument("Incorrect date. Try again.");
                }
                break;
            }
            catch (const invalid_argument& e) {
                cout << e.what() << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
    }

    void setDate(int d, int m, int y) {
        if (d < 1 || d > 31 || m < 1 || m > 12 || y < 1) {
            throw invalid_argument("Invalid date values.");
        }
        day = d;
        month = m;
        year = y;
    }

    void printDate() const {
        if (day < 10) cout << "0";
        cout << day << ".";
        if (month < 10) cout << "0";
        cout << month << "." << year;
    }

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    bool operator==(const Date& other) const {
        return day == other.day && month == other.month && year == other.year;
    }
};

enum Specialty { THERAPIST = 1, CARDIOLOGIST, DENTIST };
enum Qualification { HIGHEST = 0, FIRST, SECOND };

// Класс Doctor
class Doctor : public Comon {
private:
    Specialty specialty;
    Qualification qualification;
    static int count;

public:
    Doctor() : Comon(), specialty(THERAPIST), qualification(HIGHEST) { count++; }
    Doctor(string s, string n, Specialty sp, Qualification q)
        : Comon(s, n), specialty(sp), qualification(q) {
        count++;
    }

    static int getCount() { return count; }

    friend istream& operator>>(istream& in, Doctor& d) {
        string tempSurname, tempName;
        cout << "Enter surname: ";
        in >> tempSurname;
        if (!in.fail()) {
            d.setSurname(tempSurname);
        }
        cout << "Enter name: ";
        in >> tempName;
        if (!in.fail()) {
            d.setName(tempName);
        }
        d.specialty = static_cast<Specialty>(getValidatedInput(
            "Enter speciality (1-therapist, 2-cardiologist, 3-dentist): ", 1, 3));
        d.qualification = static_cast<Qualification>(getValidatedInput(
            "Enter qualification (0-highest, 1-first, 2-second): ", 0, 2));
        return in;
    }

    friend ostream& operator<<(ostream& out, const Doctor& d) {
        d.print();
        return out;
    }

    void print() const override {
        Comon::print();
        cout << setw(20) << specialtyToString(specialty)
            << setw(20) << qualificationToString(qualification);
    }

    Specialty getSpecialty() const { return specialty; }
    Qualification getQualification() const { return qualification; }

    static int getValidatedInput(const string& prompt, int min, int max) {
        int value;
        while (true) {
            try {
                cout << prompt;
                cin >> value;
                if (cin.fail() || value < min || value > max) {
                    throw invalid_argument("Error: incorrect number. Try again.");
                }
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }
            catch (const invalid_argument& e) {
                cout << e.what() << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        return value;
    }

    static string specialtyToString(Specialty s) {
        switch (s) {
        case THERAPIST: return "Therapist";
        case CARDIOLOGIST: return "Cardiologist";
        case DENTIST: return "Dentist";
        default: return "Unknown";
        }
    }

    static string qualificationToString(Qualification q) {
        switch (q) {
        case HIGHEST: return "Highest";
        case FIRST: return "First";
        case SECOND: return "Second";
        default: return "Unknown";
        }
    }
};

int Doctor::count = 0;

// Класс Reception
class Reception : public Comon {
private:
    Date date;
    int patientCount;

public:
    Reception() : Comon(), patientCount(0) {}
    Reception(string s, string n, Date d, int p) : Comon(s, n), date(d), patientCount(p) {}

    void readReception() {
        string tempSurname, tempName;
        cout << "Enter surname: ";
        cin >> tempSurname;
        setSurname(tempSurname);
        cout << "Enter name: ";
        cin >> tempName;
        setName(tempName);
        date.readDate();
        cout << "Enter number of patients: ";
        cin >> patientCount;
        if (cin.fail() || patientCount < 0) {
            cout << "Invalid number of patients." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    Date getDate() const { return date; }
    int getPatientCount() const { return patientCount; }

    void bookAppointment() {
        if (patientCount > 0) {
            patientCount--;
            // Закомментируем или удалим строку ниже, чтобы не выводить сообщение об успешной записи
            // cout << "Appointment booked successfully." << endl;
        }
        else {
            cout << "No available appointments for this day." << endl;
        }
    }

    void print() const override {
        Comon::print();
        cout << setw(20) << patientCount;
        date.printDate();
    }
};

// Функции для работы с файлами
Doctor* loadDoctors(const string& filename, int& size) {
    ifstream file(filename);
    Doctor* doctors = nullptr;
    size = 0;
    if (file.is_open()) {
        string surname, name;
        int spec, qual;
        while (file >> surname >> name >> spec >> qual) {
            Doctor* newDoctors = new Doctor[size + 1];
            for (int i = 0; i < size; ++i) {
                newDoctors[i] = doctors[i];
            }
            newDoctors[size] = Doctor(surname, name, static_cast<Specialty>(spec), static_cast<Qualification>(qual));
            delete[] doctors;
            doctors = newDoctors;
            ++size;
        }
        file.close();
    }
    return doctors;
}

Reception* loadReceptions(const string& filename, int& size) {
    ifstream file(filename);
    Reception* receptions = new Reception[100];
    size = 0;
    if (file.is_open()) {
        string surname, name;
        int patients, day, month, year;
        while (file >> surname >> name >> patients >> day >> month >> year) {
            Date date;
            date.setDate(day, month, year);
            receptions[size] = Reception(surname, name, date, patients);
            ++size;
        }
        file.close();
    }
    return receptions;
}

void saveDoctors(Doctor* doctors, int size, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open file for writing.\n";
        return;
    }
    for (int i = 0; i < size; ++i) {
        file << doctors[i].getSurname() << " "
            << doctors[i].getName() << " "
            << doctors[i].getSpecialty() << " "
            << doctors[i].getQualification() << endl;
    }
    file.close();
}

void saveReceptions(Reception* receptions, int size, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Unable to open reception file for writing.\n";
        return;
    }
    for (int i = 0; i < size; ++i) {
        file << receptions[i].getSurname() << " "
            << receptions[i].getName() << " "
            << receptions[i].getPatientCount() << " ";
        Date date = receptions[i].getDate();
        file << date.getDay() << " "
            << date.getMonth() << " "
            << date.getYear() << endl;
    }
    file.close();
}

void printDoctors(const Doctor* doctors, int docSize, const Reception* receptions, int recSize) {
    cout << left << setw(20) << "Surname" << setw(20) << "Name"
        << setw(20) << "Speciality" << setw(20) << "Qualification"
        << setw(20) << "Patients" << "Date" << endl;
    cout << string(120, '-') << endl;
    for (int i = 0; i < docSize; ++i) {
        bool hasReceptions = false;
        for (int j = 0; j < recSize; ++j) {
            if (doctors[i].getSurname() == receptions[j].getSurname() &&
                doctors[i].getName() == receptions[j].getName()) {
                doctors[i].print();
                cout << setw(20) << receptions[j].getPatientCount();
                receptions[j].getDate().printDate();
                cout << endl;
                hasReceptions = true;
            }
        }
        if (!hasReceptions) {
            doctors[i].print();
            cout << setw(20) << "-" << "-" << endl;
        }
    }
}

void bubbleSort(Doctor* doctors, int size) {
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - i - 1; ++j) {
            if (doctors[j].getSurname() > doctors[j + 1].getSurname()) {
                swap(doctors[j], doctors[j + 1]);
            }
        }
    }
}

void dialogInterface(Doctor*& MDoc, int& docSize, Reception*& MRec, int& recSize, const string& filename) {
    int choice;
    do {
        cout << "\nMenu:\n"
            << "1. Add new doctor\n"
            << "2. Show all doctors\n"
            << "3. Find the doctor by speciality and qualification\n"
            << "4. Sort doctors by surname\n"
            << "5. Find the doctor by name and surname\n"
            << "6. Enter reception data (first add doc if new)\n"
            << "7. Book an appointment\n"
            << "8. Clear all data\n"
            << "9. Exit\n"
            << "Choose action: ";
        cin >> choice;
        if (cin.fail()) {
            cout << "Error: incorrect input. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1: {
            Doctor newDoctor;
            cin >> newDoctor;
            Doctor* newMDoc = new Doctor[docSize + 1];
            for (int i = 0; i < docSize; ++i) {
                newMDoc[i] = MDoc[i];
            }
            newMDoc[docSize] = newDoctor;
            delete[] MDoc;
            MDoc = newMDoc;
            ++docSize;
            printDoctors(MDoc, docSize, MRec, recSize); // Print table after adding
            break;
        }
        case 2: {
            printDoctors(MDoc, docSize, MRec, recSize);
            break;
        }
        case 3: {
            int spec = Doctor::getValidatedInput(
                "Enter speciality (1-therapist, 2-cardiologist, 3-dentist): ", 1, 3);
            int qual = Doctor::getValidatedInput(
                "Enter qualification (0-highest, 1-first, 2-second): ", 0, 2);
            cout << left << setw(20) << "Surname" << setw(20) << "Name"
                << setw(20) << "Speciality" << setw(20) << "Qualification" << endl;
            cout << string(80, '-') << endl;
            for (int i = 0; i < docSize; ++i) {
                if (MDoc[i].getSpecialty() == static_cast<Specialty>(spec) &&
                    MDoc[i].getQualification() == static_cast<Qualification>(qual)) {
                    MDoc[i].print();
                    cout << endl;
                }
            }
            break;
        }
        case 4: {
            bubbleSort(MDoc, docSize);
            cout << "Doctors were sorted by surname.\n";
            printDoctors(MDoc, docSize, MRec, recSize); // Print table after sorting
            break;
        }
        case 5: {
            Comon find;
            string tempSurname, tempName;
            cout << "Enter surname for searching: ";
            cin >> tempSurname;
            find.setSurname(tempSurname);
            cout << "Enter name for searching: ";
            cin >> tempName;
            find.setName(tempName);
            bool found = false;
            for (int i = 0; i < docSize; ++i) {
                if (MDoc[i] == find) {
                    cout << "The doctor was found: ";
                    MDoc[i].print();
                    cout << endl;
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "The doctor was not found.\n";
            }
            break;
        }
        case 6: {
            if (recSize < 100) {
                Reception newReception;
                newReception.readReception();
                // Проверяем, существует ли такой доктор
                bool exists = false;
                for (int i = 0; i < docSize; i++) {
                    if (MDoc[i].getSurname() == newReception.getSurname() &&
                        MDoc[i].getName() == newReception.getName()) {
                        exists = true;
                        break;
                    }
                }
                if (!exists) {
                    cout << "The doctor was not mentioned before. Please, add him at 2\n";
                    break; // Возвращаемся в меню
                }
                // Если доктор существует, добавляем данные о приеме
                MRec[recSize] = newReception;
                recSize++;
                cout << "Added new reception data for existing doctor.\n";
                printDoctors(MDoc, docSize, MRec, recSize); // Print table after adding reception
            }
            else {
                cout << "Reception array is full\n";
            }
            break;
        }
        case 7: {
            string tempSurname, tempName;
            cout << "Enter surname for booking: ";
            cin >> tempSurname;
            cout << "Enter name for booking: ";
            cin >> tempName;
            Date bookingDate;
            bookingDate.readDate();
            bool found = false;
            for (int i = 0; i < recSize; ++i) {
                if (MRec[i].getSurname() == tempSurname && MRec[i].getName() == tempName && MRec[i].getDate() == bookingDate) {
                    MRec[i].bookAppointment();
                    found = true;
                    break;
                }
            }
            if (!found) {
                cout << "Reception not found for the given doctor on the specified date.\n";
            }
            break;
        }
        case 8: {
            delete[] MDoc;
            MDoc = nullptr;
            docSize = 0;
            delete[] MRec;
            MRec = new Reception[100];
            recSize = 0;
            ofstream docFile(filename, ios::trunc);
            docFile.close();
            ofstream recFile("receptions.csv", ios::trunc);
            recFile.close();
            cout << "All data has been cleared.\n";
            break;
        }
        case 9: {
            saveDoctors(MDoc, docSize, filename);
            saveReceptions(MRec, recSize, "receptions.csv");
            cout << "All data was saved. Exit from the program.\n";
            break;
        }
        default:
            cout << "Incorrect choice. Try again.\n";
        }
    } while (choice != 9);
}

int main() {
    Doctor* MDoc = nullptr;
    Reception* MRec = nullptr;
    int docSize = 0;
    int recSize = 0;
    string docFilename = "polyclinic.csv";
    string recFilename = "receptions.csv";
    MDoc = loadDoctors(docFilename, docSize);
    MRec = loadReceptions(recFilename, recSize);

    // Print table at the first launch
    printDoctors(MDoc, docSize, MRec, recSize);

    dialogInterface(MDoc, docSize, MRec, recSize, docFilename);

    delete[] MDoc;
    delete[] MRec;

    return 0;
}