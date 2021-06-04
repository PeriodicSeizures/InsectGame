#include <iostream>

struct Base {
	virtual ~Base() {

	}

};

struct IntBase {
	//virtual ~IntBase() {
	//
	//}
	virtual void something() {
		std::cout << "IntBase: something()\n";
	}
};

struct A : Base, IntBase {


	void something() override {
		std::cout << "A: something()\n";
	}
};

int main() {
	Base* base = new A();
	//auto&& int_base = static_cast<A*>(base);
	auto&& int_base2 = static_cast<IntBase*>(base);
	int_base->something();
}