#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <limits>

struct Ticket {
    char* fam;      // Фамилия пассажира
    char* name;     // Имя пассажира
    char* arrival;  // Пункт прибытия
    double price;   // Цена билета
};

Ticket create_ticket(const char* fam, const char* name, const char* arrival, double price) {
    Ticket ticket;
    ticket.fam = strdup(fam);
    ticket.name = strdup(name);
    ticket.arrival = strdup(arrival);
    ticket.price = price;
    return ticket;
}

void destroy_ticket(Ticket& ticket) {
    delete[] ticket.fam;
    delete[] ticket.name;
    delete[] ticket.arrival;
}

void add_ticket(Ticket*& tickets, int& count) {
    Ticket* new_tickets = new Ticket[count + 1];
    for (int i = 0; i < count; ++i) {
        new_tickets[i] = tickets[i];
    }
    delete[] tickets;
    tickets = new_tickets;

    char fam[50], name[50], arrival[100];
    double price;

    std::cout << "Enter surname: ";
    std::cin >> fam;
    std::cout << "Enter name: ";
    std::cin >> name;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера ввода
    std::cout << "Enter arrival point: ";
    std::cin.getline(arrival, 100);
    std::cout << "Enter price: ";
    std::cin >> price;

    tickets[count] = create_ticket(fam, name, arrival, price);
    ++count;
}

void print_tickets(const Ticket* tickets, int count) {
    std::cout << std::left << std::setw(20) << "Surname"
              << std::setw(20) << "Name"
              << std::setw(20) << "Arrival"
              << std::setw(10) << "Price" << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;
    for (int i = 0; i < count; ++i) {
        std::cout << std::left << std::setw(20) << tickets[i].fam
                  << std::setw(20) << tickets[i].name
                  << std::setw(20) << tickets[i].arrival
                  << std::setw(10) << tickets[i].price << std::endl;
    }
}

Ticket* find_ticket_by_name(Ticket* tickets, int count, const char* fam, const char* name) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(tickets[i].fam, fam) == 0 && strcmp(tickets[i].name, name) == 0) {
            return &tickets[i];
        }
    }
    return nullptr;
}

void find_tickets_by_price(const Ticket* tickets, int count, double min_price) {
    for (int i = 0; i < count; ++i) {
        if (tickets[i].price > min_price) {
            std::cout << "Ticket found: " << tickets[i].fam << " " << tickets[i].name << " to " << tickets[i].arrival << ", Price: " << tickets[i].price << std::endl;
        }
    }
}

void save_tickets_to_file(const Ticket* tickets, int count, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing" << std::endl;
        return;
    }

    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (int i = 0; i < count; ++i) {
        int len = strlen(tickets[i].fam) + 1;
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(tickets[i].fam, len);

        len = strlen(tickets[i].name) + 1;
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(tickets[i].name, len);

        len = strlen(tickets[i].arrival) + 1;
        file.write(reinterpret_cast<const char*>(&len), sizeof(len));
        file.write(tickets[i].arrival, len);

        file.write(reinterpret_cast<const char*>(&tickets[i].price), sizeof(tickets[i].price));
    }
}

Ticket* load_tickets_from_file(int& count, const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        count = 0;
        return nullptr;
    }

    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    Ticket* tickets = new Ticket[count];
    for (int i = 0; i < count; ++i) {
        int len;

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        tickets[i].fam = new char[len];
        file.read(tickets[i].fam, len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        tickets[i].name = new char[len];
        file.read(tickets[i].name, len);

        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        tickets[i].arrival = new char[len];
        file.read(tickets[i].arrival, len);

        file.read(reinterpret_cast<char*>(&tickets[i].price), sizeof(tickets[i].price));
    }
    return tickets;
}

int main() {
    Ticket* tickets = nullptr;
    int count = 0;
    const std::string filename = "tickets.dat";

    // Загрузка данных с файла, если он существует
    tickets = load_tickets_from_file(count, filename);

    int choice;
    do {
        std::cout << "\nMenu:\n";
        std::cout << "1. Add new ticket\n";
        std::cout << "2. Print all tickets\n";
        std::cout << "3. Find ticket by name\n";
        std::cout << "4. Find tickets by price\n";
        std::cout << "5. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear(); // Очистка флага ошибки
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Очистка буфера ввода
            std::cout << "Invalid input. Please enter a number between 1 and 5.\n";
            continue;
        }

        switch (choice) {
            case 1:
                add_ticket(tickets, count);
                break;
            case 2:
                print_tickets(tickets, count);
                break;
            case 3: {
                char fam[50], name[50];
                std::cout << "Enter surname: ";
                std::cin >> fam;
                std::cout << "Enter name: ";
                std::cin >> name;
                Ticket* ticket = find_ticket_by_name(tickets, count, fam, name);
                if (ticket != nullptr) {
                    std::cout << "Ticket found: " << ticket->fam << " " << ticket->name << " to " << ticket->arrival << ", Price: " << ticket->price << std::endl;
                } else {
                    std::cout << "Ticket not found\n";
                }
                break;
            }
            case 4: {
                double min_price;
                std::cout << "Enter minimum price: ";
                std::cin >> min_price;
                find_tickets_by_price(tickets, count, min_price);
                break;
            }
            case 5:
                save_tickets_to_file(tickets, count, filename);
                std::cout << "Data saved. Exiting...\n";
                break;
            default:
                std::cout << "Invalid choice. Please enter a number between 1 and 5.\n";
        }
    } while (choice != 5);

    // Освобождение памяти
    for (int i = 0; i < count; ++i) {
        destroy_ticket(tickets[i]);
    }
    delete[] tickets;

    return 0;