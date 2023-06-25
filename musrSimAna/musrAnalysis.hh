//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Mar 25 15:00:51 2010 by ROOT version 5.24/00
// from TTree t1/a simple Tree with simple variables
// found on file: data/musr_40003.root
//////////////////////////////////////////////////////////

#ifndef musrAnalysis_h
#define musrAnalysis_h

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TVectorD.h>
#include "musrCounter.hh"
#include "musrWriteDump.hh"
//#include "musrTH.hh"
#include <math.h>

const double pi=3.14159265358979324; // initialise here instead JSL
const double microsecond=1.;
const double nanosecond=0.001;
const double picosecond=0.000001;


class musrTH;

//#include "musrSimGlobal.hh"

typedef std::map<int,int> debugEventMapType;
extern debugEventMapType debugEventMap;
extern Bool_t bool_debugingRequired;


class musrAnalysis {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           runID;
   Int_t           eventID;
   Double_t        weight;
   Double_t        timeToNextEvent;
   Double_t        BFieldAtDecay_Bx;
   Double_t        BFieldAtDecay_By;
   Double_t        BFieldAtDecay_Bz;
   Double_t        BFieldAtDecay_B3;
   Double_t        BFieldAtDecay_B4;
   Double_t        BFieldAtDecay_B5;
   Double_t        muIniTime;
   Double_t        muIniPosX;
   Double_t        muIniPosY;
   Double_t        muIniPosZ;
   Double_t        muIniMomX;
   Double_t        muIniMomY;
   Double_t        muIniMomZ;
   Double_t        muIniPolX;
   Double_t        muIniPolY;
   Double_t        muIniPolZ;
   Int_t           muDecayDetID;
   Double_t        muDecayPosX;
   Double_t        muDecayPosY;
   Double_t        muDecayPosZ;
   Double_t        muDecayTime;
   Double_t        muDecayPolX;
   Double_t        muDecayPolY;
   Double_t        muDecayPolZ;
   Double_t        muTargetTime;
   Double_t        muTargetPolX;
   Double_t        muTargetPolY;
   Double_t        muTargetPolZ;
   Double_t        muTargetMomX;
   Double_t        muTargetMomY;
   Double_t        muTargetMomZ;
   Double_t        muM0Time;
   Double_t        muM0PolX;
   Double_t        muM0PolY;
   Double_t        muM0PolZ;
   Double_t        posIniMomX;
   Double_t        posIniMomY;
   Double_t        posIniMomZ;
   Int_t           nFieldNomVal;
   Double_t        fieldNomVal[50];   //[nFieldNomVal]
   Int_t           det_n;
   Int_t           det_ID[500];   //[det_n]
   Double_t        det_edep[500];   //[det_n]
   Double_t        det_edep_el[500];   //[det_n]
   Double_t        det_edep_pos[500];   //[det_n]
   Double_t        det_edep_gam[500];   //[det_n]
   Double_t        det_edep_mup[500];   //[det_n]
   Int_t           det_nsteps[500];   //[det_n]
   Double_t        det_length[500];   //[det_n]
   Double_t        det_time_start[500];   //[det_n]
   Double_t        det_time_end[500];   //[det_n]
   Double_t        det_x[500];   //[det_n]
   Double_t        det_y[500];   //[det_n]
   Double_t        det_z[500];   //[det_n]
   Double_t        det_kine[500];   //[det_n]
   Double_t        det_VrtxKine[500];   //[det_n]
   Double_t        det_VrtxX[500];   //[det_n]
   Double_t        det_VrtxY[500];   //[det_n]
   Double_t        det_VrtxZ[500];   //[det_n]
   Int_t           det_VrtxVolID[500];   //[det_n]
   Int_t           det_VrtxProcID[500];   //[det_n]
   Int_t           det_VrtxTrackID[500];   //[det_n]
   Int_t           det_VrtxParticleID[500];   //[det_n]
   Double_t        det_VvvKine[500];   //[det_n]
   Double_t        det_VvvX[500];   //[det_n]
   Double_t        det_VvvY[500];   //[det_n]
   Double_t        det_VvvZ[500];   //[det_n]
   Int_t           det_VvvVolID[500];   //[det_n]
   Int_t           det_VvvProcID[500];   //[det_n]
   Int_t           det_VvvTrackID[500];   //[det_n]
   Int_t           det_VvvParticleID[500];   //[det_n]

