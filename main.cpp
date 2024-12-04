#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <iostream>
#include <stdexcept>
#include "SchoolDiary.h"

SchoolDiary diary;

// Okna dla różnych ról
Fl_Window* studentWindow;
Fl_Window* teacherWindow;
Fl_Window* parentWindow;
Fl_Window* adminWindow;
Fl_Window* loginWindow;

// Funkcja pomocnicza do bezpiecznego pobierania wartości z Fl_Input
std::string safeInputValue(Fl_Input* input) {
    const char* val = input->value();
    return val ? val : "";
}

// Callback dla wylogowania
void logout_cb(Fl_Widget* widget, void* data) {
    Fl_Window* window = (Fl_Window*)data;
    window->hide();
    loginWindow->show();
}

// Callback dla logowania
void login_cb(Fl_Widget* widget, void* data) {
    Fl_Input* input = (Fl_Input*)data;
    std::string username = safeInputValue(input);

    if (username == "student") {
        std::cout << "Logged in as Student" << std::endl;
        studentWindow->show();
        loginWindow->hide();
    } else if (username == "teacher") {
        std::cout << "Logged in as Teacher" << std::endl;
        teacherWindow->show();
        loginWindow->hide();
    } else if (username == "parent") {
        std::cout << "Logged in as Parent" << std::endl;
        parentWindow->show();
        loginWindow->hide();
    } else if (username == "admin") {
        std::cout << "Logged in as Admin" << std::endl;
        adminWindow->show();
        loginWindow->hide();
    } else {
        std::cout << "Login failed" << std::endl;
    }
}

// Callbacki dla ucznia
void viewGrades_cb(Fl_Widget* widget, void* data) {
    std::cout << "Viewing grades..." << std::endl;
    // Dodaj logikę wyświetlania ocen
}

void viewNotes_cb(Fl_Widget* widget, void* data) {
    std::cout << "Viewing notes..." << std::endl;
    // Dodaj logikę wyświetlania notatek
}

// Callbacki dla nauczyciela
void assignGrade_cb(Fl_Widget* widget, void* data) {
    std::cout << "Assigning grade..." << std::endl;
    // Dodaj logikę przypisywania ocen
}

void addNote_cb(Fl_Widget* widget, void* data) {
    std::cout << "Adding note..." << std::endl;
    // Dodaj logikę dodawania notatek
}

// Callbacki dla rodzica
void viewChildGrades_cb(Fl_Widget* widget, void* data) {
    std::cout << "Viewing child grades..." << std::endl;
    // Dodaj logikę wyświetlania ocen dziecka
}

void addExcuse_cb(Fl_Widget* widget, void* data) {
    std::cout << "Adding excuse..." << std::endl;
    // Dodaj logikę dodawania usprawiedliwień
}

// Callbacki admina
void addStudent_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    std::string username = safeInputValue(inputs[0]);
    std::string password = safeInputValue(inputs[1]);
    std::string name = safeInputValue(inputs[2]);
    std::string surname = safeInputValue(inputs[3]);
    std::string id_str = safeInputValue(inputs[4]);

    if (username.empty() || password.empty() || name.empty() || surname.empty() || id_str.empty()) {
        std::cerr << "All fields must be filled!" << std::endl;
        return;
    }

    int id = std::stoi(id_str);

    Student student(username, password, name, surname, id);
    diary.addStudent(student);
    std::cout << "Student added: " << name << " " << surname << std::endl;
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
}

void addParent_cb(Fl_Widget* widget, void* data) {
    Fl_Input** inputs = (Fl_Input**)data;
    std::string username = safeInputValue(inputs[0]);
    std::string password = safeInputValue(inputs[1]);

    if (username.empty() || password.empty()) {
        std::cerr << "All fields must be filled!" << std::endl;
        return;
    }

    Parent parent(username, password);
    diary.addParent(parent);
    std::cout << "Parent added: " << username << std::endl;
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
        list += "Username: " + parent.getUsername() + "\n";
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

    std::cout << "Assigned parent " << parentUsername << " to student " << studentUsername << std::endl;
}

