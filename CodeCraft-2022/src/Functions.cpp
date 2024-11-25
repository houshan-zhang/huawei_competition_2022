#include "Functions.h"

int Max(int a, int b)
{
   return a >= b ? a : b;
}

int Min(int a, int b)
{
   return a <= b ? a : b;
}

bool SortArcIn(ArcStruct* A, ArcStruct* B)
{
   return A->Site->degree > B->Site->degree;
}

bool SortArcOut(ArcStruct* A, ArcStruct* B)
{
   return A->Cust->degree > B->Cust->degree;
}

bool SortStream(StreamStruct* A, StreamStruct* B)
{
   return A->value > B->value;
}

void ReadFile(InstanceStruct* Inst)
{
   string linestr;
   int rowidx;
   int colidx;
   string str;
   //======================== config.csv ========================//
   ifstream configfile("/data/config.ini", ios::in);
   rowidx = 0;
   while( getline(configfile, linestr) )
   {
      rowidx++;
      stringstream ss(linestr);
      if( rowidx == 2 )
      {
         colidx = 0;
         while( getline(ss, str, '=') )
         {
            colidx++;
            if( colidx != 1 )
            {
               Inst->qos = stoi(str);
            }
         }
      }
      else if( rowidx == 3 )
      {
         colidx = 0;
         while( getline(ss, str, '=') )
         {
            colidx++;
            if( colidx != 1 )
            {
               Inst->base_cost = stoi(str);
            }
         }
      }
   }
   //======================== demand.csv ========================//
   ifstream demandfile("/data/demand.csv", ios::in);
   rowidx = 0;
   string streamname;
   int sampleidx = -1;
   bool isnewsample = true;
   while( getline(demandfile, linestr) )
   {
      rowidx++;
      stringstream ss(linestr);
      if( rowidx == 1 )
      {
         colidx = 0;
         while( getline(ss, str, ',') )
         {
            colidx++;
            if( colidx >= 3 )
            {
               //init CustStruct
               CustStruct* CustTemp = new CustStruct;
               CustTemp->name = str;
               CustTemp->idx = colidx-3;
               Inst->Cust.push_back(CustTemp);
            }
         }
      }
      else
      {
         colidx = 0;
         while( getline(ss, str, ',') )
         {
            colidx++;
            if( colidx == 1 )
            {
               if( Inst->Sample.size() == 0 || str != Inst->Sample[Inst->Sample.size()-1]->name )
               {
                  //init SampleStruct
                  sampleidx++;
                  isnewsample = true;
                  SampleStruct* SampleTemp = new SampleStruct;
                  SampleTemp->idx = sampleidx;
                  SampleTemp->name = str;
                  SampleTemp->TotalResidualDemandEachSample = 0;
                  Inst->Sample.push_back(SampleTemp);
               }
               else
               {
                  isnewsample = false;
               }
            }
            else if( colidx == 2 )
            {
               streamname = str;
            }
            else
            {
               if( isnewsample )
               {
                  Inst->Cust[colidx-3]->demand.push_back(vector<StreamStruct*>(0));
                  Inst->Cust[colidx-3]->demandSum.push_back(0);
               }
               if( stoi(str) != 0 )
               {
                  StreamStruct* StreamTemp = new StreamStruct;
                  StreamTemp->name = streamname;
                  StreamTemp->value = stoi(str);
                  Inst->Cust[colidx-3]->demand[sampleidx].push_back(StreamTemp);
                  Inst->Cust[colidx-3]->demandSum[sampleidx] += stoi(str);
                  Inst->Sample[sampleidx]->TotalResidualDemandEachSample += stoi(str);
               }
            }
         }
      }
   }
   Inst->NumCust = Inst->Cust.size();
   Inst->NumSample = Inst->Sample.size();
   //======================== site_bandwidth.csv ========================//
   ifstream site_bandwidthfile("/data/site_bandwidth.csv", ios::in);
   rowidx = 0;
   while( getline(site_bandwidthfile, linestr) )
   {
      rowidx++;
      stringstream ss(linestr);
      if( rowidx != 1 )
      {
         colidx = 0;
         while( getline(ss, str, ',') )
         {
            colidx++;
            if( colidx == 1 )
            {
               SiteStruct* SiteTemp = new SiteStruct;
               SiteTemp->name = str;
               SiteTemp->idx = rowidx-2;
               Inst->Site.push_back(SiteTemp);
            }
            else
            {
               Inst->Site[rowidx-2]->bandwidth = stoi(str);
            }
         }
      }
   }
   Inst->NumSite = Inst->Site.size();
   //======================== qos.csv ========================//
   ifstream qosfile("/data/qos.csv", ios::in);
   rowidx = 0;
   vector<string> custname;
   vector<int> custidx(Inst->NumCust);
   int siteidx = 0;
   while( getline(qosfile, linestr) )
   {
      rowidx++;
      stringstream ss(linestr);
      if( rowidx == 1 )
      {
         while( getline(ss, str, ',') )
         {
            custname.push_back(str);
         }
         custname.erase(custname.begin());
         for( int i = 0; i < Inst->NumCust; i++ )
         {
            for( int j = 0; j < Inst->NumCust; j++ )
            {
               if( custname[i] == Inst->Cust[j]->name )
               {
                  custidx[i] = j;
                  break;
               }
            }
         }
      }
      else
      {
         colidx = 0;
         while( getline(ss, str, ',') )
         {
            colidx++;
            if( colidx == 1 )
            {
               for( int i = 0; i < Inst->NumSite; i++ )
               {
                  if( str == Inst->Site[i]->name )
                  {
                     siteidx = Inst->Site[i] ->idx;
                  }
               }
            }
            else
            {
               if( stoi(str) < Inst->qos )
               {
                  ArcStruct* ArcTemp = new ArcStruct;
                  ArcTemp->flow.resize(Inst->NumSample, 0);
                  ArcTemp->flowinteger.resize(Inst->NumSample, 0);
                  ArcTemp->Stream.resize(Inst->NumSample);
                  ArcTemp->Cust = Inst->Cust[custidx[colidx-2]];
                  ArcTemp->Site = Inst->Site[siteidx];
                  Inst->Cust[custidx[colidx-2]]->ArcIn.push_back(ArcTemp);
                  Inst->Site[siteidx]->ArcOut.push_back(ArcTemp);
                  Inst->Arc.push_back(ArcTemp);
               }
            }
         }
      }
   }
   Inst->NumArc = Inst->Arc.size();
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      Inst->Cust[i]->degree = Inst->Cust[i]->ArcIn.size();
   }
   for( int i = 0; i < Inst->NumSite; i++ )
   {
      Inst->Site[i]->degree = Inst->Site[i]->ArcOut.size();
   }
   //print statistical information
   cout<<"@01 READ FILE OVER Customer: "<<Inst->NumCust<<" Site: "<<Inst->NumSite<<" Sample: "<<Inst->NumSample<<" NumArc: "<<Inst->NumArc<<endl;
}