   // List of branches
   TBranch        *b_runID;   //!
   TBranch        *b_eventID;   //!
   TBranch        *b_weight;   //!
   TBranch        *b_timeToNextEvent;   //!
   TBranch        *b_BFieldAtDecay;   //!
   TBranch        *b_muIniTime;   //!
   TBranch        *b_muIniPosX;   //!
   TBranch        *b_muIniPosY;   //!
   TBranch        *b_muIniPosZ;   //!
   TBranch        *b_muIniMomX;   //!
   TBranch        *b_muIniMomY;   //!
   TBranch        *b_muIniMomZ;   //!
   TBranch        *b_muIniPolX;   //!
   TBranch        *b_muIniPolY;   //!
   TBranch        *b_muIniPolZ;   //!
   TBranch        *b_muDecayDetID;   //!
   TBranch        *b_muDecayPosX;   //!
   TBranch        *b_muDecayPosY;   //!
   TBranch        *b_muDecayPosZ;   //!
   TBranch        *b_muDecayTime;   //!
   TBranch        *b_muDecayPolX;   //!
   TBranch        *b_muDecayPolY;   //!
   TBranch        *b_muDecayPolZ;   //!
   TBranch        *b_muTargetTime;   //!
   TBranch        *b_muTargetPolX;   //!
   TBranch        *b_muTargetPolY;   //!
   TBranch        *b_muTargetPolZ;   //!
   TBranch        *b_muTargetMomX;   //!
   TBranch        *b_muTargetMomY;   //!
   TBranch        *b_muTargetMomZ;   //!
   TBranch        *b_muM0Time;   //!
   TBranch        *b_muM0PolX;   //!
   TBranch        *b_muM0PolY;   //!
   TBranch        *b_muM0PolZ;   //!
   TBranch        *b_posIniMomX;   //!
   TBranch        *b_posIniMomY;   //!
   TBranch        *b_posIniMomZ;   //!
   TBranch        *b_nFieldNomVal;   //!
   TBranch        *b_fieldNomVal;   //!
   TBranch        *b_det_n;   //!
   TBranch        *b_det_ID;   //!
   TBranch        *b_det_edep;   //!
   TBranch        *b_det_edep_el;   //!
   TBranch        *b_det_edep_pos;   //!
   TBranch        *b_det_edep_gam;   //!
   TBranch        *b_det_edep_mup;   //!
   TBranch        *b_det_nsteps;   //!
   TBranch        *b_det_length;   //!
   TBranch        *b_det_time_start;   //!
   TBranch        *b_det_time_end;   //!
   TBranch        *b_det_x;   //!
   TBranch        *b_det_y;   //!
   TBranch        *b_det_z;   //!
   TBranch        *b_det_kine;   //!
   TBranch        *b_det_VrtxKine;   //!
   TBranch        *b_det_VrtxX;   //!
   TBranch        *b_det_VrtxY;   //!
   TBranch        *b_det_VrtxZ;   //!
   TBranch        *b_det_VrtxVolID;   //!
   TBranch        *b_det_VrtxProcID;   //!
   TBranch        *b_det_VrtxTrackID;   //!
   TBranch        *b_det_VrtxParticleID;   //!
   TBranch        *b_det_VvvKine;   //!
   TBranch        *b_det_VvvX;   //!
   TBranch        *b_det_VvvY;   //!
   TBranch        *b_det_VvvZ;   //!
   TBranch        *b_det_VvvVolID;   //!
   TBranch        *b_det_VvvProcID;   //!
   TBranch        *b_det_VvvTrackID;   //!
   TBranch        *b_det_VvvParticleID;   //!

   typedef std::map<std::string, Double_t*> variableMapType;
   variableMapType variableMap;
   Double_t        runID_double;
   Double_t        eventID_double;
   Double_t        muDecayDetID_double;
   Double_t        det_n_double;
   Double_t	   det_multifirst_double;
   Double_t        muDecayPosR;
   Double_t        wght;
   Double_t        det_m0edep;
   Double_t        det_posEdep;
   Double_t        muIniPosR;
   Double_t        muIniMomTrans;
   Double_t        muTargetPol_Theta;
   Double_t        muTargetPol_Theta360;
   Double_t        muTargetPol_Phi;
   Double_t        muTargetPol_Phi360;
   Double_t        muDecayPol_Theta;
   Double_t        muDecayPol_Theta360;
   Double_t        muDecayPol_Phi;
   Double_t        muDecayPol_Phi360;
   Double_t        pos_Trans_Momentum;
   Double_t        pos_Momentum;
   Double_t        pos_Radius;
   Double_t        pos_Theta;
   Double_t        pos_Theta360;
   Double_t        pos_Phi;
   Double_t        pos_Phi360;
   Double_t        pos_Theta_MINUS_muDecayPol_Theta;
   Double_t        pos_Theta_MINUS_muDecayPol_Theta360;
   Double_t        pos_Phi_MINUS_muDecayPol_Phi;
   Double_t        pos_Phi_MINUS_muDecayPol_Phi360;
   Double_t        pos_detID;
   Double_t        pos_detID_doubleHit;
   Double_t        pos_doubleHit_dPhi;
  //   Double_t        det_time0;
  //   Double_t        get_time0;
  //   Double_t        det_time1;
  //   Double_t        gen_time1;
   Double_t        det_time10;
   Double_t        gen_time10;
   Double_t        det_time10_MINUS_gen_time10;
   Double_t        det_time20;
   Double_t        det_time31;
   Double_t        det_time1_MINUS_muDecayTime;
   Double_t        det_multi_interval;
   Double_t        detP_x;
   Double_t        detP_y;
   Double_t        detP_z;
   Double_t        detP_time_start;
   Double_t        detP_time_end;
   Double_t        detP_theta;
   Double_t        detP_phi;
   Double_t        detP_phi_MINUS_pos_Phi;
   Double_t        detP_phi_MINUS_pos_Phi360;
   Double_t        detP_theta_MINUS_pos_Theta;
   Double_t        detP_theta_MINUS_pos_Theta360;
   Double_t        detP_time_start_MINUS_muDecayTime;
   Double_t        pileup_eventID;
   Double_t        pileup_muDecayDetID_double;
   Double_t        pileup_muDecayPosX;
   Double_t        pileup_muDecayPosY;
   Double_t        pileup_muDecayPosZ;
   Double_t        pileup_muDecayPosR;


