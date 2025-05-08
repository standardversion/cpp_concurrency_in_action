#include <thread>

struct widget_id
{};
struct widget_data
{};

void update_data_for_widget(widget_id w, widget_data& data)
{

}

/*
Although update_data_for_widget expects the second parameter to be passed by
reference, the std::thread constructor doesn’t know that; it’s oblivious to the
types of the arguments expected by the function and blindly copies the supplied values.
When it calls update_data_for_widget, it will end up passing a reference to
the internal copy of data and not a reference to data itself. Consequently, when the
thread finishes, these updates will be discarded as the internal copies of the supplied
arguments are destroyed, and process_widget_data will be passed an unchanged
data rather than a correctly updated version. For those of you familiar with
std::bind, the solution will be readily apparent: you need to wrap the arguments that
really need to be references in std::ref. In this case, if you change the thread invocation
to
std::thread t(update_data_for_widget,w,std::ref(data));
and then update_data_for_widget will be correctly passed a reference to data rather
than a reference to a copy of data.
*/

void oops(widget_id w)
{
	widget_data data{};
	std::thread t{ update_data_for_widget, w, data };

	//std::thread t(update_data_for_widget, w, std::ref(data)); // correct
	display_status();
	t.join();
	process_widget_data(data);
}


/*
This code will invoke my_x.do_lengthy_work() on the new thread, because the
address of my_x is supplied as the object pointer. You can also supply arguments to
such a member function call: the third argument to the std::thread constructor will
be the first argument to the member function and so forth
*/
class X
{
public:
	void do_lengthy_work();
};

X my_x;
std::thread t(&X::do_lengthy_work, &my_x);