void WriteFile(InstanceStruct* Inst)
{
   remove("/output/solution.txt");
   fstream outfile;
   outfile.open("/output/solution.txt", ios::out);
   int n90 = 0;
   for( int siteidx = 0; siteidx < Inst->NumSite; siteidx++ )
   {
      if( Inst->Site[siteidx]->Is90 )
      {
         if( n90 == 0 )
         {
            outfile<<Inst->Site[siteidx]->name;
         }
         else
         {
            outfile<<","<<Inst->Site[siteidx]->name;
         }
         n90++;
      }
   }
   outfile<<endl;
   assert(n90 <= 10);
   for( int time = 0; time < Inst->NumSample; time++ )
   {
      bool iswrite;
      for( int i = 0; i < Inst->NumCust; i++ )
      {
         iswrite = false;
         CustStruct* CustTemp = Inst->Cust[i];
         outfile<<CustTemp->name<<":";
         for( int j = 0; j < CustTemp->degree; j++ )
         {
            ArcStruct* ArcTemp = CustTemp->ArcIn[j];
            int streamsize = ArcTemp->Stream[time].size();
            if( streamsize > 0 )
            {
               if( iswrite )
               {
                  outfile<<",";
               }
               iswrite = true;
               outfile<<"<"<<ArcTemp->Site->name;
               for( int j = 0; j < streamsize; j++ )
               {
                  outfile<<","<<ArcTemp->Stream[time][j]->name;
               }
               outfile<<">";
            }
         }
         outfile<<endl;
      }
   }
   outfile.close();
}

