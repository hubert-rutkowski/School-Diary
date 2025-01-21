#ifndef SCHOOL_DIARY_H
#define SCHOOL_DIARY_H

#include <string>
#include <vector>

class Teacher;
class Parent;
class SchoolDiary;

class Grade {
public:
    double value;
    std::string date;
    Teacher* teacher;
    std::string subject; // Dodajemy pole na nazwę przedmiotu

    std::string getGradeInfo() const;
};

class Note {
public:
    std::string content;
    std::string date;
    Teacher* teacher;
    std::string getNoteInfo() const;
};

class Praise {
public:
    std::string content;
    std::string date;
    Teacher* teacher;
    std::string getPraiseInfo() const;
};

class Excuse {
public:
    std::string content;
    std::string date;
    Parent* parent;
    std::string getExcuseInfo() const;
};

class User {
protected:
    std::string username;
    std::string password;
    std::string role;

public:
    User(const std::string& username, const std::string& password, const std::string& role)
        : username(username), password(password), role(role) {}

    virtual ~User() {}
    virtual std::string getRole() const { return role; }
    std::string getUsername() const { return username; }
    std::string getPassword() const { return password; }
};

class Student : public User {
public:
    std::string name;
    std::string surname;
    int id;
    std::vector<Grade> grades;
    std::vector<Note> notes;
    std::vector<Praise> praises;
    std::vector<Excuse> excuses;
    Parent* parent;

    Student(const std::string& username, const std::string& password, const std::string& name,
            const std::string& surname, int id)
        : User(username, password, "Student"), name(name), surname(surname), id(id), parent(nullptr) {}

    void addGrade(const Grade& grade);
    void addNote(const Note& note);
    void addPraise(const Praise& praise);
    void addExcuse(const Excuse& excuse);
    void setParent(Parent* parent);

    std::string getGradesInfo() const;
    std::string getNotesInfo() const;
    std::string getPraisesInfo() const;
    std::string getExcusesInfo() const;
};

class Parent : public User {
public:
    std::vector<Student*> children;

    Parent(const std::string& username, const std::string& password)
        : User(username, password, "Parent") {}

    void addExcuse(Student* student, const Excuse& excuse);
    void addChild(Student* student);
};

class Teacher : public User {
public:
    std::string name;
    std::string surname;
    std::string subject;
    std::vector<Student*> students;

    Teacher(const std::string& username, const std::string& password, const std::string& name,
            const std::string& surname, const std::string& subject)
        : User(username, password, "Teacher"), name(name), surname(surname), subject(subject) {}

    void assignGrade(Student* student, const Grade& grade);
    void addNoteToStudent(Student* student, const Note& note);
    void addPraiseToStudent(Student* student, const Praise& praise);
    void addStudent(Student* student);
};

class Admin : public User {
public:
    Admin(const std::string& username, const std::string& password)
        : User(username, password, "Admin") {}

    void addStudent(SchoolDiary& diary, const Student& student);
    void addTeacher(SchoolDiary& diary, const Teacher& teacher);
    void addParent(SchoolDiary& diary, const Parent& parent);
    void assignStudentToParent(Student* student, Parent* parent);
    void assignStudentToTeacher(Student* student, Teacher* teacher);
};

class SchoolDiary {
public:
    std::vector<Student> students;
    std::vector<Teacher> teachers;
    std::vector<Parent> parents;
    std::vector<Admin> admins;

    void addStudent(const Student& student);
    void addTeacher(const Teacher& teacher);
    void addParent(const Parent& parent);
    void addAdmin(const Admin& admin);

    Student* findStudentByUsername(const std::string& username);
    Teacher* findTeacherByUsername(const std::string& username);
    Parent* findParentByUsername(const std::string& username);
    Admin* findAdminByUsername(const std::string& username);

    std::string getStudentInfo(int id) const;
    std::string getTeacherInfo(const std::string& name) const;

    const std::vector<Student>& getStudents() const;
    const std::vector<Teacher>& getTeachers() const;
    const std::vector<Parent>& getParents() const;

    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void saveToDatabase(const std::string& filename);
    void loadAssignmentsFromDatabase(const std::string& filename);

    void assignStudentToTeacher(Student* student, Teacher* teacher);

private:
    int nextStudentId = 1;
};

#endif