   typedef std::map<int, Bool_t*> conditionMapType;
   conditionMapType conditionMap;
   Bool_t          alwaysTrue;
   Bool_t          oncePerEvent;
   Bool_t          muonDecayedInSample_gen;
   Bool_t          muonDecayedInSampleOnce_gen;
   Bool_t          muonTriggered_gen;
   Bool_t          muonTriggered_gen_AND_muonDecayedInSample_gen;
   Bool_t          muonTriggered_det;
   Bool_t          positronHit_det;
   Bool_t          goodEvent_det;
   Bool_t          goodEvent_gen;
   Bool_t          goodEvent_det_AND_goodEvent_gen;
   Bool_t          pileupEventCandidate;
   Bool_t          pileupEvent;
   Bool_t          goodEvent_det_AND_muonDecayedInSample_gen;
   Bool_t          goodEvent_F_det_AND_muonDecayedInSample_gen;
   Bool_t          goodEvent_B_det_AND_muonDecayedInSample_gen;
   Bool_t          goodEvent_U_det_AND_muonDecayedInSample_gen;
   Bool_t          goodEvent_D_det_AND_muonDecayedInSample_gen;
   Bool_t          goodEvent_L_det_AND_muonDecayedInSample_gen;
   Bool_t          goodEvent_R_det_AND_muonDecayedInSample_gen;
   Bool_t          goodEvent_F_det;
   Bool_t          goodEvent_B_det;
   Bool_t          goodEvent_U_det;
   Bool_t          goodEvent_D_det;
   Bool_t          goodEvent_L_det;
   Bool_t          goodEvent_R_det;
   Bool_t          goodEvent_F_det_AND_pileupEvent;
   Bool_t          goodEvent_B_det_AND_pileupEvent;
   Bool_t          goodEvent_U_det_AND_pileupEvent;
   Bool_t          goodEvent_D_det_AND_pileupEvent;
   Bool_t          goodEvent_L_det_AND_pileupEvent;
   Bool_t          goodEvent_R_det_AND_pileupEvent;
   Bool_t          promptPeak;
   Bool_t          promptPeakF;
   Bool_t          promptPeakB;
   Bool_t          promptPeakU;
   Bool_t          promptPeakD;
   Bool_t          promptPeakL;
   Bool_t          promptPeakR;
   Bool_t          doubleHit;
   // pulsed:
   // goodEvent_det = (discrim triggered, however happened)
   // goodEvent_gen = (1st or only over-threshold event, even if killed by dead time)
   // doubleHitEvent_gen = (2nd onwards of a double hit sequence, whether detected or dead)
   Bool_t doubleHitEvent_gen;
   // doubleHit = (2nd and subsequent hits, counted)
   // to subdivide double counting among banks (2nd and subsequent)
   Bool_t goodEvent_F_det_AND_doubleHit;
   Bool_t goodEvent_B_det_AND_doubleHit;
   Bool_t goodEvent_U_det_AND_doubleHit;
   Bool_t goodEvent_D_det_AND_doubleHit;
   Bool_t goodEvent_L_det_AND_doubleHit;
   Bool_t goodEvent_R_det_AND_doubleHit;
   // singleHitEvent_gen = (single hits only)
   Bool_t singleHitEvent_gen;
   // stackedEvent_gen = (multiple small hits adding to threshold, muon details refer to last one)
   Bool_t stackedEvent_gen;
   // pileupEvent = (all events killed by dead time)
   // ..._AND_pileupEvent = subdivided by bank (note the _det isn't appropriate here)