void createStudentWindow() {
    studentWindow = new Fl_Window(400, 300, "Student Window");
    new Fl_Box(20, 40, 360, 30, "Welcome, Student!");

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
    new Fl_Box(20, 40, 360, 30, "Welcome, Teacher!");

    Fl_Button* assignGrade = new Fl_Button(150, 100, 100, 25, "Assign Grade");
    assignGrade->callback(assignGrade_cb);

    Fl_Button* addNote = new Fl_Button(150, 150, 100, 25, "Add Note");
    addNote->callback(addNote_cb);

    Fl_Button* logoutButton = new Fl_Button(150, 200, 100, 25, "Logout");
    logoutButton->callback(logout_cb, teacherWindow);

    teacherWindow->end();
}

void createParentWindow() {
    parentWindow = new Fl_Window(400, 300, "Parent Window");
    new Fl_Box(20, 40, 360, 30, "Welcome, Parent!");

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

    // Inputy dla dodawania ucznia
    Fl_Input* studentUsername = new Fl_Input(150, 100, 150, 25, "Student Username:");
    Fl_Input* studentPassword = new Fl_Input(150, 130, 150, 25, "Student Password:");
    Fl_Input* studentName = new Fl_Input(150, 160, 150, 25, "Student Name:");
    Fl_Input* studentSurname = new Fl_Input(150, 190, 150, 25, "Student Surname:");
    Fl_Input* studentId = new Fl_Input(150, 220, 150, 25, "Student ID:");

    // Dynamiczna alokacja tablicy
    Fl_Input** studentInputs = new Fl_Input*[5];
    studentInputs[0] = studentUsername;
    studentInputs[1] = studentPassword;
    studentInputs[2] = studentName;
    studentInputs[3] = studentSurname;
    studentInputs[4] = studentId;

    Fl_Button* addStudent = new Fl_Button(150, 250, 100, 25, "Add Student");
    addStudent->callback(addStudent_cb, studentInputs);

    // Inputy dla dodawania nauczyciela
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

    // Inputy dla dodawania rodzica
    Fl_Input* parentUsername = new Fl_Input(150, 500, 150, 25, "Parent Username:");
    Fl_Input* parentPassword = new Fl_Input(150, 530, 150, 25, "Parent Password:");

    Fl_Input** parentInputs = new Fl_Input*[2];
    parentInputs[0] = parentUsername;
    parentInputs[1] = parentPassword;

    Fl_Button* addParent = new Fl_Button(150, 560, 100, 25, "Add Parent");
    addParent->callback(addParent_cb, parentInputs);

    // Przyciski do wyświetlania list
    Fl_Button* showStudents = new Fl_Button(400, 100, 150, 25, "Show Students");
    showStudents->callback(showStudents_cb);

    Fl_Button* showTeachers = new Fl_Button(400, 130, 150, 25, "Show Teachers");
    showTeachers->callback(showTeachers_cb);

    Fl_Button* showParents = new Fl_Button(400, 160, 150, 25, "Show Parents");
    showParents->callback(showParents_cb);

    // Przypisywanie rodzica do studenta
    Fl_Input* assignParentUsername = new Fl_Input(400, 220, 150, 25, "Parent Username:");
    Fl_Input* assignStudentUsername = new Fl_Input(400, 250, 150, 25, "Student Username:");
    Fl_Input** assignInputs = new Fl_Input*[2];
    assignInputs[0] = assignParentUsername;
    assignInputs[1] = assignStudentUsername;
    Fl_Button* assignParent = new Fl_Button(400, 280, 150, 25, "Assign Parent");
    assignParent->callback(assignParentToStudent_cb, assignInputs);

    Fl_Button* logoutButton = new Fl_Button(150, 590, 100, 25, "Logout");
    logoutButton->callback(logout_cb, adminWindow);

    adminWindow->end();
}

int main() {
    loginWindow = new Fl_Window(400, 300, "School Diary");
    Fl_Input* input = new Fl_Input(120, 100, 150, 25, "Username:");
    Fl_Button* login = new Fl_Button(150, 150, 100, 25, "Login");

    login->callback(login_cb, input);

    loginWindow->end();
    loginWindow->show();

    // Tworzenie okien dla różnych ról
    createStudentWindow();
    createTeacherWindow();
    createParentWindow();
    createAdminWindow();

    return Fl::run();
}