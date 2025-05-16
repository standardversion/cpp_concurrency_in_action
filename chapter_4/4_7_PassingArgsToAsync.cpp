#include <string>
#include <future>

struct X
{
	void foo(int, const std::string&);
	std::string bar(const std::string&);
};

X x;
auto f1{ std::async(&X::foo, &x, 42, "hello") }; // calls p->foo(42, "hello") where p is &x
auto f2{ std::async(&X::bar, x, "goodbye") }; // calls tmpx.bar("goodbye") where tmpx is a copy of x

struct Y
{
	double operator() (double);
};
Y y;
auto f3{ std::async(Y(), 3.141) }; // calls tmpy(3.141) where tmpy is move-constructed from Y()
auto f4{ std::async(std::ref(y), 2.718) }; // call y(2.718)

X baz(X&);
auto f5{ std::async(baz, std::ref(x)) }; // calls baz(x)

class move_only
{
public:
	move_only();
	move_only(move_only&&);
	move_only(const move_only&) = delete;
	move_only& operator=(move_only&&);
	move_only& operator=(const move_only&) = delete;

	void operator()();
};
auto f6{ std::async(move_only()) };