   musrAnalysis(TTree *tree=0);
   virtual ~musrAnalysis();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(char* runChar, char* v1190FileName, Int_t nrEvents);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
   virtual void     ReadInInputParameters(char* charV1190FileName);
   virtual void     CreateHistograms();
   virtual void     AnalyseEvent(Long64_t iiiEntry);
   virtual void     FillHistograms(Int_t iiiEntry);
   virtual void     FillHistogramsPulsed(Int_t iiiEntry1, Int_t iiiEntry2);
   virtual void     SaveHistograms(char* runChar,char* v1190FileName);
   virtual void     RemoveOldHitsFromCounters(Long64_t timeBinLimit);
  //   virtual void     RewindAllTimeInfo(Double_t timeToRewind);
  //   virtual void     RewindAllTimeInfo(Long64_t timeBinsToRewind);
   virtual void     RewindAllTimeInfo();
   virtual void     PrintHitsInAllCounters();
   virtual void     InitialiseEvent();
   virtual Double_t PreprocessEvent(Long64_t iEn);
   virtual Bool_t   PositronCounterHit(Int_t evID, Long64_t dataBinMin, Long64_t dataBinMax, Long64_t positronBinMax, Long64_t& tBin1, Long64_t& tBin2, Long64_t& tBinDoubleHit, Int_t& kEntry, Int_t& idetP, Int_t& idetP_ID, Double_t& idetP_edep, Int_t& idetP_ID_doubleHit);
  //   virtual Bool_t   MuonCounterHit(Int_t evID, Long64_t timeBinMin, Long64_t& timeBin0, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep);
   void CopySubstring(char* inputChar,int iStart,int iEnd,char* outputChar);
   void MyPrintTree();
   void MyPrintConditions();
   Double_t myAtan2(Double_t y, Double_t x);
   Double_t deltaAngle(Double_t alpha, Double_t beta);

   typedef std::map<int,Double_t> phaseShiftMapType;
   phaseShiftMapType phaseShiftMap;
   TH1D* hGeantParameters;
   TH1D* hInfo;

   typedef std::multimap<double,int> doubleHitSorterType;

  //   typedef std::map<int,int> debugEventMapType;
  //   debugEventMapType debugEventMap;
  //   Bool_t bool_debugingRequired;

  //JSL   static const Double_t pi=3.14159265358979324;
   static musrWriteDump* myWriteDump;

 private:
   std::string instrument;
   std::string description;
   std::string tdchwtype;
   Double_t tdcresolution;
   Long64_t nentries;
   Int_t  mdelay;
   Int_t  pdelay;
   Int_t  mcoincwin;
   Int_t  pcoincwin; // pulsed: for dead time or add-up overlap. Model as exponential decay of this time constant?
   Int_t  vcoincwin;
   Double_t muonRateFactor; // also mean rate for pulsed mode
   Double_t muonPulseWidthFactor; // pulsed
   Double_t dataWindowMin;
   Double_t dataWindowMax;
   Double_t pileupWindowMin;
   Double_t pileupWindowMax;
   Double_t promptPeakWindowMin;
   Double_t promptPeakWindowMax;
   Int_t    overallBinDelay;
   Bool_t   boolInfinitelyLowMuonRate;
   Double_t frameInterval; // JSL: pulsed: to divide muons into frames
   Double_t detRecoveryTime; // JSL: pulsed: for dead time effects
   Bool_t doPartialFrameAtEnd; // process the left over muons if the beam goes off mid frame having done some complete frames
   // Bool_t processInBulk; // do all the muons in one go rather than re-filtering at end of run
   Double_t commonThreshold; // set all thresholds to this value regardless of individual settings

   char   musrMode;  // D = time diferential;  I = time integral; P = pulsed time differential
   Double_t safeTimeWindow;
   Double_t currentTime;
  //   Double_t nextEventTime;
   Double_t nextUnfilledEventTime;
   Long64_t numberOfRewinds; // JSL: number of frames for Pulsed Mode
   Long64_t numberOfGoodMuons;
  //   Int_t    currentEventID;
   Long64_t lastPreprocessedEntry;
   Long64_t firstPreprocessedEntry; // JSL: first in this frame
   musrCounter* mCounter;
   typedef std::map<int,musrCounter*> counterMapType;
   counterMapType pCounterMap;
   counterMapType kCounterMap;
   counterMapType vCounterMap;
   counterMapType allCounterMap;
   Int_t testIVar1;
   Double_t omega;
   Bool_t   bool_muDecayTimeTransformation;
   Double_t muDecayTime_Transformation_min, muDecayTime_Transformation_max, muDecayTime_t_min, muDecayTime_t_max;

