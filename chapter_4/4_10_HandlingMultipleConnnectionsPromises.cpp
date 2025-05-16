#include <future>

void process_connections(connection_set& connections)
{
	while (!done(connections))
	{
		for (connection_iterator connection{ connections.begin() }, end{ connections.end() }; connection != end; ++connection)
		{
			if (connection->has_incoming_data())
			{
				data_packet data{ connection->incoming() };
				std::promise<payload_type>& p{ connection->get_promise(data.id) };
				p.set_value(data.payload);
			}
			if (connection->has_outgoing_data())
			{
				outgoing_packet data{ connection->top_of_outgoing_queue() };
				connection->send(data.payload);
				data.promise.set_value(true);
			}
		}
	}
}

// connection class would have this:
std::future<payload_type> connection::send_request(payload_type payload) {
	packet_id id = generate_unique_id();

	// Create promise/future pair
	std::promise<payload_type> p;
	std::future<payload_type> fut = p.get_future();

	// Store the promise so it can be fulfilled later
	pending_promises[id] = std::move(p);

	// Send request over network with ID and payload
	queue_outgoing_packet(outgoing_packet{ id, payload });

	return fut;
}

// client code would look like
std::future<payload_type> response = connection.send_request(my_payload);
// later...
auto result = response.get();  // will block until process_connections() fulfills the promise
