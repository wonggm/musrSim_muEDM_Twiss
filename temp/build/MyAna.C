#include "MyAna.h"
#include <iostream>
#include <cmath>
#include <TROOT.h>
#include <TRandom.h>
#include <TCanvas.h>
#include <string.h>
#include <stdio.h>
#include "TSystem.h"

void MyAna::Loop (string filename)
{
   //  cout << "Loop()" << endl;

   initialize (filename);

   if (fChain == 0)
      return;

   Long64_t nentries = fChain->GetEntriesFast ();
   Int_t nbytes = 0, nb = 0;

   for (Long64_t jentry = 0; jentry < nentries; jentry++)
   {
      Long64_t ientry = LoadTree (jentry);
      if (ientry < 0)
	 break;
      nb = fChain->GetEntry (jentry);
      nbytes += nb;

      clear ();
      execute ();

   }

   finalize ();

}

void MyAna::initialize (string filename)
{

   cout << "initialize()" << endl;

   filename.append ("_hist.root");
   char *out = (char *) filename.c_str ();
   m_file = new TFile (out, "recreate");

   total_event = 0; // count total number of events

   mup_energy = new TH1D("mup_energy","mup_energy",120,0,60);

}

void MyAna::execute ()
{
 //  if(total_event%1000==0){
      cout<<"--> Event ID: "<<eventID<<endl;
 // }
   total_event++; 
   double energy_pos=0.0;
   energy_pos = pow(pow(posIniMomX,2)+pow(posIniMomY,2)+pow(posIniMomZ,2),0.5);

   for(int i=0;i<det_n;i++){
      cout<<"Hit Edep: "<<det_edep[i]<<endl;
      cout<<"Hit Time: "<<det_time_start[i]<<endl;
      cout<<"Muon Hit Edep: "<<det_edep_mup[i]<<endl;
      cout<<"Electron Hit Edep: "<<det_edep_el[i]<<endl;
      cout<<"Positron Hit Edep: "<<det_edep_pos[i]<<endl;
      cout<<"Positron Energy: "<<energy_pos<<endl;
      }

    mup_energy->Fill(energy_pos);
}

void MyAna::finalize ()
{
   cout<<"Finalizing......"<<endl;
   cout<<"Total Event: "<<total_event<<endl;

   m_file->Write ();
   m_file->Close ();
}

void MyAna::clear (){}