  //   static const Double_t microsecond=1.;
  //   static const Double_t nanosecond=0.001;
  //   static const Double_t picosecond=0.000001;
  //   static const Double_t rewindTime=1000000.;
  //   static const Double_t rewindTime=1000.;
  //   static const Long64_t rewindTimeBins=1000000000000000; // Max Long64_t can be +9,223,372,036,854,775,807
  //   static const Long64_t rewindTimeBins  =  1000000000;
  //   static const Double_t dataWindowMin   = -0.2;
  //   static const Double_t dataWindowMax   = 10.0;
  //   static const Double_t pileupWindowMin = -10.5;
  //   static const Double_t pileupWindowMax = 10.5;
//ckdel 20.9.2010   static const int maxChannels=32;

public:
   static const Int_t nrConditions = 31;
   Bool_t condition[nrConditions];
   Long64_t conditionCounter[nrConditions];
   static Long64_t rewindTimeBins;
  //   static Int_t clock_channelID;
  //   static Long64_t clock_interval;
   static Long64_t pileupWindowBinMin;
   static Long64_t pileupWindowBinMax;
   static Long64_t dataWindowBinMin;
   static Long64_t dataWindowBinMax;

private:
  // HISTOGRAMS:
  std::list <musrTH*> listOfAllHistograms1D;
  std::list <musrTH*> listOfAllHistograms2D;
  TH1D** hTargetZ;
  TH2D** hMuDecayMap;
  //
  musrTH* humanDecayHistograms;
  musrTH* motherOfHumanDecayHistograms;
  musrTH* humanDecayPileupHistograms;
  musrTH* motherOfHumanDecayPileupHistograms;
  //  int indexHuman;
  //  int  nHumanDecayArray[50]; 
  //  std::string sHumanDecayArray[50];

  typedef std::map<Int_t,std::string> humanDecayMapType;
  typedef std::multimap<Int_t,Int_t> humanDecayMultimapType;
  humanDecayMapType humanDecayMap;
  humanDecayMultimapType humanDecayMultimap;

  typedef std::multimap<int,int> clonedChannelsMultimapType;
  clonedChannelsMultimapType clonedChannelsMultimap;
  Bool_t bool_clonedChannelsMultimap_NotEmpty;

  // List of group of detectors: F,B,U,D,L,R:
  std::list <Int_t> F_posCounterList;
  std::list <Int_t> B_posCounterList;
  std::list <Int_t> U_posCounterList;
  std::list <Int_t> D_posCounterList;
  std::list <Int_t> L_posCounterList;
  std::list <Int_t> R_posCounterList;
  std::list <Int_t> SampleDetIDList;
  //  std::list <Int_t>::iterator posCounterList_Iterator;
};

#endif

#ifdef musrAnalysis_cxx

Long64_t musrAnalysis::rewindTimeBins  =  1000000000;
Long64_t musrAnalysis::pileupWindowBinMin;
Long64_t musrAnalysis::pileupWindowBinMax;
Long64_t musrAnalysis::dataWindowBinMin;
Long64_t musrAnalysis::dataWindowBinMax;

//Long64_t musrAnalysis::clock_interval  =  512000;
//Int_t    musrAnalysis::clock_channelID  =  31;

