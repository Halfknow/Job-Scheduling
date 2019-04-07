#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <fmt/format.h>
#include <ctime>

using namespace std;

struct Job
{
	int job;
	int order;
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
	o << fmt::format("Order={}, Index={}, Release={}, Finish={}", j.order, j.job, j.r, j.f);
	return o;
}

bool operator<(const Job& a, const Job& b)
{
	return a.r < b.r;
}

int main(int argc, const char* argv[]) {
	vector<Job> job;
	queue<Job> remainingJobs;
	queue<Job> cur_remainingJobs;

	// open a file.
	fstream file;
	file.open("JobList.txt");

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
		job[i].order = i;
		cout << job[i] <<endl;
	}

	cout << endl;

	struct JobOverlap
	{
		int num;
		int time;
		vector<Job> overlap;
	};

	struct Block
	{
		int num;
		int time;
		vector<Job> job;
	};

	vector<vector<JobOverlap>> job_overlap;
	vector<Block> blocks;

	for (size_t i = 1; i < job.size(); i++)
	{
		remainingJobs.push(job[i]);
	}

	job_overlap.resize(remainingJobs.size());

	int cur_num = 0;

	while (!remainingJobs.empty())
	{
		int i = remainingJobs.front().order;
		int maxNum = 1;
		int maxj = 0;
		auto interval = job[i].f-job[i].r;
		assert(interval > 0);
		auto& cur_overlap = job_overlap[i];
		cur_overlap.resize(interval);
		remainingJobs.pop();
		for (int j = 0; j < interval; j++)
		{	
			cur_overlap[j].num = 1;
			cur_overlap[j].time = job[i].r + j;
			cur_overlap[j].overlap.emplace_back(job[i]);

			//Compare remaining jobs whether overlapping with the current job
			for (size_t k = remainingJobs.front().order; k <= remainingJobs.back().order; k++)
			{
				//Whether overlapping
				if (job[k].r<= cur_overlap[j].time)
				{
					cur_overlap[j].num ++;
					cur_overlap[j].overlap.emplace_back(job[k]);
				}
			}
			//Whether the number of overlapped jobs is maximum
			if (cur_overlap[j].num > maxNum)
			{
				maxNum = cur_overlap[j].num;
				maxj = j;
			}
		}
		//blocks.emplace(job_overlap[i][maxj].overlap);
		for (int n = 1; n < maxNum; n++)
		{
			remainingJobs.pop();
		}

		//Blocks information
		blocks.resize(job.size());
		for (size_t b = 0; b < maxNum; b++) {
			blocks[cur_num].job.emplace_back(job_overlap[i][maxj].overlap[b]);
		}
		blocks[cur_num].num = maxNum;
		blocks[cur_num].time = job_overlap[i][maxj].time;

		//Print information
		cout << "Blocks " << cur_num+1 << " time: "<< blocks[cur_num].time << "   Order of jobs in the block: ";
		for (size_t a = 0; a < maxNum; a++) {
			cout << blocks[cur_num].job[a].order << " ";
		}
		cur_num++;
		cout << endl;
	}

	return 0;

}