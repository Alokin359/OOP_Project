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


int main() {
    
    return 0;
}