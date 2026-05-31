#include <iostream>
#include <string>
#include <vector>
#include <fstream> 
#include <sstream> 

using namespace std;

class Task {
private:
    string id;
    string title;
    bool isCompleted;

public:
    Task(string id, string title) {
        this->id = id;
        this->title = title;
        this->isCompleted = false;
    }

    virtual ~Task() {} 

    virtual double getProgress() const = 0;
    virtual string getType() const = 0; 

    string getId() const { return id; }
    string getTitle() const { return title; }
    bool getIsCompleted() const { return isCompleted; }
    void setCompleted(bool completed) { isCompleted = completed; }
};

class SimpleTask : public Task {
public:
    SimpleTask(string id, string title) : Task(id, title) {}

    double getProgress() const override {
        return getIsCompleted() ? 100.0 : 0.0;
    }

    string getType() const override { return "SIMPLE"; }
};

class ProjectTask : public Task {
private:
    vector<string> subtasks;
    vector<bool> subtaskStatuses;

public:
    ProjectTask(string id, string title) : Task(id, title) {}

    void addSubtask(string subtask) {
        subtasks.push_back(subtask);
        subtaskStatuses.push_back(false);
    }

    void completeSubtask(int index) {
        if (index >= 0 && index < subtaskStatuses.size()) {
            subtaskStatuses[index] = true;
        }
    }

    double getProgress() const override {
        if (subtasks.empty()) {
            return getIsCompleted() ? 100.0 : 0.0;
        }
        int completedCount = 0;
        for (bool status : subtaskStatuses) {
            if (status) completedCount++;
        }
        return (static_cast<double>(completedCount) / subtasks.size()) * 100.0;
    }

    string getType() const override { return "PROJECT"; }
    int getSubtasksCount() const { return subtasks.size(); }
    string getSubtaskName(int i) const { return subtasks[i]; }
    bool getSubtaskStatus(int i) const { return subtaskStatuses[i]; }
};

class TaskManager {
private:
    vector<Task*> tasks;

public:
    ~TaskManager() {
        for (Task* t : tasks) {
            delete t;
        }
    }

    void addTask(Task* task) {
        tasks.push_back(task);
    }

    void printTasks() const {
        if (tasks.empty()) {
            cout << "\nНяма намерени задачи.\n";
            return;
        }
        cout << "\n--- СПИСЪК СЪС ЗАДАЧИ ---\n";
        for (const Task* task : tasks) {
            cout << "[" << task->getId() << "] " << task->getTitle() 
                 << " (" << task->getType() << ") | Прогрес: " << task->getProgress() << "%\n";
        }
    }

    void searchTasks(const string& keyword) const {
        bool found = false;
        cout << "\n--- РЕЗУЛТАТИ ОТ ТЪРСЕНЕТО: \"" << keyword << "\" ---\n";
        for (const Task* task : tasks) {
            if (task->getTitle().find(keyword) != string::npos || task->getId().find(keyword) != string::npos) {
                cout << "[" << task->getId() << "] " << task->getTitle() 
                     << " (" << task->getType() << ") | Прогрес: " << task->getProgress() << "%\n";
                found = true;
            }
        }
        if (!found) {
            cout << "Няма намерени задачи с тази ключова дума.\n";
        }
    }

    // ДОБАВЕН МЕТОД: Филтриране по статус (завършени / незавършени)
    void filterTasks(int filterType) const {
        bool found = false;
        string statusStr = (filterType == 1) ? "ЗАВЪРШЕНИ" : "НЕЗАВЪРШЕНИ";
        cout << "\n--- " << statusStr << " ЗАДАЧИ ---\n";

        for (const Task* task : tasks) {
            bool isDone = (task->getProgress() >= 100.0);
            
            if ((filterType == 1 && isDone) || (filterType == 2 && !isDone)) {
                cout << "[" << task->getId() << "] " << task->getTitle() 
                     << " (" << task->getType() << ") | Прогрес: " << task->getProgress() << "%\n";
                found = true;
            }
        }
        if (!found) {
            cout << "Няма намерени задачи в тази категория.\n";
        }
    }

    Task* findTaskById(string id) {
        for (Task* task : tasks) {
            if (task->getId() == id) {
                return task;
            }
        }
        return nullptr;
    }

    const vector<Task*>& getTasks() const { return tasks; } 
    
    void clearTasks() { 
        for (Task* t : tasks) delete t;
        tasks.clear(); 
    }

    void saveToFile(string filename) {
        ofstream outFile(filename);
        if (!outFile) {
            cout << "Грешка при отваряне на файла за запис!\n";
            return;
        }
        for (const Task* t : tasks) {
            outFile << t->getType() << "," << t->getId() << "," << t->getTitle() << "," << t->getIsCompleted();
            
            if (t->getType() == "PROJECT") {
                const ProjectTask* pt = static_cast<const ProjectTask*>(t);
                int count = pt->getSubtasksCount();
                outFile << "," << count;
                for (int i = 0; i < count; i++) {
                    outFile << "," << pt->getSubtaskName(i) << "," << pt->getSubtaskStatus(i);
                }
            }
            outFile << "\n";
        }
        outFile.close();
        cout << "Задачите бяха успешно записани във файл!\n";
    }

