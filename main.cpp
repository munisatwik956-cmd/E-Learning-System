#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>
#ifdef _WIN32
#include <conio.h>
#include <io.h>
#include <cstdio>
#else
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================
vector<string> splitString(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

string readLinePrompt(const string& prompt) {
    cout << prompt;
    string line;
    getline(cin, line);
    // Trim potential trailing \r if read on windows
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return line;
}

string readPasswordPrompt(const string& prompt) {
    cout << prompt;
    cout.flush();
    string pass;
#ifdef _WIN32
    if (!_isatty(_fileno(stdin))) {
        getline(cin, pass);
        if (!pass.empty() && pass.back() == '\r') {
            pass.pop_back();
        }
        return pass;
    }
    while (true) {
        int ch = _getch();
        if (ch == '\r' || ch == '\n') {
            break;
        } else if (ch == '\b' || ch == 127) {
            if (!pass.empty()) {
                pass.pop_back();
            }
        } else if (ch == 0 || ch == 224) {
            _getch(); // Skip extended key codes (arrows, function keys)
        } else if (ch == 3) {
            exit(0); // Ctrl+C
        } else if (ch >= 32 && ch <= 126) {
            // Silently store character (display nothing as in linux terminal)
            pass.push_back(static_cast<char>(ch));
        }
    }
    cout << "\n";
#else
    if (!isatty(STDIN_FILENO)) {
        getline(cin, pass);
        if (!pass.empty() && pass.back() == '\r') {
            pass.pop_back();
        }
        return pass;
    }
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    getline(cin, pass);
    if (!pass.empty() && pass.back() == '\r') {
        pass.pop_back();
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    cout << "\n";
#endif
    return pass;
}

// ============================================================================
// CHAPTER CLASS
// ============================================================================
class Chapter {
private:
    int chapterId;
    string title;
    string summary;
    int durationMinutes;

public:
    Chapter() : chapterId(0), title(""), summary(""), durationMinutes(0) {}
    Chapter(int id, const string& t, const string& s, int dur)
        : chapterId(id), title(t), summary(s), durationMinutes(dur) {}

    int getChapterId() const { return chapterId; }
    string getTitle() const { return title; }
    string getSummary() const { return summary; }
    int getDurationMinutes() const { return durationMinutes; }

    void display(int index = 0) const {
        if (index > 0) {
            cout << "  Chapter " << index << " [ID: " << chapterId << "]: " << title 
                 << " (" << durationMinutes << " mins)\n";
        } else {
            cout << "  Chapter ID: " << chapterId << " | Title: " << title 
                 << " | Duration: " << durationMinutes << " mins\n";
        }
        cout << "    Content: " << summary << "\n";
    }
};

// ============================================================================
// COURSE CLASS
// ============================================================================
class Course {
private:
    int courseId;
    string courseName;
    string description;
    string teacherName;
    vector<Chapter> chapters;

public:
    Course() : courseId(0), courseName(""), description(""), teacherName("") {}
    Course(int id, const string& name, const string& desc, const string& teacher)
        : courseId(id), courseName(name), description(desc), teacherName(teacher) {}

    int getCourseId() const { return courseId; }
    string getCourseName() const { return courseName; }
    string getDescription() const { return description; }
    string getTeacherName() const { return teacherName; }
    const vector<Chapter>& getChapters() const { return chapters; }
    vector<Chapter>& getChaptersRef() { return chapters; }

    void setTeacherName(const string& t) { teacherName = t; }

    void addChapter(const Chapter& ch) {
        chapters.push_back(ch);
    }

    int getChapterCount() const {
        return static_cast<int>(chapters.size());
    }

    void displaySummary() const {
        cout << "\n +---------------------------------------------------------+\n";
        cout << " | Course ID   : " << left << setw(41) << courseId << "|\n";
        cout << " | Title       : " << left << setw(41) << courseName << "|\n";
        cout << " | Instructor  : " << left << setw(41) << teacherName << "|\n";
        cout << " | Description : " << left << setw(41) << description << "|\n";
        cout << " | Total Chaps : " << left << setw(41) << chapters.size() << "|\n";
        cout << " +---------------------------------------------------------+\n";
    }

    void displayFullDetails() const {
        displaySummary();
        if (chapters.empty()) {
            cout << "   (No chapters added yet for this course)\n";
        } else {
            cout << "\n   === Course Chapters ===\n";
            for (size_t i = 0; i < chapters.size(); ++i) {
                chapters[i].display(static_cast<int>(i + 1));
                cout << "  ---------------------------------------------------------\n";
            }
        }
    }
};

// ============================================================================
// BASE USER CLASS (OOP Base Class with Pure Virtual / Polymorphism)
// ============================================================================
class User {
protected:
    int userId;
    string name;
    string username;
    string password;

public:
    User() : userId(0), name(""), username(""), password("") {}
    User(int id, const string& n, const string& u, const string& p)
        : userId(id), name(n), username(u), password(p) {}

    virtual ~User() {}

    int getUserId() const { return userId; }
    string getName() const { return name; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }

    bool checkPassword(const string& pass) const {
        return password == pass;
    }

    virtual string getRole() const = 0;

    virtual void displayInfo() const {
        cout << " ID       : " << userId << "\n";
        cout << " Name     : " << name << "\n";
        cout << " Username : " << username << "\n";
        cout << " Role     : " << getRole() << "\n";
    }
};

// ============================================================================
// ENROLLMENT CLASS
// ============================================================================
class Enrollment {
private:
    int studentId;
    int courseId;
    int completedChapters;

public:
    Enrollment() : studentId(0), courseId(0), completedChapters(0) {}
    Enrollment(int sId, int cId, int completed = 0)
        : studentId(sId), courseId(cId), completedChapters(completed) {}

    int getStudentId() const { return studentId; }
    int getCourseId() const { return courseId; }
    int getCompletedChapters() const { return completedChapters; }

    void completeChapter() { completedChapters++; }
    void setCompletedChapters(int count) { completedChapters = count; }
};

// ============================================================================
// STUDENT / USER CLASS (Derived from User)
// ============================================================================
class Student : public User {
private:
    vector<Enrollment> enrollments;

public:
    Student() : User() {}
    Student(int id, const string& n, const string& u, const string& p)
        : User(id, n, u, p) {}

    string getRole() const override { return "Student"; }

    const vector<Enrollment>& getEnrollments() const { return enrollments; }
    vector<Enrollment>& getEnrollmentsRef() { return enrollments; }

    bool isEnrolled(int courseId) const {
        for (const auto& e : enrollments) {
            if (e.getCourseId() == courseId) return true;
        }
        return false;
    }

    void enrollInCourse(int courseId, int completed = 0) {
        if (!isEnrolled(courseId)) {
            enrollments.push_back(Enrollment(userId, courseId, completed));
        }
    }

    Enrollment* getEnrollmentPtr(int courseId) {
        for (auto& e : enrollments) {
            if (e.getCourseId() == courseId) return &e;
        }
        return nullptr;
    }
};

// ============================================================================
// TEACHER CLASS (Derived from User)
// ============================================================================
class Teacher : public User {
private:
    string department;

public:
    Teacher() : User(), department("General") {}
    Teacher(int id, const string& n, const string& u, const string& p, const string& dept = "Computer Science")
        : User(id, n, u, p), department(dept) {}

    string getRole() const override { return "Teacher"; }
    string getDepartment() const { return department; }

    void displayInfo() const override {
        User::displayInfo();
        cout << " Department: " << department << "\n";
    }
};

// ============================================================================
// ADMIN CLASS (Derived from User)
// ============================================================================
class Admin : public User {
public:
    Admin() : User() {}
    Admin(int id, const string& n, const string& u, const string& p)
        : User(id, n, u, p) {}

    string getRole() const override { return "Admin"; }
};

// ============================================================================
// E-LEARNING SYSTEM MANAGEMENT & FILE HANDLING ENGINE
// ============================================================================
class ElearningSystem {
private:
    vector<Student> students;
    vector<Teacher> teachers;
    vector<Admin> admins;
    vector<Course> courses;

    // Strict .txt Filenames
    const string USER_FILE = "users.txt";
    const string ADMIN_FILE = "admin.txt";
    const string TEACHER_FILE = "teachers.txt";
    const string COURSE_FILE = "course.txt";
    const string ENROLLMENT_FILE = "enrollments.txt";

public:
    ElearningSystem() {
        // Load data from .txt files at the start of the program
        loadDataFromFiles();
    }

    ~ElearningSystem() {
        // Automatically save on system exit
        saveDataToFiles();
    }

    // ========================================================================
    // FILE HANDLING - LOADING AT START OF PROGRAM
    // ========================================================================
    void loadDataFromFiles() {
        cout << "[System Setup] Loading data from .txt files...\n";

        loadAdmins();
        loadTeachers();
        loadStudents();
        loadCourses();
        loadEnrollments();

        // Seed initial default records if files were missing/empty
        seedInitialDataIfEmpty();

        cout << "[System Setup] Data loaded successfully!\n";
        cout << "               - Admins   : " << admins.size() << "\n";
        cout << "               - Teachers : " << teachers.size() << "\n";
        cout << "               - Students : " << students.size() << "\n";
        cout << "               - Courses  : " << courses.size() << "\n";
    }

    void loadAdmins() {
        admins.clear();
        ifstream file(ADMIN_FILE);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            auto parts = splitString(line, '|');
            if (parts.size() >= 4) {
                int id = stoi(parts[0]);
                admins.push_back(Admin(id, parts[1], parts[2], parts[3]));
            }
        }
        file.close();
    }

    void loadTeachers() {
        teachers.clear();
        ifstream file(TEACHER_FILE);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            auto parts = splitString(line, '|');
            if (parts.size() >= 4) {
                int id = stoi(parts[0]);
                string dept = (parts.size() >= 5) ? parts[4] : "Computer Science";
                teachers.push_back(Teacher(id, parts[1], parts[2], parts[3], dept));
            }
        }
        file.close();
    }

    void loadStudents() {
        students.clear();
        ifstream file(USER_FILE);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            auto parts = splitString(line, '|');
            if (parts.size() >= 4) {
                int id = stoi(parts[0]);
                students.push_back(Student(id, parts[1], parts[2], parts[3]));
            }
        }
        file.close();
    }

    void loadCourses() {
        courses.clear();
        ifstream file(COURSE_FILE);
        if (!file.is_open()) return;

        string line;
        Course* currentCourse = nullptr;

        while (getline(file, line)) {
            if (line.empty()) continue;
            auto parts = splitString(line, '|');
            if (parts.empty()) continue;

            if (parts[0] == "COURSE" && parts.size() >= 5) {
                int cId = stoi(parts[1]);
                courses.push_back(Course(cId, parts[2], parts[3], parts[4]));
                currentCourse = &courses.back();
            } else if (parts[0] == "CHAPTER" && currentCourse != nullptr && parts.size() >= 5) {
                int chId = stoi(parts[1]);
                int duration = stoi(parts[4]);
                currentCourse->addChapter(Chapter(chId, parts[2], parts[3], duration));
            }
        }
        file.close();
    }

    void loadEnrollments() {
        ifstream file(ENROLLMENT_FILE);
        if (!file.is_open()) return;

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            auto parts = splitString(line, '|');
            if (parts.size() >= 3) {
                int sId = stoi(parts[0]);
                int cId = stoi(parts[1]);
                int comp = stoi(parts[2]);

                Student* st = findStudentById(sId);
                if (st) {
                    st->enrollInCourse(cId, comp);
                }
            }
        }
        file.close();
    }

    // ========================================================================
    // FILE HANDLING - SAVING BACK TO .TXT FILES
    // ========================================================================
    void saveDataToFiles() const {
        saveAdmins();
        saveTeachers();
        saveStudents();
        saveCourses();
        saveEnrollments();
    }

    void saveAdmins() const {
        ofstream file(ADMIN_FILE);
        if (!file.is_open()) return;
        for (const auto& a : admins) {
            file << a.getUserId() << "|" << a.getName() << "|" 
                 << a.getUsername() << "|" << a.getPassword() << "\n";
        }
        file.close();
    }

    void saveTeachers() const {
        ofstream file(TEACHER_FILE);
        if (!file.is_open()) return;
        for (const auto& t : teachers) {
            file << t.getUserId() << "|" << t.getName() << "|" 
                 << t.getUsername() << "|" << t.getPassword() << "|" 
                 << t.getDepartment() << "\n";
        }
        file.close();
    }

    void saveStudents() const {
        ofstream file(USER_FILE);
        if (!file.is_open()) return;
        
        for (const auto& s : students) {
            file << s.getUserId() << "|" << s.getName() << "|" 
                 << s.getUsername() << "|" << s.getPassword() << "\n";
        }
        file.close();
    }

    void saveCourses() const {
        ofstream file(COURSE_FILE);
        if (!file.is_open()) return;

        for (const auto& c : courses) {
            file << "COURSE|" << c.getCourseId() << "|" 
                 << c.getCourseName() << "|" << c.getDescription() << "|" 
                 << c.getTeacherName() << "|" << c.getChapterCount() << "\n";

            for (const auto& ch : c.getChapters()) {
                file << "CHAPTER|" << ch.getChapterId() << "|" 
                     << ch.getTitle() << "|" << ch.getSummary() << "|" 
                     << ch.getDurationMinutes() << "\n";
            }
        }
        file.close();
    }

    void saveEnrollments() const {
        ofstream file(ENROLLMENT_FILE);
        if (!file.is_open()) return;

        for (const auto& s : students) {
            for (const auto& e : s.getEnrollments()) {
                file << e.getStudentId() << "|" << e.getCourseId() << "|" 
                     << e.getCompletedChapters() << "\n";
            }
        }
        file.close();
    }

    // ========================================================================
    // INITIAL DEFAULT SEEDING IF FILES DID NOT EXIST
    // ========================================================================
    void seedInitialDataIfEmpty() {
        if (admins.empty()) {
            admins.push_back(Admin(1, "System Admin", "admin", "admin123"));
        }
        if (teachers.empty()) {
            teachers.push_back(Teacher(101, "Prof. Robert Martin", "teacher1", "pass123", "Software Engineering"));
            teachers.push_back(Teacher(102, "Dr. Grace Hopper", "teacher2", "pass123", "Computer Science"));
        }
        if (students.empty()) {
            students.push_back(Student(201, "Alice Smith", "student1", "pass123"));
            students.push_back(Student(202, "Bob Jones", "student2", "pass123"));
        }
        if (courses.empty()) {
            Course c1(1001, "C++ Object Oriented Programming", 
                      "Comprehensive guide to Classes, Inheritance, Polymorphism & File I/O.", 
                      "Prof. Robert Martin");
            c1.addChapter(Chapter(1, "Introduction to OOP Concepts", "Overview of Classes, Objects, Abstraction & Encapsulation.", 25));
            c1.addChapter(Chapter(2, "Constructors & Destructors", "Memory lifecycle, constructors, destructors and copy semantics.", 30));
            c1.addChapter(Chapter(3, "Inheritance & Dynamic Polymorphism", "Base/Derived classes, virtual functions, and vtables.", 40));
            c1.addChapter(Chapter(4, "C++ File Handling & Streams", "Reading and writing structured data using fstream.", 35));

            Course c2(1002, "Data Structures & Algorithms", 
                      "Master Arrays, Linked Lists, Trees, Graphs & Algorithm Analysis.", 
                      "Dr. Grace Hopper");
            c2.addChapter(Chapter(1, "Arrays & Pointers", "Contiguous memory allocation and pointer arithmetic.", 20));
            c2.addChapter(Chapter(2, "Dynamic Linked Lists", "Singly & Doubly Linked Lists implementation.", 35));
            c2.addChapter(Chapter(3, "Binary Search Trees", "Tree traversals, insertion, and deletion algorithms.", 45));

            courses.push_back(c1);
            courses.push_back(c2);

            students[0].enrollInCourse(1001, 2);
            students[1].enrollInCourse(1002, 1);
        }
        saveDataToFiles();
    }

    // ========================================================================
    // FINDER UTILITIES
    // ========================================================================
    Student* findStudentById(int id) {
        for (auto& s : students) {
            if (s.getUserId() == id) return &s;
        }
        return nullptr;
    }

    Course* findCourseById(int id) {
        for (auto& c : courses) {
            if (c.getCourseId() == id) return &c;
        }
        return nullptr;
    }

    Teacher* findTeacherByUsername(const string& uname) {
        for (auto& t : teachers) {
            if (t.getUsername() == uname) return &t;
        }
        return nullptr;
    }

    // ========================================================================
    // LOGIN AUTHENTICATION
    // ========================================================================
    Student* loginStudent(const string& uname, const string& pass) {
        for (auto& s : students) {
            if (s.getUsername() == uname && s.checkPassword(pass)) {
                return &s;
            }
        }
        return nullptr;
    }

    Teacher* loginTeacher(const string& uname, const string& pass) {
        for (auto& t : teachers) {
            if (t.getUsername() == uname && t.checkPassword(pass)) {
                return &t;
            }
        }
        return nullptr;
    }

    Admin* loginAdmin(const string& uname, const string& pass) {
        for (auto& a : admins) {
            if (a.getUsername() == uname && a.checkPassword(pass)) {
                return &a;
            }
        }
        return nullptr;
    }

    // ========================================================================
    // PUBLIC DISPLAY & REGISTRATION UTILITIES
    // ========================================================================
    void browseAllCourses() const {
        if (courses.empty()) {
            cout << "\n[Notice] No courses are currently available.\n";
            return;
        }
        cout << "\n=========================================================\n";
        cout << "                  AVAILABLE COURSES LIST                 \n";
        cout << "=========================================================\n";
        for (const auto& c : courses) {
            c.displaySummary();
        }
    }

    void registerNewStudent() {
        cout << "\n--- STUDENT REGISTRATION ---\n";
        string name = readLinePrompt(" Enter Full Name: ");
        string uname = readLinePrompt(" Enter Username: ");

        // Check unique username
        for (const auto& s : students) {
            if (s.getUsername() == uname) {
                cout << "\n[Error] Username already taken! Please choose another.\n";
                return;
            }
        }
        string pass = readPasswordPrompt(" Enter Password: ");

        int newId = 201;
        if (!students.empty()) {
            newId = students.back().getUserId() + 1;
        }

        students.push_back(Student(newId, name, uname, pass));
        saveDataToFiles();

        cout << "\n[Success] Registration successful!\n";
        cout << "          Your Assigned Student ID: " << newId << "\n";
        cout << "          Data saved to users.txt!\n";
    }

    // ========================================================================
    // STUDENT ROLE MENUS
    // ========================================================================
    void studentMenu(Student* student) {
        int choice = 0;
        do {
            cout << "\n=========================================================\n";
            cout << "              STUDENT DASHBOARD | " << student->getName() << "\n";
            cout << "=========================================================\n";
            cout << " 1. Browse All Courses\n";
            cout << " 2. View Course Details & Chapters\n";
            cout << " 3. Enroll in a Course\n";
            cout << " 4. View My Enrolled Courses & Progress\n";
            cout << " 5. Read / Complete Chapter\n";
            cout << " 6. Logout\n";
            cout << "---------------------------------------------------------\n";
            cout << " Select Option (1-6): ";
            
            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case 1:
                browseAllCourses();
                break;
            case 2: {
                cout << "\nEnter Course ID to view: ";
                int cId;
                if (cin >> cId) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    Course* c = findCourseById(cId);
                    if (c) c->displayFullDetails();
                    else cout << "\n[Error] Course not found!\n";
                }
                break;
            }
            case 3: {
                browseAllCourses();
                cout << "\nEnter Course ID to enroll in: ";
                int cId;
                if (cin >> cId) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    Course* c = findCourseById(cId);
                    if (!c) {
                        cout << "\n[Error] Course ID does not exist!\n";
                    } else if (student->isEnrolled(cId)) {
                        cout << "\n[Notice] You are already enrolled in this course!\n";
                    } else {
                        student->enrollInCourse(cId, 0);
                        saveDataToFiles();
                        cout << "\n[Success] Enrolled in '" << c->getCourseName() << "' successfully!\n";
                    }
                }
                break;
            }
            case 4: {
                const auto& enrs = student->getEnrollments();
                if (enrs.empty()) {
                    cout << "\n[Notice] You have not enrolled in any courses yet.\n";
                } else {
                    cout << "\n=========================================================\n";
                    cout << "                 MY ENROLLED COURSES                     \n";
                    cout << "=========================================================\n";
                    for (const auto& e : enrs) {
                        Course* c = findCourseById(e.getCourseId());
                        if (c) {
                            int totalCh = c->getChapterCount();
                            int compCh = e.getCompletedChapters();
                            float pct = (totalCh > 0) ? ((float)compCh / totalCh * 100.0f) : 0.0f;

                            cout << " Course ID   : " << c->getCourseId() << "\n";
                            cout << " Title       : " << c->getCourseName() << "\n";
                            cout << " Instructor  : " << c->getTeacherName() << "\n";
                            cout << " Progress    : " << compCh << " / " << totalCh << " chapters completed ("
                                 << fixed << setprecision(1) << pct << "%)\n";
                            cout << "---------------------------------------------------------\n";
                        }
                    }
                }
                break;
            }
            case 5: {
                cout << "\nEnter Course ID to study: ";
                int cId;
                if (cin >> cId) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    if (!student->isEnrolled(cId)) {
                        cout << "\n[Error] You must be enrolled in this course first!\n";
                        break;
                    }
                    Course* c = findCourseById(cId);
                    if (!c) break;

                    c->displayFullDetails();
                    const auto& chapters = c->getChapters();
                    if (chapters.empty()) break;

                    Enrollment* enr = student->getEnrollmentPtr(cId);
                    cout << "\nEnter Chapter Number to read/complete (1-" << chapters.size() << "): ";
                    int chNum;
                    if (cin >> chNum && chNum >= 1 && chNum <= static_cast<int>(chapters.size())) {
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        const auto& targetChapter = chapters[chNum - 1];

                        cout << "\n=========================================================\n";
                        cout << " READING: " << targetChapter.getTitle() << "\n";
                        cout << "=========================================================\n";
                        cout << targetChapter.getSummary() << "\n";
                        cout << "=========================================================\n";

                        cout << "\nDo you want to mark this chapter as completed? (y/n): ";
                        string mark;
                        getline(cin, mark);
                        if (mark == "y" || mark == "Y") {
                            if (chNum > enr->getCompletedChapters()) {
                                enr->setCompletedChapters(chNum);
                                saveDataToFiles();
                                cout << "\n[Success] Progress updated! Completed " << chNum 
                                     << " of " << chapters.size() << " chapters.\n";
                            } else {
                                cout << "\n[Notice] Chapter already completed previously.\n";
                            }
                        }
                    } else {
                        cout << "\n[Error] Invalid chapter selection.\n";
                    }
                }
                break;
            }
            case 6:
                cout << "\nLogging out...\n";
                break;
            default:
                cout << "\n[Error] Invalid option! Try again.\n";
            }
        } while (choice != 6);
    }

    // ========================================================================
    // TEACHER ROLE MENUS
    // ========================================================================
    void teacherMenu(Teacher* teacher) {
        int choice = 0;
        do {
            cout << "\n=========================================================\n";
            cout << "             TEACHER DASHBOARD | " << teacher->getName() << "\n";
            cout << "=========================================================\n";
            cout << " 1. View My Assigned Courses\n";
            cout << " 2. Create a New Course\n";
            cout << " 3. Add Chapter to Course\n";
            cout << " 4. View Course Chapters & Content\n";
            cout << " 5. View Enrolled Students for My Courses\n";
            cout << " 6. Logout\n";
            cout << "---------------------------------------------------------\n";
            cout << " Select Option (1-6): ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case 1: {
                bool found = false;
                cout << "\n--- MY ASSIGNED COURSES ---\n";
                for (const auto& c : courses) {
                    if (c.getTeacherName() == teacher->getName() || 
                        c.getTeacherName() == teacher->getUsername()) {
                        c.displaySummary();
                        found = true;
                    }
                }
                if (!found) cout << " (No courses currently assigned to you)\n";
                break;
            }
            case 2: {
                cout << "\n--- CREATE NEW COURSE ---\n";
                string name = readLinePrompt(" Enter Course Name: ");
                string desc = readLinePrompt(" Enter Course Description: ");

                int newId = 1001;
                if (!courses.empty()) {
                    newId = courses.back().getCourseId() + 1;
                }

                courses.push_back(Course(newId, name, desc, teacher->getName()));
                saveDataToFiles();
                cout << "\n[Success] Course created! Assigned ID: " << newId << "\n";
                cout << "          Saved to course.txt!\n";
                break;
            }
            case 3: {
                cout << "\nEnter Course ID to add chapter: ";
                int cId;
                if (cin >> cId) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    Course* c = findCourseById(cId);
                    if (!c) {
                        cout << "\n[Error] Course ID not found!\n";
                        break;
                    }
                    string title = readLinePrompt(" Enter Chapter Title: ");
                    string summary = readLinePrompt(" Enter Chapter Content / Summary: ");
                    cout << " Enter Duration in Minutes: ";
                    int dur = 30;
                    if (cin >> dur) {
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }

                    int chId = c->getChapterCount() + 1;
                    c->addChapter(Chapter(chId, title, summary, dur));
                    saveDataToFiles();
                    cout << "\n[Success] Chapter " << chId << " added to course '" 
                         << c->getCourseName() << "'!\n";
                }
                break;
            }
            case 4: {
                cout << "\nEnter Course ID: ";
                int cId;
                if (cin >> cId) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    Course* c = findCourseById(cId);
                    if (c) c->displayFullDetails();
                    else cout << "\n[Error] Course not found!\n";
                }
                break;
            }
            case 5: {
                cout << "\n--- ENROLLED STUDENTS IN YOUR COURSES ---\n";
                for (const auto& c : courses) {
                    if (c.getTeacherName() == teacher->getName() || 
                        c.getTeacherName() == teacher->getUsername()) {
                        cout << "\nCourse: " << c.getCourseName() << " [ID: " << c.getCourseId() << "]\n";
                        bool enrolledFound = false;
                        for (const auto& s : students) {
                            if (s.isEnrolled(c.getCourseId())) {
                                cout << "   - Student: " << s.getName() << " (ID: " << s.getUserId() << ")\n";
                                enrolledFound = true;
                            }
                        }
                        if (!enrolledFound) cout << "   - (No students enrolled yet)\n";
                    }
                }
                break;
            }
            case 6:
                cout << "\nLogging out...\n";
                break;
            default:
                cout << "\n[Error] Invalid option! Try again.\n";
            }
        } while (choice != 6);
    }

    // ========================================================================
    // ADMIN ROLE MENUS
    // ========================================================================
    void adminMenu(Admin* admin) {
        int choice = 0;
        do {
            cout << "\n=========================================================\n";
            cout << "              ADMINISTRATOR CONTROL PANEL                \n";
            cout << "=========================================================\n";
            cout << " 1. View System Summary & Statistics\n";
            cout << " 2. View All Registered Users (Admins, Teachers, Students)\n";
            cout << " 3. Register New Teacher (save to teachers.txt)\n";
            cout << " 4. Register New Admin (save to admin.txt)\n";
            cout << " 5. Register New Student (save to users.txt)\n";
            cout << " 6. Create New Course (save to course.txt)\n";
            cout << " 7. Add Chapter to Any Course\n";
            cout << " 8. View All Courses & Chapters\n";
            cout << " 9. Logout\n";
            cout << "---------------------------------------------------------\n";
            cout << " Select Option (1-9): ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            switch (choice) {
            case 1:
                cout << "\n=========================================================\n";
                cout << "                 SYSTEM DATABASE STATS                   \n";
                cout << "=========================================================\n";
                cout << " Admins Count   : " << admins.size() << " (file: admin.txt)\n";
                cout << " Teachers Count : " << teachers.size() << " (file: teachers.txt)\n";
                cout << " Students Count : " << students.size() << " (file: users.txt)\n";
                cout << " Courses Count  : " << courses.size() << " (file: course.txt)\n";
                cout << "=========================================================\n";
                break;
            case 2:
                cout << "\n=== ADMINS ===\n";
                for (const auto& a : admins) a.displayInfo();

                cout << "\n=== TEACHERS ===\n";
                for (const auto& t : teachers) t.displayInfo();

                cout << "\n=== STUDENTS / USERS ===\n";
                for (const auto& s : students) s.displayInfo();
                break;
            case 3: {
                cout << "\n--- ADD NEW TEACHER ---\n";
                string name = readLinePrompt(" Enter Full Name: ");
                string uname = readLinePrompt(" Enter Username: ");
                string pass = readPasswordPrompt(" Enter Password: ");
                string dept = readLinePrompt(" Enter Department: ");

                int newId = 101;
                if (!teachers.empty()) newId = teachers.back().getUserId() + 1;

                teachers.push_back(Teacher(newId, name, uname, pass, dept));
                saveDataToFiles();
                cout << "\n[Success] Teacher added and saved to teachers.txt!\n";
                break;
            }
            case 4: {
                cout << "\n--- ADD NEW ADMIN ---\n";
                string name = readLinePrompt(" Enter Full Name: ");
                string uname = readLinePrompt(" Enter Username: ");
                string pass = readPasswordPrompt(" Enter Password: ");

                int newId = 1;
                if (!admins.empty()) newId = admins.back().getUserId() + 1;

                admins.push_back(Admin(newId, name, uname, pass));
                saveDataToFiles();
                cout << "\n[Success] Admin added and saved to admin.txt!\n";
                break;
            }
            case 5:
                registerNewStudent();
                break;
            case 6: {
                cout << "\n--- CREATE NEW COURSE ---\n";
                string name = readLinePrompt(" Enter Course Title: ");
                string desc = readLinePrompt(" Enter Course Description: ");
                string teacherName = readLinePrompt(" Enter Instructor Name: ");

                int newId = 1001;
                if (!courses.empty()) newId = courses.back().getCourseId() + 1;

                courses.push_back(Course(newId, name, desc, teacherName));
                saveDataToFiles();
                cout << "\n[Success] Course created and saved to course.txt!\n";
                break;
            }
            case 7: {
                cout << "\nEnter Course ID: ";
                int cId;
                if (cin >> cId) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    Course* c = findCourseById(cId);
                    if (!c) {
                        cout << "\n[Error] Course not found!\n";
                        break;
                    }
                    string title = readLinePrompt(" Enter Chapter Title: ");
                    string summary = readLinePrompt(" Enter Chapter Summary/Content: ");
                    cout << " Enter Duration in Minutes: ";
                    int dur = 30;
                    if (cin >> dur) cin.ignore(numeric_limits<streamsize>::max(), '\n');

                    int chId = c->getChapterCount() + 1;
                    c->addChapter(Chapter(chId, title, summary, dur));
                    saveDataToFiles();
                    cout << "\n[Success] Chapter added and saved to course.txt!\n";
                }
                break;
            }
            case 8:
                for (const auto& c : courses) c.displayFullDetails();
                break;
            case 9:
                cout << "\nLogging out of Admin portal...\n";
                break;
            default:
                cout << "\n[Error] Invalid option! Try again.\n";
            }
        } while (choice != 9);
    }
};

