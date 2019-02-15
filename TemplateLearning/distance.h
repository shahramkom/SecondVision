#ifndef DISTANCE_H
#define DISTANCE_H

#include <iostream>
#include <ostream>


namespace Distance {
	class MyDistance {
	public:
		MyDistance(double i) :m(i) {}

		friend MyDistance operator+(const MyDistance& a, const MyDistance& b) {
			return MyDistance(a.m + b.m);
		}
		friend MyDistance operator-(const MyDistance & a, const MyDistance & b) {
			return MyDistance(a.m - b.m);
		}

		friend MyDistance operator*(double m, const MyDistance & a) {
			return MyDistance(m * a.m);
		}

		friend MyDistance operator/(const MyDistance & a, int n) {
			return MyDistance(a.m / n);
		}

		friend std::ostream& operator<< (std::ostream & out, const MyDistance & myDist) {
			out << myDist.m << " m";
			return out;
		}
	private:
		double m;

	};

}

Distance::MyDistance getAverageDistance(std::initializer_list<Distance::MyDistance> inList) {
	auto sum = Distance::MyDistance{ 0.0 };
	for (auto i : inList) sum = sum + i;
	return sum / inList.size();
}


#endif