void display(InstanceStruct* Inst)
{
   for( int i = 0; i < Inst->NumSite; i++ )
   {
      SiteStruct* SiteTemp = Inst->Site[i];
      cout<<"Site["<<SiteTemp->idx<<"]";
      for( int j = 0; j < Inst->NumSample; j++ )
      {
         cout<<" - "<<SiteTemp->supplysequence[j];
      }
      cout<<endl;
   }
}

void FreeMemory(InstanceStruct* Inst)
{
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      int samplesize = Inst->Cust[i]->demand.size();
      for( int j = 0; j < samplesize; j++ )
      {
         int streamsize = Inst->Cust[i]->demand[j].size();
         for( int k = 0; k < streamsize; k++ )
         {
            delete Inst->Cust[i]->demand[j][k];
         }
      }
      delete Inst->Cust[i];
   }
   for( int i = 0; i < Inst->NumSite; i++ )
   {
      delete Inst->Site[i];
   }
   for( int i = 0; i < Inst->NumSample; i++ )
   {
      delete Inst->Sample[i];
   }
   for( int i = 0; i < Inst->NumArc; i++ )
   {
      delete Inst->Arc[i];
   }
}

bool CheckSolution(InstanceStruct* Inst)
{
   int flow;
   int streamvaluesum;
   for( int time = 0; time < Inst->NumSample; time++ )
   {
      for( int i = 0; i < Inst->NumCust; i++ )
      {
         CustStruct* CustTemp = Inst->Cust[i];
         flow = 0;
         for( int j = 0; j < CustTemp->degree; j++ )
         {
            streamvaluesum = 0;
            int streamsize = CustTemp->ArcIn[j]->Stream[time].size();
            for( int k = 0; k < streamsize; k++ )
            {
               streamvaluesum += CustTemp->ArcIn[j]->Stream[time][k]->value;
            }
            if( streamvaluesum != CustTemp->ArcIn[j]->flowinteger[time] )
            {
               return false;
               cout<<"Cust["<<i<<"] stream error! (time "<<time<<")"<<endl;
            }
            flow += CustTemp->ArcIn[j]->flowinteger[time];
         }
         if( flow != CustTemp->demandSum[time] )
         {
            cout<<"Cust["<<i<<"] flow error! (time "<<time<<")"<<endl;
            return false;
         }
      }
      for( int i = 0; i < Inst->NumSite; i++ )
      {
         SiteStruct* SiteTemp = Inst->Site[i];
         flow = 0;
         for( int j = 0; j < SiteTemp->degree; j++ )
         {
            flow += SiteTemp->ArcOut[j]->flowinteger[time];
         }
         if( flow > SiteTemp->bandwidth )
         {
            cout<<flow<<" - "<<SiteTemp->bandwidth<<endl;
            cout<<"Site["<<i<<"] flow error! (time "<<time<<")"<<endl;
            return false;
         }
      }
   }
   return true;
}

int get95bandwidth(InstanceStruct* Inst)
{
   return ceil(Inst->NumSample*0.95)-1;
}

void Presolve(InstanceStruct* Inst)
{
   //remove ^M(\r)
   string* name = &Inst->Cust[Inst->NumCust-1]->name;
   name->erase(name->end()-1);
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      CustStruct* CustTemp = Inst->Cust[i];
      CustTemp->ArcAssignedflow.resize(Inst->NumSample);
      CustTemp->residual_demand.resize(Inst->NumSample);
      sort(CustTemp->ArcIn.begin(), CustTemp->ArcIn.end(), SortArcIn);
      for( int j = 0; j < Inst->NumSample; j++ )
      {
         CustTemp->residual_demand[j] = CustTemp->demandSum[j];
         sort(CustTemp->demand[j].begin(), CustTemp->demand[j].end(), SortStream);
      }
   }
   for( int i = 0; i < Inst->NumSite; i++ )
   {
      SiteStruct* SiteTemp = Inst->Site[i];
      SiteTemp->supply.clear();
      SiteTemp->supply.resize(Inst->NumSample, 0);
      SiteTemp->IsMax.resize(Inst->NumSample, false);
      sort(SiteTemp->ArcOut.begin(), SiteTemp->ArcOut.end(), SortArcOut);
   }
}

