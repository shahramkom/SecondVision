#include <iostream>
#include <omp.h>
#include <complex>
#include <cstdio>
#include <chrono>

typedef std::complex<double> complex;

int MandelbrotCalculate(complex c, int maxiter)
{
	// iterates z = z + c until |z| >= 2 or maxiter is reached,
	// returns the number of iterations.
	complex z = c;
	int n = 0;
	for (; n < maxiter; ++n)
	{
		if (std::abs(z) >= 2.0) break;
		z = z * z + c;
	}
	return n;
}
int mainOMP()
{
	const int width = 78, height = 44, num_pixels = width * height;

	const complex center(-.7, 0), span(2.7, -(4 / 3.0) * 2.7 * height / width);
	const complex begin = center - span / 2.0;//, end = center+span/2.0;
	const int maxiter = 1000;

#pragma omp parallel for ordered schedule(dynamic)
	for (int pix = 0; pix < num_pixels; ++pix)
	{
		const int x = pix % width, y = pix / width;

		complex c = begin + complex(x * span.real() / (width + 1.0),
			y * span.imag() / (height + 1.0));

		int n = MandelbrotCalculate(c, maxiter);
		if (n == maxiter) n = 0;

#pragma omp ordered
		{
			char c = ' ';
			if (n > 0)
			{
				static const char charset[] = ".,c8M@jawrpogOQEPGJ";
				c = charset[n % (sizeof(charset) - 1)];
			}
			std::putchar(c);
			if (x + 1 == width) std::puts("|");
		}
	}

#pragma omp parallel for
	for (int n = 0; n < 10; ++n) 
		printf(" %d", n);
	printf(".\n");

	auto startPar = std::chrono::steady_clock::now().time_since_epoch().count();
	unsigned long long smm = 0;
	int n;
#pragma omp for schedule(dynamic, 3)
		for (int n = 0; n < 10000000; ++n)
			//printf(" %d", n);
			smm += n;
	auto endPar = std::chrono::steady_clock::now().time_since_epoch().count();
	std::cout << "Duration parallel:" << (endPar - startPar) << std::endl << std::endl;
	printf("%llu.\n",smm);

	return getchar();
}