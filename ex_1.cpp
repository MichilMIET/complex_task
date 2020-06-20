#include <iostream>
#include <cmath>
#include <iomanip>
#include <chrono>

#define M_PI	3.14159265358979323846
using namespace std;
using namespace chrono;

double f(double x) {
	return 4 / (1+pow(x,2));
}

void result(double a, double b, int n) {
	auto t1 = high_resolution_clock::now();
	double sum = 0;
	double h = (b - a) / n;
	double x = a + h;
	for (int i = 0; i < n; i++, x += h)
	{
		sum += f(x);
	}
	auto t2 = high_resolution_clock::now();
	cout << "Результат (правого): "<<setprecision(25) << h * sum << endl;
	cout << "Точность (правого): "<< setprecision(10) <<fixed<<(1-abs(M_PI- h * sum))*100<<" %"<< endl;
	duration<double> duration = (t2 - t1);
	cout << "Время выполнения (правого): " << setprecision(5) << duration.count() << " сек" << endl;
	cout << "-----------------------------------------------" << endl;
	 t1 = high_resolution_clock::now();
    sum = 0;
    h = (b - a) / n;
	a+=h; 
	for (int i = 0; i < n; i++, a += h)
	{
		sum += f(a);
	}
	 t2 = high_resolution_clock::now();
	cout << "Результат (левого): " << setprecision(25) << h * sum << endl;
	cout << "Точность (левого): " <<  setprecision(10) << fixed << (1-abs(M_PI - h * sum))*100 << " %" << endl;
	 duration = (t2 - t1);
	cout << "Время выполнения (левого): " << setprecision(5) << duration.count() << " сек" << endl;
}

int main() {
	setlocale(LC_ALL, "Rus");
	for (int i = 100; i < 1000001; i *= 10) {
		cout << "Интервал разбиения: " << i << endl;
	    result(0, 1, i);
		cout << endl;
	}
	system("pause");
}
