#pragma once
#include "Declaration.h"

Event event = EventData(136, true, "Nice event", 4.88, { ScheduleItemData(1111,2222), ScheduleItemData(3333,4444) }, { 45,323,55 });
string json = event->toJSON();
Event eventCopy = EventData::fromJSON(json);
string json2 = eventCopy->toJSON();
bool ok = ((json == json2) && (eventCopy->schedule.front()->start == 1111) && (eventCopy->tags.size() == 3));

if (!ok) error = "JSON serialization check fail";
