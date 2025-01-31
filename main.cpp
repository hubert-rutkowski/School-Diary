#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Choice.H>
#include <iostream>
#include <stdexcept>
#include "SchoolDiary.h"
#include <ctime>
#include <sstream>
#include <X11/X.h>
#undef None
#undef Bool
#include <gtest/gtest.h>

SchoolDiary diary;

Fl_Window* studentWindow;
Fl_Window* teacherWindow;
Fl_Window* parentWindow;
Fl_Window* adminWindow;
Fl_Window* loginWindow;

static User* currentUser = nullptr;
static Fl_Box* studentWelcomeBox = nullptr;
static Fl_Box* teacherWelcomeBox = nullptr;
static Fl_Box* parentWelcomeBox = nullptr;

std::string safeInputValue(Fl_Input* input) {
    const char* val = input->value();
    return val ? val : "";
}

std::string getCurrentDate() {
    std::time_t t = std::time(nullptr);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%d.%m.%Y", std::localtime(&t));
    return std::string(buffer);
}

void logout_cb(Fl_Widget* widget, void* data) {
    Fl_Window* window = (Fl_Window*)data;
    window->hide();
    loginWindow->show();
}

void login_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    std::string username = safeInputValue(inputs[0]);
    std::string password = safeInputValue(inputs[1]);

    if (username == "admin" && password == "admin") {
        std::cout << "Logged in as Admin" << std::endl;
        adminWindow->show();
        loginWindow->hide();
        currentUser = nullptr;
    } else {
        User* user = nullptr;
        if ((user = diary.findStudentByUsername(username)) && user->getPassword() == password) {
            Student* s = dynamic_cast<Student*>(user);
            studentWindow->show();
            loginWindow->hide();
            currentUser = s;
            if (studentWelcomeBox) {
                std::string labelStr = "Welcome " + s->name + " " + s->surname;
                studentWelcomeBox->copy_label(labelStr.c_str());
            }
        } else if ((user = diary.findTeacherByUsername(username)) && user->getPassword() == password) {
            Teacher* t = dynamic_cast<Teacher*>(user);
            teacherWindow->show();
            loginWindow->hide();
            currentUser = t;
            if (teacherWelcomeBox) {
                std::string labelStr = "Welcome " + t->name + " " + t->surname;
                teacherWelcomeBox->copy_label(labelStr.c_str());
            }
        } else if ((user = diary.findParentByUsername(username)) && user->getPassword() == password) {
            Parent* p = dynamic_cast<Parent*>(user);
            parentWindow->show();
            loginWindow->hide();
            currentUser = p;
            if (parentWelcomeBox) {
                std::string labelStr = "Welcome " + p->getUsername();
                parentWelcomeBox->copy_label(labelStr.c_str());
            }
        } else {
            std::cout << "Login failed" << std::endl;
        }
    }
}

// Wyświetlanie ocen
void viewGrades_cb(Fl_Widget* widget, void* data) {
    if (currentUser && currentUser->getRole() == "Student") {
        Student* student = dynamic_cast<Student*>(currentUser);
        Fl_Window* gradesWindow = new Fl_Window(400, 300, "Grades");
        Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
        Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
        display->buffer(textbuf);
        textbuf->text(student->getGradesInfo().c_str());
        gradesWindow->resizable(display);
        gradesWindow->end();
        gradesWindow->show();
    } else {
        std::cout << "Viewing grades..." << std::endl;
    }
}

// Wyświetlanie notatek
void viewNotes_cb(Fl_Widget* widget, void* data) {
    if (currentUser && currentUser->getRole() == "Student") {
        Student* student = dynamic_cast<Student*>(currentUser);
        Fl_Window* notesWindow = new Fl_Window(400, 300, "Notes");
        Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
        Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
        display->buffer(textbuf);
        textbuf->text(student->getNotesInfo().c_str());
        notesWindow->resizable(display);
        notesWindow->end();
        notesWindow->show();
    } else {
        std::cout << "Viewing notes..." << std::endl;
    }
}

