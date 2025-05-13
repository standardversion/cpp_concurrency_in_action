#include <map>
#include <string>
#include <shared_mutex>
class dns_entry;
class dns_cache
{
	std::map<std::string, dns_entry> entries;
	mutable std::shared_mutex entry_mutex;
public:
	dns_entry find_entry(std::string const& domain) const
	{
		std::shared_lock<std::shared_mutex> lk(entry_mutex);
		std::map<std::string, dns_entry>::const_iterator const it =
			entries.find(domain);
		return (it == entries.end()) ? dns_entry() : it->second;
	}
	void update_or_add_entry(std::string const& domain,
		dns_entry const& dns_details)
	{
		std::lock_guard<std::shared_mutex> lk(entry_mutex);
		entries[domain] = dns_details;
	}
};

/*
find_entry() uses an instance of std::shared_lock<> to protect it
for shared, read-only access; multiple threads can therefore call find_entry()
simultaneously without problems. On the other hand, `update_or_add_entry()` uses
an instance of std::lock_guard<> to provide exclusive access while the table is
updated; not only are other threads prevented from doing updates in a call update_or_add_entry(),
but threads that call find_entry() are blocked too.
*/