// ============================================================================
// MAIN APPLICATION ENTRY POINT
// ============================================================================
int main() {
    cout << "=========================================================\n";
    cout << "        E-LEARNING COURSE MANAGEMENT SYSTEM (C++ OOP)    \n";
    cout << "=========================================================\n";

    // Initialize System Engine (Loads data from users.txt, admin.txt, teachers.txt, course.txt)
    ElearningSystem system;

    int mainChoice = 0;
    do {
        cout << "\n=========================================================\n";
        cout << "                    MAIN LOGIN MENU                      \n";
        cout << "=========================================================\n";
        cout << " 1. Student / User Login\n";
        cout << " 2. Teacher Login\n";
        cout << " 3. Admin Login\n";
        cout << " 4. New Student Registration\n";
        cout << " 5. Browse All Courses (Guest Access)\n";
        cout << " 6. Exit Program\n";
        cout << "---------------------------------------------------------\n";
        cout << " Enter Choice (1-6): ";

        if (!(cin >> mainChoice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (mainChoice) {
        case 1: {
            string uname = readLinePrompt("\n Enter Student Username: ");
            string pass = readPasswordPrompt(" Enter Password: ");
            Student* student = system.loginStudent(uname, pass);
            if (student) {
                cout << "\n[Success] Welcome back, " << student->getName() << "!\n";
                system.studentMenu(student);
            } else {
                cout << "\n[Error] Invalid student credentials!\n";
            }
            break;
        }
        case 2: {
            string uname = readLinePrompt("\n Enter Teacher Username: ");
            string pass = readPasswordPrompt(" Enter Password: ");
            Teacher* teacher = system.loginTeacher(uname, pass);
            if (teacher) {
                cout << "\n[Success] Welcome, Instructor " << teacher->getName() << "!\n";
                system.teacherMenu(teacher);
            } else {
                cout << "\n[Error] Invalid teacher credentials!\n";
            }
            break;
        }
        case 3: {
            string uname = readLinePrompt("\n Enter Admin Username: ");
            string pass = readPasswordPrompt(" Enter Password: ");
            Admin* admin = system.loginAdmin(uname, pass);
            if (admin) {
                cout << "\n[Success] Admin authentication granted. Welcome, " << admin->getName() << "!\n";
                system.adminMenu(admin);
            } else {
                cout << "\n[Error] Invalid admin credentials!\n";
            }
            break;
        }
        case 4:
            system.registerNewStudent();
            break;
        case 5:
            system.browseAllCourses();
            break;
        case 6:
            cout << "\nSaving all data to .txt files and closing system...\n";
            break;
        default:
            cout << "\n[Error] Invalid choice! Please select 1-6.\n";
        }
    } while (mainChoice != 6);

    cout << "\n[System] Thank you for using the E-Learning System. Good luck!\n";
    return 0;
}
