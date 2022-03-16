#pragma once
#include <typeinfo>
#include "AppCore.h"

#define REGISTER_EVENT(even_name)\
struct even_name : EventType {\
	even_name() {};\
}


struct EventType {
	virtual ~EventType() {}
};

typedef int Event_ID;
typedef void(*callback)(const EventType&);


class Event {

public:
	// connect an event, return an event_ID
	template<typename EventWanted>
	static Event_ID Connect(const callback& fn) {
		return _impl_Connect(fn, &typeid(EventWanted) );
	}


	// disconnect a single connected event bsed on an event ID.
	static void Disconnect(const Event_ID id);
	// disconnect a single connected event bsed on an event ID.
	static void Disconnect(const EventType& Event, const int index);
	// Notify an event each call
	static void Notify(const EventType& Event);
	// Notify an event upon a call then delete the event.
	static void Notify_Once(const EventType& Event);
	// clear an event
	static void Clear(const EventType& Event);

private:
	//hidden impls
	static int _impl_Connect(const callback& fn, const std::type_info* tn);
};

///---------------- Events names

//quick events
REGISTER_EVENT(TesTEvent);
REGISTER_EVENT(OnRenderingPageGlyphs);
REGISTER_EVENT(OnGlyphPreviewRender);
REGISTER_EVENT(OnAllFontsClear);
REGISTER_EVENT(OnDeleteFontData);
REGISTER_EVENT(OnEditPageEntered);   // Edit glyph image
REGISTER_EVENT(OnSavePageEntered);   // save image
REGISTER_EVENT(OnGlyphPageEntered);  // glyph detail


// specific events
struct OnPreviewWinInitialized : EventType {
	void* window;
	OnPreviewWinInitialized(void* window) :window(window) {};
};
struct OnFontsLoading : EventType {
	int count;
	const char** paths;
	FontLoadSource fsource;
	OnFontsLoading(int count, const char** paths,FontLoadSource fsource = FontLoadSource::None) :count(count), paths(paths),fsource(FontLoadSource::None){};
};
struct OnWindowResize : EventType {
	int n_width;
	int n_height;
	OnWindowResize(int n_width, int n_height) :n_width(n_width), n_height(n_height) {};
};
struct OnScrolling : EventType {
	int horizontal;
	int vertical;
	OnScrolling(int horizontal, int vertical) :horizontal(horizontal), vertical(vertical) {};
};
struct OnWindowFocus : EventType {
	int isFocused;
	OnWindowFocus(int isFocused) :isFocused(isFocused){};
};