musrAnalysis::musrAnalysis(TTree *tree)
{
  variableMap["muDecayPosX"]=&muDecayPosX;
  variableMap["muDecayPosY"]=&muDecayPosY;
  variableMap["muDecayPosZ"]=&muDecayPosZ;
  variableMap["runID"]=&runID_double;
  variableMap["eventID"]=&eventID_double;
  variableMap["weight"]=&weight;
  variableMap["timeToNextEvent"]=&timeToNextEvent;
  variableMap["BFieldAtDecay_Bx"]=&BFieldAtDecay_Bx;
  variableMap["BFieldAtDecay_By"]=&BFieldAtDecay_By;
  variableMap["BFieldAtDecay_Bz"]=&BFieldAtDecay_Bz;
  variableMap["BFieldAtDecay_B3"]=&BFieldAtDecay_B3;
  variableMap["BFieldAtDecay_B4"]=&BFieldAtDecay_B4;
  variableMap["BFieldAtDecay_B5"]=&BFieldAtDecay_B5;
  variableMap["muIniTime"]=&muIniTime;
  variableMap["muIniPosX"]=&muIniPosX;
  variableMap["muIniPosY"]=&muIniPosY;
  variableMap["muIniPosZ"]=&muIniPosZ;
  variableMap["muIniMomX"]=&muIniMomX;
  variableMap["muIniMomY"]=&muIniMomY;
  variableMap["muIniMomZ"]=&muIniMomZ;
  variableMap["muIniPolX"]=&muIniPolX;
  variableMap["muIniPolY"]=&muIniPolY;
  variableMap["muIniPolZ"]=&muIniPolZ;
  variableMap["muDecayDetID"]=&muDecayDetID_double;
  variableMap["muDecayPosX"]=&muDecayPosX;
  variableMap["muDecayPosY"]=&muDecayPosY;
  variableMap["muDecayPosZ"]=&muDecayPosZ;
  variableMap["muDecayTime"]=&muDecayTime;
  variableMap["muDecayPolX"]=&muDecayPolX;
  variableMap["muDecayPolY"]=&muDecayPolY;
  variableMap["muDecayPolZ"]=&muDecayPolZ;
  variableMap["muTargetTime"]=&muTargetTime;
  variableMap["muTargetPolX"]=&muTargetPolX;
  variableMap["muTargetPolY"]=&muTargetPolY;
  variableMap["muTargetPolZ"]=&muTargetPolZ;
  variableMap["muTargetMomX"]=&muTargetMomX;
  variableMap["muTargetMomY"]=&muTargetMomY;
  variableMap["muTargetMomZ"]=&muTargetMomZ;
  variableMap["muM0Time"]=&muM0Time;
  variableMap["muM0PolX"]=&muM0PolX;
  variableMap["muM0PolY"]=&muM0PolY;
  variableMap["muM0PolZ"]=&muM0PolZ;
  variableMap["posIniMomX"]=&posIniMomX;
  variableMap["posIniMomY"]=&posIniMomY;
  variableMap["posIniMomZ"]=&posIniMomZ;
  //  variableMap["nFieldNomVal"]=&nFieldNomVal_double;
  //  variableMap["fieldNomVal0"]=...;   //[nFieldNomVal]
  variableMap["fieldNomVal0"]=&fieldNomVal[0]; // JSL: allow nominal field especially for background muons stopping elsewhere in map
  variableMap["fieldNomVal1"]=&fieldNomVal[1]; // a second field if defined. Could add more?
  variableMap["det_n"]=&det_n_double;
  variableMap["det_multifirst"]=&det_multifirst_double;
  //
  variableMap["muDecayPosR"]=&muDecayPosR;
  variableMap["wght"]=&wght;
  variableMap["det_m0edep"]=&det_m0edep;
  variableMap["det_posEdep"]=&det_posEdep;
  variableMap["muIniPosR"]=&muIniPosR;
  variableMap["muIniMomTrans"]=&muIniMomTrans;
  variableMap["muTargetPol_Theta"]=&muTargetPol_Theta;
  variableMap["muTargetPol_Theta360"]=&muTargetPol_Theta360;
  variableMap["muTargetPol_Phi"]=&muTargetPol_Phi;
  variableMap["muTargetPol_Phi360"]=&muTargetPol_Phi360;
  variableMap["muDecayPol_Theta"]=&muDecayPol_Theta;
  variableMap["muDecayPol_Theta360"]=&muDecayPol_Theta360;
  variableMap["muDecayPol_Phi"]=&muDecayPol_Phi;
  variableMap["muDecayPol_Phi360"]=&muDecayPol_Phi360;
  variableMap["pos_Trans_Momentum"]=&pos_Trans_Momentum;
  variableMap["pos_Momentum"]=&pos_Momentum;
  variableMap["pos_Radius"]=&pos_Radius;
  variableMap["pos_Theta"]=&pos_Theta;
  variableMap["pos_Theta360"]=&pos_Theta360;
  variableMap["pos_Phi"]=&pos_Phi;
  variableMap["pos_Phi360"]=&pos_Phi360;
  variableMap["pos_Theta_MINUS_muDecayPol_Theta"]=&pos_Theta_MINUS_muDecayPol_Theta;
  variableMap["pos_Theta_MINUS_muDecayPol_Theta360"]=&pos_Theta_MINUS_muDecayPol_Theta360;
  variableMap["pos_Phi_MINUS_muDecayPol_Phi"]=&pos_Phi_MINUS_muDecayPol_Phi;
  variableMap["pos_Phi_MINUS_muDecayPol_Phi360"]=&pos_Phi_MINUS_muDecayPol_Phi360;
  variableMap["pos_detID"]=&pos_detID;
  variableMap["pos_detID_doubleHit"]=&pos_detID_doubleHit;
  variableMap["pos_doubleHit_dPhi"]=&pos_doubleHit_dPhi;
  //  variableMap["det_time0"]=&det_time0;
  //  variableMap["gen_time0"]=&gen_time0;
  //  variableMap["det_time1"]=&det_time1;
  //  variableMap["gen_time1"]=&gen_time1;
  variableMap["det_time10"]=&det_time10;
  variableMap["gen_time10"]=&gen_time10;
  variableMap["det_time10_MINUS_gen_time10"]=&det_time10_MINUS_gen_time10;
  variableMap["det_time1_MINUS_muDecayTime"]=&det_time1_MINUS_muDecayTime;
  variableMap["multiHitInterval"]=&det_multi_interval;
  variableMap["detP_x"]=&detP_x;
  variableMap["detP_y"]=&detP_x;
  variableMap["detP_z"]=&detP_x;
  variableMap["detP_time_start"]=&detP_time_start;
  variableMap["detP_time_end"]=&detP_time_end;
  variableMap["detP_theta"]=&detP_theta;
  variableMap["detP_phi"]=&detP_phi;
  variableMap["detP_phi_MINUS_pos_Phi"]=&detP_phi_MINUS_pos_Phi;
  variableMap["detP_phi_MINUS_pos_Phi360"]=&detP_phi_MINUS_pos_Phi360;
  variableMap["detP_theta_MINUS_pos_Theta"]=&detP_theta_MINUS_pos_Theta;
  variableMap["detP_theta_MINUS_pos_Theta360"]=&detP_theta_MINUS_pos_Theta360;
  variableMap["detP_time_start_MINUS_muDecayTime"]=&detP_time_start_MINUS_muDecayTime;
  variableMap["pileup_eventID"]=&pileup_eventID;
  variableMap["pileup_muDecayDetID"]=&pileup_muDecayDetID_double;
  variableMap["pileup_muDecayPosX"]=&pileup_muDecayPosX;
  variableMap["pileup_muDecayPosY"]=&pileup_muDecayPosY;
  variableMap["pileup_muDecayPosZ"]=&pileup_muDecayPosZ;
  variableMap["pileup_muDecayPosR"]=&pileup_muDecayPosR;
  variableMap["det_time20"]=&det_time20;
  variableMap["det_time31"]=&det_time31;

  testIVar1=0;
  humanDecayHistograms=NULL;
  motherOfHumanDecayHistograms=NULL;
  humanDecayPileupHistograms=NULL;
  motherOfHumanDecayPileupHistograms=NULL;
  bool_muDecayTimeTransformation = false;
  bool_clonedChannelsMultimap_NotEmpty = false;
  bool_debugingRequired = false;

// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("data/musr_40003.root");
      if (!f) {
         f = new TFile("data/musr_40003.root");
      }
      tree = (TTree*)gDirectory->Get("t1");

   }
   hGeantParameters = (TH1D*) gDirectory->Get("hGeantParameters");
   Init(tree);
}