int CalculateScore(InstanceStruct* Inst)
{
   int score = 0;
   for( int i = 0; i < Inst->NumSite; i++ )
   {
      SiteStruct* SiteTemp = Inst->Site[i];
#ifdef _DEBUG
      //check supply
      int supplytemp;
      for( int time = 0; time < Inst->NumSample; time++ )
      {
         supplytemp = 0;
         for( int j = 0; j < SiteTemp->degree; j++ )
         {
            supplytemp += SiteTemp->ArcOut[j]->flow[time];
         }
         assert(SiteTemp->supply[time] == supplytemp);
      }
#endif
      //calculate score
      SiteTemp->supplysequence = SiteTemp->supply;
      sort(SiteTemp->supplysequence.begin(), SiteTemp->supplysequence.end());
      if( SiteTemp->supplysequence[Inst->NumSample-1] != 0 )
      {
         if( SiteTemp->supplysequence[get95bandwidth(Inst)] <= Inst->base_cost )
         {
            score += Inst->base_cost;
         }
         else
         {
            score += pow((SiteTemp->supplysequence[get95bandwidth(Inst)]-Inst->base_cost), 2)/SiteTemp->bandwidth + SiteTemp->supplysequence[get95bandwidth(Inst)];
         }
      }
   }
   return score;
}

void ResetLabel(InstanceStruct* Inst)
{
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      Inst->Cust[i]->isdetected = false;
      Inst->Cust[i]->Near = NULL;
   }
   for( int i = 0; i < Inst->NumSite; i++ )
   {
      Inst->Site[i]->isdetected = false;
      Inst->Site[i]->Near = NULL;
   }
}

void InitFlow(InstanceStruct* Inst, int time, int pattern)
{
   if( pattern < 0 )
   {
      //init_flow
      for( int i = 0; i < Inst->NumSite; i++ )
      {
         if( Inst->Site[i]->IsMax[time] )
         {
            Inst->Site[i]->temp_residual_capacity = Inst->Site[i]->bandwidth;
         }
         else
         {
            Inst->Site[i]->temp_residual_capacity = Inst->base_cost + ceil((Inst->Site[i]->bandwidth-Inst->base_cost)*(-pattern-1)/19);
         }
         for( int j = 0; j < Inst->Site[i]->degree; j++ )
         {
            Inst->Site[i]->temp_residual_capacity -= Inst->Site[i]->ArcOut[j]->flow[time];
         }
         Inst->Site[i]->temp_residual_capacity = Max(Inst->Site[i]->temp_residual_capacity, 0);
      }
   }
}

