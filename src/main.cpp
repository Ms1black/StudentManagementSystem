#include <iostream>
#include <fstream>
#include <unistd.h> 
#include <termios.h> 
#include <cstring>
#include <cstdlib> 
#include <sys/ioctl.h>
#include <thread>
#include <chrono>


struct Student {
    
    int id ;
    char lastName[30] = "";
    char firstName[30] = "";
    char patronymic[30] = "";
    int birthYear = 0;
    int course = 0;
    int marks[5]{0};
}; 


//прототипы функции для работы с меню
int runMenu(const char* options[], int count);
int getch();
void clearScreen();

//прототипы функции для корректного ввода
void getValidMarks(int marks[], int size);
int getValidYear();
char* intToString(int number);

//прототипы функции для вывода таблицы в консоли
int getlength(const char* word);
void PrintTable(const Student people[], int count);

//прототипы функций для работы со студентами
void AddStudentManual(Student people[], int &count);
void ChangeStudentInfo(Student people[], int count);
void DeleteStudent(Student people[], int &count);
void SortedAvgMarks(const Student people[], int count);

//прототипы для работы с файлами
void SaveToTextFile(const Student people[], int count) ;
void LoadFromTextFile(Student people[], int &count);
void SaveToBinaryFile(const Student people[], int count);
void LoadFromBinaryFile(Student people[], int &count);


int main() {

    const int maxCount = 100;
    Student people[maxCount];
    int count = 0;

    const char* mainMenuOptions[] = {
        "Заполнить таблицу студентов",
        "Редактировать таблицу студентов",
        "Просмотреть/сохранить таблицу",
        "Выйти из программы"
    };
    
    const char* fillTableOptions[] = {
        "Заполнить вручную",
        "Загрузить из текстового файла",
        "Загрузить из бинарного файла"
    };
    
    const char* editTableOptions[] = {
        "Изменить информацию о студенте",
        "Удалить студента"
    };
    
    const char* viewSaveTableOptions[] = {
        "Отсортировать по среднему баллу",
        "Просмотреть таблицу",
        "Сохранить в текстовый файл",
        "Сохранить в бинарный файл"
    };
    
    
    const int mainMenuCount = sizeof(mainMenuOptions) / sizeof(mainMenuOptions[0]);
    const int fillTableCount = sizeof(fillTableOptions) / sizeof(fillTableOptions[0]);
    const int editTableCount = sizeof(editTableOptions) / sizeof(editTableOptions[0]);
    const int viewSaveTableCount = sizeof(viewSaveTableOptions) / sizeof(viewSaveTableOptions[0]);

    while (true) {

        int mainChoice = runMenu(mainMenuOptions, mainMenuCount);

        if (mainChoice == mainMenuCount - 1) { 
            clearScreen();
            std::cout << "\033[1;37mУспешно. До новых встреч! :)\033[0m\n";
            break;
        }

        switch (mainChoice) {
            case 0: { 
                int subChoice = runMenu(fillTableOptions, fillTableCount);
                switch (subChoice) {
                    case 0:
                        clearScreen();
                        AddStudentManual(people, count);
                        break;
                    case 1:
                        clearScreen();
                        LoadFromTextFile(people, count);
                        break;
                    case 2:
                        clearScreen();
                        LoadFromBinaryFile(people, count);
                        break;
                }
                break;
            }
            case 1: { 
                int subChoice = runMenu(editTableOptions, editTableCount);
                switch (subChoice) {
                    case 0:
                        clearScreen();
                        PrintTable(people, count);
                        ChangeStudentInfo(people, count);
                        break;
                    case 1:
                        clearScreen();
                        PrintTable(people, count);
                        DeleteStudent(people, count);
                        break;
                }
                break;
            }
            case 2: { 
                int subChoice = runMenu(viewSaveTableOptions, viewSaveTableCount);
                switch (subChoice) {
                    case 0:
                        clearScreen();
                        SortedAvgMarks(people, count);
                        break;
                    case 1:
                        clearScreen();
                        PrintTable(people, count);
                        break;
                    case 2:
                        clearScreen();
                        SaveToTextFile(people, count);
                        break;
                    case 3:
                        clearScreen();
                        SaveToBinaryFile(people, count);
                        break;
                }
                break;
            }
        }

        std::cout << "\nНажмите любую кнопочку для продолжения...";
        getch();
    }

    return 0;
}

