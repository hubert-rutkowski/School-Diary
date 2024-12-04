#include "SchoolDiary.h"
#include <sstream>

// --- Grade Methods ---
std::string Grade::getGradeInfo() const {
    return "Grade: " + std::to_string(value) + " | Date: " + date + " | Teacher: " + (teacher ? teacher->name : "N/A");
}

// --- Note Methods ---
std::string Note::getNoteInfo() const {
    return "Note: " + content + " | Date: " + date + " | Teacher: " + (teacher ? teacher->name : "N/A");
}

// --- Praise Methods ---
std::string Praise::getPraiseInfo() const {
    return "Praise: " + content + " | Date: " + date + " | Teacher: " + (teacher ? teacher->name : "N/A");
}

// --- Excuse Methods ---
std::string Excuse::getExcuseInfo() const {
    return "Excuse: " + content + " | Date: " + date + " | Parent: " + (parent ? parent->getUsername() : "N/A");
}

// --- Student Methods ---
void Student::addGrade(const Grade& grade) {
    grades.push_back(grade);
}

void Student::addNote(const Note& note) {
    notes.push_back(note);
}

void Student::addPraise(const Praise& praise) {
    praises.push_back(praise);
}

void Student::addExcuse(const Excuse& excuse) {
    excuses.push_back(excuse);
}

void Student::setParent(Parent* parent) {
    this->parent = parent;
}

// --- Parent Methods ---
void Parent::addExcuse(Student* student, const Excuse& excuse) {
    if (student) {
        student->addExcuse(excuse);
    }
}

void Parent::addChild(Student* student) {
    if (student) {
        children.push_back(student);
    }
}

// --- Teacher Methods ---
void Teacher::assignGrade(Student* student, const Grade& grade) {
    if (student) {
        student->addGrade(grade);
    }
}

void Teacher::addNoteToStudent(Student* student, const Note& note) {
    if (student) {
        student->addNote(note);
    }
}

void Teacher::addPraiseToStudent(Student* student, const Praise& praise) {
    if (student) {
        student->addPraise(praise);
    }
}

void Teacher::addStudent(Student* student) {
    if (student) {
        students.push_back(student);
    }
}

// --- Admin Methods ---
void Admin::addStudent(SchoolDiary& diary, const Student& student) {
    diary.addStudent(student);
}

void Admin::addTeacher(SchoolDiary& diary, const Teacher& teacher) {
    diary.addTeacher(teacher);
}

void Admin::addParent(SchoolDiary& diary, const Parent& parent) {
    diary.addParent(parent);
}

void Admin::assignStudentToParent(Student* student, Parent* parent) {
    if (student && parent) {
        parent->addChild(student);
        student->setParent(parent);
    }
}

void Admin::assignStudentToTeacher(Student* student, Teacher* teacher) {
    if (student && teacher) {
        teacher->addStudent(student);
    }
}

// --- SchoolDiary Methods ---
void SchoolDiary::addStudent(const Student& student) {
    students.push_back(student);
}

void SchoolDiary::addTeacher(const Teacher& teacher) {
    teachers.push_back(teacher);
}

void SchoolDiary::addParent(const Parent& parent) {
    parents.push_back(parent);
}

void SchoolDiary::addAdmin(const Admin& admin) {
    admins.push_back(admin);
}

Student* SchoolDiary::findStudentByUsername(const std::string& username) {
    for (auto& student : students) {
        if (student.getUsername() == username) {
            return &student;
        }
    }
    return nullptr;
}

Teacher* SchoolDiary::findTeacherByUsername(const std::string& username) {
    for (auto& teacher : teachers) {
        if (teacher.getUsername() == username) {
            return &teacher;
        }
    }
    return nullptr;
}

Parent* SchoolDiary::findParentByUsername(const std::string& username) {
    for (auto& parent : parents) {
        if (parent.getUsername() == username) {
            return &parent;
        }
    }
    return nullptr;
}

Admin* SchoolDiary::findAdminByUsername(const std::string& username) {
    for (auto& admin : admins) {
        if (admin.getUsername() == username) {
            return &admin;
        }
    }
    return nullptr;
}

std::string SchoolDiary::getStudentInfo(int id) const {
    for (const auto& student : students) {
        if (student.id == id) {
            std::ostringstream oss;
            oss << "ID: " << student.id << " | Name: " << student.name << " " << student.surname;
            return oss.str();
        }
    }
    return "Student not found!";
}

std::string SchoolDiary::getTeacherInfo(const std::string& name) const {
    for (const auto& teacher : teachers) {
        if (teacher.name == name) {
            return "Teacher: " + teacher.name + " | Subject: " + teacher.subject;
        }
    }
    return "Teacher not found!";
}

const std::vector<Student>& SchoolDiary::getStudents() const {
    return students;
}

const std::vector<Teacher>& SchoolDiary::getTeachers() const {
    return teachers;
}

const std::vector<Parent>& SchoolDiary::getParents() const {
    return parents;
}