// Dodawanie notatki z wyborem studenta
void addNote_cb(Fl_Widget* widget, void* data) {
    if (currentUser && currentUser->getRole() == "Teacher") {
        Teacher* teacher = dynamic_cast<Teacher*>(currentUser);
        Fl_Window* noteWindow = new Fl_Window(300, 250, "Add Note");
        Fl_Input* noteInput = new Fl_Input(100, 50, 150, 25, "Note:");
        Fl_Input* dateInput = new Fl_Input(100, 80, 150, 25, "Date:");
        dateInput->value(getCurrentDate().c_str()); // Ustawiamy bieżącą datę
        Fl_Choice* studentChoice = new Fl_Choice(100, 110, 150, 25, "Student:");
        for (const auto& student : teacher->students) {
            studentChoice->add((student->name + " " + student->surname).c_str());
        }
        Fl_Button* submitButton = new Fl_Button(50, 180, 80, 25, "Submit");
        Fl_Button* cancelButton = new Fl_Button(150, 180, 80, 25, "Cancel");

        submitButton->callback([](Fl_Widget* w, void* d) {
            Fl_Input** inputs = (Fl_Input**)d;
            Fl_Input* noteInput = inputs[0];
            Fl_Input* dateInput = inputs[1];
            Fl_Choice* studentChoice = (Fl_Choice*)inputs[2];
            Fl_Window* noteWindow = (Fl_Window*)inputs[3];
            Teacher* teacher = (Teacher*)inputs[4];

            std::string content = safeInputValue(noteInput);
            std::string date = safeInputValue(dateInput);
            const char* studentName = studentChoice->text();
            if (!studentName) {
                std::cerr << "No student selected!" << std::endl;
                return;
            }
            Student* student = nullptr;
            for (auto& s : teacher->students) {
                if ((s->name + " " + s->surname) == studentName) {
                    student = s;
                    break;
                }
            }
            if (!student) {
                std::cerr << "Student not found!" << std::endl;
                return;
            }
            Note note{content, date, teacher};
            teacher->addNoteToStudent(student, note);
            std::cout << "Note added for " << studentName << ": " << content << std::endl;
            diary.saveToFile("database.txt");
            noteWindow->hide();
            delete[] inputs;
        }, new Fl_Input*[5]{noteInput, dateInput, (Fl_Input*)studentChoice, (Fl_Input*)noteWindow, (Fl_Input*)teacher});

        cancelButton->callback([](Fl_Widget* w, void* d) {
            ((Fl_Window*)d)->hide();
        }, noteWindow);

        noteWindow->end();
        noteWindow->show();
    } else {
        std::cout << "Add note..." << std::endl;
    }
}

void submitGrade_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    Fl_Input* gradeInput = inputs[0];
    Fl_Input* dateInput = inputs[1];
    Fl_Choice* studentChoice = (Fl_Choice*)inputs[2];
    Fl_Window* gradeWindow = (Fl_Window*)inputs[3];

    std::string gradeStr = safeInputValue(gradeInput);
    std::string date = safeInputValue(dateInput);
    double gradeValue = std::stod(gradeStr);
    const char* studentName = studentChoice->text();

    if (!studentName) {
        std::cerr << "No student selected!" << std::endl;
        return;
    }

    Student* student = nullptr;
    for (auto& s : diary.getStudents()) {
        if ((s.name + " " + s.surname) == studentName) {
            student = const_cast<Student*>(&s);
            break;
        }
    }

    if (!student) {
        std::cerr << "Student not found!" << std::endl;
        return;
    }

    Teacher* teacher = dynamic_cast<Teacher*>(currentUser);
    Grade grade{gradeValue, date, teacher, teacher ? teacher->subject : ""};
    student->addGrade(grade);
    std::cout << "Grade assigned: " << gradeStr << " on " << date << " to " << studentName << std::endl;
    diary.saveToFile("database.txt");
    gradeWindow->hide();
    delete[] inputs;
}

