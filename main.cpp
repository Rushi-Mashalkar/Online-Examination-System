#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

class Question {
    string questionText;
    vector<string> options;
    char correctAnswer;

public:
    Question(string q, vector<string> opts, char ans) {
        questionText = q;
        options = opts;
        correctAnswer = ans;
    }

    void display() {
        cout << questionText << endl;
        for (int i = 0; i < options.size(); i++) {
            cout << char('A' + i) << ") " << options[i] << endl;
        }
    }

    bool checkAnswer(char userAnswer) {
        return toupper(userAnswer) == correctAnswer;
    }

    string toFileFormat() const {
        string format = questionText + "\n";
        for (const auto& option : options) {
            format += option + "\n";
        }
        format += correctAnswer;
        return format;
    }
};

class User {
    public:
    string name;
    string rollNo;
    string division;
    int score;

    User() : name(""), rollNo(""), division(""), score(0) {}

    User(string n, string r, string d) : name(n), rollNo(r), division(d), score(0) {}
    
    string getName() const {
        return name;
    }

    string getRollNo() const {
        return rollNo;
    }

    string getDivision() const {
        return division;
    }

    void addScore(int points) {
        score += points;
    }

    int getScore() const {
        return score;
    }

    string toFileFormat() const {
        return name + "," + rollNo + "," + division + "," + to_string(score);
    }
};

class Exam {
    vector<Question> questions;
    unordered_map<string, User> users;

public:
    void addQuestion(Question q) {
        questions.push_back(q);
    }

    void registerUser(const string& name, const string& rollNo, const string& division) {
        users.emplace(name, User(name, rollNo, division));
    }

    void takeExam(const string& userName) {
        if (users.find(userName) == users.end()) {
            cout << "User not found!" << endl;
            return;
        }

        User& user = users[userName];

        for (int i = 0; i < questions.size(); i++) {
            cout << "Question " << i + 1 << ": " << endl;
            questions[i].display();
            char answer;
            cout << "Enter your answer (A/B/C/D): ";
            cin >> answer;

            if (questions[i].checkAnswer(answer)) {
                cout << "Correct!" << endl;
                user.addScore(10);
            } else {
                cout << "Incorrect!" << endl;
            }
            cout << endl;
        }

        cout << user.getName() << ", your total score is: " << user.getScore() << endl;
    }

    void saveUserDataToFile(const string& filename) {
        ofstream file(filename);
        if (file.is_open()) {
            for (const auto& entry : users) {
                const User& user = entry.second;
                file << user.toFileFormat() << endl;
            }
            file.close();
        } else {
            cerr << "Unable to open file for writing user data." << endl;
        }
    }

    void saveQuestionDataToFile(const string& filename) {
        ofstream file(filename);
        if (file.is_open()) {
            for (const auto& question : questions) {
                file << question.toFileFormat() << endl;
            }
            file.close();
        } else {
            cerr << "Unable to open file for writing question data." << endl;
        }
    }

    void loadUserDataFromFile(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                string name = line.substr(0, pos1);
                string rollNo = line.substr(pos1 + 1, pos2 - pos1 - 1);
                string division = line.substr(pos2 + 1, line.rfind(',') - pos2 - 1);
                int score = stoi(line.substr(line.rfind(',') + 1));
                users.emplace(name, User(name, rollNo, division));
                users[name].addScore(score);
            }
            file.close();
        } else {
            cerr << "Unable to open file for reading user data." << endl;
        }
    }

    void loadQuestionDataFromFile(const string& filename) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                string questionText = line;
                vector<string> options(4);
                for (int i = 0; i < 4; ++i) {
                    getline(file, options[i]);
                }
                char correctAnswer;
                file >> correctAnswer;
                file.ignore(); 
                addQuestion(Question(questionText, options, toupper(correctAnswer)));
            }
            file.close();
        } else {
            cerr << "Unable to open file for reading question data." << endl;
        }
    }

    const unordered_map<string, User>& getUsers() const {
        return users;
    }
};

int main() {
    Exam exam;

    // Load existing data from files
    exam.loadUserDataFromFile("users.txt");
    exam.loadQuestionDataFromFile("questions.txt");

    // Static user registration
    // exam.registerUser("Alice", "001", "A");
    // exam.registerUser("Bob", "002", "B");

    int numUsers;
    cout << "Enter the number of users to register: ";
    cin >> numUsers;
    cin.ignore(); 
    
    for (int i = 0; i < numUsers; i++) {
        string userName, rollNo, division;
        cout << "Enter name for user " << i + 1 << ": ";
        getline(cin, userName);
        cout << "Enter roll number for user " << i + 1 << ": ";
        getline(cin, rollNo);
        cout << "Enter division for user " << i + 1 << ": ";
        getline(cin, division);
        exam.registerUser(userName, rollNo, division);
    }

    exam.addQuestion(Question("Which of the following allows a class to have multiple forms of behavior?", 
    {"Encapsulation", "Polymorphism", "Abstraction", "Composition"}, 'B'));
    
    int numQuestions;
    cout << "Enter the number of questions to add: ";
    cin >> numQuestions;
    cin.ignore(); 
    
    for (int i = 0; i < numQuestions; i++) {
        string questionText;
        vector<string> options(4);
        char correctAnswer;

        cout << "Enter question " << i + 1 << ": ";
        getline(cin, questionText);

        cout << "Enter 4 options (A/B/C/D):\n";
        for (int j = 0; j < 4; j++) {
            cout << char('A' + j) << ") ";
            getline(cin, options[j]);
        }

        cout << "Enter the correct answer (A/B/C/D): ";
        cin >> correctAnswer;
        cin.ignore();  // Clear newline after input

        exam.addQuestion(Question(questionText, options, toupper(correctAnswer)));
    }

    for (const auto& entry : exam.getUsers()) {
        exam.takeExam(entry.first);
    }

    // Save user data to the file system
    exam.saveUserDataToFile("users.txt");
    exam.saveQuestionDataToFile("questions.txt");

    return 0;
}
