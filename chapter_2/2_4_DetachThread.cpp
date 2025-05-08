#include <thread>
#include <string>


/*If the user chooses to open a new document, you prompt them for the document to
open, start a new thread to open that document, and then detach it. Because
the new thread is doing the same operation as the current thread but on a different
file, you can reuse the same function(edit_document) with the newly chosen filename
as the supplied argument.*/
void edit_document(const std::string& filename)
{
	open_document_and_display_gui(filename);
	while (!done_editing())
	{
		user_command cmd{ get_user_input() };
		if (cmd.type == open_new_document)
		{
			const std::string new_name{ get_filename_from_user() };
			std::thread t(edit_document, new_name);
			t.detach();
		}
		else
		{
			process_user_input(cmd);
		}
	}
}