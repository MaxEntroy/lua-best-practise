#ifndef LUABESTPRACTISE_CHAPTER21_LUACALLCPP_DEMO01_INC_STUDENT_H_
#define LUABESTPRACTISE_CHAPTER21_LUACALLCPP_DEMO01_INC_STUDENT_H_
#include <string>

class Student {
public:
    Student() : id_(-1) {}
    Student(int id, const std::string& name) : id_(id), name_(name) {}

    int id() const {return id_;}
    std::string name() const {return name_;}

private:
    int id_;
    std::string name_;
};

#endif