void assignGrade_cb(Fl_Widget* widget, void* data) {
    if (currentUser && currentUser->getRole() == "Teacher") {
        Teacher* teacher = dynamic_cast<Teacher*>(currentUser);
        Fl_Window* gradeWindow = new Fl_Window(300, 250, "Assign Grade");
        Fl_Input* gradeInput = new Fl_Input(100, 50, 150, 25, "Grade:");
        Fl_Input* dateInput = new Fl_Input(100, 80, 150, 25, "Date:");
        dateInput->value(getCurrentDate().c_str()); // Ustawiamy bieżącą datę
        Fl_Choice* studentChoice = new Fl_Choice(100, 110, 150, 25, "Student:");

        for (const auto& student : teacher->students) {
            studentChoice->add((student->name + " " + student->surname).c_str());
        }

        Fl_Button* submitButton = new Fl_Button(50, 180, 80, 25, "Submit");
        Fl_Button* cancelButton = new Fl_Button(150, 180, 80, 25, "Cancel");

        Fl_Input** inputs = new Fl_Input*[4]{gradeInput, dateInput, (Fl_Input*)studentChoice, (Fl_Input*)gradeWindow};

        submitButton->callback(submitGrade_cb, inputs);

        cancelButton->callback([](Fl_Widget* widget, void* data) {
            ((Fl_Window*)data)->hide();
        }, gradeWindow);

        gradeWindow->end();
        gradeWindow->show();
    } else {
        std::cout << "Assign grade..." << std::endl;
    }
}

void viewExcuses_cb(Fl_Widget* widget, void* data) {
    if (currentUser && currentUser->getRole() == "Teacher") {
        Teacher* teacher = dynamic_cast<Teacher*>(currentUser);
        Fl_Window* excusesWindow = new Fl_Window(400, 300, "Excuses");
        Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
        Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
        display->buffer(textbuf);

        std::ostringstream oss;
        oss << "Excuses:";
        for (const auto& student : teacher->students) {
            for (const auto& excuse : student->excuses) {
                oss << "\n  - Student: " << student->name << " " << student->surname
                    << " | Excuse: " << excuse.content
                    << " | Date: " << excuse.date;
            }
        }
        textbuf->text(oss.str().c_str());

        excusesWindow->resizable(display);
        excusesWindow->end();
        excusesWindow->show();
    } else {
        std::cout << "Viewing excuses..." << std::endl;
    }
}

void viewChildGrades_cb(Fl_Widget* widget, void* data) {
    if (currentUser && currentUser->getRole() == "Parent") {
        Parent* parent = dynamic_cast<Parent*>(currentUser);
        Fl_Window* gradesWindow = new Fl_Window(400, 300, "Child Grades");
        Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
        Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
        display->buffer(textbuf);

        std::ostringstream oss;
        oss << "Child Grades:";
        for (auto& child : parent->children) {
            oss << "\n\n" << child->name << " " << child->surname << "\n"
                << child->getGradesInfo();
        }

        textbuf->text(oss.str().c_str());
        gradesWindow->resizable(display);
        gradesWindow->end();
        gradesWindow->show();
    } else {
        std::cout << "Viewing child grades..." << std::endl;
    }
}