void FillingSite(InstanceStruct* Inst, int time, int PackingSiteIdx, int pattern)
{
   //Init flow
   Inst->Site[PackingSiteIdx]->temp_residual_capacity = Inst->Site[PackingSiteIdx]->bandwidth;
   for( int j = 0; j < Inst->Site[PackingSiteIdx]->degree; j++ )
   {
      Inst->Site[PackingSiteIdx]->temp_residual_capacity -= Inst->Site[PackingSiteIdx]->ArcOut[j]->flow[time];
   }
   if( pattern >= 0 )
   {
      Inst->Site[PackingSiteIdx]->temp_residual_capacity = Min(Inst->Site[PackingSiteIdx]->temp_residual_capacity, pattern);
   }
   bool ispathfind;
   int increase_flow;
   vector<SiteStruct*> Queue;
   SiteStruct* tracebackindicator;
   ispathfind = true;
   while( ispathfind && Inst->Site[PackingSiteIdx]->temp_residual_capacity > 0 )
   {
      ResetLabel(Inst);
      Queue.clear();
      Inst->Site[PackingSiteIdx]->isdetected = true;
      Queue.push_back(Inst->Site[PackingSiteIdx]);
      ispathfind = false;
      while( Queue.size() != 0 && !ispathfind )
      {
         SiteStruct* SiteTemp = Queue[0];
         Queue.erase(Queue.begin());
         for( int i = 0; i < SiteTemp->degree; i++ )
         {
            CustStruct* CustTemp = SiteTemp->ArcOut[i]->Cust;
            if( !CustTemp->isdetected )
            {
               CustTemp->isdetected = true;
               CustTemp->Near = SiteTemp->ArcOut[i];
               if( CustTemp->residual_demand[time] > 0 )
               {
                  ispathfind = true;
                  //change_flow
                  increase_flow = Min(CustTemp->residual_demand[time], Inst->Site[PackingSiteIdx]->temp_residual_capacity);
                  tracebackindicator = CustTemp->Near->Site;
                  while( tracebackindicator->idx != Inst->Site[PackingSiteIdx]->idx )
                  {
                     increase_flow = Min(increase_flow, tracebackindicator->Near->flow[time]);
                     tracebackindicator = tracebackindicator->Near->Cust->Near->Site;
                  }
                  Inst->Sample[time]->TotalResidualDemandEachSample -= increase_flow;
                  Inst->Site[PackingSiteIdx]->supply[time] += increase_flow;
                  CustTemp->residual_demand[time] -= increase_flow;
                  CustTemp->Near->flow[time] += increase_flow;
                  Inst->Site[PackingSiteIdx]->temp_residual_capacity -= increase_flow;
                  tracebackindicator = CustTemp->Near->Site;
                  while( tracebackindicator->idx != Inst->Site[PackingSiteIdx]->idx )
                  {
                     tracebackindicator->Near->flow[time] -= increase_flow;
                     tracebackindicator->Near->Cust->Near->flow[time] += increase_flow;
                     tracebackindicator = tracebackindicator->Near->Cust->Near->Site;
                  }
                  break;
               }
               else
               {
                  for( int j = 0; j < CustTemp->degree; j++ )
                  {
                     ArcStruct* ArcTemp = CustTemp->ArcIn[j];
                     if( ArcTemp->flow[time] != 0 && !ArcTemp->Site->isdetected )
                     {
                        ArcTemp->Site->Near = ArcTemp;
                        ArcTemp->Site->isdetected = true;
                        Queue.push_back(ArcTemp->Site);
                     }
                  }
               }
            }
            else
            {
               continue;
            }
         }
      }
   }
}

void InitFlowPacking(InstanceStruct* Inst, int time, int siteidx)
{
   Inst->Site[siteidx]->temp_residual_capacity = Inst->Site[siteidx]->bandwidth - Inst->Site[siteidx]->supply[time];
   for( int i = 0; i < Inst->NumArc; i++ )
   {
      Inst->Arc[i]->temp_flow = Inst->Arc[i]->flow[time];
   }
   for( int i = 0; i < Inst->NumCust; i++ )
   {
      Inst->Cust[i]->temp_residual_demand = Inst->Cust[i]->residual_demand[time];
   }
}

