# E-Learning Course Management System (C++ OOP)

A feature-rich, interactive terminal-based **E-Learning Management System** built with **C++ Object-Oriented Programming (OOP)** principles and automatic **File Handling persistence**.

---

## 🌟 Key Features

- **Multi-Role Access Control**: Separate interactive dashboards for **Students/Users**, **Teachers**, and **Administrators**.
- **Automatic File Handling on Startup**: Reads and loads existing system state from `.txt` files when the application starts up.
- **Persistent Storage on Change & Exit**: Automatically updates and saves all user, teacher, admin, course, chapter, and enrollment data back to text files.
- **Course & Chapter Hierarchy**: Courses contain modular chapters with titles, content summaries, and duration in minutes.
- **Student Progress Tracker**: Students can enroll in courses, read chapters, mark chapters as completed, and view calculated completion progress percentages.

---

## 🏗️ Object-Oriented Programming Architecture

1. **Inheritance & Polymorphism**:
   - `User` (Abstract Base Class with pure virtual methods like `getRole()`).
   - `Student` (Derived from `User` - handles enrollments and progress tracking).
   - `Teacher` (Derived from `User` - handles course creation and chapter management).
   - `Admin` (Derived from `User` - provides system-wide administration).
2. **Encapsulation & Abstraction**:
   - Private/Protected member variables with accessor methods (`getters`/`setters`).
3. **Composition & Associations**:
   - `Course` class encapsulates a collection (`std::vector<Chapter>`) of `Chapter` objects.
   - `Student` class manages dynamic `Enrollment` instances.

---

## 📁 File Storage Specifications

| Data Type | Target Text File | Format Structure |
| :--- | :--- | :--- |
| **Students / Users** | `users.txt` | `id\|name\|username\|password` |
| **Admins** | `admin.txt` | `id\|name\|username\|password` |
| **Teachers** | `teachers.txt` | `id\|name\|username\|password\|department` |
| **Courses & Chapters** | `course.txt` | `COURSE\|id\|title\|desc\|teacher\|count`<br>`CHAPTER\|id\|title\|summary\|duration` |
| **Enrollments** | `enrollments.txt` | `studentId\|courseId\|completedChapters` |

---

## 🔑 Default Credentials (Pre-seeded if files missing)

| Role | Username | Password | Details |
| :--- | :--- | :--- | :--- |
| **Admin** | `admin` | `admin123` | System Administrator |
| **Teacher** | `teacher1` | `pass123` | Instructor (Software Engineering) |
| **Teacher** | `teacher2` | `pass123` | Instructor (Computer Science) |
| **Student** | `student1` | `pass123` | Standard Enrolled Student |

---

## ⚙️ Compilation and Execution

### Using GCC / MinGW (Windows / Linux / macOS)

```bash
# Compile
g++ -std=c++17 -Wall main.cpp -o elearning.exe

# Run on Windows
.\elearning.exe

# Run on Linux / macOS
./elearning.exe
```

---

## 🎮 How to Use

1. Launch `elearning.exe`.
2. Select **Login** matching your role or register a new student account.
3. As a **Teacher/Admin**, create new courses and append chapters.
4. As a **Student**, browse available courses, enroll, and read/complete chapters to boost your progress %!
5. All progress and changes are saved automatically upon exit.