void addExcuse_cb(Fl_Widget* widget, void* data) {
    if (currentUser && currentUser->getRole() == "Parent") {
        Parent* parent = dynamic_cast<Parent*>(currentUser);
        Fl_Window* excuseWindow = new Fl_Window(300, 250, "Add Excuse");
        Fl_Input* excuseInput = new Fl_Input(100, 50, 150, 25, "Excuse:");
        Fl_Input* dateInput = new Fl_Input(100, 80, 150, 25, "Date:");
        dateInput->value(getCurrentDate().c_str()); // Ustawiamy bieżącą datę
        Fl_Choice* studentChoice = new Fl_Choice(100, 110, 150, 25, "Student:");
        for (const auto& student : parent->children) {
            studentChoice->add((student->name + " " + student->surname).c_str());
        }
        Fl_Button* submitButton = new Fl_Button(50, 180, 80, 25, "Submit");
        Fl_Button* cancelButton = new Fl_Button(150, 180, 80, 25, "Cancel");

        submitButton->callback([](Fl_Widget* w, void* d) {
            Fl_Input** inputs = (Fl_Input**)d;
            Fl_Input* excuseInput = inputs[0];
            Fl_Input* dateInput = inputs[1];
            Fl_Choice* studentChoice = (Fl_Choice*)inputs[2];
            Fl_Window* excuseWindow = (Fl_Window*)inputs[3];
            Parent* parent = (Parent*)inputs[4];

            std::string content = safeInputValue(excuseInput);
            std::string date = safeInputValue(dateInput);
            const char* studentName = studentChoice->text();
            if (!studentName) {
                std::cerr << "No student selected!" << std::endl;
                return;
            }
            Student* student = nullptr;
            for (auto& s : parent->children) {
                if ((s->name + " " + s->surname) == studentName) {
                    student = s;
                    break;
                }
            }
            if (!student) {
                std::cerr << "Student not found!" << std::endl;
                return;
            }
            Excuse excuse{content, date, parent};
            parent->addExcuse(student, excuse);
            std::cout << "Excuse added for " << studentName << ": " << content << std::endl;
            diary.saveToFile("database.txt");
            excuseWindow->hide();
            delete[] inputs;
        }, new Fl_Input*[5]{excuseInput, dateInput, (Fl_Input*)studentChoice, (Fl_Input*)excuseWindow, (Fl_Input*)parent});

        cancelButton->callback([](Fl_Widget* w, void* d) {
            ((Fl_Window*)d)->hide();
        }, excuseWindow);

        excuseWindow->end();
        excuseWindow->show();
    } else {
        std::cout << "Add excuse..." << std::endl;
    }
}

void addStudent_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    std::string username = safeInputValue(inputs[0]);
    std::string password = safeInputValue(inputs[1]);
    std::string name = safeInputValue(inputs[2]);
    std::string surname = safeInputValue(inputs[3]);

    if (username.empty() || password.empty() || name.empty() || surname.empty()) {
        std::cerr << "All fields must be filled!" << std::endl;
        return;
    }

    Student student(username, password, name, surname, 0 /* dummy ID */);
    diary.addStudent(student);
    std::cout << "Student added: " << name << " " << surname << std::endl;
    diary.saveToFile("database.txt");
}

void addTeacher_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    std::string username = safeInputValue(inputs[0]);
    std::string password = safeInputValue(inputs[1]);
    std::string name = safeInputValue(inputs[2]);
    std::string surname = safeInputValue(inputs[3]);
    std::string subject = safeInputValue(inputs[4]);

    if (username.empty() || password.empty() || name.empty() || surname.empty() || subject.empty()) {
        std::cerr << "All fields must be filled!" << std::endl;
        return;
    }

    Teacher teacher(username, password, name, surname, subject);
    diary.addTeacher(teacher);
    std::cout << "Teacher added: " << name << " " << surname << std::endl;
    diary.saveToFile("database.txt");
}

void addParent_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    std::string username = safeInputValue(inputs[0]);
    std::string password = safeInputValue(inputs[1]);
    std::string name = safeInputValue(inputs[2]);
    std::string surname = safeInputValue(inputs[3]);

    if (username.empty() || password.empty() || name.empty() || surname.empty()) {
        std::cerr << "All fields must be filled!" << std::endl;
        return;
    }

    Parent parent(username, password, name, surname);
    diary.addParent(parent);
    std::cout << "Parent added: " << username << std::endl;
    diary.saveToFile("database.txt");
}

void showStudents_cb(Fl_Widget* widget, void* data) {
    const std::vector<Student>& students = diary.getStudents();
    Fl_Window* studentsWindow = new Fl_Window(400, 300, "List of Students");
    Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
    Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
    display->buffer(textbuf);
    std::string list;
    for (const auto& student : students) {
        list += "ID: " + std::to_string(student.id) + ", " + student.name + " " + student.surname + "\n";
    }
    textbuf->text(list.c_str());
    studentsWindow->resizable(display);
    studentsWindow->show();
}

void showTeachers_cb(Fl_Widget* widget, void* data) {
    const std::vector<Teacher>& teachers = diary.getTeachers();
    Fl_Window* teachersWindow = new Fl_Window(400, 300, "List of Teachers");
    Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
    Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
    display->buffer(textbuf);
    std::string list;
    for (const auto& teacher : teachers) {
        list += teacher.name + " " + teacher.surname + ", Subject: " + teacher.subject + "\n";
    }
    textbuf->text(list.c_str());
    teachersWindow->resizable(display);
    teachersWindow->show();
}

