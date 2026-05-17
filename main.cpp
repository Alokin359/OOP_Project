#include <iostream>
#include <string>
#include <vector>
#include <fstream> 

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

    virtual ~Task() {} // Виртуален деструктор

    virtual double getProgress() const = 0;
    virtual string getType() const = 0; // Ще ни трябва за файла по-късно

    string getId() const { return id; }
    string getTitle() const { return title; }
    bool getIsCompleted() const { return isCompleted; }
    void setCompleted(bool completed) { isCompleted = completed; }
};

int main() {
    cout << "Commit 1: Base Task class created." << endl;
    return 0;
}