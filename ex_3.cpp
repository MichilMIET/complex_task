#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <mutex>
#include <locale>
#include <chrono>
#include <cilk/cilk.h>
#include <vector>
#include <cilk/reducer_opadd.h>


#define M_PI	3.14159265358979323846

using namespace std;
using namespace chrono;

mutex gmutex;


double f(double x) {
	return 4 / (1 + pow(x, 2));
}

double result_right(double a, double b, int n) {
	double sum = 0;
	double h = (b - a) / n;
	double x = a + h;
	for (int i = 0; i < n; i++, x += h)
	{
		sum += f(x);
	}
	return h * sum;
}

	double result_left(double a, double b, int n) {
		double sum = 0;
		double h = (b - a) / n;
		h = (b - a) / n;
		a += h;
		for(int i = 0; i < n; i++, a += h)
		{
			sum += f(a);
		}
		return h * sum;
	}

	int main() {
		double a = 0;
		double b = 1;
		std::vector<double> borders = { a, 0.2, 0.4, 0.6, 0.8, b };
		setlocale(LC_ALL, "Rus");
		for (int i = 100; i < 1000001; i *= 10) {
			cout << "Интервал разбиения: " << i << endl;
			cout << endl;
			cilk::reducer_opadd<double> res_right(0.0);
			auto t1 = high_resolution_clock::now();
			cilk_for(int j = 0; j < borders.size() - 1; j++) {
				res_right += result_right(borders[j], borders[j + 1], i);
			}
			auto t2 = high_resolution_clock::now();
			duration<double> duration = (t2 - t1);
			cout << "Результат (правого): " << setprecision(15) << res_right.get_value() << " Время выполнения: " << setprecision(15) << duration.count() << " сек " << endl;

			cilk::reducer_opadd<double> res_left(0.0);
			t1 = high_resolution_clock::now();
			cilk_for(int j = 0; j < borders.size() - 1; j++) {
				res_left += result_left(borders[j], borders[j + 1], i);
			}
			t2 = high_resolution_clock::now();
			duration = t2 - t1;
			cout << "Результат (левого): " << setprecision(15) << res_left.get_value() << " Время выполнения: " << setprecision(15) << duration.count() << " сек " << endl;
			cout << "***********************************************" << endl;
			cout << endl;
		}
		system("pause");
	}