int FindDemandBiggestSite(InstanceStruct* Inst, int time, int* potentialdemand, bool pattern)
{
   int maxdemand = 0;
   int tempdemand;
   int maxdemandsiteidx = -1;
   for( int PackingSiteIdx = 0; PackingSiteIdx < Inst->NumSite; PackingSiteIdx++ )
   {
      if( !pattern && Inst->Site[PackingSiteIdx]->NumSampleMaxEachSite >= floor(Inst->NumSample*0.1) )
      {
         continue;
      }
      if( !pattern && Inst->Site[PackingSiteIdx]->NumSampleMaxEachSite >= floor(Inst->NumSample*0.05) && Inst->Num90 >= 10 && !Inst->Site[PackingSiteIdx]->Is90 )
      {
         continue;
      }
      InitFlowPacking(Inst, time, PackingSiteIdx);
      bool ispathfind;
      int increase_flow;
      vector<SiteStruct*> Queue;
      SiteStruct* tracebackindicator;
      ispathfind = true;
      while( ispathfind && Inst->Site[PackingSiteIdx]->temp_residual_capacity > 0 )
      {
         ResetLabel(Inst);
         Queue.clear();
         Inst->Site[PackingSiteIdx]->isdetected = true;
         Queue.push_back(Inst->Site[PackingSiteIdx]);
         ispathfind = false;
         while( Queue.size() != 0 && !ispathfind )
         {
            SiteStruct* SiteTemp = Queue[0];
            Queue.erase(Queue.begin());
            for( int i = 0; i < SiteTemp->degree; i++ )
            {
               CustStruct* CustTemp = SiteTemp->ArcOut[i]->Cust;
               if( !CustTemp->isdetected )
               {
                  CustTemp->isdetected = true;
                  CustTemp->Near = SiteTemp->ArcOut[i];
                  if( CustTemp->temp_residual_demand > 0 )
                  {
                     ispathfind = true;
                     //change_flow
                     increase_flow = Min(CustTemp->temp_residual_demand, Inst->Site[PackingSiteIdx]->temp_residual_capacity);
                     tracebackindicator = CustTemp->Near->Site;
                     while( tracebackindicator->idx != Inst->Site[PackingSiteIdx]->idx )
                     {
                        increase_flow = Min(increase_flow, tracebackindicator->Near->temp_flow);
                        tracebackindicator = tracebackindicator->Near->Cust->Near->Site;
                     }
                     Inst->Site[PackingSiteIdx]->temp_residual_capacity -= increase_flow;
                     CustTemp->temp_residual_demand -= increase_flow;
                     CustTemp->Near->temp_flow += increase_flow;
                     tracebackindicator = CustTemp->Near->Site;
                     while( tracebackindicator->idx != Inst->Site[PackingSiteIdx]->idx )
                     {
                        tracebackindicator->Near->temp_flow -= increase_flow;
                        tracebackindicator->Near->Cust->Near->temp_flow += increase_flow;
                        tracebackindicator = tracebackindicator->Near->Cust->Near->Site;
                     }
                     break;
                  }
                  else
                  {
                     for( int j = 0; j < CustTemp->degree; j++ )
                     {
                        ArcStruct* ArcTemp = CustTemp->ArcIn[j];
                        if( ArcTemp->temp_flow != 0 && !ArcTemp->Site->isdetected )
                        {
                           ArcTemp->Site->Near = ArcTemp;
                           ArcTemp->Site->isdetected = true;
                           Queue.push_back(ArcTemp->Site);
                        }
                     }
                  }
               }
               else
               {
                  continue;
               }
            }
         }
      }
      tempdemand = Inst->Site[PackingSiteIdx]->bandwidth - Inst->Site[PackingSiteIdx]->supply[time] - Inst->Site[PackingSiteIdx]->temp_residual_capacity;
      if( tempdemand > maxdemand )
      {
         maxdemand = tempdemand;
         maxdemandsiteidx = PackingSiteIdx;
      }
   }
   *potentialdemand = maxdemand;
   return maxdemandsiteidx;
}

