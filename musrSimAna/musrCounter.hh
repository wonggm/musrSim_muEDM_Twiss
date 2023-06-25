#ifndef musrCounter_h
#define musrCounter_h 1
#include <iostream>
#include <fstream>
#include <TApplication.h>
#include <TSystem.h>
#include <TH1.h>
#include <iostream>
#include <map>
#include <list>

class hitInfo {
public:
  hitInfo(Int_t kEntry, Int_t evID, Int_t deI, Int_t detectorID, Double_t deEDEP, Long64_t timeBIN1, Long64_t timeBIN2, Int_t multiCtr, hitInfo* firstMulti, Int_t posQuality) {eventEntry=kEntry; eventIDnumber=evID; det_i=deI; det_id = detectorID; det_edep=deEDEP; timeBin1=timeBIN1; timeBin2=timeBIN2; multiHitCtr=multiCtr; firstMulti=NULL;posQual=posQuality;}
  ~hitInfo() {}
  void RewindTimeBin2(Long64_t timeBinsToRewind) {timeBin2-=timeBinsToRewind;}
  Int_t eventEntry;
  Int_t eventIDnumber;
  Int_t det_i;
  Int_t det_id;
  Double_t det_edep;
  Long64_t timeBin1; // repeated here in case this is not stored in a Map with First=timeBin1
  Long64_t timeBin2;
  Int_t multiHitCtr;
  hitInfo* firstMulti;
  Int_t posQual;

  //  extern double GlobalKamil;
  //  typedef std::map<int,int> debugEventMapType;
  //  extern debugEventMapType debugEventMap;
  //  extern Bool_t bool_debugingRequired;
};

class musrCounter {
  public:
  musrCounter(int CHANNEL_NR, char CHANNEL_NAME[200], char CHANNEL_TYPE, float E_THRESH, int TIME_SHIFT, double DEADTIME, Bool_t KEEP_BELOW_THRESH);
    ~musrCounter();
    int  GetCounterNr()   {return counterNr;}
    char GetCounterType() {return counterType;}
    void SetCoincidenceCounter(musrCounter* c, int icNr) {
      int cNr = abs(icNr);
      if (icNr>0) {
	std::cout<<"SetCoincidenceCounter:  Adding counter ="<<cNr<<" to coincidence with the counter "<<counterNr<<std::endl;
	koincidenceCounterMap[cNr] = c;
      }
      else {
	std::cout<<"SetCoincidenceCounter:  Adding counter ="<<cNr<<" as veto to the counter "<<counterNr<<std::endl;
	vetoCounterMap[cNr] = c;
      }
    }
    void SetMaxCoincidenceTimeWindow(Long64_t val) {maxCoincidenceTimeWindow=val;}
    Long64_t GetMaxCoincidenceTimeWindow() {return maxCoincidenceTimeWindow;}
    void SetCoincidenceTimeWindowOfAllCoincidenceDetectors(char motherCounter, Long64_t maxCoinc, Long64_t min, Long64_t max);
    void SetCoincidenceTimeWindowOfAllVetoDetectors(Long64_t maxCoinc, Long64_t min, Long64_t max);
    void SetCoincidenceTimeWindow_M(Long64_t min, Long64_t max) {coincidenceTimeWindowMin_M=min; coincidenceTimeWindowMax_M=max;}
    void SetCoincidenceTimeWindow_P(Long64_t min, Long64_t max) {coincidenceTimeWindowMin_P=min; coincidenceTimeWindowMax_P=max;}
    void SetAntiCoincidenceTimeWindowMin(Long64_t min) {antiCoincidenceTimeWindowMin=min;}
    void SetAntiCoincidenceTimeWindowMax(Long64_t max) {antiCoincidenceTimeWindowMax=max;}
    Long64_t GetAntiCoincidenceTimeWindowMin() {return antiCoincidenceTimeWindowMin;}
    Long64_t GetAntiCoincidenceTimeWindowMax() {return antiCoincidenceTimeWindowMax;}
    void SetTDChistogram(char hName[200],int t0,int t1,int t2,int hNr,char hNameAdd[200]);
    void FillTDChistogram(Double_t variable, Double_t vaha);
    void DrawTDChistogram();
    void FillHitInCounter(Double_t edep, Long64_t timeBin, Long64_t timeBin2, Int_t kEntry, Int_t eveID, Int_t iDet, Int_t detectorID, Int_t eventNum, Int_t& multiCtr);
    void RemoveHitsInCounter(Long64_t timeBinLimit);
  //  void RewindHitsInCounter(Long64_t timeBinsToRewind);
    void RewindHitsInCounter();
    Bool_t IsInCoincidence(Long64_t timeBin, char motherCounter);
    Bool_t GetNextGoodMuon(Int_t evtID, Long64_t timeBinMin, Long64_t& timeBinOfNextHit, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep);
    Bool_t CheckForPileupMuons(Long64_t timeBin0);
    Int_t GetNextGoodPositron(Int_t evtID, Long64_t timeBinMin, Long64_t timeBinMax, Long64_t& timeBinOfNextGoodHit, Long64_t& timeBinOfNextGoodHit_phaseShifted, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep);
    Int_t GetNextGoodPositronPulsed(Int_t entWanted1, Int_t entWanted2, Long64_t timeBinMin, Long64_t timeBinMax, Long64_t& timeBinOfNextGoodHit, Long64_t& timeBinOfNextGoodHit_phaseShifted, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep, Int_t& idetFirstMulti, hitInfo*& hitStruct);
    void myPrintThisCounter(Int_t evtID,  Int_t detail=2);
  //    void CheckClockInfo(Long64_t timeBin);
    Long64_t GetNumberOfMuonCandidates(){return numberOfMuonCandidates;}
    Long64_t GetNumberOfMuonCandidatesAfterVK(){return numberOfMuonCandidatesAfterVK;}
    Long64_t GetNumberOfMuonCandidatesAfterVKandDoubleHitRemoval(){return numberOfMuonCandidatesAfterVKandDoubleHitRemoval;}
    void DumpInfoToDumpFile(Int_t eventNr, Int_t detID, Long64_t tdcBin);
    void WriteRewindIntoDumpFile();

