#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <fmt/format.h>

using namespace std;
//
//void bubble_sort(vector<int>& r_nums, vector<int>& f_nums)
//{
//	for (size_t i = 0; i < r_nums.size() - 1; i++) { // times
//		for (size_t j = 0; j < r_nums.size() - i - 1; j++) { // position
//			if (r_nums[j] > r_nums[j + 1]) {
//				int r_temp = r_nums[j];
//				r_nums[j] = r_nums[j + 1];
//				r_nums[j + 1] = r_temp;
//
//				int f_temp = f_nums[j];
//				f_nums[j] = f_nums[j + 1];
//				f_nums[j + 1] = f_temp;
//			}
//		}
//	}
//}

struct Job
{
	int job;
	int r;
	int f;
};

std::istream& operator>>(std::istream& i, Job& j)
{
	i >> j.r >> j.f;
	return i;
}

std::ostream& operator<<(std::ostream& o, Job& j)
{
	o << fmt::format("Index={}, Rel={}, Finish={}\n", j.job, j.r, j.f);
	return o;
}

bool operator<(const Job& a, const Job& b)
{
	return a.r < b.r;
}

int main(int argc, const char* argv[]) {
	char c, data[100];
	int number;
	vector<Job> job;
	priority_queue<int> remainingJobs;
	priority_queue<int> blocks;

	// open a file.
	fstream file;
	file.open("JobList.txt");

	cout << "Writing to the file" << endl;
	cout << "Enter your name: "<< endl;

	{
		int i = 1;
		while (file)
		{
			file >> job.emplace_back();
			job.back().job = i;
			++i;
		}
	}


	std::sort(job.begin(), job.end());


	return 0;

	//int jobNum[100][100];
	//int time[100][100];
	//int overlappedJobs[100][100];

	//for (int i = 0; i < r.size(); i++)
	//{
	//	remainingJobs.push(i);
	//}

	//while (!remainingJobs.empty())
	//{
	//	int i = remainingJobs.top();
	//	int maxNum = 1;
	//	int maxj = 1;
	//	for (int j = 0; j < f[i]-r[i]; j++)
	//	{
	//		remainingJobs.pop();
	//		jobNum[i][j] = 1;
	//		time [i][j] = r[i] + j;
	//		while (!remainingJobs.empty())
	//		{
	//			int k = remainingJobs.top();
	//			if (r[k]<=time[i][j])
	//			{
	//				jobNum[i][j]++;
	//				//overlappedJobs[i][j] = k;
	//			}
	//		}
	//		if (jobNum[i][j]>maxNum)
	//		{
	//			maxNum = jobNum[i][j];
	//			maxj = j;
	//		}
	//	}
	//	blocks.push(time[i][maxj]);
	//	for (int n = 1; n < maxNum; n++)
	//	{
	//		remainingJobs.pop();
	//	}
	//}

	//for (size_t i = 0; i < r.size(); i++)
	//{
	//	cout << r[i] << endl;
	//}
	//return 0;

}