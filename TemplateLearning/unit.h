
#ifndef UNIT_H
#define UNIT_H

#include "distance.h"

namespace Distance {

	namespace Unit {
		MyDistance operator "" _km(long double d) {
			return MyDistance(1000 * d);
		}
		MyDistance operator "" _m(long double m) {
			return MyDistance(m);
		}
		MyDistance operator "" _dm(long double d) {
			return MyDistance(d / 10);
		}
		MyDistance operator "" _cm(long double c) {
			return MyDistance(c / 100);
		}
	}
}

#endif