musrAnalysis::~musrAnalysis()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t musrAnalysis::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}

Long64_t musrAnalysis::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void musrAnalysis::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("runID", &runID, &b_runID);
   fChain->SetBranchAddress("eventID", &eventID, &b_eventID);
   fChain->SetBranchAddress("weight", &weight, &b_weight);
   fChain->SetBranchAddress("timeToNextEvent", &timeToNextEvent, &b_timeToNextEvent);
   fChain->SetBranchAddress("BFieldAtDecay", &BFieldAtDecay_Bx, &b_BFieldAtDecay);
   fChain->SetBranchAddress("muIniTime", &muIniTime, &b_muIniTime);
   fChain->SetBranchAddress("muIniPosX", &muIniPosX, &b_muIniPosX);
   fChain->SetBranchAddress("muIniPosY", &muIniPosY, &b_muIniPosY);
   fChain->SetBranchAddress("muIniPosZ", &muIniPosZ, &b_muIniPosZ);
   fChain->SetBranchAddress("muIniMomX", &muIniMomX, &b_muIniMomX);
   fChain->SetBranchAddress("muIniMomY", &muIniMomY, &b_muIniMomY);
   fChain->SetBranchAddress("muIniMomZ", &muIniMomZ, &b_muIniMomZ);
   fChain->SetBranchAddress("muIniPolX", &muIniPolX, &b_muIniPolX);
   fChain->SetBranchAddress("muIniPolY", &muIniPolY, &b_muIniPolY);
   fChain->SetBranchAddress("muIniPolZ", &muIniPolZ, &b_muIniPolZ);
   fChain->SetBranchAddress("muDecayDetID", &muDecayDetID, &b_muDecayDetID);
   fChain->SetBranchAddress("muDecayPosX", &muDecayPosX, &b_muDecayPosX);
   fChain->SetBranchAddress("muDecayPosY", &muDecayPosY, &b_muDecayPosY);
   fChain->SetBranchAddress("muDecayPosZ", &muDecayPosZ, &b_muDecayPosZ);
   fChain->SetBranchAddress("muDecayTime", &muDecayTime, &b_muDecayTime);
   fChain->SetBranchAddress("muDecayPolX", &muDecayPolX, &b_muDecayPolX);
   fChain->SetBranchAddress("muDecayPolY", &muDecayPolY, &b_muDecayPolY);
   fChain->SetBranchAddress("muDecayPolZ", &muDecayPolZ, &b_muDecayPolZ);
   fChain->SetBranchAddress("muTargetTime", &muTargetTime, &b_muTargetTime);
   fChain->SetBranchAddress("muTargetPolX", &muTargetPolX, &b_muTargetPolX);
   fChain->SetBranchAddress("muTargetPolY", &muTargetPolY, &b_muTargetPolY);
   fChain->SetBranchAddress("muTargetPolZ", &muTargetPolZ, &b_muTargetPolZ);
   fChain->SetBranchAddress("muTargetMomX", &muTargetMomX, &b_muTargetMomX);
   fChain->SetBranchAddress("muTargetMomY", &muTargetMomY, &b_muTargetMomY);
   fChain->SetBranchAddress("muTargetMomZ", &muTargetMomZ, &b_muTargetMomZ);
   fChain->SetBranchAddress("muM0Time", &muM0Time, &b_muM0Time);
   fChain->SetBranchAddress("muM0PolX", &muM0PolX, &b_muM0PolX);
   fChain->SetBranchAddress("muM0PolY", &muM0PolY, &b_muM0PolY);
   fChain->SetBranchAddress("muM0PolZ", &muM0PolZ, &b_muM0PolZ);
   fChain->SetBranchAddress("posIniMomX", &posIniMomX, &b_posIniMomX);
   fChain->SetBranchAddress("posIniMomY", &posIniMomY, &b_posIniMomY);
   fChain->SetBranchAddress("posIniMomZ", &posIniMomZ, &b_posIniMomZ);
   fChain->SetBranchAddress("nFieldNomVal", &nFieldNomVal, &b_nFieldNomVal);
   fChain->SetBranchAddress("fieldNomVal", &fieldNomVal, &b_fieldNomVal);
   fChain->SetBranchAddress("det_n", &det_n, &b_det_n);
   fChain->SetBranchAddress("det_ID", det_ID, &b_det_ID);
   fChain->SetBranchAddress("det_edep", det_edep, &b_det_edep);
   fChain->SetBranchAddress("det_edep_el", det_edep_el, &b_det_edep_el);
   fChain->SetBranchAddress("det_edep_pos", det_edep_pos, &b_det_edep_pos);
   fChain->SetBranchAddress("det_edep_gam", det_edep_gam, &b_det_edep_gam);
   fChain->SetBranchAddress("det_edep_mup", det_edep_mup, &b_det_edep_mup);
   fChain->SetBranchAddress("det_nsteps", det_nsteps, &b_det_nsteps);
   fChain->SetBranchAddress("det_length", det_length, &b_det_length);
   fChain->SetBranchAddress("det_time_start", det_time_start, &b_det_time_start);
   fChain->SetBranchAddress("det_time_end", det_time_end, &b_det_time_end);
   fChain->SetBranchAddress("det_x", det_x, &b_det_x);
   fChain->SetBranchAddress("det_y", det_y, &b_det_y);
   fChain->SetBranchAddress("det_z", det_z, &b_det_z);
   fChain->SetBranchAddress("det_kine", det_kine, &b_det_kine);
   fChain->SetBranchAddress("det_VrtxKine", det_VrtxKine, &b_det_VrtxKine);
   fChain->SetBranchAddress("det_VrtxX", det_VrtxX, &b_det_VrtxX);
   fChain->SetBranchAddress("det_VrtxY", det_VrtxY, &b_det_VrtxY);
   fChain->SetBranchAddress("det_VrtxZ", det_VrtxZ, &b_det_VrtxZ);
   fChain->SetBranchAddress("det_VrtxVolID", det_VrtxVolID, &b_det_VrtxVolID);
   fChain->SetBranchAddress("det_VrtxProcID", det_VrtxProcID, &b_det_VrtxProcID);
   fChain->SetBranchAddress("det_VrtxTrackID", det_VrtxTrackID, &b_det_VrtxTrackID);
   fChain->SetBranchAddress("det_VrtxParticleID", det_VrtxParticleID, &b_det_VrtxParticleID);
   fChain->SetBranchAddress("det_VvvKine", det_VvvKine, &b_det_VvvKine);
   fChain->SetBranchAddress("det_VvvX", det_VvvX, &b_det_VvvX);
   fChain->SetBranchAddress("det_VvvY", det_VvvY, &b_det_VvvY);
   fChain->SetBranchAddress("det_VvvZ", det_VvvZ, &b_det_VvvZ);
   fChain->SetBranchAddress("det_VvvVolID", det_VvvVolID, &b_det_VvvVolID);
   fChain->SetBranchAddress("det_VvvProcID", det_VvvProcID, &b_det_VvvProcID);
   fChain->SetBranchAddress("det_VvvTrackID", det_VvvTrackID, &b_det_VvvTrackID);
   fChain->SetBranchAddress("det_VvvParticleID", det_VvvParticleID, &b_det_VvvParticleID);
   Notify();
}

Bool_t musrAnalysis::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void musrAnalysis::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t musrAnalysis::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef musrAnalysis_cxx
