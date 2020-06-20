#include <iostream>
#include <cmath>
#include <iomanip>
#include <thread>
#include <mutex>
#include <locale>
#include <chrono>

#define M_PI	3.14159265358979323846

using namespace std;
using namespace chrono;

mutex gmutex;

double time_right = 0;
double time_left = 0;
double I_right;
double I_left;

double f(double x) {
	return 4 / (1 + pow(x, 2));
}

void result(int n) {
	double a = 0, b = 1;
	auto t1 = high_resolution_clock::now();
	double sum = 0;
	double h = (b - a) / n;
	double x = a + h;
	for (int i = 0; i < n; i++, x += h)
	{
		sum += f(x);
	}
	auto t2 = high_resolution_clock::now();
	cout << "Точность (правого): "<< setprecision(10) << fixed << (1-abs(h * sum - M_PI))*100 <<" %" << endl;
	duration<double> duration = (t2 - t1);
	cout << "Время выполнения (правого): " << setprecision(5) << duration.count() << " сек" << endl;
	cout << "-----------------------------------------------" << endl;
	t1 = high_resolution_clock::now();
	sum = 0;
	h = (b - a) / n;
	a += h;
	for (int i = 0; i < n; i++, a += h)
	{
		sum += f(a);
	}
	t2 = high_resolution_clock::now();
	cout << "Точность (левого) = " << setprecision(10) << fixed << (1 - abs(M_PI - h * sum)) * 100 << " %" << endl;
	duration = (t2 - t1);
	cout << "Время выполнения (левого): " << setprecision(5) << duration.count() << " сек" << endl;
}

void result_qpar(int n) {
	double a = 0, b = 1;
	auto t1 = high_resolution_clock::now();
	double sum = 0;
	double h = (b - a) / n;
	double x = a + h;
#pragma loop(hint_parallel(4))
	for (int i = 0; i < n; i++, x += h)
	{
		sum += f(x);
	}
	auto t2 = high_resolution_clock::now();
	cout << "Точность (правого): " << setprecision(10) << fixed << (1 - abs(h * sum - M_PI)) * 100 << " %" << endl;
	duration<double> duration = (t2 - t1);
	cout << "Время выполнения (правого): " << setprecision(5) << duration.count() << " сек" << endl;
	cout << "-----------------------------------------------" << endl;
	t1 = high_resolution_clock::now();
	sum = 0;
	h = (b - a) / n;
	a += h;
#pragma loop(hint_parallel(4))
	for (int i = 0; i < n; i++, a += h)
	{
		sum += f(a);
	}
	t2 = high_resolution_clock::now();
	cout << "Точность (левого) = " << setprecision(10) << fixed << (1 - abs(M_PI - h * sum)) * 100 << " %" << endl;
	duration = (t2 - t1);
	cout << "Время выполнения (левого): " << setprecision(5) << duration.count() << " сек" << endl;
}

void result_novector(int n) {
	double a = 0, b = 1;
	auto t1 = high_resolution_clock::now();
	double sum = 0;
	double h = (b - a) / n;
	double x = a + h;
#pragma loop(no_vector)
	for (int i = 0; i < n; i++, x += h)
	{
		sum += f(x);
	}
	auto t2 = high_resolution_clock::now();
	cout << "Точность (правого): " << setprecision(10) << fixed << (1 - abs(h * sum - M_PI)) * 100 << " %" << endl;
	duration<double> duration = (t2 - t1);
	cout << "Время выполнения (правого): " << setprecision(5) << duration.count() << " сек" << endl;
	cout << "-----------------------------------------------" << endl;
	t1 = high_resolution_clock::now();
	sum = 0;
	h = (b - a) / n;
	a += h;
#pragma loop(no_vector)
	for (int i = 0; i < n; i++, a += h)
	{
		sum += f(a);
	}
	t2 = high_resolution_clock::now();
	cout << "Точность (левого) = " << setprecision(10) << fixed << (1 - abs(M_PI - h * sum)) * 100 << " %" << endl;
	duration = (t2 - t1);
	cout << "Время выполнения (левого): " << setprecision(5) << duration.count() << " сек" << endl;
}

void compute4threads(double a, double b, int n) {
	auto t1 = high_resolution_clock::now();
	double sum = 0;
	double h = (b - a) / n;
	double x = a + h;
	for (int i = 0; i < n; i++, x += h)
	{
		sum += f(x);
	}
	auto t2 = high_resolution_clock::now();

	gmutex.lock();
	I_right += (h * sum);
	duration<double> duration = (t2 - t1);
	time_right += duration.count();
	gmutex.unlock();

	t1 = high_resolution_clock::now();
	sum = 0;
	h = (b - a) / n;
	a += h;
	for (int i = 0; i < n; i++, a += h)
	{
		sum += f(a);
	}
	t2 = high_resolution_clock::now();
	gmutex.lock();
	I_left += (h * sum);
	duration = (t2 - t1);
	time_left += duration.count();
	gmutex.unlock();
}

int main() {
	setlocale(LC_ALL, "Rus");
	for (int i = 100; i < 1000001; i *= 10) {
		cout << "Интервал разбиения: " << i << endl;
		cout << endl;
		cout << "Без исользования потоков:" << endl;
		result(i);
		cout << "***********************************************" << endl;
		cout << "С использованием Qpar:" << endl;
		result_qpar(i);
		cout << "***********************************************" << endl;
		cout << "С использованием автоматической векторизацией:" << endl;
		result_novector(i);
		I_right = 0;
		I_left = 0;
		time_right = 0;
		time_left = 0;
		cout << "***********************************************" << endl;
		cout << "С использованием thread и mutex: "<<endl;
		thread t1(compute4threads, 0.0, 0.2, i);
		thread t2(compute4threads, 0.2, 0.4, i);
		thread t3(compute4threads, 0.4, 0.6, i);
		thread t4(compute4threads, 0.6, 0.8, i);
		thread t5(compute4threads, 0.8, 1, i);
		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
		cout << "Точность (правого) = " << setprecision(10) << fixed << (1 - abs(M_PI - I_right)) * 100 << " %" << endl;
		cout << "Время выполнения (правого): " << setprecision(5) << time_right << " сек" << endl;
		cout << "-----------------------------------------------" << endl;
		cout << "Точность (левого) = " << setprecision(10) << fixed << (1 - abs(M_PI - I_left)) * 100 << " %" << endl;
		cout << "Время выполнения (левого): " << setprecision(5) << time_left << " сек" << endl;
		cout << endl;
	}

	system("pause");
}