void showParents_cb(Fl_Widget* widget, void* data) {
    const std::vector<Parent>& parents = diary.getParents();
    Fl_Window* parentsWindow = new Fl_Window(400, 300, "List of Parents");
    Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
    Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
    display->buffer(textbuf);
    std::string list;
    for (const auto& parent : parents) {
        list += "Username: " + parent.getUsername() + ", Name: " + parent.name + " " + parent.surname + "\n";
    }
    textbuf->text(list.c_str());
    parentsWindow->resizable(display);
    parentsWindow->show();
}

void assignParentToStudent_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    std::string parentUsername = safeInputValue(inputs[0]);
    std::string studentUsername = safeInputValue(inputs[1]);

    Parent* parent = diary.findParentByUsername(parentUsername);
    Student* student = diary.findStudentByUsername(studentUsername);

    if (!parent || !student) {
        std::cerr << "Invalid parent or student username!" << std::endl;
        return;
    }

    student->setParent(parent);
    parent->addChild(student);
    diary.saveToFile("database.txt");

    std::cout << "Assigned parent " << parentUsername << " to student " << studentUsername << std::endl;
}

void assignStudentToTeacher_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    std::string teacherUsername = safeInputValue(inputs[0]);
    std::string studentUsername = safeInputValue(inputs[1]);

    Teacher* teacher = diary.findTeacherByUsername(teacherUsername);
    Student* student = diary.findStudentByUsername(studentUsername);

    if (!teacher || !student) {
        std::cerr << "Invalid teacher or student username!" << std::endl;
        return;
    }

    teacher->addStudent(student);
    diary.saveToFile("database.txt");

    std::cout << "Assigned student " << studentUsername << " to teacher " << teacherUsername << std::endl;
}

void showTeacherAssignments_cb(Fl_Widget* widget, void* data) {
    const std::vector<Teacher>& teachers = diary.getTeachers();
    Fl_Window* assignmentsWindow = new Fl_Window(400, 300, "Teacher Assignments");
    Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
    Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
    display->buffer(textbuf);
    std::string list;
    for (const auto& teacher : teachers) {
        if (!teacher.students.empty()) {
            list += "Teacher: " + teacher.name + " " + teacher.surname + "\n";
            for (const auto& student : teacher.students) {
                list += "  - Student: " + student->name + " " + student->surname + "\n";
            }
        }
    }
    textbuf->text(list.c_str());
    assignmentsWindow->resizable(display);
    assignmentsWindow->show();
}

void showParentAssignments_cb(Fl_Widget* widget, void* data) {
    const std::vector<Parent>& parents = diary.getParents();
    Fl_Window* assignmentsWindow = new Fl_Window(400, 300, "Parent Assignments");
    Fl_Text_Buffer* textbuf = new Fl_Text_Buffer();
    Fl_Text_Display* display = new Fl_Text_Display(20, 20, 360, 260);
    display->buffer(textbuf);
    std::string list;
    for (const auto& parent : parents) {
        if (!parent.children.empty()) {
            list += "Parent: " + parent.name + " " + parent.surname + "\n";
            for (const auto& student : parent.children) {
                list += "  - Student: " + student->name + " " + student->surname + "\n";
            }
        }
    }
    textbuf->text(list.c_str());
    assignmentsWindow->resizable(display);
    assignmentsWindow->show();
}

void createStudentWindow() {
    studentWindow = new Fl_Window(400, 300, "Student Window");
    studentWelcomeBox = new Fl_Box(20, 40, 360, 30, "Welcome, Student!");

    Fl_Button* viewGrades = new Fl_Button(150, 100, 100, 25, "View Grades");
    viewGrades->callback(viewGrades_cb);

    Fl_Button* viewNotes = new Fl_Button(150, 150, 100, 25, "View Notes");
    viewNotes->callback(viewNotes_cb);

    Fl_Button* logoutButton = new Fl_Button(150, 200, 100, 25, "Logout");
    logoutButton->callback(logout_cb, studentWindow);

    studentWindow->end();
}

