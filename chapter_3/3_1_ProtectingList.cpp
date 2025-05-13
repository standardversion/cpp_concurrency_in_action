#include <list>
#include <mutex>
#include <algorithm>

/*
The use of std::lock_guard<std::mutex> in
add_to_list() and again in list_contains() means that the accesses in these
functions are mutually exclusive: list_contains() will never see the list partway
through a modification by add_to_list().
*/
std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value)
{
	//lock_guard locks the supplied mutex on construction and unlocks it
	//on destruction, thus ensuring a locked mutex is always correctly unlocked
	std::lock_guard<std::mutex> guard(some_mutex); 
	// don't need to explicitly connect mutex with list
	// anything within the scope fo the lock_guard will be protected
	some_list.push_back(new_value);
}

bool list_contains(int value_to_find)
{
	std::lock_guard<std::mutex> guard(some_mutex);
	return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

/*
Although there are occasions where this use of global variables is appropriate, in
the majority of cases it’s common to group the mutex and the protected data together
in a class rather than use global variables.This is a standard application of objectoriented
design rules : by putting them in a class, you’re clearly marking them as
related, and you can encapsulate the functionality and enforce the protection.In this
case, the functions add_to_list and list_contains would become member functions
of the class, and the mutex and protected data would both become private
members of the class, making it much easier to identify which code has access to the
data and thus which code needs to lock the mutex.If all the member functions of
the class lock the mutex before accessing any other data members and unlock it when
done, the data is nicely protected from all comers.
Well, that’s not quite true, if one of the
member functions returns a pointer or reference to the protected data, then it
doesn’t matter that the member functions all lock the mutex in a nice orderly fashion,
because you’ve just blown a big hole in the protection.Any code that has access to that
pointer or reference can now access(and potentially modify) the protected data without locking the
mutex.Protecting data with a mutex therefore requires careful interface design, to
ensure that the mutex is locked before there’s any access to the protected data and
that there are no backdoors.
*/