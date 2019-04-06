#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <fmt/format.h>

using namespace std;

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
	o << fmt::format("Index={}, Release={}, Finish={}\n", j.job, j.r, j.f);
	return o;
}

bool operator<(const Job& a, const Job& b)
{
	return a.r < b.r;
}

int main(int argc, const char* argv[]) {
	vector<Job> job;
	priority_queue<Job> remainingJobs;
	priority_queue<int> blocks;

	// open a file.
	fstream file;
	file.open("JobList.txt");

	// cout << "Writing to the file" << endl;
	// cout << "Enter your name: "<< endl;
	// cout

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

	for (size_t i = 1; i < job.size(); i++)
	{
		cout << job[i] <<endl;
	}

	vector<vector<int>> jobNum;
	vector<vector<int>> time;
	vector<vector<int>> overlappedJobs;

	for (int i = job.size()-1; i >=0; i--)
	{
		remainingJobs.push(job[i]);
	}

	cout << remainingJobs.top().job;

	// while (!remainingJobs.empty())
	// {
	// 	int i = remainingJobs.top().job;
	// 	int maxNum = 1;
	// 	int maxj = 1;
	// 	for (int j = 0; j < job[i].f-job[i].r; j++)
	// 	{
	// 		remainingJobs.pop();
	// 		jobNum[i][j] = 1;
	// 		time [i][j] = job[i].r + j;
	// 		while (!remainingJobs.empty())
	// 		{
	// 			int k = remainingJobs.top().job;
	// 			if (job[k].r<=time[i][j])
	// 			{
	// 				jobNum[i][j]++;
	// 				// overlappedJobs[i][j] = k;
	// 			}
	// 		}
	// 		if (jobNum[i][j]>maxNum)
	// 		{
	// 			maxNum = jobNum[i][j];
	// 			maxj = j;
	// 		}
	// 	}
	// 	blocks.push(time[i][maxj]);
	// 	for (int n = 1; n < maxNum; n++)
	// 	{
	// 		remainingJobs.pop();
	// 	}
	// }

	return 0;

}