int runMenu(const char* options[], int count) {

    int selected = 0;

    while (true) {
        clearScreen();
        std::cout << "\033[1;37mДобро пожаловать в систему управления студентами\033[0m\n\n";

        for (int i = 0; i < count; ++i) {
            if (i == selected) {
                std::cout << "\033[1;32m> " << options[i] << "\033[0m\n";
            } else {
                std::cout << "  " << options[i] << "\n";
            }
        }

        int ch = getch();
        switch (ch) {
            case 65: 
            case 119: 
                if (selected > 0) {
                    selected--;
                }
                break;
            case 66: 
            case 115:
                if (selected < count - 1) {
                    selected++;
                }
                break;
            case 10: 
                return selected; 
        }
    }
}

int getch() {

    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void clearScreen() {

    std::cout << "\033[H\033[J"; 
}

void getValidMarks(int marks[], int size) {
    while (true) {
        std::cout << "Введите " << size << " оценок (через пробел, от 0 до 5): ";
        char input[100];
        std::cin.getline(input, sizeof(input));

        int index = 0;
        char* token = strtok(input, " ");

        bool hasError = false;

        while (token != nullptr) {
            if (index >= size) {
                hasError = true;
                break;
            }

            // Проверка на начальный пробел
            if (token[0] == ' ') {
                std::cout << "\nОшибка: \n\033[1;33mПеред цифрами не должно быть пробелов. Повторите ввод...\033[0m\n";
                hasError = true;
                break;
            }

            // Преобразование строки в число
            char* endPtr;
            int mark = std::strtol(token, &endPtr, 10);

            if (*endPtr != '\0' || mark < 0 || mark > 5) {
                std::cout << "\nОшибка: \n\033[1;33mОценка должна быть числом от 0 до 5. Повторите ввод...\033[0m\n";
                hasError = true;
                break;
            }

            // Запись оценки
            marks[index++] = mark;

            // Переход к следующему токену
            token = strtok(nullptr, " ");
        }

        // Проверка на количество оценок
        if (!hasError && index != size) {
            std::cout << "\nОшибка: \n\033[1;33mВведено неправильное количество оценок. Повторите ввод...\033[0m\n";
            hasError = true;
        }

        // Если ошибок нет, завершаем ввод
        if (!hasError) {
            return;
        }
    }
}

int getValidYear() {

    int year;
    while (true) {
        std::cin >> year;

        if (std::cin.fail() || std::cin.peek() != '\n') {
            std::cin.clear(); 
            while (std::cin.get() != '\n'); 
            std::cout << "\nОшибка: \n\033[1;33mвведите год рождения цифрой...\033[0m\n";
            continue; 
        }

        if (year >= 1924 && year <= 2010) {
            return year;    
        } else {

            std::cout << "\nОшибка: \n\033[1;33mВведите корректный год...\033[0m\n ";
        }
    }
}

char* intToString(int number) {

    static char buffer[20]; 
    sprintf(buffer, "%d", number);
    return buffer;
}

int getlength(const char* word) {

    int pos = 0;
    int length = 0;
    while (word[pos] != '\0') {
        if (word[pos] > 0) { 
            length += 1;
            pos += 1;
        } else { 
            length += 1;
            pos += 2;
        }
    }
    return length;
}

char* textalign(const char* word, int align, int size) {

    int length = getlength(word);
    char* aligned = new char[strlen(word) + size - length + 1];
    aligned[0] = '\0'; 

switch (align) {
    case 0: 
        strcpy(aligned, word);
        for (int i = 0; i < (size - length); i++) {
            strcat(aligned, " ");
        }
        break;
    case 1: 
        for (int i = 0; i < (size - length) / 2; i++) {
            strcat(aligned, " ");
        }
        strcat(aligned, word);
        for (int i = 0; i < (size - length) / 2 + (size - length) % 2; i++) {
            strcat(aligned, " ");
        }
        break;
    case 2: 
        for (int i = 0; i < (size - length); i++) {
            strcat(aligned, " ");
        }
        strcat(aligned, word);
        break;
    }
    return aligned;
}

void PrintLine() {

    std::cout << "\033[1;32m+~~~~~~+~~~~~~~~~~~~~~~+~~~~~~~~~~~~~~~+~~~~~~~~~~~~~~~~~~~~+~~~~~~~~~+~~~~~~~~~+~~~~~~~~~~~+\033[0m\n";
}

void PrintTable(const Student people[], int count) {

    const int idSize = 5;
    const int lastNameSize = 14;
    const int firstNameSize = 14;
    const int patronymicSize = 19;
    const int birthYearSize = 8;
    const int courseSize = 8;
    const int marksSize = 9;

    PrintLine();
    std::cout << "\033[1;32m| " << textalign("№ ", 1, idSize)
              << "\033[1;32m| " << textalign("Фамилия", 1, lastNameSize)
              << "\033[1;32m| " << textalign("Имя", 1, firstNameSize)
              << "\033[1;32m| " << textalign("Отчество", 1, patronymicSize)
              << "\033[1;32m| " << textalign("Год", 1, birthYearSize)
              << "\033[1;32m| " << textalign("Курс", 1, courseSize)
              << "\033[1;32m| " << textalign("Оценки", 1, marksSize) << " |\033[0m\n";
    PrintLine();

    for (int i = 0; i < count; ++i) {
        std::cout << "\033[1;32m|\033[0m " << textalign(intToString(people[i].id), 0, idSize)
                  << "\033[1;32m|\033[0m " << textalign(people[i].lastName, 0, lastNameSize)
                  << "\033[1;32m|\033[0m " << textalign(people[i].firstName, 0, firstNameSize)
                  << "\033[1;32m|\033[0m " << textalign(people[i].patronymic, 0, patronymicSize)
                  << "\033[1;32m|\033[0m " << textalign(intToString(people[i].birthYear), 2, birthYearSize)
                  << "\033[1;32m|\033[0m " << textalign(intToString(people[i].course), 2, courseSize)
                  << "\033[1;32m|\033[0m ";

        for (int j = 0; j < 5; ++j) {
            std::cout << people[i].marks[j] << (j < 4 ? " " : "");
        }

        std::cout << " \033[1;32m|\033[0m\n";
        PrintLine();
    }
}

void AddStudentManual(Student people[], int &count) {

    if (count >= 100) {
        std::cout << "\n\033[1;33mНевозможно добавить больше студентов.\033[0m\n";
        return;
    }

    Student newStudent;
    newStudent.id = count + 1; 

    clearScreen();
    PrintTable(&newStudent, 1);
    std::cout << "Введите фамилию: ";
    std::cin >> newStudent.lastName;

    clearScreen();
    PrintTable(&newStudent, 1);
    std::cout << "Введите имя: ";
    std::cin >> newStudent.firstName;

    clearScreen();
    PrintTable(&newStudent, 1);
    std::cout << "Введите отчество: ";
    std::cin >> newStudent.patronymic;

    clearScreen();
    PrintTable(&newStudent, 1);
    std::cout << "Введите год рождения: ";
    newStudent.birthYear = getValidYear(); 

    clearScreen();
    PrintTable(&newStudent, 1);
    std::cout << "Введите курс: ";
    while (!(std::cin >> newStudent.course) || (std::cin.peek() != '\n')) {
        std::cin.clear();
        while (std::cin.get() != '\n');
        clearScreen();
        PrintTable(&newStudent, 1);
        std::cout << "\nОшибка: \n\033[1;33mвведите цифру курса цифрой...\033[0m\n";
        std::cout << "Введите курс: ";
    }

    clearScreen();
    PrintTable(&newStudent, 1);
     while (true) {
        std::cout << "Введите 5 оценок через пробел: ";
        std::cin.getline(input, sizeof(input));

        if (getValidMarks(input, marks, size)) {
            break;  // Выход из цикла при корректном вводе
        }
    }

    people[count] = newStudent;
    count++;

    char continueChoice;
    std::cout << "\nДобавить еще одного студента? (+/-): ";
    std::cin >> continueChoice;

    if (continueChoice == '+' || continueChoice == '=') {
        AddStudentManual(people, count);
    }
}

void ChangeStudentInfo(Student people[], int count) {

    if (count == 0) {
        std::cout << "Ошибка: \n\033[1;33mНет студентов для изменения...\033[0m\n";
        return;
    }

    int id;
    std::cout << "Введите ID студента для изменения:";
    std::cin >> id;

    int index = -1;
    for (int i = 0; i < count; ++i) {
        if (people[i].id == id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        std::cout << "Ошибка: \n\033[1;33mСтудент с таким ID не найден.\033[0m\n";
        return;
    }

    const char* menuOptions[] = {
        "Изменить фамилию",
        "Изменить имя",
        "Изменить отчество",
        "Изменить год рождения",
        "Изменить курс",
        "Изменить оценки",
        "Выйти"
    };
    const int menuCount = sizeof(menuOptions) / sizeof(menuOptions[0]);

    int selected = -1; 
    int choice = 0;    

    while (true) {

        clearScreen(); 

        std::cout << "Студент: " << people[index].lastName << " " << people[index].firstName << " " << people[index].patronymic << "\n\n";

        for (int i = 0; i < menuCount; ++i) {
            if (i == selected) {
                std::cout << "\033[1;32m> " << menuOptions[i] << "\033[0m\n"; 
            } else {
                std::cout << "  " << menuOptions[i] << "\n";
            }
        }

        int ch = getch();
        switch (ch) {
            case 65:
                if (selected > 0) {
                    selected--;
                }
                break;
            case 66: 
                if (selected < menuCount - 1) {
                    selected++;
                }
                break;
            case 10: 
                choice = selected + 1; 
                break;
        }

        if (choice != 0) {
            clearScreen();

            switch (choice) {
                case 1: 
                    std::cout << "Текущая фамилия: " << people[index].lastName << "\n";
                    std::cout << "Введите новую фамилию: ";
                    std::cin >> people[index].lastName;
                    break;
                case 2: 
                    std::cout << "Текущее имя: " << people[index].firstName << "\n";
                    std::cout << "Введите новое имя: ";
                    std::cin >> people[index].firstName;
                    break;
                case 3: 
                    std::cout << "Текущее отчество: " << people[index].patronymic << "\n";
                    std::cout << "Введите новое отчество: ";
                    std::cin >> people[index].patronymic;
                    break;
                case 4: 
                    std::cout << "Текущий год рождения: " << people[index].birthYear << "\n";
                    std::cout << "Введите новый год рождения: ";
                    people[index].birthYear = getValidYear(); 
                    break;
                case 5: 
                    std::cout << "Текущий курс: " << people[index].course << "\n";
                    std::cout << "Введите новый курс: ";
                    while (!(std::cin >> people[index].course) || (std::cin.peek() != '\n')) {
                        std::cin.clear();
                        while (std::cin.get() != '\n');
                        clearScreen();
                        std::cout << "\n\033[1;33mОшибка: \n введите цифру курса правильно...\033[0m\n";
                        std::cout << "Введите курс: ";
                    }
                    break;
                case 6: 
                    std::cout << "Текущие оценки: ";
                    for (int i = 0; i < 5; ++i) {
                        std::cout << people[index].marks[i] << " ";
                    }
                    std::cout << "\nВведите новые 5 оценок: ";
                    getValidMarks(people[index].marks, 5); 
                    break;
                case 7: 
                    std::cout << "Изменения сохранены.\n";
                    return;
                default:
                    std::cout << "Неверный выбор, попробуйте снова.\n";
            }

            std::cout << "\nНажмите любую кнопочку...";
            getch(); 
            choice = 0; 
        }
    }
}

void DeleteStudent(Student people[], int &count) {
    if (count == 0) {
        std::cout << "Ошибка: \n\033[1;33mНет студентов для удаления. Пожалуйста подождите...\033[0m\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    int id;
    std::cout << "Введите ID студента для удаления: ";
    std::cin >> id;

    if (id <= 0 || id > count) {
        std::cout << "\nОшибка: \n\033[1;33mСтудент с таким ID не найден. Пожалуйста подождите...\033[0m\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return;
    }

    int index = -1;
    for (int i = 0; i < count; ++i) {
        if (people[i].id == id) {
            index = i;
            break;
        }
    }

    for (int i = index; i < count - 1; ++i) {
        people[i] = people[i + 1];
        people[i].id = i + 1; 
    }
    --count;

    std::cout << "Студент успешно удален. Пожалуйста подождите...\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));

}

void SortedAvgMarks(const Student people[], int count) {
    Student peopleCopy[100];

    for (int i = 0; i < count; ++i) {
        std::strcpy(peopleCopy[i].lastName, people[i].lastName);
        std::strcpy(peopleCopy[i].firstName, people[i].firstName);
        std::strcpy(peopleCopy[i].patronymic, people[i].patronymic);
        peopleCopy[i].birthYear = people[i].birthYear;
        peopleCopy[i].course = people[i].course;
        peopleCopy[i].id = people[i].id; 
        for (int j = 0; j < 5; ++j) {
            peopleCopy[i].marks[j] = people[i].marks[j];
        }
    }

    for (int i = 0; i < count - 1; ++i) {
        for (int j = 0; j < count - 1 - i; ++j) {
            double avgMarks1 = (peopleCopy[j].marks[0] + peopleCopy[j].marks[1] + peopleCopy[j].marks[2] + peopleCopy[j].marks[3] + peopleCopy[j].marks[4]) / 5.0;
            double avgMarks2 = (peopleCopy[j + 1].marks[0] + peopleCopy[j + 1].marks[1] + peopleCopy[j + 1].marks[2] + peopleCopy[j + 1].marks[3] + peopleCopy[j + 1].marks[4]) / 5.0;

            if (avgMarks1 < avgMarks2) {
                Student temp = peopleCopy[j];
                peopleCopy[j] = peopleCopy[j + 1];
                peopleCopy[j + 1] = temp;
            }
        }
    }

    std::cout << "Студенты, отсортированные по среднему баллу:\n";
    std::cout << "--------------------------------------------\n";
    PrintTable(peopleCopy, count);
}

void SaveToTextFile(const Student people[], int count) {

    std::ofstream fout("../data/students.txt");
    if (!fout) {
        std::cout << "Ошибка при открытии файла для записи.\n";
        return;
    }

    for (int i = 0; i < count; ++i) {
        fout << people[i].id << " "
             << people[i].lastName << " "
             << people[i].firstName << " "
             << people[i].patronymic << " "
             << people[i].birthYear << " "
             << people[i].course << " ";
        for (int j = 0; j < 5; ++j) {
            fout << people[i].marks[j] << (j < 4 ? " " : "\n");
        }
    }
    fout.close();
    std::cout << "Данные успешно сохранены в текстовый файл.\n";
}


void LoadFromTextFile(Student people[], int &count) {

    std::ifstream fin("../data/students.txt");
    if (!fin) {
        std::cout << "\033[1;33mОшибка при открытии файла для чтения.\033[0m\n";
        return;
    }

    count = 0;
    while (fin >> people[count].id
               >> people[count].lastName
               >> people[count].firstName
               >> people[count].patronymic
               >> people[count].birthYear
               >> people[count].course) {
        for (int j = 0; j < 5; ++j) {
            fin >> people[count].marks[j];
        }
        ++count;
    }
    fin.close();
    std::cout << "Данные успешно загружены из текстового файла.\n";
}


void SaveToBinaryFile(const Student people[], int count) {

    std::ofstream fout("../data/students.bin", std::ios::binary);
    if (!fout) {
        std::cout << "\033[1;33mОшибка при открытии файла для записи.\033[0m\n";
        return;
    }

    fout.write(reinterpret_cast<const char*>(people), count * sizeof(Student));
    fout.close();
    std::cout << "Данные успешно сохранены в бинарный файл.\n";
}


void LoadFromBinaryFile(Student people[], int &count) {

    std::ifstream fin("../data/students.bin", std::ios::binary);
    if (!fin) {
        std::cout << "\033[1;33mОшибка при открытии файла для чтения.\033[0m\n";
        return;
    }

    count = 0;
    while (fin.read(reinterpret_cast<char*>(&people[count]), sizeof(Student))) {
        ++count;
    }
    fin.close();
    std::cout << "Данные успешно загружены из бинарного файла.\n";
}
