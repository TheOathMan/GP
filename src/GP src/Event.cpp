#include <map>
#include "Event.h"
#include "AppCore.h"


struct Signature {
	callback func;
	Event_ID id;
	bool ConnectOnce;
};
// elements are sorted (map)
using EventMap = std::multimap<const std::type_info*, const Signature>;
static EventMap eventMap;
//EventMap Event::Impl::eventMap;
int id=0;


int Event::_impl_Connect(const callback& fn, const std::type_info* tn)
{
	Signature sg;
	sg.func = fn; sg.id = ++id;
	eventMap.emplace(tn, sg);
	return id;
}



void Event::Disconnect(const Event_ID id)
{
	EventMap::iterator it;
	for (it = eventMap.begin(); it != eventMap.end(); ++it)
		if (id == (*it).second.id)
			break;

	eventMap.erase(it);
}

void Event::Disconnect(const EventType& Event, const int index)
{
	auto range = eventMap.equal_range(&typeid(Event));
	int ind = 0;
	EventMap::iterator it;
	for (it = range.first; it != range.second; ++it) {
		if (ind == index) break;
		ind++;
	}
	if(it != range.second) eventMap.erase(it);
}

void Event::Notify(const EventType& Event)
{
	// return pair of lower and upper bounds in the range
	auto range = eventMap.equal_range(&typeid(Event));
	for (auto it = range.first; it != range.second; ++it) {
		it->second.func(Event);
	}

}

void Event::Notify_Once(const EventType& Event)
{
	if (eventMap.find(&typeid(Event)) == eventMap.end()) return;

	Notify(Event);
	Clear(Event);
}

void Event::Clear(const EventType& Event)
{
	auto range = eventMap.equal_range(&typeid(Event));
	eventMap.erase(range.first, range.second);
}