void CalculateflowPerMoment(InstanceStruct* Inst, int time, bool islimit)
{
for( int pattern = -1; pattern >= -20; pattern-- ){
   InitFlow(Inst, time, pattern);
   bool ispathfind;
   int increase_flow;
   vector<CustStruct*> Queue;
   int idx = 0;
   CustStruct* tracebackindicator;
   while( idx < Inst->NumCust )
   {
      ispathfind = true;
      while( ispathfind && Inst->Cust[idx]->residual_demand[time] > 0 )
      {
         ResetLabel(Inst);
         Queue.clear();
         Inst->Cust[idx]->isdetected = true;
         Queue.push_back(Inst->Cust[idx]);
         ispathfind = false;
         while( Queue.size() != 0 && !ispathfind )
         {
            CustStruct* CustTemp = Queue[0];
            Queue.erase(Queue.begin());
            for( int i = 0; i < CustTemp->degree; i++ )
            {
               SiteStruct* SiteTemp = CustTemp->ArcIn[i]->Site;
               if( !SiteTemp->isdetected )
               {
                  SiteTemp->isdetected = true;
                  SiteTemp->Near = CustTemp->ArcIn[i];
                  if( (SiteTemp->IsUse || !islimit) && SiteTemp->temp_residual_capacity > 0 )
                  {
                     if( !SiteTemp->IsUse )
                     {
                        SiteTemp->IsUse = true;
                        Inst->NumUse++;
                     }
                     ispathfind = true;
                     //change_flow
                     increase_flow = Min(SiteTemp->temp_residual_capacity, Inst->Cust[idx]->residual_demand[time]);
                     tracebackindicator = SiteTemp->Near->Cust;
                     while( tracebackindicator->idx != Inst->Cust[idx]->idx )
                     {
                        increase_flow = Min(increase_flow, tracebackindicator->Near->flow[time]);
                        tracebackindicator = tracebackindicator->Near->Site->Near->Cust;
                     }
                     Inst->Sample[time]->TotalResidualDemandEachSample -= increase_flow;
                     Inst->Cust[idx]->residual_demand[time] -= increase_flow;
                     SiteTemp->temp_residual_capacity -= increase_flow;
                     SiteTemp->supply[time] += increase_flow;
                     SiteTemp->Near->flow[time] += increase_flow;
                     tracebackindicator = SiteTemp->Near->Cust;
                     while( tracebackindicator->idx != Inst->Cust[idx]->idx )
                     {
                        tracebackindicator->Near->flow[time] -= increase_flow;
                        tracebackindicator->Near->Site->Near->flow[time] += increase_flow;
                        tracebackindicator = tracebackindicator->Near->Site->Near->Cust;
                     }
                     break;
                  }
                  else
                  {
                     for( int j = 0; j < SiteTemp->degree; j++ )
                     {
                        ArcStruct* ArcTemp = SiteTemp->ArcOut[j];
                        if( ArcTemp->flow[time] != 0 && !ArcTemp->Cust->isdetected )
                        {
                           ArcTemp->Cust->Near = ArcTemp;
                           ArcTemp->Cust->isdetected = true;
                           Queue.push_back(ArcTemp->Cust);
                        }
                     }
                  }
               }
               else
               {
                  continue;
               }
            }
         }
      }
      idx++;
   }
}}

int FindDemandBiggestTime(InstanceStruct* Inst, bool pattern)
{
   int mindemand = 0;
   int idx = -1;
   for( int i = 0; i < Inst->NumSample; i++ )
   {
      if( Inst->Sample[i]->TotalResidualDemandEachSample > mindemand && (pattern || !Inst->Sample[i]->IsBannedToAddBiggestSite) )
      {
         mindemand = Inst->Sample[i]->TotalResidualDemandEachSample;
         idx = i;
      }
   }
   return idx;
}

