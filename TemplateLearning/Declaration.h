#pragma once
// Immutable serialisable data structures in C++11
// Example code from http://vitiy.info/immutable-data-and-serialisation-in-cpp11/
// Written by Victor Laskin (victor.laskin@gmail.com)

class ScheduleItemData : public IImmutable {
public:
	const time_t start;
	const time_t finish;
	SERIALIZE_JSON(ScheduleItemData, start, finish);
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


using Event = EventData::Ptr;