void createTeacherWindow() {
    teacherWindow = new Fl_Window(400, 300, "Teacher Window");
    teacherWelcomeBox = new Fl_Box(20, 40, 360, 30, "Welcome, Teacher!");

    Fl_Button* assignGrade = new Fl_Button(150, 100, 100, 25, "Assign Grade");
    assignGrade->callback(assignGrade_cb);

    Fl_Button* addNote = new Fl_Button(150, 150, 100, 25, "Add Note");
    addNote->callback(addNote_cb);

    Fl_Button* viewExcuses = new Fl_Button(150, 200, 100, 25, "View Excuses");
    viewExcuses->callback(viewExcuses_cb);

    Fl_Button* logoutButton = new Fl_Button(150, 250, 100, 25, "Logout");
    logoutButton->callback(logout_cb, teacherWindow);

    teacherWindow->end();
}

void createParentWindow() {
    parentWindow = new Fl_Window(400, 300, "Parent Window");
    parentWelcomeBox = new Fl_Box(20, 40, 360, 30, "Welcome, Parent!");

    Fl_Button* viewChildGrades = new Fl_Button(150, 100, 100, 25, "View Child Grades");
    viewChildGrades->callback(viewChildGrades_cb);

    Fl_Button* addExcuse = new Fl_Button(150, 150, 100, 25, "Add Excuse");
    addExcuse->callback(addExcuse_cb);

    Fl_Button* logoutButton = new Fl_Button(150, 200, 100, 25, "Logout");
    logoutButton->callback(logout_cb, parentWindow);

    parentWindow->end();
}

void createAdminWindow() {
    adminWindow = new Fl_Window(800, 800, "Admin Window");
    new Fl_Box(20, 40, 760, 30, "Welcome, Admin!");

    Fl_Input* studentUsername = new Fl_Input(150, 100, 150, 25, "Student Username:");
    Fl_Input* studentPassword = new Fl_Input(150, 130, 150, 25, "Student Password:");
    Fl_Input* studentName = new Fl_Input(150, 160, 150, 25, "Student Name:");
    Fl_Input* studentSurname = new Fl_Input(150, 190, 150, 25, "Student Surname:");

    Fl_Input** studentInputs = new Fl_Input*[4];
    studentInputs[0] = studentUsername;
    studentInputs[1] = studentPassword;
    studentInputs[2] = studentName;
    studentInputs[3] = studentSurname;

    Fl_Button* addStudent = new Fl_Button(150, 220, 100, 25, "Add Student");
    addStudent->callback(addStudent_cb, studentInputs);

    Fl_Input* teacherUsername = new Fl_Input(150, 300, 150, 25, "Teacher Username:");
    Fl_Input* teacherPassword = new Fl_Input(150, 330, 150, 25, "Teacher Password:");
    Fl_Input* teacherName = new Fl_Input(150, 360, 150, 25, "Teacher Name:");
    Fl_Input* teacherSurname = new Fl_Input(150, 390, 150, 25, "Teacher Surname:");
    Fl_Input* teacherSubject = new Fl_Input(150, 420, 150, 25, "Teacher Subject:");

    Fl_Input** teacherInputs = new Fl_Input*[5];
    teacherInputs[0] = teacherUsername;
    teacherInputs[1] = teacherPassword;
    teacherInputs[2] = teacherName;
    teacherInputs[3] = teacherSurname;
    teacherInputs[4] = teacherSubject;

    Fl_Button* addTeacher = new Fl_Button(150, 450, 100, 25, "Add Teacher");
    addTeacher->callback(addTeacher_cb, teacherInputs);

    Fl_Input* parentUsername = new Fl_Input(150, 500, 150, 25, "Parent Username:");
    Fl_Input* parentPassword = new Fl_Input(150, 530, 150, 25, "Parent Password:");
    Fl_Input* parentName = new Fl_Input(150, 560, 150, 25, "Parent Name:");
    Fl_Input* parentSurname = new Fl_Input(150, 590, 150, 25, "Parent Surname:");

    Fl_Input** parentInputs = new Fl_Input*[4];
    parentInputs[0] = parentUsername;
    parentInputs[1] = parentPassword;
    parentInputs[2] = parentName;
    parentInputs[3] = parentSurname;

    Fl_Button* addParent = new Fl_Button(150, 620, 100, 25, "Add Parent");
    addParent->callback(addParent_cb, parentInputs);

    Fl_Input* assignParentUsername = new Fl_Input(450, 100, 150, 25, "Parent Username:");
    Fl_Input* assignStudentUsername = new Fl_Input(450, 130, 150, 25, "Student Username:");

    Fl_Input** assignParentInputs = new Fl_Input*[2];
    assignParentInputs[0] = assignParentUsername;
    assignParentInputs[1] = assignStudentUsername;

    Fl_Button* assignParent = new Fl_Button(450, 160, 150, 25, "Assign Parent");
    assignParent->callback(assignParentToStudent_cb, assignParentInputs);

    Fl_Input* assignTeacherUsername = new Fl_Input(450, 200, 150, 25, "Teacher Username:");
    Fl_Input* assignStudentToTeacherUsername = new Fl_Input(450, 230, 150, 25, "Student Username:");

    Fl_Input** assignTeacherInputs = new Fl_Input*[2];
    assignTeacherInputs[0] = assignTeacherUsername;
    assignTeacherInputs[1] = assignStudentToTeacherUsername;

    Fl_Button* assignTeacher = new Fl_Button(450, 260, 150, 25, "Assign Teacher");
    assignTeacher->callback(assignStudentToTeacher_cb, assignTeacherInputs);

    Fl_Button* showStudents = new Fl_Button(450, 300, 150, 25, "Show Students");
    showStudents->callback(showStudents_cb);

    Fl_Button* showTeachers = new Fl_Button(450, 330, 150, 25, "Show Teachers");
    showTeachers->callback(showTeachers_cb);

    Fl_Button* showParents = new Fl_Button(450, 360, 150, 25, "Show Parents");
    showParents->callback(showParents_cb);

    Fl_Button* showTeacherAssignments = new Fl_Button(450, 390, 150, 25, "Show Teacher Assignments");
    showTeacherAssignments->callback(showTeacherAssignments_cb);

    Fl_Button* showParentAssignments = new Fl_Button(450, 420, 150, 25, "Show Parent Assignments");
    showParentAssignments->callback(showParentAssignments_cb);

    Fl_Button* logoutButton = new Fl_Button(450, 450, 150, 25, "Logout");
    logoutButton->callback(logout_cb, adminWindow);

    adminWindow->end();
}

