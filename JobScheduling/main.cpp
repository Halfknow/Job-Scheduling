#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <gdiplus.h>
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

vector<Job> job;

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

void openfile()
{
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
	for (size_t i = 1; i < job.size(); i++)
	{
		job[i].order = i;
		job[i].removed = false;
		// cout << job[i] << endl;
	}

}

void drawFile(HDC hdc)
{
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen      pen(Gdiplus::Color(255, 0, 0, 0));
	for (size_t i = 1; i < job.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * job[i].r, 20 * i + 10, 20 * job[i].f, 20 * i + 10);
		// cout << job[i] << endl;
	}

	graphics.DrawLine(&pen, 20, 10, 1100, 10);

	for (size_t i = 1; i <= 1100 / 20; i++)
	{
		graphics.DrawLine(&pen, 20 * i, 5, 20 * i, 10);
	}
}

void approAlgI(HDC hdc, HWND hWnd, RECT rect)
{
	vector<Job> job_1;
	deque<Job> remainingJobs;
	vector<JobOverlap> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;
	int blocksNum = 0;
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen      pen(Gdiplus::Color(255, 255, 0, 0));

	//Initial job_1
	job_1.resize(job.size());

	for (size_t i = 0; i < job.size(); i++)
	{
		job_1[i] = job[i];
	}

	//Initial remainingJobs
	for (size_t i = 1; i < job_1.size(); i++)
	{
		remainingJobs.emplace_back(job_1[i]);
	}

	auto maxf = max_element(std::begin(job_1), std::end(job_1),
		[](const Job & a, const Job & b)
		{
			return a.f < b.f;
		}
	)->f;

	int minr = job_1[1].r;

	//Unit version 2-Approximation Algorithm I
	rect.left = 700;
	rect.top = 50;
	DrawText(hdc, TEXT("Unit version 2-Approximation Algorithm I"), -1, &rect,
		NULL);

	int removedNum = job_1.size()-1;

	//Core Algorithm
	while (removedNum > 0)
	{
		job_overlap.clear();
		job_overlap.resize(maxf);
		int maxNum = 0;
		int maxt = minr;
		for (size_t j = minr; j < maxf; j++)
		{
			job_overlap[j].num = 0;
			job_overlap[j].time = j;
			for (int k = 1; k <= remainingJobs.size(); k++)
			{
				if (job_1[k].r <= j && j < job_1[k].f && !job_1[k].removed)
				{
					job_overlap[j].num++;
					job_overlap[j].overlap.emplace_back(job_1[k]);
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
			remainingJobs[job_overlap[maxt].overlap[n].order-1].removed = true;
			job_1[job_overlap[maxt].overlap[n].order].removed = true;
			removedNum--;
		}
		//Blocks information
		blocks.resize(job_1.size());
		for (size_t b = 0; b < maxNum; b++)
		{
			blocks[cur_num].job.emplace_back(job_overlap[maxt].overlap[b]);
		}
		blocks[cur_num].num = maxNum;
		blocks[cur_num].time = job_overlap[maxt].time;
		blocksNum++;

		// Print information
		TCHAR szBuffer[100];
		wsprintf(szBuffer, TEXT("Blocks %i time: %i Jobs in block: "),
			cur_num + 1,
			blocks[cur_num].time
		);
		GetClientRect(hWnd, &rect);
		rect.left = 700;
		rect.top = 100 + 20 * cur_num;
		DrawText(hdc, szBuffer, -1, &rect,
			NULL);

		for (size_t a = 0; a < maxNum; a++)
		{
			TCHAR Buffer[10];
			wsprintf(Buffer, TEXT("%i"), blocks[cur_num].job[a].order);
			GetClientRect(hWnd, &rect);
			rect.left = 920 + 20 * a;
			rect.top = 100 + 20 * cur_num;
			DrawText(hdc, Buffer, -1, &rect,
				NULL);
		}

		cur_num++;
	}
	for (int i = 0 ; i< blocksNum;i++)
	{
		graphics.DrawLine(&pen, 20 * blocks[i].time, 20 * (blocks[i].job[0].order) - 9 + 10, 20 * blocks[i].time, 20 * (blocks[i].job.back().order) + 9 + 10);
		graphics.DrawLine(&pen, 20 * (blocks[i].time + 1), 20 * (blocks[i].job[0].order) - 9 + 10, 20 * (blocks[i].time + 1), 20 * (blocks[i].job.back().order) + 9 + 10);
	}
}

void approAlgII(HDC hdc, HWND hWnd, RECT rect)
{
	queue<Job> remainingJobs;
	vector<vector<JobOverlap>> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;
	int blocksNum = 0;
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen      pen(Gdiplus::Color(255, 255, 0, 0));

	for (size_t i = 1; i < job.size(); i++)
	{
		remainingJobs.emplace(job[i]);
	}

	//Unit version 2-Approximation Algorithm II
	rect.left = 700;
	rect.top = 50;
	DrawText(hdc, TEXT("Unit version 2-Approximation Algorithm II"), -1, &rect,
		NULL);

	job_overlap.resize(remainingJobs.size()+1);

	//Core Algorithm
	while (!remainingJobs.empty())
	{
		int i = remainingJobs.front().order;
		int maxNum = 1;
		int maxj = 0;
		auto interval = job[i].f - job[i].r;
		assert(interval > 0);
		auto& cur_overlap = job_overlap[i];
		cur_overlap.resize(interval);
		for (int j = 0; j < interval; j++)
		{
			cur_overlap[j].num = 0;
			cur_overlap[j].time = job[i].r + j;

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
		for (int n = 0; n < maxNum; n++)
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
		blocksNum ++ ;

		//Print information
		// struct JobTimeIterator
		// {
		// 	vector<Job>::iterator it;
		//
		// 	JobTimeIterator& operator++()
		// 	{
		// 		++it;
		// 		return *this;
		// 	}
		//
		// 	int operator *() const
		// 	{
		// 		return it->order;
		// 	}
		//
		// 	using value_type = int;
		// };
		//
		// wsprintf(szBuffer, fmt::format(
		// 	TEXT("Blocks {} time: {} Jobs in block: {}"),
		// 	cur_num + 1,
		// 	blocks[cur_num].time,
		// 	fmt::join(JobTimeIterator{ blocks[cur_num].job.begin() },
		// 		JobTimeIterator{ blocks[cur_num].job.end() },
		// 		L"")
		// ).c_str());
		// GetClientRect(hWnd, &rect);
		// rect.left = 700;
		// rect.top = 50 + 20 * cur_num;
		// DrawText(hdc, szBuffer, -1, &rect,
		// 	NULL);

		TCHAR szBuffer[100];
		wsprintf(szBuffer, TEXT("Blocks %i time: %i Jobs in block: "),
			cur_num + 1, 
			blocks[cur_num].time
		);
		GetClientRect(hWnd, &rect);
		rect.left = 700;
		rect.top = 100+20*cur_num;
		DrawText(hdc, szBuffer, -1, &rect,
			NULL);

		for (size_t a = 0; a < maxNum; a++)
		{
			TCHAR Buffer[10];
			wsprintf(Buffer, TEXT("%i"), blocks[cur_num].job[a].order);
			GetClientRect(hWnd, &rect);
			rect.left = 920+20*a;
			rect.top = 100 + 20 * cur_num;
			DrawText(hdc,Buffer, -1, &rect,
				NULL);
			// cout << blocks[cur_num].job[a].order << " ";
		}
		cur_num++;
		cout << endl;
	}
	for (int i = 0; i < blocksNum; i++)
	{
		graphics.DrawLine(&pen, 20 * blocks[i].time, 20 * (blocks[i].job[0].order) - 9 + 10, 20 * blocks[i].time, 20 * (blocks[i].job.back().order) + 9 + 10);
		graphics.DrawLine(&pen, 20 * (blocks[i].time + 1), 20 * (blocks[i].job[0].order) - 9 + 10, 20 * (blocks[i].time + 1), 20 * (blocks[i].job.back().order) + 9 + 10);
	}
}

void optAlg(HDC hdc, HWND hWnd, RECT rect)
{
	vector<Job> job_1;
	vector<Job> job_2;
	queue<Job> remainingELI;
	queue<Job> remainingJobs;
	queue<Job> remainingJobs_2;
	vector<JobOverlap> job_overlap;
	vector<Block> blocks;
	int cur_num = 0;
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen      pen(Gdiplus::Color(255, 255, 0, 0));

	//Initial job_1
	job_1.resize(job.size());

	for (size_t i = 0; i < job.size(); i++)
	{
		job_1[i] = job[i];
	}

	job_2.resize(job.size());

	//Initial job_2
	for (size_t i = 0; i < job.size(); i++)
	{
		job_2[i] = job[i];
	}

	//Initial remainingJobs
	for (size_t i = 1; i < job.size(); i++)
	{
		remainingJobs.emplace(job[i]);
	}

	job_overlap.resize(remainingJobs.size()+1);

	int order = 1;

	job_1.erase(job_1.begin() + 0);

	while (!job_1.empty())
	{
		int num = 0;
		pen.SetColor(Gdiplus::Color(255, 0, 0, 255));
		Job ELI = *min_element(std::begin(job_1), std::end(job_1),
			[](const Job & a, const Job & b)
			{
				return a.f < b.f;
			}
		);
		remainingELI.emplace(ELI);
		for (int k = 0; k< job_1.size();k++)
		{
			if (job_1[k].r < ELI.f)
			{
				num++;
			}
		}
		for (size_t i = 0; i < num; i++)
		{
			job_1.erase(job_1.begin() + 0);
		}
		graphics.DrawLine(&pen, 20 * remainingELI.back().r, 20 * remainingELI.back().order + 10, 20 * remainingELI.back().f, 20 * remainingELI.back().order + 10);
		// cout <<"Order: "<< remainingELI.back().order << " job: " << remainingELI.back().r << " ";
	}

	//Optimal Algorithm
	rect.left = 700;
	rect.top = 50;
	DrawText(hdc, TEXT("Optimal Algorithm"), -1, &rect,
		NULL);

	blocks.resize(remainingELI.size());

	while (!remainingJobs.empty())
	{
		int j = remainingELI.front().order;
		job_overlap[j].time = remainingELI.front().f - 1;
		job_overlap[j].num = 0;
		for (int k = 1; k<= remainingJobs.size();k++)
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
		// cout << "Blocks " << cur_num + 1 << " time: " << blocks[cur_num].time <<" num: "<<blocks[cur_num].num << " Order of jobs in the block: ";
		TCHAR szBuffer[100];
		wsprintf(szBuffer, TEXT("Blocks %i time: %i Jobs in block: "),
			cur_num + 1,
			blocks[cur_num].time
		);
		GetClientRect(hWnd, &rect);
		rect.left = 700;
		rect.top = 100 + 20 * cur_num;
		DrawText(hdc, szBuffer, -1, &rect,
			NULL);
		for (size_t a = 0; a < blocks[cur_num].num; a++)
		{
			TCHAR Buffer[10];
			wsprintf(Buffer, TEXT("%i"), blocks[cur_num].job[a].order);
			GetClientRect(hWnd, &rect);
			rect.left = 920 + 20 * a;
			rect.top = 100 + 20 * cur_num;
			DrawText(hdc, Buffer, -1, &rect,
				NULL);
			// cout << blocks[cur_num].job[a].order << " ";
		}
		cur_num++;
		cout << endl;
	}
	pen.SetColor(Gdiplus::Color(255, 255, 0, 0));
	for (int i = 0; i < blocks.size(); i++)
	{
		graphics.DrawLine(&pen, 20 * blocks[i].time, 20 * (blocks[i].job[0].order) - 9 + 10, 20 * blocks[i].time, 20 * (blocks[i].job.back().order) + 9+10);
		graphics.DrawLine(&pen, 20 * (blocks[i].time + 1), 20 * (blocks[i].job[0].order) - 9+10, 20 * (blocks[i].time + 1), 20 * (blocks[i].job.back().order) + 9+10);
	}
}

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	HWND                hWnd;
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	openfile();

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("JobScheduling");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		TEXT("JobScheduling"),   // window class name
		TEXT("Job Scheduling"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		CW_USEDEFAULT,            // initial x size
		CW_USEDEFAULT,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);

	return msg.wParam;
}  // WinMain

int input = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	HDC          hdc;
	PAINTSTRUCT  ps;
	
	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		//Clear Window
		RECT rect;
		HBRUSH hBr;
		SetRect(&rect, 0, 0, 2000, 2000);
		hBr = CreateSolidBrush(RGB(255, 255, 255));
		FillRect(hdc, &rect, hBr);

		drawFile(hdc);

		switch (input)
		{
		case 1:
			approAlgI(hdc, hWnd, rect);
			break;
		case 2:
			approAlgII(hdc, hWnd, rect);
			break;
		case 3:
			optAlg(hdc, hWnd, rect);
			break;
		}
		EndPaint(hWnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case '1':
			input = 1;
			// SendMessage(hWnd, WM_PAINT, input = 1, 0);
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			// RedrawWindow(hWnd, &rect, NULL, RDW_UPDATENOW | RDW_VALIDATE);
			break;
		case '2':
			input = 2;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		case '3':
			input = 3;
			InvalidateRect(hWnd, NULL, NULL);
			UpdateWindow(hWnd);
			break;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc
