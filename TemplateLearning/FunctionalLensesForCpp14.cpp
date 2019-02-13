#include "includes.h"
#include "Serialisation.h"

template <typename Function>
struct function_traits
	: public function_traits<decltype(&Function::operator())>
{};

// Functional lens: G(T)->F + S(T,F)->T
template <typename G, typename S,
	typename T = decay_t< typename function_traits<S>::result >,
	typename F = decay_t< typename function_traits<G>::result >
>
class Lens {
private:
	G getter;
	S setter;
public:
	// Constructor - provide getter and setter
	Lens(G&& g, S&& s) : getter(forward<G>(g)), setter(forward<S>(s)) {}
	// getter
	F operator()(T holder) const { return getter(holder); };
	// setter
	T set(T holder, F&& value) const { return setter(holder, forward<F>(value)); }
	T set(T holder, const F& value) const { return setter(holder, value); }
};

// Lens builder
template <typename G, typename S>
auto make_lens(G&& g, S&& s) {
	return Lens<G, S>(forward<G>(g), forward<S>(s));
}


class ScheduleItemData : public IImmutable {
public:
	const int id;
	const time_t start;
	const time_t finish;
	SERIALIZE_JSON(ScheduleItemData, id, start, finish);
};


using ScheduleItem = ScheduleItemData::Ptr;

class EventData : public IImmutable {
public:
	const int id;
	const bool isPublic;
	const string title;
	const double rating;
	const vector<ScheduleItem> schedule;
	const vector<int> tags;

	SERIALIZE_JSON(EventData, id, isPublic, title, rating, schedule, tags);
};

auto lensTitle = make_lens([](Event e) { return e->title; }, [](Event e, string title) { return e->set_title(title); });

// Sample event:            
Event event = EventData(136, true, "Nice event", 4.88, { ScheduleItemData(1,1111,2222), ScheduleItemData(2,3333,4444) }, { 45,323,55 });

cout << lensTitle(event) << endl;
// Nice event

auto correctedEvent = lensTitle.set(event, "Very nice event");
cout << correctedEvent->title << endl;
// Very nice event


// Composition of lenses
template<typename G1, typename S1, typename T1, typename F1,
	typename G2, typename S2, typename T2, typename F2>
	iendline auto operator|(Lens<G1, S1, T1, F1> lens1, Lens<G2, S2, T2, F2> lens2)
{
	return make_lens([=](T1 holder)->F2 {
		return lens2(lens1(holder));
	},
		[=](T1 holder, F2 value)->T1 {
		return lens1.set(holder, lens2.set(lens1(holder), value));
	});
}

using Event = EventData::Ptr;


// Lens: look at element with specific id in list
template <typename C, typename ID, typename T = std::decay_t<decltype(std::declval<C>().front())> >
auto lens_list_item_by_id(ID id) {
	return make_lens(
		[=](C list) {
		return list | ffind >> [=](auto el) { return (el->id == id); };
	},
		[=](C list, T value) {
		return list | filter >> [=](auto el) { return (el->id != id); } | fappend >> value;
	});
}


auto lens = EventData::lens_schedule() | lens_list_item_by_id<vector<ScheduleItem>>(1) | ScheduleItemData::lens_start();

auto correctedEvent = lens.set(event, 222);
cout << lens(correctedEvent) << endl;
// 222

// Print whole structure
cout << correctedEvent->toJSON() << endl;

// {"id":136,"isPublic":true,"title":"Nice event \"3,4\"","rating":4.88,"schedule":[{"id":2,"start":3333,"finish":4444},{"id":1,"start":222,"finish":2222}],"tags":[45,323,55]}


// Lens: look at element with specific id in list
template <typename C, typename ID, typename T = std::decay_t<decltype(std::declval<C>().front())> >
auto lens_list_item_by_id(ID id) {
	return make_lens(
		[=](C list) {
		return list | ffind >> [=](auto el) { return (el->id == id); };
	},
		[=](C list, T value) {
		return list | filter >> [=](auto el) { return (el->id != id); } | fappend >> value;
	},
		[=](C list) {
		return list | fcontains >> [=](auto el) { return (el->id == id); };
	}
	);
}

// Composition of lenses
template<typename G1, typename S1, typename C1, typename T1, typename F1,
	typename G2, typename S2, typename C2, typename T2, typename F2>
	iendline auto operator|(Lens<G1, S1, C1, T1, F1> lens1, Lens<G2, S2, C2, T2, F2> lens2)
{
	return make_lens([=](T1 holder)->F2 {
		return lens2(lens1(holder));
	},
		[=](T1 holder, F2 value)->T1 {
		return lens1.set(holder, lens2.set(lens1(holder), value));
	},
		[=](T1 holder)->bool {
		if (!lens1.hasFocus(holder)) return false;
		return lens2.hasFocus(lens1(holder));
	});
}


auto defocusedLens = EventData::lens_schedule() | lens_list_item_by_id<vector<ScheduleItem>>(10) | ScheduleItemData::lens_start();

cout << "Is focused?: " << defocusedLens.hasFocus(event) << endl;

// FALSE


// Lens: find element in array by predicate
template <typename C, typename F, typename T = std::decay_t<decltype(std::declval<C>().front())> >
auto lens_find(const F & f) {
	return make_lens(
		[=](C list) {
		return fn_findone(list, f);
	},
		[=](C list, T value) {
		return list | filter >> [f](auto el) { return !f(el); } | fappend >> value;
	},
		[=](C list) {
		return list | fcontains >> f;
	}
	);
}


auto sameLens = EventData::lens_schedule() | lens_find<vector<ScheduleItem>>([](auto item) { return (item->id == 1); }) | ScheduleItemData::lens_start();
if (sameLens.hasFocus(event))
cout << "Value: " << sameLen2(event) << endl;

// Value: 1111
*/