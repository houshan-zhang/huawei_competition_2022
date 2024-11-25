#include "Functions.h"


int main()
{
   double runtime;
   clock_t time_start = clock();
   //init
   InstanceStruct Inst;
   ReadFile(&Inst);
   Presolve(&Inst);
   //Predict Used Sites
   /*SampleStruct SampleBig;
   SampleBig.TotalResidualDemandEachSample = 0;
   for( int i = 0; i < Inst.NumCust; i++ )
   {
      CustStruct* CustTemp = Inst.Cust[i];
      CustTemp->residual_demand.push_back(0);
      for( int time = 0; time < Inst.NumSample; time++ )
      {
         if( CustTemp->residual_demand[Inst.NumSample] < CustTemp->residual_demand[time] )
         {
            CustTemp->residual_demand[Inst.NumSample] = CustTemp->residual_demand[time];
         }
      }
      SampleBig.TotalResidualDemandEachSample += CustTemp->residual_demand[Inst.NumSample];
   }
   Inst.Sample.push_back(&SampleBig);
   for( int i = 0; i < Inst.NumArc; i++ )
   {
      Inst.Arc[i]->flow.push_back(0);
   }
   for( int i = 0; i < Inst.NumSite; i++ )
   {
      Inst.Site[i]->supply.push_back(0);
   }
   int rate = 1;
   while( Inst.NumUse < Inst.NumSite || SampleBig.TotalResidualDemandEachSample > 0 )
   {
      int potentialdemand = 0;
      int siteidx = FindDemandBiggestSite(&Inst, Inst.NumSample, &potentialdemand, true);
      if( siteidx != -1 )
      {
         if( !Inst.Site[siteidx]->IsPredictUse )
         {
            Inst.Site[siteidx]->IsPredictUse = true;
            Inst.NumPredictUse++;
         }
         FillingSite(&Inst, Inst.NumSample, siteidx, Inst.base_cost + floor((Inst.Site[siteidx]->bandwidth-Inst.base_cost)*rate));
      }
      else
      {
         break;
      }
   }
   cout<<"@02 PREDICT MINIMUM USED SITES: "<<Inst.NumPredictUse<<endl;*/
   //allocate 5% site
   while( Inst.NumSSMax < floor(Inst.NumSample*0.05)*Inst.NumSite )
   {
      //find the biggest demand time
      int time = FindDemandBiggestTime(&Inst, false);
      if( time == -1 )
      {
         break;
      }
      int BiggestSiteIdx;
      int potentialdemand = 0;
      BiggestSiteIdx = FindDemandBiggestSite(&Inst, time, &potentialdemand, false);
      int rate = 2;
      if( potentialdemand < rate*Inst.base_cost )
      {
         Inst.Sample[time]->IsBannedToAddBiggestSite = true;
      }
      else
      {
         Inst.Site[BiggestSiteIdx]->NumSampleMaxEachSite++;
         Inst.NumSSMax++;
         Inst.Site[BiggestSiteIdx]->IsMax[time] = true;
         FillingSite(&Inst, time, BiggestSiteIdx, -1);
         if( Inst.Site[BiggestSiteIdx]->NumSampleMaxEachSite > floor(Inst.NumSample*0.05) && !Inst.Site[BiggestSiteIdx]->Is90 )
         {
            Inst.Site[BiggestSiteIdx]->Is90 = true;
            Inst.Num90++;
         }
         if( !Inst.Site[BiggestSiteIdx]->IsUse )
         {
            Inst.Site[BiggestSiteIdx]->IsUse = true;
            Inst.NumUse++;
            /*or( int i = 0; i < Inst.NumSample; i++ )
            {
               if( rate*Inst.base_cost - Inst.Site[BiggestSiteIdx]->supply[i] > 0 )
               {
                  FillingSite(&Inst, i, BiggestSiteIdx, rate*Inst.base_cost - Inst.Site[BiggestSiteIdx]->supply[i]);
               }
            }*/
         }
      }
   }
   cout<<"@03 NUMSSMAX "<<Inst.NumSSMax<<" NUM90SITE "<<Inst.Num90<<endl;
   //get the initial solution
   for( int time = 0; time < Inst.NumSample; time++ )
   {
      CalculateflowPerMoment(&Inst, time, true);
   }
   int score;
   score = CalculateScore(&Inst);
   cout<<"Score: "<<score<<endl;
   int NumOld = Inst.NumUse;
   AssignStream(&Inst);
   int NumNew = Inst.NumUse;
   cout<<"@04 ASSIGN STREAM USED SITES "<<NumOld<<" -> "<<NumNew<<endl;
   WriteFile(&Inst);
#ifdef _DEBUG
   display(&Inst);
   assert(CheckSolution(&Inst));
#endif
   FreeMemory(&Inst);
   clock_t time_end = clock();
   runtime = (double)(time_end-time_start)/(double)CLOCKS_PER_SEC;
   cout<<"@05 RUNTIME "<<runtime<<" s"<<endl;
	return 0;
}