// Testy
TEST(SchoolDiaryTest, AddStudentTest) {
    SchoolDiary d;
    Student s("TestUsername", "TestPassword", "Name", "Surname", 0);
    d.addStudent(s);
    EXPECT_NE(nullptr, d.findStudentByUsername("TestUsername"));
    auto found = d.findStudentByUsername("TestUsername");
    EXPECT_EQ(s.getUsername(), found->getUsername());
}

TEST(SchoolDiaryTest, AddTeacherTest) {
    SchoolDiary d;
    Teacher t("TeacherUsername", "TeacherPassword", "Jon", "Doe", "Math");
    d.addTeacher(t);
    auto found = d.findTeacherByUsername("TeacherUsername");
    EXPECT_NE(nullptr, found);
    EXPECT_EQ(t.getUsername(), found->getUsername());
}

TEST(SchoolDiaryTest, AddParentTest) {
    SchoolDiary d;
    Parent p("ParentUsername", "ParentPassword", "Mary", "Smith");
    d.addParent(p);
    auto found = d.findParentByUsername("ParentUsername");
    EXPECT_NE(nullptr, found);
    EXPECT_EQ(p.getUsername(), found->getUsername());
}

TEST(SchoolDiaryTest, AddGradeTest) {
    SchoolDiary d;
    Teacher t("TeachUsername", "TeachPass", "John", "Doe", "Math");
    Student s("StudUsername", "StudPass", "Jane", "Smith", 0);
    d.addTeacher(t);
    d.addStudent(s);
    auto* foundT = d.findTeacherByUsername("TeachUsername");
    auto* foundS = d.findStudentByUsername("StudUsername");
    Grade g(5.0, "01.02.2025", foundT, "Math");
    foundS->addGrade(g);
    EXPECT_TRUE(foundS->getGradesInfo().find("5.0") != std::string::npos);
}