  private:
  //    static musrCounter* pointerToAnalysis;
  int   counterNr;
  char  counterName[200];
  char  counterType;
  double couterEThreshold;
  Bool_t keepBelowThresholdHits; // JSL: for pulsed mode overlap
  Long64_t counterTimeShift;
  typedef std::map<int,musrCounter*> counterMapType;
  counterMapType koincidenceCounterMap;
  counterMapType vetoCounterMap;
  char  TDC_histoName[200];
  int   TDC_t0, TDC_t1, TDC_t2, TDC_histoNrAdd;
  char  TDC_histoNameAdd[200];
  TH1D* histTDC;
  Long64_t antiCoincidenceTimeWindowMin, coincidenceTimeWindowMin_M, coincidenceTimeWindowMin_P;
  Long64_t antiCoincidenceTimeWindowMax, coincidenceTimeWindowMax_M, coincidenceTimeWindowMax_P;
  Long64_t maxCoincidenceTimeWindow;
  //  typedef std::map<Long64_t,Int_t> hitMap_TYPE;  // Long64_t = timeBin, Int_t=eventID
  typedef std::map<Long64_t,hitInfo*> hitMap_TYPE;  // Long64_t = timeBin, hitInfo = eventID and det_i
  hitMap_TYPE hitMap;
  //  std::list<Double_t> timeOfHitsList;
  // for pulsed double hit processing
  double discriminatorLastVolts;
  Long64_t discriminatorLastTime;
  hitMap_TYPE::iterator discriminatorIterator;
  Double_t discriminatorRecoveryTime; // in TDC bins!

  Int_t doubleHitN;
  Long64_t numberOfMuonCandidates;
  Long64_t numberOfMuonCandidatesAfterVK;
  Long64_t numberOfMuonCandidatesAfterVKandDoubleHitRemoval;
  
  public:
  static Bool_t bool_ignoreUnperfectMuons;
  static Bool_t bool_ignoreUnperfectPositrons;
  static Bool_t bool_WriteDataToDumpFile;
  //  static ofstream dumpFile;
  //  static Long64_t previousClock;
  //  static Long64_t CLOCK_INTERVAL;
};

#endif