    void loadFromFile(string filename) {
        ifstream inFile(filename);
        if (!inFile) {
            cout << "Няма намерен съществуващ файл със задачи. Започвате на чисто.\n";
            return;
        }
        clearTasks(); 

        string line;
        while (getline(inFile, line)) {
            if (line.empty()) continue;
            
            stringstream ss(line);
            string type, id, title, isCompStr;
            
            getline(ss, type, ',');
            getline(ss, id, ',');
            getline(ss, title, ',');
            
            if (type == "SIMPLE") {
                getline(ss, isCompStr);
                SimpleTask* st = new SimpleTask(id, title);
                if (isCompStr == "1") st->setCompleted(true);
                addTask(st);
            } 
            else if (type == "PROJECT") {
                getline(ss, isCompStr, ',');
                ProjectTask* pt = new ProjectTask(id, title);
                if (isCompStr == "1") pt->setCompleted(true);
                
                string subCountStr;
                if (getline(ss, subCountStr, ',')) {
                    int subCount = stoi(subCountStr);
                    for (int i = 0; i < subCount; i++) {
                        string subName, subStatusStr;
                        getline(ss, subName, ',');
                        if (i == subCount - 1) {
                            getline(ss, subStatusStr);
                        } else {
                            getline(ss, subStatusStr, ',');
                        }
                        pt->addSubtask(subName);
                        if (subStatusStr == "1") {
                            pt->completeSubtask(i);
                        }
                    }
                }
                addTask(pt);
            }
        }
        inFile.close();
        cout << "Задачите бяха успешно заредени от файла!\n";
    }
};

int main() {
    TaskManager manager;
    
    manager.loadFromFile("tasks.txt");

    int choice;
    string id, title, subtaskName, keyword;

    while (true) {
        cout << "\n=== МЕНИДЖЪР НА ЗАДАЧИ ===\n";
        cout << "1. Добави обикновена задача (SimpleTask)\n";
        cout << "2. Добави проектна задача (ProjectTask)\n";
        cout << "3. Преглед на всички задачи\n";
        cout << "4. Добави подзадача към проект\n";
        cout << "5. Маркирай задача като завършена\n";
        cout << "6. Запиши промените във файл\n";
        cout << "7. Търсене на задача по ключова дума\n";
        cout << "8. Филтриране на задачи (Завършени / Незавършени)\n"; // Нова опция
        cout << "9. Изход\n"; // Променено от 8 на 9
        cout << "Избор: ";
        cin >> choice;

        if (choice == 9) { // Променено от 8 на 9
            manager.saveToFile("tasks.txt");
            break;
        }

        switch (choice) {
            case 1:
                cout << "Въведете ID (без интервали): ";
                cin >> id;
                cout << "Въведете Заглавие: ";
                cin.ignore();
                getline(cin, title);
                manager.addTask(new SimpleTask(id, title));
                break;

            case 2:
                cout << "Въведете ID (без интервали): ";
                cin >> id;
                cout << "Въведете Заглавие на проекта: ";
                cin.ignore();
                getline(cin, title);
                manager.addTask(new ProjectTask(id, title));
                break;

            case 3:
                manager.printTasks();
                break;

            case 4: {
                cout << "Въведете ID на проекта: ";
                cin >> id;
                Task* t = manager.findTaskById(id);
                
                if (t && t->getType() == "PROJECT") {
                    ProjectTask* pt = (ProjectTask*)t; 
                    cout << "Име на подзадачата: ";
                    cin.ignore();
                    getline(cin, subtaskName);
                    pt->addSubtask(subtaskName);
                    cout << "Подзадачата е добавена!\n";
                } else {
                    cout << "Проект с това ID не съществува.\n";
                }
                break;
            }

            case 5: {
                cout << "Въведете ID на задачата: ";
                cin >> id;
                Task* t = manager.findTaskById(id);
                if (!t) {
                    cout << "Задачата не е намерена.\n";
                    break;
                }

                if (t->getType() == "PROJECT") {
                    ProjectTask* pt = (ProjectTask*)t;
                    int subIndex;
                    cout << "Това е проект. Въведете индекс на подзадачата (0, 1, 2...): ";
                    cin >> subIndex;
                    pt->completeSubtask(subIndex);
                } else {
                    t->setCompleted(true);
                }
                cout << "Статусът е обновен успешно!\n";
                break;
            }

            case 6:
                manager.saveToFile("tasks.txt");
                break;

            case 7:
                cout << "Въведете ключова дума за търсене: ";
                cin.ignore();
                getline(cin, keyword);
                manager.searchTasks(keyword);
                break;

            case 8: { // Нов case за филтриране
                int filterChoice;
                cout << "Филтриране:\n1. Завършени задачи\n2. Незавършени задачи\nИзбор: ";
                cin >> filterChoice;
                if (filterChoice == 1 || filterChoice == 2) {
                    manager.filterTasks(filterChoice);
                } else {
                    cout << "Невалиден избор за филтриране.\n";
                }
                break;
            }

            default:
                cout << "Невалиден избор.\n";
        }
    }

    return 0;
}