TEST(SchoolDiaryTest, AddNoteTest) {
    SchoolDiary d;
    Teacher t("NTUser", "NTPass", "Mark", "Brown", "History");
    Student s("SNUser", "SNPass", "Tim", "Cook", 0);
    d.addTeacher(t);
    d.addStudent(s);
    auto* foundT = d.findTeacherByUsername("NTUser");
    auto* foundS = d.findStudentByUsername("SNUser");
    Note n{"SampleNote", "02.02.2025", foundT};
    foundT->addNoteToStudent(foundS, n);
    EXPECT_TRUE(foundS->getNotesInfo().find("SampleNote") != std::string::npos);
}

TEST(SchoolDiaryTest, AddExcuseTest) {
    SchoolDiary d;
    Parent p("PUser", "Ppass", "Ed", "White");
    Student s("StExUser", "StExPass", "Max", "Payne", 0);
    d.addParent(p);
    d.addStudent(s);
    auto* foundP = d.findParentByUsername("PUser");
    auto* foundS = d.findStudentByUsername("StExUser");
    Excuse e{"Was sick", "03.02.2025", foundP};
    foundP->addExcuse(foundS, e);
    EXPECT_TRUE(foundS->getExcusesInfo().find("Was sick") != std::string::npos);
}

TEST(SchoolDiaryTest, AssignParentChildTest) {
    SchoolDiary d;
    Parent p("SomeParent", "Pass", "Anna", "Doe");
    Student s("ChUser", "ChPass", "Child", "Doe", 0);
    d.addParent(p);
    d.addStudent(s);
    auto* foundP = d.findParentByUsername("SomeParent");
    auto* foundS = d.findStudentByUsername("ChUser");
    foundS->setParent(foundP);
    foundP->addChild(foundS);
    EXPECT_FALSE(foundP->children.empty());
    EXPECT_EQ(foundP->children.front(), foundS);
}

TEST(SchoolDiaryTest, AssignStudentTeacherTest) {
    SchoolDiary d;
    Teacher t("TUser", "TPass", "Richard", "Roe", "Biology");
    Student s("SUser", "SPass", "Aria", "Stark", 0);
    d.addTeacher(t);
    d.addStudent(s);
    auto* foundT = d.findTeacherByUsername("TUser");
    auto* foundS = d.findStudentByUsername("SUser");
    foundT->addStudent(foundS);
    EXPECT_FALSE(foundT->students.empty());
    EXPECT_EQ(foundT->students.front(), foundS);
}

int main(int argc, char** argv) {
    // Inicjowanie Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // Jeśli uruchomiono z argumentem "test", odpalamy testy zamiast główną aplikację
    if (argc > 1 && std::string(argv[1]) == "test") {
        return RUN_ALL_TESTS();
    }
    
    try {
        diary.loadFromFile("database.txt");
        diary.loadAssignmentsFromDatabase("database.txt");
        diary.loadParentAssignmentsFromDatabase("database.txt");
    } catch(const std::exception& e) {
        std::cerr << "Błąd przy wczytywaniu pliku: " << e.what() << std::endl;
    }
    loginWindow = new Fl_Window(400, 300, "Login");
    Fl_Input* usernameInput = new Fl_Input(150, 100, 150, 25, "Username:");
    Fl_Input* passwordInput = new Fl_Input(150, 150, 150, 25, "Password:");
    Fl_Button* loginButton = new Fl_Button(150, 200, 100, 25, "Login");

    Fl_Input** loginInputs = new Fl_Input*[2];
    loginInputs[0] = usernameInput;
    loginInputs[1] = passwordInput;

    loginButton->callback(login_cb, loginInputs);

    loginWindow->end();
    loginWindow->show(argc, argv);

    createStudentWindow();
    createTeacherWindow();
    createParentWindow();
    createAdminWindow();

    int ret = Fl::run();
    diary.saveToFile("database.txt");
    return ret;
}