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
	bool removed;
};

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
	return a.r < b.r || (a.r == b.r && a.f<b.f);
}

void approAlgI()
{
	vector<Job> job;
	deque<Job> remainingJobs;
	vector<JobOverlap> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;

	//Open a file
	fstream file("JobList.txt");
	{
		int i = 1;
		while (file)
		{
			file >> job.emplace_back();
			job.back().job = i;
			++i;
		}
	}

	std::sort(job.begin(), job.end()); //Sort

	//Print initial order of jobs
	for (size_t i = 1; i < job.size() - 1; i++)
	{
		job[i].order = i;
		job[i].removed = false;
		cout << job[i] << endl;
	}
	cout << endl;

	for (size_t i = 1; i < job.size(); i++)
	{
		remainingJobs.emplace_back(job[i]);
	}

	auto maxf = max_element(std::begin(job), std::end(job),
		[](const Job & a, const Job & b)
		{
			return a.f < b.f;
		}
	)->f;

	int minr = job[1].r;

	int removedNum = job.size()-2;

	while (removedNum != 0)
	{
		job_overlap.clear();
		job_overlap.resize(maxf);
		int maxNum = 0;
		int maxt = 0;
		for (size_t j = 1; j < maxf - minr + 1; j++)
		{
			job_overlap[j].num = 0;
			job_overlap[j].time = j;
			for (int k = 1; k <= remainingJobs.size(); k++)
			{
				if (job[k].r <= j && j < job[k].f && !job[k].removed)
				{
					job_overlap[j].num++;
					job_overlap[j].overlap.emplace_back(job[k]);
				}
			}
			// Whether the number of overlapped jobs is maximum
			if (job_overlap[j].num > maxNum)
			{
				maxNum = job_overlap[j].num;
				maxt = j;
			}
		}
		//Remove Jobs
		for (int n = 0; n < maxNum; n++)
		{
			remainingJobs[job_overlap[maxt].overlap[n].order].removed = true;
			job[job_overlap[maxt].overlap[n].order].removed = true;
			removedNum--;
		}
		//Blocks information
		blocks.resize(job.size());
		for (size_t b = 0; b < maxNum; b++)
		{
			blocks[cur_num].job.emplace_back(job_overlap[maxt].overlap[b]);
		}
		blocks[cur_num].num = maxNum;
		blocks[cur_num].time = job_overlap[maxt].time;

		//Remove jobs

		//Print information
		cout << "Blocks " << cur_num + 1 << " time: " << blocks[cur_num].time << "   Order of jobs in the block: ";
		for (size_t a = 0; a < maxNum; a++)
		{
			cout << blocks[cur_num].job[a].order << " ";
		}
		cur_num++;
		cout << endl;
	}
}

void approAlgII()
{
	vector<Job> job;
	queue<Job> remainingJobs;
	vector<vector<JobOverlap>> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;

	//Open a file
	fstream file("JobList.txt");

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
		cout << job[i] << endl;
	}

	cout << endl;

	for (size_t i = 1; i < job.size(); i++)
	{
		remainingJobs.emplace(job[i]);
	}

	job_overlap.resize(remainingJobs.size());

	while (!remainingJobs.empty())
	{
		int i = remainingJobs.front().order;
		int maxNum = 1;
		int maxj = 0;
		auto interval = job[i].f - job[i].r;
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
				if (job[k].r <= cur_overlap[j].time)
				{
					cur_overlap[j].num++;
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
		for (size_t b = 0; b < maxNum; b++)
		{
			blocks[cur_num].job.emplace_back(job_overlap[i][maxj].overlap[b]);
		}
		blocks[cur_num].num = maxNum;
		blocks[cur_num].time = job_overlap[i][maxj].time;

		//Print information
		cout << "Blocks " << cur_num + 1 << " time: " << blocks[cur_num].time << "   Order of jobs in the block: ";
		for (size_t a = 0; a < maxNum; a++)
		{
			cout << blocks[cur_num].job[a].order << " ";
		}
		cur_num++;
		cout << endl;
	}
}

void optAlg()
{
	vector<Job> job;
	vector<Job> job_2;
	queue<Job> remainingELI;
	queue<Job> remainingJobs;
	queue<Job> remainingJobs_2;
	vector<JobOverlap> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;

	//Open a file
	fstream file("JobList.txt");

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
		cout << job[i] << endl;
	}
	cout << endl;

	job_2.resize(job.size());

	for (size_t i = 0; i < job.size(); i++)
	{
		job_2[i] = job[i];
		// cout << job_2[i] << endl;
	}
	// cout << endl;

	for (size_t i = 1; i < job.size(); i++)
	{
		remainingJobs.emplace(job[i]);
	}

	for (size_t i = 1; i < job.size(); i++)
	{
		remainingJobs_2.emplace(job[i]);
	}

	job_overlap.resize(remainingJobs.size());

	int order = 1;

	job.erase(job.begin() + 0);

	while (!job.empty())
	{
		int num = 0;
		Job ELI = *min_element(std::begin(job), std::end(job),
			[](const Job & a, const Job & b)
			{
				return a.f < b.f;
			}
		);
		remainingELI.emplace(ELI);
		remainingELI.back().order = order;
		for (int k = 0; k<job.size();k++)
		{
			if (job[k].r < ELI.f)
			{
				// job.erase(job.begin() + 0);
				num++;
			}
		}
		for (size_t i = 0; i < num; i++)
		{
			job.erase(job.begin() + 0);
		}
		order++;
		cout <<"Order: "<< remainingELI.back().order << " job: " << remainingELI.back().r << " ";
	}

	cout << endl;

	blocks.resize(remainingELI.size());

	while (!remainingJobs.empty())
	{
		int j = remainingELI.front().order;
		job_overlap[j].time = remainingELI.front().f - 1;
		job_overlap[j].num = 0;
		for (int k = 1; k<=remainingJobs.size();k++)
		{
			if (job_2[k].r<remainingELI.front().f)
			{
				job_overlap[j].num++;
				job_overlap[j].overlap.emplace_back(job_2[k]);
			}
		}
		//Remove overlapped jobs
		//Remove the first ELI
		for (int n = 0; n < job_overlap[j].num; n++)
		{
			remainingJobs.pop();
			job_2.erase(job_2.begin()+0);
		}
		remainingELI.pop();
		//Blocks information
		for (size_t b = 0; b < job_overlap[j].num; b++)
		{
			blocks[cur_num].job.emplace_back(job_overlap[j].overlap[b]);
		}
		blocks[cur_num].num = job_overlap[j].num;
		blocks[cur_num].time = job_overlap[j].time;

		//Print information
		cout << "Blocks " << cur_num + 1 << " time: " << blocks[cur_num].time <<" num: "<<blocks[cur_num].num << " Order of jobs in the block: ";
		for (size_t a = 0; a < blocks[cur_num].num; a++)
		{
			cout << blocks[cur_num].job[a].order << " ";
		}
		cur_num++;
		cout << endl;
	}
}

int main(int argc, const char* argv[])
{
	// approAlgII();
	approAlgI();
	// optAlg();

	return 0;
}
