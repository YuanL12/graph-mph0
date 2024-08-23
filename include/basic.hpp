#pragma once
#include <string>
struct Pet {
	Pet(const std::string &name, int age) : name(name), age(age) { }
	Pet(const std::string &name) : name(name), age(0) { }
	
	void setName(const std::string &name_) { name = name_; }
	const std::string &getName() const { return name; }
	void setAge(int age_) {age = age_; }
	int getAge() const { return age; }
	
	// overload
	void set(int age_) { age = age_; }
	void set(const std::string &name_) { name = name_; }
	std::string name;
	int age;

};

struct Dog : Pet {
	Dog(const std::string &name) : Pet(name) { }
	std::string bark() const { return "woof!"; }
};

int add(int i, int j) {
	return i + j;
}