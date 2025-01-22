#include "SchoolDiary.h"
#include <sstream>
#include <fstream>
#include <algorithm> // Dodajemy ten include
#include <iostream>

// --- Grade Methods ---
std::string Grade::getGradeInfo() const {
    return "Grade: " + std::to_string(value) +
           " | Date: " + date +
           " | Teacher: " + (teacher ? (teacher->name + " " + teacher->surname) : "N/A") +
           " | Subject: " + subject;
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
    return "Excuse: " + content + " | Date: " + date;
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

std::string Student::getGradesInfo() const {
    std::ostringstream oss;
    oss << "Grades:";
    for (const auto& g : grades) {
        oss << "\n  - " << g.getGradeInfo();
    }
    return oss.str();
}

std::string Student::getNotesInfo() const {
    std::ostringstream oss;
    oss << "Notes:";
    for (const auto& n : notes) {
        oss << "\n  - " << n.getNoteInfo();
    }
    return oss.str();
}

std::string Student::getPraisesInfo() const {
    std::ostringstream oss;
    oss << "Praises:";
    for (const auto& p : praises) {
        oss << "\n  - " << p.getPraiseInfo();
    }
    return oss.str();
}

std::string Student::getExcusesInfo() const {
    std::ostringstream oss;
    oss << "Excuses:";
    for (const auto& e : excuses) {
        oss << "\n  - " << e.getExcuseInfo();
    }
    return oss.str();
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
    Student copy = student;
    int newId = 1;
    while (std::any_of(students.begin(), students.end(), [newId](const Student& s) { return s.id == newId; })) {
        ++newId;
    }
    copy.id = newId;
    students.push_back(copy);
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

void SchoolDiary::loadFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if(!in) return; // Brak pliku, pomiń

    std::string line;
    while (std::getline(in, line)) {
        if (line == "Studenci:") {
            while (std::getline(in, line) && !line.empty() && line != "Oceny:") {
                if (line.empty()) break;
                std::istringstream iss(line);
                std::string usr, pass, nm, srn;
                int id;
                iss >> usr >> pass >> nm >> srn >> id;
                Student student(usr, pass, nm, srn, id);
                students.push_back(student);
            }
            if (line == "Oceny:") {
                in.seekg(-(static_cast<std::streamoff>(line.size() + 1)), std::ios::cur);
            }
        } else if (line == "Nauczyciele:") {
            while (std::getline(in, line) && !line.empty() && line != "Studenci:") {
                if (line.empty()) break;
                std::istringstream iss(line);
                std::string usr, pass, nm, srn, subj;
                iss >> usr >> pass >> nm >> srn >> subj;
                Teacher teacher(usr, pass, nm, srn, subj);

                int stId;
                while (iss >> stId) {
                    for (auto& s : students) {
                        if (s.id == stId) {
                            teacher.addStudent(&s);
                            break;
                        }
                    }
                }
                teachers.push_back(teacher);
            }
            if (line == "Studenci:") {
                in.seekg(-(static_cast<std::streamoff>(line.size() + 1)), std::ios::cur);
            }
        } else if (line == "Oceny:") {
            while (std::getline(in, line) && !line.empty() && line != "Notatki:") {
                std::istringstream iss(line);
                int sid;
                double val;
                std::string date, teacherUsr, subj;
                iss >> sid >> val >> date >> teacherUsr >> subj;
                Student* st = nullptr;
                for (auto& tmp : students)
                    if (tmp.id == sid) { st = &tmp; break; }
                if (!st) continue;
                Teacher* t = findTeacherByUsername(teacherUsr);
                Grade grade{val, date, t, subj};
                st->addGrade(grade);
            }
            if (line == "Notatki:") {
                in.seekg(-(static_cast<std::streamoff>(line.size() + 1)), std::ios::cur);
            }
        } else if (line == "Notatki:") {
            while (std::getline(in, line) && !line.empty() && line != "Usprawiedliwienia:") {
                std::istringstream iss(line);
                int sid;
                std::string content, date, teacherUsr;
                iss >> sid >> content >> date >> teacherUsr;
                Student* st = nullptr;
                for (auto& tmp : students)
                    if (tmp.id == sid) { st = &tmp; break; }
                if (!st) continue;
                Teacher* t = findTeacherByUsername(teacherUsr);
                Note note{content, date, t};
                st->addNote(note);
            }
            if (line == "Usprawiedliwienia:") {
                in.seekg(-(static_cast<std::streamoff>(line.size() + 1)), std::ios::cur);
            }
        } else if (line == "Usprawiedliwienia:") {
            while (std::getline(in, line) && !line.empty() && line != "Rodzice:") {
                std::istringstream iss(line);
                int sid;
                std::string content, date, parentUsr;
                iss >> sid >> content >> date >> parentUsr;
                Student* st = nullptr;
                for (auto& tmp : students)
                    if (tmp.id == sid) { st = &tmp; break; }
                if (!st) continue;
                Parent* p = findParentByUsername(parentUsr);
                Excuse excuse{content, date, p};
                st->addExcuse(excuse);
            }
            if (line == "Rodzice:") {
                in.seekg(-(static_cast<std::streamoff>(line.size() + 1)), std::ios::cur);
            }
        } else if (line == "Rodzice:") {
            while (std::getline(in, line) && !line.empty()) {
                std::istringstream iss(line);
                std::string usr, pass, nm, srn;
                iss >> usr >> pass >> nm >> srn;
                if (usr.empty()) break;
                Parent parent(usr, pass, nm, srn);
                parents.push_back(parent);
            }
        }
    }
}

void SchoolDiary::saveToFile(const std::string& filename) const {
    std::ofstream out(filename);

    // Zapis nauczycieli
    out << "Nauczyciele:\n";
    for (const auto& t : teachers) {
        out << t.getUsername() << " "
            << t.getPassword() << " "
            << t.name << " "
            << t.surname << " "
            << t.subject;
        for (auto* st : t.students) {
            out << " " << st->id; 
        }
        out << "\n";
    }

    // Zapis studentów
    out << "Studenci:\n";
    for (const auto& s : students) {
        out << s.getUsername() << " "
            << s.getPassword() << " "
            << s.name << " "
            << s.surname << " "
            << s.id << "\n";
    }

    // Zapis ocen
    out << "Oceny:\n";
    for (const auto& s : students) {
        for (const auto& g : s.grades) {
            out << s.id << " "
                << g.value << " "
                << g.date << " "
                << (g.teacher ? g.teacher->getUsername() : "") << " "
                << g.subject << "\n";
        }
    }

    // Zapis notatek
    out << "Notatki:\n";
    for (const auto& s : students) {
        for (const auto& n : s.notes) {
            out << s.id << " "
                << n.content << " "
                << n.date << " "
                << (n.teacher ? n.teacher->getUsername() : "") << "\n";
        }
    }

    // Zapis usprawiedliwień
    out << "Usprawiedliwienia:\n";
    for (const auto& s : students) {
        for (const auto& e : s.excuses) {
            out << s.id << " "
                << e.content << " "
                << e.date << " "
                << (e.parent ? e.parent->getUsername() : "") << "\n";
        }
    }

    // Zapis rodziców
    out << "Rodzice:\n";
    for (const auto& p : parents) {
        out << p.getUsername() << " "
            << p.getPassword() << " "
            << p.name << " "
            << p.surname;
        for (auto* ch : p.children) {
            out << " " << ch->id;
        }
        out << "\n";
    }

    out.close();
}

void SchoolDiary::assignStudentToTeacher(Student* student, Teacher* teacher) {
    if (student && teacher) {
        teacher->addStudent(student);
    }
}

void SchoolDiary::loadAssignmentsFromDatabase(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return; // Brak pliku, pomiń

    std::string line;
    while (std::getline(in, line)) {
        if (line == "Nauczyciele:") {
            while (std::getline(in, line) && !line.empty() && line != "Studenci:") {
                std::istringstream iss(line);
                std::string usr, pass, nm, srn, subj;
                iss >> usr >> pass >> nm >> srn >> subj;
                Teacher* teacher = findTeacherByUsername(usr);
                if (!teacher) continue;

                int stId;
                while (iss >> stId) {
                    Student* student = nullptr;
                    for (auto& s : students) {
                        if (s.id == stId) {
                            student = &s;
                            break;
                        }
                    }
                    if (student) {
                        teacher->addStudent(student);
                    }
                }
            }
        }
    }
}

void SchoolDiary::loadParentAssignmentsFromDatabase(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return; // Brak pliku, pomiń

    std::string line;
    while (std::getline(in, line)) {
        if (line == "Rodzice:") {
            while (std::getline(in, line) && !line.empty()) {
                std::istringstream iss(line);
                std::string usr, pass, nm, srn;
                iss >> usr >> pass >> nm >> srn;
                Parent* parent = findParentByUsername(usr);
                if (!parent) continue;

                int stId;
                while (iss >> stId) {
                    Student* student = nullptr;
                    for (auto& s : students) {
                        if (s.id == stId) {
                            student = &s;
                            break;
                        }
                    }
                    if (student && parent) {
                        parent->addChild(student);
                        student->setParent(parent);
                    } else {
                        if (!student) {
                            std::cerr << "Błąd: Nie można wczytać danych studenta o ID: " << stId << std::endl;
                        }
                        if (!parent) {
                            std::cerr << "Błąd: Nie można wczytać danych rodzica o nazwie użytkownika: " << usr << std::endl;
                        }
                    }
                }
            }
        }
    }
}