//#include <iostream>
#include "musrCounter.hh"
#include "TCanvas.h"
#include "musrAnalysis.hh"

typedef std::map<int,int> debugEventMapType;
debugEventMapType debugEventMap;
Bool_t bool_debugingRequired;

Bool_t musrCounter::bool_ignoreUnperfectMuons     = true;
Bool_t musrCounter::bool_ignoreUnperfectPositrons = true;
Bool_t musrCounter::bool_WriteDataToDumpFile      = false;
//Long64_t musrCounter::previousClock               = -1;
//Long64_t musrCounter::CLOCK_INTERVAL              = 512000;
//ofstream musrCounter::dumpFile;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

musrCounter::musrCounter(int CHANNEL_NR, char CHANNEL_NAME[200], char CHANNEL_TYPE, float E_THRESH, int TIME_SHIFT, double DEADTIME, Bool_t KEEP_BELOW_THRESH) {
  //  pointerToAnalysis=this;
  counterNr = CHANNEL_NR;
  strcpy(counterName,CHANNEL_NAME);
  counterType = CHANNEL_TYPE;
  couterEThreshold = E_THRESH;
  counterTimeShift = (Long64_t) TIME_SHIFT;
  keepBelowThresholdHits = KEEP_BELOW_THRESH;
  discriminatorRecoveryTime = DEADTIME;
  std::cout<<"musrCounter::musrCounter: Creating counter "<<counterNr<<"  "<<counterName<<"  "<<counterType<<"  "<<couterEThreshold<<" "<<counterTimeShift<<std::endl;
  strcpy(TDC_histoName,"Unset");
  TDC_t0=0;
  TDC_t1=0;
  TDC_t2=0;
  TDC_histoNrAdd=0;
  antiCoincidenceTimeWindowMin=0;
  antiCoincidenceTimeWindowMax=0;
  coincidenceTimeWindowMin_M=0;
  coincidenceTimeWindowMax_M=0;
  coincidenceTimeWindowMin_P=0;
  coincidenceTimeWindowMax_P=0;
  maxCoincidenceTimeWindow=0;
  strcpy(TDC_histoNameAdd,"Unset");
  doubleHitN=0;
  numberOfMuonCandidates=0;
  numberOfMuonCandidatesAfterVK=0;
  numberOfMuonCandidatesAfterVKandDoubleHitRemoval=0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

musrCounter::~musrCounter() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void musrCounter::SetTDChistogram(char hName[200],int t0,int t1,int t2,int hNr,char hNameAdd[200]) {
  strcpy(TDC_histoName,hName);
  TDC_t0=t0;
  TDC_t1=t1;
  TDC_t2=t2;
  TDC_histoNrAdd=hNr;
  strcpy(TDC_histoNameAdd,hNameAdd);
  std::cout<<"TDC_histogram: "<<TDC_histoName<<" "<<TDC_t0<<" "<<TDC_t1<<" "<<TDC_t2<<" "<<TDC_histoNrAdd<<" "<<TDC_histoNameAdd<<std::endl;
  histTDC = new TH1D(TDC_histoName,TDC_histoName,t2,0.,float(t2));
}
//================================================================
void musrCounter::FillTDChistogram(Double_t variable, Double_t vaha) {
  histTDC->Fill(variable,vaha);
}

//================================================================
void musrCounter::DrawTDChistogram() {
  char canvasName[501]; 
  sprintf(canvasName,"c%s",TDC_histoName);
  TCanvas* cTmp = new TCanvas(canvasName,canvasName);
  histTDC->Draw();
}

//================================================================
void musrCounter::FillHitInCounter(Double_t edep, Long64_t timeBin, Long64_t timeBin2, Int_t kEntry, Int_t eveID, Int_t iDet, Int_t detectorID, Int_t eventNum, Int_t& multiCtr){
	static hitInfo *hFlagged; // common to all histograms
  //cDEL  std::cout<<"FillHitInCounter: timeBin="<<timeBin<<"   timeBin2="<<timeBin2<<"    counterTimeShift="<< counterTimeShift<<std::endl;
  //cDEL  std::cout<<"                                                FillHitInCounter  I: timeBin-counterTimeShift="<<timeBin-counterTimeShift<<"   timeBin2-counterTimeShift="<<timeBin2-counterTimeShift<<std::endl;
  //  std::cout<<"musrCounter::FillHitInCounter:"<<counterNr<<std::endl;
	// JSL: allow keeping of small hits (but label them)
  if ((edep>=couterEThreshold) || ((counterType=='P') && keepBelowThresholdHits)) {
	  // multiple count labelling (JSL)
	  // counter provided by caller (per event across all counters), reset for new muon
	  // first big hit numbered 0
	  // first hit relabelled 1 when second significant hit comes in
	  // second and subsequent big hits labelled 2,3,etc
	  // small ones labelled -1 regardless of sequence
	  // Only for P counters; V,K,M all labelled 0.
	  Int_t multiCtrTmp;
	  if(counterType=='P') {
	  if(edep>=couterEThreshold) {
		  if((multiCtr==1) && hFlagged) {
			  // a second good hit, relabel the first hit
			  // std::cout << "flagged double count in event "<<kEntry<<" hist "<<detectorID<<" time "<<timeBin<<std::endl;
			  hFlagged->multiHitCtr = 1;
			  // hFlagged=NULL;
			  multiCtr++;
		  }
		  multiCtrTmp=multiCtr;
		  multiCtr++;
	  } else {
		  multiCtrTmp=-1;
	  }
	  } else {
		  multiCtrTmp=0; // M, V or K counter
	  }
    hitInfo* hInfo = new hitInfo(kEntry,eveID,iDet,detectorID,edep,timeBin-counterTimeShift,timeBin2-counterTimeShift, multiCtrTmp, NULL,0);
	if((multiCtrTmp==0) && (counterType=='P')) { hFlagged=hInfo; } // save pointer to the first hit
	if(multiCtrTmp>0) {
		hInfo->firstMulti = hFlagged;
	}
    //cDEL    std::cout<<detectorID<<"                                                FillHitInCounter II: timeBin-counterTimeShift="<<timeBin-counterTimeShift<<"   timeBin2-counterTimeShift="<<timeBin2-counterTimeShift<<std::endl;
    hitMap.insert( std::pair<Long64_t,hitInfo*>(timeBin-counterTimeShift,hInfo) );
    if (bool_WriteDataToDumpFile) {               // write data into an output file
      //      CheckClockInfo(timeBin);
      //      dumpFile<<eventNum<<"\t"<<detectorID<<"\t"<<timeBin<<"\n";
      DumpInfoToDumpFile(eventNum,detectorID,timeBin);
    }
  }
}

//================================================================
void musrCounter::RemoveHitsInCounter(Long64_t timeBinLimit) {
  // Remove the obsolete hits (i.e. hits that happaned well before t0) from the Counter class
	hitMap_TYPE::iterator it2; // JSL
  if (hitMap.empty()) return;
  //  myPrintThisCounter();
  //  if (counterNr==1) {std::cout<<"ooooo1  timeBinLimit="<<timeBinLimit<<std::endl;  myPrintThisCounter();}
  for (hitMap_TYPE::iterator it = hitMap.begin(); it != hitMap.end();) { // JSL: move  ++it into body
    //    std::cout<<"  musrCounter::RemoveHitsInCounter:  counterNr="<<counterNr<<"  timeBinLimit="<<timeBinLimit<<"  maxCoincidenceTimeWindow="<<maxCoincidenceTimeWindow<<"    counterTimeShift="<<counterTimeShift<<std::endl;
    if ((it->first)>(timeBinLimit+maxCoincidenceTimeWindow-counterTimeShift)) return;  //note that maxCoincidenceTimeWindow is usually negative number
    else {
      //      std::cout<<"       Deleting hit from counter "<<counterNr<<",  time bin = "<<(it->first)<<std::endl;
		it2=it;
		it2++;
      delete (it->second);
      hitMap.erase(it);
	  it=it2;
    }
  }
  //  if (counterNr==1) {std::cout<<"ooooo2"<<std::endl;  myPrintThisCounter();}
}

//================================================================
//void musrCounter::RewindHitsInCounter(Long64_t timeBinsToRewind) {
void musrCounter::RewindHitsInCounter() {
  // Reset time in hits from the Counter class
  if (hitMap.empty()) return;

  //  Long64_t timeBinsToRewind = musrAnalysis::rewindTimeBins;
  hitMap_TYPE hitMap_TMP;
  for (hitMap_TYPE::iterator it = hitMap.begin(); it != hitMap.end(); ++it) {
    Long64_t tempBinT   = it->first;
    hitInfo* tempEvnr= it->second;
    tempEvnr->RewindTimeBin2(musrAnalysis::rewindTimeBins);
    hitMap_TMP.insert( std::pair<Long64_t,hitInfo*>(tempBinT-musrAnalysis::rewindTimeBins,tempEvnr) );
  }
  hitMap.swap(hitMap_TMP);
}

//================================================================
Bool_t musrCounter::IsInCoincidence(Long64_t timeBin, char motherCounter){
  // timeBin ... time bin, at which the coincidence is searched

  if (hitMap.empty()) return false;
  Long64_t timeBinMin;
  Long64_t timeBinMax;

  // If timeBinMinimum and timeBinMaximum are not specified, use internal time window of the detector (koincidence or veto detectors).
  // Otherwise use timeBinMinimum and timeBinMaximum (e.g.coincidence of a positron counter with other positron counters).
  if      (counterType == 'V')     timeBinMin = timeBin + antiCoincidenceTimeWindowMin; // this is veto detector
  else if (motherCounter=='M')     timeBinMin = timeBin + coincidenceTimeWindowMin_M;   // this is coinc. detector connected to M
  else if (motherCounter=='P')     timeBinMin = timeBin + coincidenceTimeWindowMin_P;   // this is coinc. detector connected to P

  if      (counterType == 'V')     timeBinMax = timeBin + antiCoincidenceTimeWindowMax; // this is veto detector
  else if (motherCounter=='M')     timeBinMax = timeBin + coincidenceTimeWindowMax_M;   // this is coinc. detector connected to M
  else if (motherCounter=='P')     timeBinMax = timeBin + coincidenceTimeWindowMax_P;   // this is coinc. detector connected to P

  for (hitMap_TYPE::iterator it = hitMap.begin(); it != hitMap.end(); ++it) {
    Long64_t timeBinOfCount_tmp = it->first;
    if ((timeBinOfCount_tmp >= timeBinMin) && (timeBinOfCount_tmp <= timeBinMax)) {
      //      if ((timeBin!=timeBinOfCount_tmp)||(!ignoreHitsAtBinZero)) {
      return true;
      //      }
    }
    else if (timeBinOfCount_tmp > timeBinMax) return false;
  }
  return false;
}

//================================================================
Bool_t musrCounter::GetNextGoodMuon(Int_t evtID, Long64_t timeBinMin, Long64_t& timeBinOfNextHit, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep) {
  // This function searches for a good muon, i.e. the muon 
  //         1) belongs to the currently analysed event
  //         2) is in coincidence with all required coincidence detectors
  //         3) is not in coincidence with veto detectors
  // INPUT PARAMETERS:  evtID, timeBinMin
  // OUTPUT PARAMETERS: timeBinOfNextHit
  //
  // Loop over the hits in the counter
  //  std::cout<<" musrCounter::GetNextGoodMuon    timeBinMin="<<timeBinMin<<std::endl;
  if (hitMap.empty()) return false;
  if (counterType!='M') {std::cout<<"\n!!! FATAL ERROR !!! musrCounter::GetNextGoodMuon: not the muon counter! ==> S T O P !!!\n"; exit(1);}
  
  std::list<hitMap_TYPE::iterator> it_muon_hits_to_be_deleted;
  for (hitMap_TYPE::iterator it = hitMap.begin(); it != hitMap.end(); ++it) {

    Long64_t timeBinOfCount_tmp = it->first;
    timeBinOfNextHit = timeBinOfCount_tmp;
    Int_t modulo     = timeBinOfNextHit % 524288;
    if (timeBinOfCount_tmp <= timeBinMin) continue;   // This hit was already processed previously ==> skip it

    Int_t    eventNumber = (it->second)->eventIDnumber;
    if (eventNumber!=evtID) continue;          // This trigger hit does not correspond to the currently processed event
                                               //  ==> skip it, because it was already proceesed or will be processed in future
    numberOfMuonCandidates++;
    if (bool_debugingRequired) {if (debugEventMap[evtID]>3) std::cout<<"GetNextGoodMuon: muon candidate found ("<<timeBinOfNextHit<<" , "<<modulo<<")"<<std::endl;}

    // Hit candidate was found.  Now check its coincidences and vetos
    Bool_t bool_coincidenceConditions = true;
    for (counterMapType::const_iterator itCounter = koincidenceCounterMap.begin(); itCounter!=koincidenceCounterMap.end(); ++itCounter) {
      if (!( (itCounter->second)->IsInCoincidence(timeBinOfCount_tmp,'M') )) {  // no coincidence found ==> skip hit 
	//	if (bool_ignoreUnperfectMuons) hitMap.erase(it);
	if (bool_ignoreUnperfectMuons) it_muon_hits_to_be_deleted.push_back(it);
	bool_coincidenceConditions = false;
	if (bool_debugingRequired) {if (debugEventMap[evtID]>3) std::cout<<"GetNextGoodMuon: muon candidate  not in koincidence ("<<timeBinOfNextHit<<" , "<<modulo<<")"<<std::endl;}
	goto MuonCoincidencesChecked;
      }
    }
    for (counterMapType::const_iterator itCounter = vetoCounterMap.begin(); itCounter!=vetoCounterMap.end(); ++itCounter) {
      if ( (itCounter->second)->IsInCoincidence(timeBinOfCount_tmp,'M') ) {  //  coincidence with veto found ==> skip hit 
	//	if (bool_ignoreUnperfectMuons) hitMap.erase(it);
	if (bool_ignoreUnperfectMuons) it_muon_hits_to_be_deleted.push_back(it);
	bool_coincidenceConditions = false;
	if (bool_debugingRequired) {if (debugEventMap[evtID]>3) std::cout<<"GetNextGoodMuon: muon candidate  vetoed ("<<timeBinOfNextHit<<" , "<<modulo<<")"<<std::endl;}
	goto MuonCoincidencesChecked;
      }
    }

  MuonCoincidencesChecked:
    if (!bool_coincidenceConditions) continue;  // This hit does not fulfill coincidence and veto criteria
    numberOfMuonCandidatesAfterVK++;
    if (bool_debugingRequired) {if (debugEventMap[evtID]>3) std::cout<<"GetNextGoodMuon: muon candidate after VK ("<<timeBinOfNextHit<<" , "<<modulo<<")"<<std::endl;}
  
    if ( CheckForPileupMuons(timeBinOfNextHit) )  {
      //      std::cout<<"CheckForPileupMuons=true"<<std::endl;
      if (bool_debugingRequired) {if (debugEventMap[evtID]>3) std::cout<<"GetNextGoodMuon: muon candidate killed by pileup muon ("<<timeBinOfNextHit<<" , "<<modulo<<")"<<std::endl;}
      continue;  // This muon candidate is killed due to a double hit rejection.
    }
    kEntry   = (it->second)->eventEntry;
    idet     = (it->second)->det_i;
    idetID   = (it->second)->det_id;
    idetEdep = (it->second)->det_edep;
    if (bool_debugingRequired) {if (debugEventMap[evtID]>1) std::cout<<"GetNextGoodMuon: GOOD muon candidate found ("
								     <<timeBinOfNextHit<<" , "<<modulo<<" ; "<<evtID<<")"<<std::endl;}
    for(std::list<hitMap_TYPE::iterator>::iterator itt = it_muon_hits_to_be_deleted.begin(); itt != it_muon_hits_to_be_deleted.end(); ++itt) {
      hitMap.erase(*itt);
    }
    return true;
  }
  for(std::list<hitMap_TYPE::iterator>::iterator itt = it_muon_hits_to_be_deleted.begin(); itt != it_muon_hits_to_be_deleted.end(); ++itt) {
    hitMap.erase(*itt);
  }
  return false;
}

//================================================================
Bool_t musrCounter::CheckForPileupMuons(Long64_t timeBin0) {
  // Check for pileup muons.  If double hit in M-counter is found, return true.
  Long64_t timeBinMinimum = timeBin0;
  for (hitMap_TYPE::iterator it = hitMap.begin(); it != hitMap.end(); ++it) {    
    Long64_t timeBinOfCount_tmp = it->first;
    //    std::cout<<"timeBin0="<<timeBin0<<"      timeBinOfCount_tmp="<<timeBinOfCount_tmp<<std::endl;
    if (timeBinOfCount_tmp < timeBinMinimum+musrAnalysis::pileupWindowBinMin) continue;   // This hit happened too long ago
    if (timeBinOfCount_tmp > timeBinMinimum+musrAnalysis::pileupWindowBinMax) break;      // This hit happened too late
    //    if ((timeBinOfCount_tmp == timeBinMinimum)&&( ((it->second)->eventEntry) == kEntry_NN)) continue;   
    if (timeBinOfCount_tmp == timeBinMinimum) continue;  
                  // This is the M-counter hit for which we check the pileup -> ignore this hit in this double-hit check.
    // We have found the hit, which could be the double hit.  If it is required,  we now have 
    // to check the coincidences and anticoincidences of this double-hit muon:
    if (!bool_ignoreUnperfectMuons) {
      return true; // In this case we do not check for coincidences and anticoincidences ==> double hit found.
    }
    for (counterMapType::const_iterator itCounter = koincidenceCounterMap.begin(); itCounter!=koincidenceCounterMap.end(); ++itCounter) {
      if (!( (itCounter->second)->IsInCoincidence(timeBinOfCount_tmp,'M') )) goto endOfThisHit;   // no coincidence found ==> skip hit 
    }
    for (counterMapType::const_iterator itCounter = vetoCounterMap.begin(); itCounter!=vetoCounterMap.end(); ++itCounter) {
      if ( (itCounter->second)->IsInCoincidence(timeBinOfCount_tmp,'M') ) goto endOfThisHit;   //  coincidence with veto found ==> skip hit 
    }
    // The double hit was found (the pileup muon fulfils all veto and coincidence requirements) ==> end of the search
    return true;
    
    endOfThisHit:
    ;
  }
  numberOfMuonCandidatesAfterVKandDoubleHitRemoval++;
  return false;
}
//================================================================
Int_t musrCounter::GetNextGoodPositron(Int_t evtID, Long64_t timeBinMin, Long64_t timeBinMax, Long64_t& timeBinOfNextGoodHit, Long64_t& timeBinOfNextGoodHit_phaseShifted, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep) {
  // INPUT PARAMETERS:  evtID, timeBinMin
  // OUTPUT PARAMETERS: timeBinOfNextGoodHit
  //                    positronQuality = 0 ... no positron candidate found
  //                                    = 2 ... good positron found
  //                                    = 3 ... double hit
  // Loop over the hits in the counter
  Int_t positronQuality=0;
  if (bool_debugingRequired) {if (debugEventMap[evtID]>4) myPrintThisCounter(evtID,0);}
  if (hitMap.empty()) return 0;
  if (counterType!='P') {std::cout<<"\n!!! FATAL ERROR !!! musrCounter::GetNextGoodPositron: not the positron counter! ==> S T O P !!!\n"; exit(1);}
  std::list<hitMap_TYPE::iterator> it_positron_hits_to_be_deleted;
  for (hitMap_TYPE::iterator it = hitMap.begin(); it != hitMap.end(); ++it) {
    //    Int_t    eventNumber = (it->second)->eventIDnumber;
    Long64_t timeBinOfCount_tmp = it->first;
    Int_t modulo     = timeBinOfCount_tmp % 524288;
    if ((timeBinOfCount_tmp <= timeBinMin) || (timeBinOfCount_tmp > timeBinMax)) {
      if (bool_debugingRequired) {
	if (debugEventMap[evtID]>3) {std::cout<<"GetNextGoodPositron:  Hit out of data interval"<<std::endl;}
      }
      continue;   // This hit is out of the data interval ==> skip it
    }

    // Hit candidate was found.  Now check its coincidences and vetos
    Bool_t bool_coincidenceConditions = true;
    for (counterMapType::const_iterator itCounter = koincidenceCounterMap.begin(); itCounter!=koincidenceCounterMap.end(); ++itCounter) {
      if (bool_debugingRequired) {
	if (debugEventMap[evtID]>4) { (itCounter->second)->myPrintThisCounter(evtID); }
      }
      if (!( (itCounter->second)->IsInCoincidence(timeBinOfCount_tmp,'P') )) {
	if (bool_debugingRequired) {
	  if (debugEventMap[evtID]>3) {std::cout<<"GetNextGoodPositron:  Coincidence required but not found (timeBin="<<timeBinOfCount_tmp<<" , "<<modulo<<")"<<std::endl;}
	}
	//	if (bool_ignoreUnperfectPositrons) hitMap.erase(it); // no coincidence found ==> remove the candidate.
	if (bool_ignoreUnperfectPositrons) it_positron_hits_to_be_deleted.push_back(it);
	bool_coincidenceConditions = false;
	goto CoincidencesChecked;
	//	goto endOfThisHit;   // no coincidence found ==> skip hit 
      }
    }
    for (counterMapType::const_iterator itCounter = vetoCounterMap.begin(); itCounter!=vetoCounterMap.end(); ++itCounter) {
      if ( (itCounter->second)->IsInCoincidence(timeBinOfCount_tmp,'P') ) {
	if (bool_debugingRequired) {
	  if (debugEventMap[evtID]>3) {std::cout<<"GetNextGoodPositron:  Coincidence vith veto detector found (timeBin="<<timeBinOfCount_tmp<<" , "<<modulo<<")"<<std::endl;}
	}
	//	if (bool_ignoreUnperfectPositrons) hitMap.erase(it); // coincidence with veto found ==> remove the candidate.
	if (bool_ignoreUnperfectPositrons) it_positron_hits_to_be_deleted.push_back(it);
	bool_coincidenceConditions = false;
	goto CoincidencesChecked;
	//	goto endOfThisHit;   //  coincidence with veto found ==> skip hit
      } 
    }

  CoincidencesChecked:
    if (!bool_coincidenceConditions) continue;  // This hit does not fulfill coincidence and veto criteria
    if (positronQuality==2) {
      for(std::list<hitMap_TYPE::iterator>::iterator itt = it_positron_hits_to_be_deleted.begin(); itt != it_positron_hits_to_be_deleted.end(); ++itt) {
	hitMap.erase(*itt);
      }
      return 3;   // An electron was already found before, and now again ==> double hit
    }
    else positronQuality=2;

    kEntry   = (it->second)->eventEntry;
    idet     = (it->second)->det_i;
    idetID   = (it->second)->det_id;
    idetEdep = (it->second)->det_edep;
    timeBinOfNextGoodHit = timeBinOfCount_tmp;
    timeBinOfNextGoodHit_phaseShifted = (it->second) -> timeBin2;
    if (bool_debugingRequired) {
      if (debugEventMap[evtID]>3) {std::cout<<"GetNextGoodPositron:  Good positron candidate found in this counter. (timeBin="<<timeBinOfCount_tmp<<" , "<<modulo<<"  "<<(it->second)->eventIDnumber<<")"<<std::endl;}
    }
  }
  for(std::list<hitMap_TYPE::iterator>::iterator itt = it_positron_hits_to_be_deleted.begin(); itt != it_positron_hits_to_be_deleted.end(); ++itt) {
    hitMap.erase(*itt);
  }
  
  return positronQuality;
}

//================================================================
Int_t musrCounter::GetNextGoodPositronPulsed(Int_t entWanted1, Int_t entWanted2, Long64_t timeBinMin, Long64_t timeBinMax, Long64_t& timeBinOfNextGoodHit, Long64_t& timeBinOfNextGoodHit_phaseShifted, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep, Int_t& idetFirstMulti, hitInfo*& hitStruct) {
// pulsed. Sort through one detector
	// return positron quality: (>= -1 are "counted" events, 0,1,-2,-3 are "ideal")
	//	-1000: no more positron candidates
	//	0: a normal "good" positron, the only above threshold hit for that muon
	//	1..n: "good" positrons, but double count events. 1 - 1st such hit 2=2nd, etc
	//	-1: "stacked" event, this positron plus remaining "charge" adds to threshold (double hit status of such events not recorded)
	//	-2..-999: "dead" event, good positron but will not be counted. -2=only hit -3=1st of double -4=2nd of double, etc
	Int_t positronQuality=-1000;
	Int_t idMin=+1000000000;
	Int_t idMax=-1000000000;
	//
	// entWanted: must be in range
	// timeBinMin, timeBinMax: range to search
	// timeBinOfNextGoodHit: starting point on entry, filled with time. If < timeBinMin, reset discriminator to beginning and run through pre-time-window events
	// timeBinOfNextGoodHit_phaseShifted: adjusted (out only)
	// kEntry: entry number for this event
	// idet: detector number
	// idetID:
	// idetEdep: energy deposited by this positron event (may be < threshold!)
	//
	// may no longer need to unpackage all the vars?

	if(timeBinOfNextGoodHit < timeBinMin || timeBinOfNextGoodHit < discriminatorLastTime) {
		// rewind discriminator
		discriminatorLastVolts = 0.0;
		discriminatorLastTime = timeBinOfNextGoodHit;
		discriminatorIterator = hitMap.begin();
	}
	// process events until a good hit found, or end
	while((discriminatorIterator != hitMap.end()) && (discriminatorIterator->first <= timeBinMax)) {
		if(discriminatorIterator->second->eventEntry > idMax) { idMax=discriminatorIterator->second->eventEntry; }
		if(discriminatorIterator->second->eventEntry < idMin) { idMin=discriminatorIterator->second->eventEntry; }
		Long_t dT=((discriminatorIterator->first)-discriminatorLastTime);
		double discriminatorDropVolts=discriminatorLastVolts * exp(-dT/discriminatorRecoveryTime);
		discriminatorLastVolts=discriminatorDropVolts + discriminatorIterator->second->det_edep;
		discriminatorLastTime=discriminatorIterator->first;
		// discriminate it!
		Bool_t discriminatorTriggered = (discriminatorDropVolts < couterEThreshold && discriminatorLastVolts > couterEThreshold);
		// have we found the wanted event?
		if ((((discriminatorIterator->second->eventEntry >= entWanted1) && (discriminatorIterator->second->eventEntry <= entWanted2)) && discriminatorIterator->first > timeBinMin) && (discriminatorTriggered || (discriminatorIterator->second->det_edep > couterEThreshold))) {
			//yes
			if(discriminatorTriggered) {
				positronQuality=discriminatorIterator->second->multiHitCtr; // already checked to see if it was above threshold on its own and/or double
			} else { // a missed good count, record these too
				positronQuality= -2 - discriminatorIterator->second->multiHitCtr;
			}
			kEntry   = (discriminatorIterator->second)->eventEntry;
			idet     = (discriminatorIterator->second)->det_i;
		    idetID   = (discriminatorIterator->second)->det_id;
		    idetEdep = (discriminatorIterator->second)->det_edep;
		    timeBinOfNextGoodHit = discriminatorIterator->first;
			timeBinOfNextGoodHit_phaseShifted = (discriminatorIterator->second) -> timeBin2;
			if(((discriminatorIterator->second)->multiHitCtr > 1) && (discriminatorIterator->second)->firstMulti) {
				idetFirstMulti = (discriminatorIterator->second)->firstMulti->det_i;
			} else {
				idetFirstMulti = -1000; // not a second or subsequent multi hit, or not recorded for some reason
			}
			hitStruct = discriminatorIterator->second;
			discriminatorIterator->second->posQual = positronQuality;
			discriminatorIterator++;
			break;
		} else {
			discriminatorIterator++;
			hitStruct = NULL;
		}
	}
	if(positronQuality==-1000 && ((idMax>idMin) && timeBinOfNextGoodHit<0)) {
		// std::cout << "Didn't find entry " << entWanted << " but only range " << idMin << " to " << idMax << " from " << timeBinOfNextGoodHit << std::endl;
	}
return positronQuality;
}

//================================================================
void musrCounter::SetCoincidenceTimeWindowOfAllCoincidenceDetectors(char motherCounter, Long64_t maxCoinc, Long64_t min, Long64_t max) {
  //  std::cout<<"QQQQQQQQQQQQQQQQQQQQQ koincidenceCounterMap.size()="<<koincidenceCounterMap.size()<<std::endl;
  for (counterMapType::const_iterator it = koincidenceCounterMap.begin(); it!=koincidenceCounterMap.end(); ++it) {
    Long64_t maxCoinc_AlreadySet = ((it->second)->GetMaxCoincidenceTimeWindow());
    if (maxCoinc < maxCoinc_AlreadySet)    (it->second)->SetMaxCoincidenceTimeWindow(maxCoinc);
 
    if      (motherCounter=='M') (it->second)->SetCoincidenceTimeWindow_M(min,max);
    else if (motherCounter=='P') (it->second)->SetCoincidenceTimeWindow_P(min,max);
    else {
      std::cout<<"musrCounter::SetCoincidenceTimeWindowOfAllCoincidenceDetectors ERROR: Strange motherCounter "
	       <<motherCounter<<"\n   ==> S T O P "<<std::endl; 
      exit(1);
    }
  }
}

//================================================================
void musrCounter::SetCoincidenceTimeWindowOfAllVetoDetectors(Long64_t maxCoinc, Long64_t min, Long64_t max) {
  for (counterMapType::const_iterator it = vetoCounterMap.begin(); it!=vetoCounterMap.end(); ++it) {
    musrCounter* counter = it->second;
    Long64_t maxCoinc_AlreadySet = counter->GetMaxCoincidenceTimeWindow();
    Long64_t      min_AlreadySet = counter->GetAntiCoincidenceTimeWindowMin();
    Long64_t      max_AlreadySet = counter->GetAntiCoincidenceTimeWindowMax();
    if (maxCoinc < maxCoinc_AlreadySet)    counter->SetMaxCoincidenceTimeWindow(maxCoinc);
    if (min      < min_AlreadySet)         counter->SetAntiCoincidenceTimeWindowMin(min);
    if (max      > max_AlreadySet)         counter->SetAntiCoincidenceTimeWindowMax(max);
  }
}

//================================================================
void musrCounter::myPrintThisCounter(Int_t evtID, Int_t detail) {
  Bool_t eventMixing=false;
  if ((hitMap.begin()==hitMap.end()) && (detail<=1) ) return;
  //  if (detail>1) std::cout<<"musrCounter::myPrintThisCounter:  counterNr = "<<counterNr<<":   ";
  //  else
  std::cout<<"   counter = "<<counterNr<<":   ";
  for (hitMap_TYPE::iterator it = hitMap.begin(); it != hitMap.end(); ++it) {
    std::cout<<"\t"<<it->first<<" ("<<(it->first)%524288 <<")  "<<(it->second)->eventIDnumber<<",";
    if (evtID != (it->second)->eventIDnumber) {eventMixing=true;}
  }
  if (eventMixing) {std::cout<<"   Potential event mixing";}
  std::cout<<std::endl;
}

//================================================================
//void musrCounter::CheckClockInfo(Long64_t timeBin) {
//  Int_t clock_detectorID=musrAnalysis::clock_channelID;
//  if (timeBin > (previousClock+musrAnalysis::clock_interval)) {
//    previousClock += musrAnalysis::clock_interval;
//    //    dumpFile<<"-1\t"<<clock_detectorID<<"\t"<<previousClock<<"\n";
//    DumpInfoToDumpFile(-1,clock_detectorID,previousClock);
//  }
//}
//================================================================
void musrCounter::DumpInfoToDumpFile(Int_t eventNr, Int_t detID, Long64_t tdcBin) {
  if (tdcBin>=musrAnalysis::rewindTimeBins)  tdcBin -= musrAnalysis::rewindTimeBins;
  else if (tdcBin<0)                         tdcBin += musrAnalysis::rewindTimeBins;
  //  Int_t tdc = (tdcBin<musrAnalysis::rewindTimeBins) ? tdcBin : tdcBin-musrAnalysis::rewindTimeBins;
  //  dumpFile<<eventNr<<"\t"<<detID<<"\t"<<tdcBin<<"\n";
  musrAnalysis::myWriteDump->send_to_dump(detID,tdcBin,false);
}
//================================================================
void musrCounter::WriteRewindIntoDumpFile() {
  DumpInfoToDumpFile(-2,1000,0);
}
//================================================================
