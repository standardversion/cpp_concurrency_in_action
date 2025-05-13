#include <mutex>

class X
{
private:
	connection_info connection_details;
	connection_handle connection;
	std::once_flag connection_init_flag;
	
	void open_connection()
	{
		connection = connection_manager.open(connection_details);
	}
public:
	X(const connection_info& connection_details_)
		: connection_details{ connection_details_ }
	{ }

	void send_data(const data_packet& data)
	{
		// initialization done here
		std::call_once(connection_init_flag, &X::open_connection, this);
		connection.send_data(data);
	}

	data_packet receive_data()
	{
		// or initialization done here
		std::call_once(connection_init_flag, &X::open_connection, this);
		return connection.receive_data();
	}
};