void AssignStream(InstanceStruct* Inst)
{
   CustStruct* CustTemp;
   ArcStruct* ArcTemp;
   StreamStruct* StreamTemp;
   bool isassigned;
   int arcsize;
   int streamsize;
#ifdef _DEBUG
   //check flow
   int flowsum;
   int streamsum;
   for( int time = 0; time < Inst->NumSample; time++ )
   {
      for( int i = 0; i < Inst->NumCust; i++ )
      {
         flowsum = 0;
         streamsum = 0;
         CustTemp = Inst->Cust[i];
         for( int j = 0; j < CustTemp->degree; j++ )
         {
            flowsum += CustTemp->ArcIn[j]->flow[time];
         }
         int streamsize = CustTemp->demand[time].size();
         for( int j = 0; j < streamsize; j++ )
         {
            streamsum += CustTemp->demand[time][j]->value;
         }
         assert(flowsum == CustTemp->demandSum[time]);
         assert(streamsum == CustTemp->demandSum[time]);
      }
   }
#endif
   //count the pre-allocated arcs
   for( int time = 0; time < Inst->NumSample; time++ )
   {
      for( int i = 0; i < Inst->NumCust; i++ )
      {
         CustTemp = Inst->Cust[i];
         CustTemp->ArcAssignedflow[time].clear();
         for( int j = 0; j < CustTemp->degree; j++ )
         {
            ArcTemp = CustTemp->ArcIn[j];
            if( ArcTemp->flow[time] != 0 )
            {
               CustTemp->ArcAssignedflow[time].push_back(ArcTemp);
            }
         }
      }
   }
   //allocate stream
   for( int time = 0; time < Inst->NumSample; time++ )
   {
      for( int i = 0; i < Inst->NumCust; i++ )
      {
         CustTemp = Inst->Cust[i];
         if( CustTemp->ArcAssignedflow[time].size() == 0 )
         {
            continue;
         }
         else if( CustTemp->ArcAssignedflow[time].size() == 1 )
         {
            ArcTemp = CustTemp->ArcAssignedflow[time][0];
            streamsize = CustTemp->demand[time].size();
            for( int streamidx = 0; streamidx < streamsize; streamidx++ )
            {
               StreamTemp = CustTemp->demand[time][streamidx];
               StreamTemp->IsAssigned = true;
               ArcTemp->Stream[time].push_back(StreamTemp);
            }
            ArcTemp->flowinteger[time] = ArcTemp->flow[time];
         }
         else
         {
            streamsize = CustTemp->demand[time].size();
            for( int streamidx = 0; streamidx < streamsize; streamidx++ )
            {
               StreamTemp = CustTemp->demand[time][streamidx];
               isassigned = false;
               arcsize = CustTemp->ArcAssignedflow[time].size();
               for( int arcidx = 0; arcidx < arcsize; arcidx++ )
               {
                  ArcTemp = CustTemp->ArcAssignedflow[time][arcidx];
                  if( ArcTemp->flow[time] - ArcTemp->flowinteger[time] >= StreamTemp->value )
                  {
                     StreamTemp->IsAssigned = true;
                     ArcTemp->Stream[time].push_back(StreamTemp);
                     ArcTemp->flowinteger[time] += StreamTemp->value;
                     isassigned = true;
                     break;
                  }
               }
               if( isassigned )
               {
                  continue;
               }
               arcsize = CustTemp->degree;
               for( int arcidx = 0; arcidx < arcsize; arcidx++ )
               {
                  ArcTemp = CustTemp->ArcIn[arcidx];
                  if( ArcTemp->Site->IsUse && ArcTemp->Site->bandwidth - ArcTemp->Site->supply[time] >= StreamTemp->value )
                  {
                     StreamTemp->IsAssigned = true;
                     ArcTemp->Stream[time].push_back(StreamTemp);
                     ArcTemp->Site->supply[time] += StreamTemp->value;
                     ArcTemp->flowinteger[time] += StreamTemp->value;
                     isassigned = true;
                     break;
                  }
               }
               if( isassigned )
               {
                  continue;
               }
               for( int arcidx = 0; arcidx < arcsize; arcidx++ )
               {
                  ArcTemp = CustTemp->ArcIn[arcidx];
                  if( !ArcTemp->Site->IsUse && ArcTemp->Site->bandwidth - ArcTemp->Site->supply[time] >= StreamTemp->value )
                  {
                     StreamTemp->IsAssigned = true;
                     ArcTemp->Site->IsUse = true;
                     Inst->NumUse++;
                     ArcTemp->Stream[time].push_back(StreamTemp);
                     ArcTemp->Site->supply[time] += StreamTemp->value;
                     ArcTemp->flowinteger[time] += StreamTemp->value;
                     isassigned = true;
                     break;
                  }
               }
               assert(StreamTemp->IsAssigned);
            }
         }
      }
   }
#ifdef _DEBUG
   for( int time = 0; time < Inst->NumSample; time++ )
   {
      for( int i = 0; i < Inst->NumCust; i++ )
      {
         CustTemp = Inst->Cust[i];
         for( int j = 0; j < CustTemp->degree; j++ )
         {
            streamsum = 0;
            int streamsize = CustTemp->ArcIn[j]->Stream[time].size();
            for( int k = 0; k < streamsize; k++ )
            {
               streamsum += CustTemp->ArcIn[j]->Stream[time][k]->value;
            }
            assert(CustTemp->ArcIn[j]->flowinteger[time] == streamsum);
         }
      }
   }
#endif
}
