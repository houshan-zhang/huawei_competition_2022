#ifndef FUNCTIONS_HEADER
#define FUNCTIONS_HEADER

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <math.h>

using namespace std;

struct SiteStruct;
struct CustStruct;
struct ArcStruct;
struct StreamStruct;

struct CustStruct
{
   int idx;
   string name;
   vector<ArcStruct*> ArcIn;
   int degree;
   vector<int> residual_demand;
   int temp_residual_demand;
   bool isdetected;
   ArcStruct* Near;
   vector<vector<StreamStruct*> > demand;
   vector<vector<ArcStruct*>> ArcAssignedflow;
   vector<int> demandSum;
};

struct StreamStruct
{
   string name;
   int value;
   bool IsAssigned = false;
};

struct SiteStruct
{
   int idx;
   string name;
   vector<ArcStruct*> ArcOut;
   int degree;
   int bandwidth;
   int basic_capacity;
   int temp_residual_capacity;
   bool isdetected;
   ArcStruct* Near;
   vector<int> supply;
   //be initialized after CalculateflowPerMoment with pattern 0
   vector<int> supplysequence;
   int NumSampleMaxEachSite = 0;
   vector<bool> IsMax;
   bool Is90 = false;
   bool IsUse = false;
   bool IsPredictUse = false;
};

struct SampleStruct
{
   int idx;
   bool IsBannedToAddBiggestSite = false;
   string name;
   int TotalResidualDemandEachSample;
};

struct ArcStruct
{
   SiteStruct* Site;
   CustStruct* Cust;
   vector<int> flow;
   vector<int> flowinteger;
   vector<vector<StreamStruct*>> Stream;
   int temp_flow;
};

struct InstanceStruct
{
   int qos;
   int base_cost;
   int NumCust;
   int NumSite;
   int NumSample;
   int NumArc;
   vector<CustStruct*> Cust;
   vector<SiteStruct*> Site;
   vector<SampleStruct*> Sample;
   vector<ArcStruct*> Arc;
   int NumUse = 0;
   int NumPredictUse = 0;
   //just used by finding biggest-potential-demand site
   int NumSSMax = 0;
   int Num90 = 0;
};

int Max(int a, int b);

int Min(int a, int b);

void ReadFile(InstanceStruct* Inst);

void WriteFile(InstanceStruct* Inst);

bool CheckSolution(InstanceStruct* Inst);

void FreeMemory(InstanceStruct* Inst);

void Presolve(InstanceStruct* Inst);

void ResetLabel(InstanceStruct* Inst);

int FindDemandBiggestTime(InstanceStruct* Inst, bool pattern);

void Initflow(InstanceStruct* Inst, int time, int siteidx);

void InitflowPacking(InstanceStruct* Inst, int time, int siteidx);

int FindDemandBiggestSite(InstanceStruct* Inst, int time, int* potentialdemand, bool pattern);

void FillingSite(InstanceStruct* Inst, int time, int siteidx, int pattern);

void CalculateflowPerMoment(InstanceStruct* Inst, int time, bool islimit);

int CalculateScore(InstanceStruct* Inst);

int get95bandwidth(InstanceStruct* Inst);

void display(InstanceStruct* Inst);

void AssignStream(InstanceStruct* Inst);

#endif
