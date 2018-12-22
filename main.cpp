#include <iomanip>
#include <vector>
#include <random>
#include "pcg_random.hpp"
#include <thread>

using namespace std;

const int num_thr = 3;
const int N = 300000;


void MC(pcg64& rng, int& sucsess, int n)
{
	std::uniform_real_distribution<double> unif(-1, 1);
	sucsess = 0;
	double x, y;
	for (int i = 0; i < n; ++i)
	{
		x = unif(rng);
		y = unif(rng);
		if (x * x + y * y < 1)
			++sucsess;
	}
}


int main()
{
	int sucsess = 0;
	cout << "This programm calculate area of a circle." << endl;
	pcg_extras::seed_seq_from<random_device> seed_source;
	pcg64 rng(seed_source); 

	//******************One thread
	clock_t tim = clock();
	pcg64 rng_save = rng;
	MC(rng, sucsess, N);
	double area = 4 * (double)sucsess / N;
	cout << "N = " << N << ", 1 thread, time: " << (double)(clock() - tim) / CLOCKS_PER_SEC << ", result = " << setprecision(10) << area << endl;

	//******************Many threads
	vector<int> vec_sucsess(num_thr);
	rng = rng_save;
	vector<pcg64> vec_rng(num_thr);
	for (int i = 0; i < num_thr; ++i)
	{
		vec_rng[i] = rng;
		rng.discard(2 * N / num_thr);	//jump ahead
	}

	vector<thread> vec_thr(num_thr);

	tim = clock();
	for (int i = 0; i < num_thr; ++i)
	{
		vec_thr[i] = (thread(MC, ref(vec_rng[i]), ref(vec_sucsess[i]), N / num_thr));
	}
	int s = 0;
	for (int i = 0; i < num_thr; ++i)
	{
		vec_thr[i].join();
		s += vec_sucsess[i];
	}
		

	area = 4 * (double)s / N;

	cout << "N = " << N << ", num of threads = " << num_thr << ", time: " << (double)(clock() - tim) / CLOCKS_PER_SEC << ", result = " << setprecision(10) << area << endl;
	system("pause");
	return 0;
}
