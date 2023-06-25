#define musrAnalysis_cxx
#include "musrAnalysis.hh"
#include "musrTH.hh"
//#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>
#include <TF1.h>
//#include <math.h>


musrWriteDump* musrAnalysis::myWriteDump = NULL;

void musrAnalysis::Loop(char* runChar, char* v1190FileName, Int_t nrEvents)
{
//   In a ROOT session, you can do:
//      Root > .L musrAnalysis.C
//      Root > musrAnalysis t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   std::cerr<<"musrSimAnalysis::Loop() :  Analysing run "<<v1190FileName<<std::endl;

   hInfo = new TH1D("hInfo","Different values that need to be passed to ploting program",10000,1.,10001.);
   ReadInInputParameters(v1190FileName);
   CreateHistograms();

   nentries = fChain->GetEntriesFast();
   if (nrEvents!=0)  nentries = nrEvents;

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);  InitialiseEvent();  nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      if (jentry%10000==0) std::cout << "Analysing event nr. jentry=" << jentry << " and found " << numberOfGoodMuons << " muons so far" << std::endl;
      AnalyseEvent(jentry);
   }
   SaveHistograms(runChar,v1190FileName);
}



//================================================================

void musrAnalysis::ReadInInputParameters(char* charV1190FileName) {
  std::cout<<"musrAnalysis::ReadInInputParameters()"<<std::endl;
 
  //  std::string V1190_FileName = "2.v1190";
  //  FILE *fSteeringFile=fopen(V1190_FileName.c_str(),"r");
  FILE *fSteeringFile=fopen(charV1190FileName,"r");
  if (fSteeringFile==NULL) {
    if (strcmp(charV1190FileName,"Unset")==0) {
      std::cout<<"musrDetectorConstruction:  No input macro file specified"<<std::endl;
    }
    else {
      std::cout << "E R R O R :    Failed to open detector configuration file " << charV1190FileName << std::endl;
    }
    std::cout << "S T O P    F O R C E D" << std::endl;
    exit(1);
  }
  std::cout << "Configuration file \"" << charV1190FileName << "\" was opened."<<std::endl;
  char  line[10001];

  // Write out the configuration file into the output file:
  std::cout << "\n\n....oooOO0OOooo........oooOO0OOooo......Configuration file used for this run....oooOO0OOooo........oooOO0OOooo......"<<std::endl; 
  while (!feof(fSteeringFile)) {
    fgets(line,10000,fSteeringFile);
    //    if ((line[0]!='#')&&(line[0]!='\n')&&(line[0]!='\r'))  // TS: Do not print comments
    std::cout << line;
  }
  std::cout <<"....oooOO0OOooo........oooOO0OOooo......End of the configuration file.....oooOO0OOooo........oooOO0OOooo......\n\n"<<std::endl;

  //  Loop
  rewind (fSteeringFile);
  instrument = "Unset";
  description = "Unset";
  tdchwtype = "Unset";
  tdcresolution = 195.3125*picosecond;
  mdelay    = 0;
  pdelay    = 800;
  mcoincwin = 40;
  pcoincwin = 40;
  vcoincwin = 80;
  muonRateFactor = 1.;
  muonPulseWidthFactor = 1.;
  //  rewindTimeBins  =  1000000000;
  dataWindowMin   = -0.2;
  dataWindowMax   = 10.0;
  pileupWindowMin = -10.5;
  pileupWindowMax = 10.5;
  promptPeakWindowMin = -0.010;
  promptPeakWindowMax =  0.010;
  musrMode = 'D';
  overallBinDelay = 0;
  boolInfinitelyLowMuonRate = false;
  frameInterval = 25000.0 * microsecond; // JSL: ISIS "40Hz" average with TS1 & TS2 running
  detRecoveryTime=10.0 * nanosecond; // JSL: typical ISIS PMT detector
  doPartialFrameAtEnd = false; // tidy end of run
  // processInBulk = true;
  commonThreshold = -1000. ; // allow individual settings
  //  globTime  = 0.;
  
  int    tmpivar;
  float  tmpfvar;
  //  int    tmpCoincidenceMultimap[maxChannels][maxChannels]; 
  std::multimap<int, int> tmpCoincidenceMultimap;
  //  for (int i=0; i<maxChannels; i++) {
  //    for (int j=0; j<maxChannels; j++) {
  //      //  tmpCoincidenceMultimap[i][j]=0;
  //      tmpCoincidenceMultimap.insert(pair<int, int>("a", 1));
  //
  //    }
  //  }
  while (!feof(fSteeringFile)) {
    fgets(line,10000,fSteeringFile);
    if ((line[0]!='#')&&(line[0]!='\n')&&(line[0]!='\r')&&(line[0]!='$')&&(line[0]!='!')) {
      char tmpString0[200]="Unset";
      char tmpString1[200]="Unset";
      sscanf(&line[0],"%s",tmpString0);
      if (strncmp(tmpString0,"INSTRUMENT=",strlen("INSTRUMENT="))==0) {
	instrument=&line[strlen("INSTRUMENT=")];
      }
      else if (strncmp(tmpString0,"DESCRIPTION=",strlen("DESCRIPTION="))==0) {
	description=&line[strlen("DESCRIPTION=")];
      }
      else if (strncmp(tmpString0,"TYPE=",strlen("TYPE="))==0) {
	tdchwtype=&line[strlen("TYPE=")];
      }
      else if (strncmp(tmpString0,"RESOLUTION=",strlen("RESOLUTION="))==0) {
	sscanf(&line[strlen("RESOLUTION=")],"%g",&tmpfvar);
	tdcresolution = tmpfvar * picosecond;
	if (tdcresolution<1*picosecond) {
	  std::cout<<"\n !!! ERROR !!! Time resolution of TDC must be larger than 1 ps (you requested "<<tdcresolution/picosecond<<" ps)"<<std::endl;
	  std::cout<<"   ===> too low time resolution might lead to numerical problems in musrCounter ==> S T O P"<<std::endl;
	  exit(1);
	}
      }
      else if (strncmp(tmpString0,"MDELAY=",strlen("MDELAY="))==0) {
	sscanf(&line[strlen("MDELAY=")],"%d",&tmpivar);
	mdelay = tmpivar;
      }
      else if (strncmp(tmpString0,"PDELAY=",strlen("PDELAY="))==0) {
	sscanf(&line[strlen("PDELAY=")],"%d",&tmpivar);
	pdelay = tmpivar;
      }
      else if (strncmp(tmpString0,"MCOINCIDENCEW=",strlen("MCOINCIDENCEW="))==0) {
	sscanf(&line[strlen("MCOINCIDENCEW=")],"%d",&tmpivar);
	mcoincwin = tmpivar;
      }
      else if (strncmp(tmpString0,"PCOINCIDENCEW=",strlen("PCOINCIDENCEW="))==0) {
	sscanf(&line[strlen("PCOINCIDENCEW=")],"%d",&tmpivar);
	pcoincwin = tmpivar;
      }
      else if (strncmp(tmpString0,"VCOINCIDENCEW=",strlen("VCOINCIDENCEW="))==0) {
	sscanf(&line[strlen("VCOINCIDENCEW=")],"%d",&tmpivar);
	vcoincwin = tmpivar;
      }
      else if (strncmp(tmpString0,"MUONRATEFACTOR=",strlen("MUONRATEFACTOR="))==0) {
	sscanf(&line[strlen("MUONRATEFACTOR=")],"%g",&tmpfvar);
	muonRateFactor = tmpfvar;
      }
      else if (strncmp(tmpString0,"MUONPULSEWIDTHFACTOR=",strlen("MUONPULSEWIDTHFACTOR="))==0) {
	sscanf(&line[strlen("MUONPULSEWIDTHFACTOR=")],"%g",&tmpfvar);
	muonPulseWidthFactor = tmpfvar;
      }
      else if (strncmp(tmpString0,"INFINITELYLOWMUONRATE=",strlen("INFINITELYLOWMUONRATE"))==0) {
	boolInfinitelyLowMuonRate=true;
      }
      else if (strncmp(tmpString0,"FRAMEINTERVAL=",strlen("FRAMEINTERVAL="))==0) { // JSL: pulsed mode params. Frame interval in ms
	sscanf(&line[strlen("FRAMEINTERVAL=")],"%g",&tmpfvar);
	frameInterval = tmpfvar * microsecond * 1000.0;
	  }
      else if (strncmp(tmpString0,"DEADTIME=",strlen("DEADTIME="))==0) { // JSL: pulsed mode params, dead time in ns. Redefine part way between detectors if not all the same
	sscanf(&line[strlen("DEADTIME=")],"%g",&tmpfvar);
	detRecoveryTime = tmpfvar * nanosecond;
	  }
      else if (strncmp(tmpString0,"COMMONTHRESHOLD=",strlen("COMMONTHRESHOLD="))==0) { // JSL: pulsed mode params, dead time in ns. Redefine part way between detectors if not all the same
	sscanf(&line[strlen("COMMONTHRESHOLD=")],"%g",&tmpfvar);
	commonThreshold = tmpfvar;
      }
      else if (strncmp(tmpString0,"PARTIALFRAMEATEND=",strlen("PARTIALFRAMEATEND"))==0) {
	doPartialFrameAtEnd=true;
      }
    //  else if (strncmp(tmpString0,"PROCESSINBULK=",strlen("PROCESSINBULK"))==0) {
	//processInBulk=true;
    //  }
      else if (strncmp(tmpString0,"REWINDTIMEBINS=",strlen("REWINDTIMEBINS="))==0) {
	sscanf(&line[strlen("REWINDTIMEBINS=")],"%d",&tmpivar);
	if (tmpivar<0) {rewindTimeBins = Long64_t(tmpivar)* Long64_t(tmpivar);}
	else {rewindTimeBins = tmpivar;}
	std::cout<<" JUHELAK: rewindTimeBins="<<rewindTimeBins<<std::endl;
      }
      else if (strncmp(tmpString0,"DATAWINDOWMIN=",strlen("DATAWINDOWMIN="))==0) {
	sscanf(&line[strlen("DATAWINDOWMIN=")],"%g",&tmpfvar);
	dataWindowMin = tmpfvar;
      }
      else if (strncmp(tmpString0,"DATAWINDOWMAX=",strlen("DATAWINDOWMAX="))==0) {
	sscanf(&line[strlen("DATAWINDOWMAX=")],"%g",&tmpfvar);
	dataWindowMax = tmpfvar;
      }
      else if (strncmp(tmpString0,"PILEUPWINDOWMIN=",strlen("PILEUPWINDOWMIN="))==0) {
	sscanf(&line[strlen("PILEUPWINDOWMIN=")],"%g",&tmpfvar);
	pileupWindowMin = tmpfvar;
      }
      else if (strncmp(tmpString0,"PILEUPWINDOWMAX=",strlen("PILEUPWINDOWMAX="))==0) {
	sscanf(&line[strlen("PILEUPWINDOWMAX=")],"%g",&tmpfvar);
	pileupWindowMax = tmpfvar;
      }
      else if (strncmp(tmpString0,"PROMPTPEAKMIN=",strlen("PROMPTPEAKMIN="))==0) {
	sscanf(&line[strlen("PROMPTPEAKMIN=")],"%g",&tmpfvar);
	promptPeakWindowMin = tmpfvar;
      }
      else if (strncmp(tmpString0,"PROMPTPEAKMAX=",strlen("PROMPTPEAKMAX="))==0) {
	sscanf(&line[strlen("PROMPTPEAKMAX=")],"%g",&tmpfvar);
	promptPeakWindowMax = tmpfvar;
      }
      else if (strncmp(tmpString0,"MUSRMODE=",strlen("MUSRMODE="))==0) {
	sscanf(&line[strlen("MUSRMODE=")],"%s",&tmpString1);
	musrMode = tmpString1[0];
      }
      else if (strcmp(tmpString0,"CLONECHANNEL")==0) {
	int ichannel_orig_tmp, ichannel_new_tmp;
	sscanf(&line[0],"%*s %d %d",&ichannel_orig_tmp,&ichannel_new_tmp);
	bool_clonedChannelsMultimap_NotEmpty = true;
	clonedChannelsMultimap.insert(std::pair<int,int>(ichannel_orig_tmp,ichannel_new_tmp));
      }
      else if (strcmp(tmpString0,"DEBUGEVENT")==0) {
	int ieventToDebug_tmp, iLevelToDebug_tmp;
	sscanf(&line[0],"%*s %d %d",&ieventToDebug_tmp,&iLevelToDebug_tmp);
	bool_debugingRequired=true;
	if (ieventToDebug_tmp>-1) {
	  debugEventMap.insert(std::pair<int,int>(ieventToDebug_tmp,iLevelToDebug_tmp));
	}
	else {
	  for (int j=0; j<=-ieventToDebug_tmp;j++) {
	    debugEventMap.insert(std::pair<int,int>(j,iLevelToDebug_tmp));
	  }
	}
      }
      else if (strcmp(tmpString0,"WRITE_OUT_DUMP_FILE")==0) {
	int  clock_channelID;
	int  max_timeBin_jitter;
	//	int  clock_interval_tmp;
	//	sscanf(&line[0],"%*s %s %d %d",tmpString1,&clock_channelID_tmp,&clock_interval_tmp);
	sscanf(&line[0],"%*s %s %d %d",tmpString1,&clock_channelID,&max_timeBin_jitter);
	//	clock_channelID = clock_channelID_tmp;
	//	clock_interval  = clock_interval_tmp;
	musrCounter::bool_WriteDataToDumpFile = true;
	//	std::cout<<"tmpString1="<<tmpString1<<std::endl;
	//	if ((strcmp(tmpString1,"")!=0)&&(strcmp(tmpString1,"Unset")!=0)) musrCounter::dumpFile.open(tmpString1);
	//	//	musrCounter::dumpFile.open("dumpFile.txt");
	//	if (! (musrCounter::dumpFile.is_open()) ) {
	//	  std::cout<<"Writing data into a DumpFile was requested, but the dump file can not be opened!"<<std::cout;
	//	  std::cout<<"  ===> STOP !!!"<<std::endl;
	//	  exit(1);
	//	}
	//	myWriteDump = new musrWriteDump(charV1190FileName,clock_channelID,max_timeBin_jitter);
	myWriteDump = new musrWriteDump(tmpString1,clock_channelID,max_timeBin_jitter);
      }
      else if (strcmp(tmpString0,"USE_UNPERFECT_POSITRONS_IN_DOUBLE_HITS")==0) {
	musrCounter::bool_ignoreUnperfectPositrons = false;
      }
      else if (strcmp(tmpString0,"USE_UNPERFECT_MUONS_IN_DOUBLE_HITS")==0) {
	musrCounter::bool_ignoreUnperfectMuons = false;
      }      
      else if (strncmp(tmpString0,"musrTH",strlen("musrTH"))==0) {
	// Definition of the histograms - either musrTH1D or musrTH2D
	int beginningOfHistoTitle=0, endOfHistoTitle =0;
	for (Int_t i=0; i<(int)strlen(line); i++) {
	  if (line[i]=='"') {
	    if (beginningOfHistoTitle==0) beginningOfHistoTitle=i+1;
	    else                          endOfHistoTitle=i-1;
	  }
	}
	char histoName[200];
	sscanf(&line[0],"%*s %s",histoName);
	char histoTitle[501];
	CopySubstring(line,beginningOfHistoTitle,endOfHistoTitle,histoTitle);
	int nrBinsX, nrBinsY;
	float minX, maxX, minY, maxY;
	char varXName[500];  char varYName[500];
	char furtherOption[200]; sprintf(furtherOption,"Undefined");
	float rot_ref_frequency, rot_ref_phase;
	if (strcmp(tmpString0,"musrTH1D")==0) {
	  sscanf(&line[endOfHistoTitle+2],"%d %g %g %s %s %g %g",&nrBinsX,&minX,&maxX,varXName,furtherOption,&rot_ref_frequency,&rot_ref_phase);
	}
	else if (strcmp(tmpString0,"musrTH2D")==0) {
	  sscanf(&line[endOfHistoTitle+2],"%d %g %g %d %g %g %s %s",&nrBinsX,&minX,&maxX,&nrBinsY,&minY,&maxY,varXName,varYName);
	}
	else {
	  std::cout<<" !!! ERROR:  keyword \""<<tmpString0<<"\" in the steering file not understood ==> S T O P !!!"<<std::endl;
	  exit(1);
	}
	std::string stringVariableName1 = varXName;
	std::string stringVariableName2 = varYName;
	variableMapType::iterator iter1 = variableMap.find(stringVariableName1);
	variableMapType::iterator iter2;
	if (iter1==variableMap.end()) {
	  std::cout<<" !!!  ERROR: Variable with the name \""<<stringVariableName1<<"\" not defined in variableMap ==> ignored, histogram not defined!   S T O P !!!"<<std::endl;
	  exit(1);
	}
	if (strcmp(tmpString0,"musrTH2D")==0) {
	  iter2 = variableMap.find(stringVariableName2);
	  if (iter2==variableMap.end()) {
	    std::cout<<" !!!  ERROR: Variable with the name \""<<stringVariableName2<<"\" not defined in variableMap ==> ignored, histogram not defined!   S T O P !!!"<<std::endl;
	    exit(1);
	  }
	}
	
	Double_t* pointerToVariable1 = iter1->second;
	if (strcmp(tmpString0,"musrTH1D")==0) {
	  //	  std::cout<<"pointerToVariable1="<<pointerToVariable1<<"  pointer to muDecayPosZ="<<&muDecayPosZ<<std::endl;
	  musrTH* myTH = new musrTH("1D",histoName,histoTitle,nrBinsX,minX,maxX,nrBinsY,minY,maxY,pointerToVariable1,NULL);
	  listOfAllHistograms1D.push_back(myTH);
	  if (strcmp(furtherOption,"rotreference")==0) {
	    myTH->SetRotatingReferenceFrame(rot_ref_frequency, rot_ref_phase);
	    hInfo->Fill(21, rot_ref_frequency);  // value may be overwritten - just the last rot. ref. frame will be saved
	    hInfo->Fill(22, rot_ref_phase);      // value may be overwritten - just the last rot. ref. frame will be saved
	  }
	  else if (strcmp(furtherOption,"correctexpdecay")==0) {
	    myTH->SetExpDecayCorrection();
	  }
	}
	else {
	  Double_t* pointerToVariable2 = iter2->second;
	  musrTH* myTH = new musrTH("2D",histoName,histoTitle,nrBinsX,minX,maxX,nrBinsY,minY,maxY,pointerToVariable1,pointerToVariable2);
	  listOfAllHistograms2D.push_back(myTH);
	}
      } 
      else if  (strncmp(tmpString0,"humanDecayHistograms",strlen("humanDecayHistograms"))==0) {
	char motherHistoName[200];
	char motherHistoPileupName[200];
	sscanf(&line[0],"%*s %s %s",motherHistoName,motherHistoPileupName);
	std::cout<<" Defining humanDecayHistograms (motherHistoName="<<motherHistoName<<")"<<std::endl;
	for(std::list<musrTH*>::const_iterator it = listOfAllHistograms1D.begin(); it != listOfAllHistograms1D.end(); ++it) {
	  if ((*it)->IsThisHistoNamed(motherHistoName)) {
	    motherOfHumanDecayHistograms = *it;
	  }
	  if ((*it)->IsThisHistoNamed(motherHistoPileupName)) {
	    motherOfHumanDecayPileupHistograms = *it;
	  }
	}
	if (motherOfHumanDecayHistograms==NULL) {
	  std::cout<<"\n\nmusrAnalysis::ReadInInputParameters  Mother histogram (motherOfHumanDecayHistograms)"
		   <<"of the \"Human decay histogram\" not found!"<<std::endl;
	  std::cout<<"                 "<<line<<std::endl;
	  std::cout<<"  ==> S T O P "<<std::endl;
	  exit(1);
	}
	if (motherOfHumanDecayPileupHistograms==NULL) {
	  std::cout<<"\n\nmusrAnalysis::ReadInInputParameters  Mother histogram (motherOfHumanDecayPileupHistograms)"
		   <<"of the \"Pileup human decay histogram\" not found!"<<std::endl;
	  std::cout<<"                 "<<line<<std::endl;
	  std::cout<<"  ==> S T O P "<<std::endl;
	  exit(1);
	}

	char* pch0 = strstr(line,motherHistoName)+strlen(motherHistoName);
	char* pch  = strstr(pch0,motherHistoPileupName)+strlen(motherHistoPileupName);

	int nscan; int N1; char NAME1[100];
	nscan = sscanf(pch,"%d %s",&N1,NAME1);
	//	indexHuman=0;
	Int_t iBinHuman=0;
	Int_t nBinHuman=0;
	do {
	  Bool_t nameDefined=false;
	  for (humanDecayMapType::const_iterator it = humanDecayMap.begin(); it!=humanDecayMap.end(); ++it) {
	    if ((it->second)==(std::string) NAME1) {
	      nameDefined = true;
	      iBinHuman   = it->first;
	    }
	  }
	  if (!nameDefined) {
	    nBinHuman++;
	    iBinHuman=nBinHuman;
	    humanDecayMap.insert(std::pair<Int_t,std::string>(iBinHuman,NAME1));
	  }
	  humanDecayMultimap.insert(std::pair<Int_t,Int_t>(iBinHuman,N1));
	  char* pch2 = strstr(pch,NAME1)+strlen(NAME1);
	  pch=pch2;
	  nscan = sscanf(pch,"%d %s",&N1,NAME1);
	  //	  indexHuman++;
	} while (nscan==2);
	
	Int_t nrBinsHumanHist=humanDecayMap.size();
	//	Int_t nrBinsHumanHist=0;
	//	std::string oldString="blaBlaoldStringUndefiNED";
	//	for (humanDecayMultimapType::const_iterator it = humanDecayMultimap.begin(); it!=humanDecayMultimap.end(); ++it) {
	//	  if (oldString!=(it->first)) nrBinsHumanHist++;
	//	}
	std::cout<<"\nnrBinsHumanHist="<<nrBinsHumanHist<<std::endl;
	//	humanDecayHistograms = new musrTH("1D","humanDecayHistograms","Where the muons stop and decay",indexHuman,0,float(indexHuman),0,0,0,NULL,NULL);
	humanDecayHistograms = new musrTH("1D","humanDecayHistograms","Where the muons stop and decay",nrBinsHumanHist,0,float(nrBinsHumanHist),0,0,0,NULL,NULL);
	humanDecayPileupHistograms = new musrTH("1D","humanDecayPileupHistograms","Where the muons contr. to pileup stop and decay",nrBinsHumanHist,0,float(nrBinsHumanHist),0,0,0,NULL,NULL);
	//	for (Int_t k=0; k<indexHuman; k++) {
	//	  humanDecayHistograms->SetBinLabel1D(k+1,sHumanDecayArray[k]);
	//	}
	//	oldString="blaBlaoldStringUndefiNED";
	for (humanDecayMapType::const_iterator it = humanDecayMap.begin(); it!=humanDecayMap.end(); ++it) {
	  humanDecayHistograms        -> SetBinLabel1D(it->first,it->second);
	  humanDecayPileupHistograms -> SetBinLabel1D(it->first,it->second);
	}
      }
      else if  (strncmp(tmpString0,"condition",strlen("condition"))==0) {
	// Selection of the predefined conditions to be applied
	int  iConditionTMP;
	char conditionNameTMP[200];
	sscanf(&line[0],"%*s %d %s",&iConditionTMP,conditionNameTMP);
	if (iConditionTMP<0) {
	  std::cout<<" !!!  ERROR: Condition number must be >= 0  (not "<<iConditionTMP<<")"<< std::endl; 
	  exit(1);
	}
	if (iConditionTMP>=nrConditions) {
	  std::cout<<" !!!  ERROR: Condition number must be < "<<nrConditions<<"  (not "<<iConditionTMP<<")"<< std::endl; 
	  exit(1);
	}
	if      (strcmp(conditionNameTMP,"alwaysTrue")==0)              conditionMap[iConditionTMP]=&alwaysTrue;
	else if (strcmp(conditionNameTMP,"oncePerEvent")==0)            conditionMap[iConditionTMP]=&oncePerEvent;
	else if (strcmp(conditionNameTMP,"muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"muonDecayedInSampleOnce_gen")==0) conditionMap[iConditionTMP]=&muonDecayedInSampleOnce_gen;
	else if (strcmp(conditionNameTMP,"muonTriggered_gen")==0)       conditionMap[iConditionTMP]=&muonTriggered_gen;
	else if (strcmp(conditionNameTMP,"muonTriggered_gen_AND_muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&muonTriggered_gen_AND_muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"muonTriggered_det")==0)       conditionMap[iConditionTMP]=&muonTriggered_det;
	else if (strcmp(conditionNameTMP,"positronHit_det")==0)         conditionMap[iConditionTMP]=&positronHit_det;
	else if (strcmp(conditionNameTMP,"goodEvent_det")==0)           conditionMap[iConditionTMP]=&goodEvent_det;
	else if (strcmp(conditionNameTMP,"goodEvent_gen")==0)           conditionMap[iConditionTMP]=&goodEvent_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_det_AND_goodEvent_gen")==0)  conditionMap[iConditionTMP]=&goodEvent_det_AND_goodEvent_gen;
	else if (strcmp(conditionNameTMP,"pileupEventCandidate")==0)    conditionMap[iConditionTMP]=&pileupEventCandidate;
	else if (strcmp(conditionNameTMP,"pileupEvent")==0)             conditionMap[iConditionTMP]=&pileupEvent;
	else if (strcmp(conditionNameTMP,"goodEvent_det_AND_muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&goodEvent_det_AND_muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_F_det")==0)         conditionMap[iConditionTMP]=&goodEvent_F_det;
	else if (strcmp(conditionNameTMP,"goodEvent_B_det")==0)         conditionMap[iConditionTMP]=&goodEvent_B_det;
	else if (strcmp(conditionNameTMP,"goodEvent_U_det")==0)         conditionMap[iConditionTMP]=&goodEvent_U_det;
	else if (strcmp(conditionNameTMP,"goodEvent_D_det")==0)         conditionMap[iConditionTMP]=&goodEvent_D_det;
	else if (strcmp(conditionNameTMP,"goodEvent_L_det")==0)         conditionMap[iConditionTMP]=&goodEvent_L_det;
	else if (strcmp(conditionNameTMP,"goodEvent_R_det")==0)         conditionMap[iConditionTMP]=&goodEvent_R_det;
	else if (strcmp(conditionNameTMP,"goodEvent_F_det_AND_muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&goodEvent_F_det_AND_muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_B_det_AND_muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&goodEvent_B_det_AND_muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_U_det_AND_muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&goodEvent_U_det_AND_muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_D_det_AND_muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&goodEvent_D_det_AND_muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_L_det_AND_muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&goodEvent_L_det_AND_muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_R_det_AND_muonDecayedInSample_gen")==0) conditionMap[iConditionTMP]=&goodEvent_R_det_AND_muonDecayedInSample_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_F_det_AND_pileupEvent")==0) conditionMap[iConditionTMP]=&goodEvent_F_det_AND_pileupEvent;
	else if (strcmp(conditionNameTMP,"goodEvent_B_det_AND_pileupEvent")==0) conditionMap[iConditionTMP]=&goodEvent_B_det_AND_pileupEvent;
	else if (strcmp(conditionNameTMP,"goodEvent_U_det_AND_pileupEvent")==0) conditionMap[iConditionTMP]=&goodEvent_U_det_AND_pileupEvent;
	else if (strcmp(conditionNameTMP,"goodEvent_D_det_AND_pileupEvent")==0) conditionMap[iConditionTMP]=&goodEvent_D_det_AND_pileupEvent;
	else if (strcmp(conditionNameTMP,"goodEvent_L_det_AND_pileupEvent")==0) conditionMap[iConditionTMP]=&goodEvent_L_det_AND_pileupEvent;
	else if (strcmp(conditionNameTMP,"goodEvent_R_det_AND_pileupEvent")==0) conditionMap[iConditionTMP]=&goodEvent_R_det_AND_pileupEvent;
	else if (strcmp(conditionNameTMP,"promptPeak")==0)              conditionMap[iConditionTMP]=&promptPeak;
	else if (strcmp(conditionNameTMP,"promptPeakF")==0)             conditionMap[iConditionTMP]=&promptPeakF;
	else if (strcmp(conditionNameTMP,"promptPeakB")==0)             conditionMap[iConditionTMP]=&promptPeakB;
	else if (strcmp(conditionNameTMP,"promptPeakU")==0)             conditionMap[iConditionTMP]=&promptPeakU;
	else if (strcmp(conditionNameTMP,"promptPeakD")==0)             conditionMap[iConditionTMP]=&promptPeakD;
	else if (strcmp(conditionNameTMP,"promptPeakL")==0)             conditionMap[iConditionTMP]=&promptPeakL;
	else if (strcmp(conditionNameTMP,"promptPeakR")==0)             conditionMap[iConditionTMP]=&promptPeakR;
	else if (strcmp(conditionNameTMP,"doubleHit")==0)             conditionMap[iConditionTMP]=&doubleHit;
	else if (strcmp(conditionNameTMP,"doubleHitEvent_gen")==0)             conditionMap[iConditionTMP]=&doubleHitEvent_gen;
	else if (strcmp(conditionNameTMP,"goodEvent_F_det_AND_doubleHit")==0)             conditionMap[iConditionTMP]=&goodEvent_F_det_AND_doubleHit;
	else if (strcmp(conditionNameTMP,"goodEvent_B_det_AND_doubleHit")==0)             conditionMap[iConditionTMP]=&goodEvent_B_det_AND_doubleHit;
	else if (strcmp(conditionNameTMP,"goodEvent_U_det_AND_doubleHit")==0)             conditionMap[iConditionTMP]=&goodEvent_U_det_AND_doubleHit;
	else if (strcmp(conditionNameTMP,"goodEvent_D_det_AND_doubleHit")==0)             conditionMap[iConditionTMP]=&goodEvent_D_det_AND_doubleHit;
	else if (strcmp(conditionNameTMP,"goodEvent_L_det_AND_doubleHit")==0)             conditionMap[iConditionTMP]=&goodEvent_L_det_AND_doubleHit;
	else if (strcmp(conditionNameTMP,"goodEvent_R_det_AND_doubleHit")==0)             conditionMap[iConditionTMP]=&goodEvent_R_det_AND_doubleHit;
	else if (strcmp(conditionNameTMP,"singleHitEvent_gen")==0)             conditionMap[iConditionTMP]=&singleHitEvent_gen;
	else if (strcmp(conditionNameTMP,"stackedEvent_gen")==0)             conditionMap[iConditionTMP]=&stackedEvent_gen;
	else {
	  std::cout<<" !!!  ERROR: Condition of the name \""<<conditionNameTMP<<"\" not predefined ==> Add it in the musrAnalysis.cxx   S T O P !!!"<<std::endl;
	  exit(1);
	}
      }
      else if  (strcmp(tmpString0,"draw")==0) {
	// Which histograms should be drawn
	int  iConditionTMP;
	char histoNameTMP[200];
	sscanf(&line[0],"%*s %s %d",histoNameTMP,&iConditionTMP);
	if (iConditionTMP<-1) {
	  std::cout<<" !!!  ERROR: draw: condition number must be >= -1  (not "<<iConditionTMP<<")"<< std::endl; 
	  exit(1);
	}
	if (iConditionTMP>=nrConditions) {
	  std::cout<<" !!!  ERROR: draw: condition number must be < "<<nrConditions<<"  (not "<<iConditionTMP<<")"<< std::endl; 
	  exit(1);
	}
	for(std::list<musrTH*>::const_iterator it = listOfAllHistograms1D.begin(); it != listOfAllHistograms1D.end(); ++it) {
	  if ((*it)->IsThisHistoNamed(histoNameTMP)) {
	    (*it)->SetDrawListOfHistograms(iConditionTMP);
	  }
	}
	for(std::list<musrTH*>::const_iterator it = listOfAllHistograms2D.begin(); it != listOfAllHistograms2D.end(); ++it) {
	  if ((*it)->IsThisHistoNamed(histoNameTMP)) {
	    (*it)->SetDrawListOfHistograms(iConditionTMP);
	  }
	}
	if (humanDecayHistograms->IsThisHistoNamed(histoNameTMP)) {
	  humanDecayHistograms->SetDrawListOfHistograms(iConditionTMP);
	}
	if (humanDecayPileupHistograms->IsThisHistoNamed(histoNameTMP)) {
	  humanDecayPileupHistograms->SetDrawListOfHistograms(iConditionTMP);
	}
      }
      else if  (strcmp(tmpString0,"counterPhaseShifts")==0) {
	int nscan; int N1; float PHASE_SHIFT; char NAME1[100]; char NAME2[100];
	char *pch = line + strlen("counterPhaseShifts");
	do {
	  nscan = sscanf(pch,"%d %g",&N1,&PHASE_SHIFT);
	  phaseShiftMap.insert(std::pair<int,Double_t>(N1,PHASE_SHIFT/180*pi));
	  std::cout<<"N1="<<N1<<", PHASE_SHIFT="<<PHASE_SHIFT/180<<" pi"<<std::endl;
	  nscan = sscanf(pch,"%s %s",NAME1,NAME2);
	  char* pch1 = strstr(pch ,NAME1)+strlen(NAME1);
	  char* pch2 = strstr(pch1,NAME2)+strlen(NAME2);
	  pch=pch2;
	  nscan = sscanf(pch,"%d %g",&N1,&PHASE_SHIFT);
	} while (nscan==2);
      }
      else if (strcmp(tmpString0,"counterGrouping")==0) {
	int nscan; int N1; char NAME[100];
	char *pch = line + strlen("counterGrouping");
	char counterGroupName[100];
	nscan = sscanf(pch,"%s",counterGroupName);
	char* pch1 = strstr(pch,counterGroupName)+strlen(counterGroupName);
	pch = pch1;
	do {
	  nscan = sscanf(pch,"%d",&N1);
	  if      (strcmp(counterGroupName,"F")==0) F_posCounterList.push_back(N1);
	  else if (strcmp(counterGroupName,"B")==0) B_posCounterList.push_back(N1);
	  else if (strcmp(counterGroupName,"U")==0) U_posCounterList.push_back(N1);
	  else if (strcmp(counterGroupName,"D")==0) D_posCounterList.push_back(N1);
	  else if (strcmp(counterGroupName,"L")==0) L_posCounterList.push_back(N1);
	  else if (strcmp(counterGroupName,"R")==0) R_posCounterList.push_back(N1);
	  else {
	    std::cout<<"\n\n UNKNOWN COUNTER GROUP REQUIRED !!!    =====>    S T O P    F O R C E D"<<std::endl;
	    std::cout<<line<<std::endl;
	    exit(1);
	  }
	  std::cout<<"counterGroupName="<<counterGroupName<<"   N1="<<N1<<std::endl;
	  nscan = sscanf(pch,"%s",NAME);
	  char* pch2 = strstr(pch ,NAME)+strlen(NAME);
	  pch=pch2;
	  nscan = sscanf(pch,"%d",&N1);
	} while (nscan==1);
      }
      else if (strcmp(tmpString0,"sampleID")==0) {
	int nscan; int N1; char NAME[100];
	char *pch = line + strlen("sampleID");
	do {
	  nscan = sscanf(pch,"%d",&N1);
	  SampleDetIDList.push_back(N1);
	  nscan = sscanf(pch,"%s",NAME);
	  char* pch2 = strstr(pch ,NAME)+strlen(NAME);
	  pch=pch2;
	  nscan = sscanf(pch,"%d",&N1);
	} while (nscan==1);
	std::cout<<"SampleID :";
	for (std::list <Int_t>::const_iterator it=SampleDetIDList.begin(); it!=SampleDetIDList.end(); ++it) {
	  std::cout<<" "<< *it;
	}
	std::cout<<std::endl;
      }
      else if  (strcmp(tmpString0,"setSpecialAnticoincidenceTimeWindow")==0) {
	int channelNumber;
	double min, max;
	char unit[100]="Undefined";
	Long64_t lmin, lmax;
	sscanf(&line[0],"%*s %d %lf %lf %s",&channelNumber,&min,&max,unit);
	if      ((strcmp(unit,"bin")==0)||(strcmp(unit,"bins")==0)) {lmin = Long64_t (min);  lmax = Long64_t (max);}
	else if ((strcmp(unit,"ns") ==0)||(strcmp(unit,"nanosecond") ==0)) {
	  lmin = Long64_t (min*nanosecond/tdcresolution);  
	  lmax = Long64_t (max*nanosecond/tdcresolution);
	}
	else if ((strcmp(unit,"mus") ==0)||(strcmp(unit,"microsecond") ==0)) {
	  lmin = Long64_t (min*microsecond/tdcresolution);  
	  lmax = Long64_t (max*microsecond/tdcresolution);
	}

	else {
	  std::cout<<" ERROR!  musrAnalysis: unknown unit in setSpecialAnticoincidenceTimeWindow !!!  (unit="<<unit<<")"<<std::endl;
	  std::cout<<"    ==> S T O P "<<std::endl;
	  exit(1);
	}
	std::cout<<" lmin="<<lmin<<" bins"<<std::endl;
	std::cout<<" lmax="<<lmax<<" bins"<<std::endl;

	if (allCounterMap.find(channelNumber)==allCounterMap.end()) {
	  std::cout<<" ERROR!  musrAnalysis: unknown counter ("<<channelNumber<<") in setSpecialAnticoincidenceTimeWindow"<<std::endl;
	  std::cout<<"    ==> S T O P "<<std::endl;
	  exit(1);
	}
	musrCounter* counter = (allCounterMap.find(channelNumber))->second;
	counter->SetAntiCoincidenceTimeWindowMin(lmin);
	counter->SetAntiCoincidenceTimeWindowMax(lmax);
      }
      else if  (strcmp(tmpString0,"artificiallyChangeMuDecayTime")==0) {
	float min, max, mmmin, mmmax;
	sscanf(&line[0],"%*s %g %g %g %g %g %g %g",&min,&max,&mmmin,&mmmax);
	bool_muDecayTimeTransformation = true;
	muDecayTime_t_min              = min;
        muDecayTime_t_max              = max;
	muDecayTime_Transformation_min = mmmin;
        muDecayTime_Transformation_max = mmmax;
	if ((muDecayTime_t_max <= muDecayTime_t_min) || (muDecayTime_Transformation_max <= muDecayTime_Transformation_min)) {
	  std::cout<<" ERROR!  musrAnalysis: error when setting the \"artificiallyChangeMuDecayTime\" parameters!  Min > Max !!!"<<std::endl;
	  std::cout<<"    ==> S T O P "<<std::endl;
	  exit(1);
	}
      }
      else if  (strcmp(tmpString0,"fit")==0) {
	char histoName[100]; char functionName[100]; char functOption[100]; float xMin; float xMax; float p0, p1, p2, p3, p4, p5, p6;
	sscanf(&line[0],"%*s %s %s %s %g %g %g %g %g %g %g",histoName,functionName,functOption,&xMin,&xMax,&p0,&p1,&p2,&p3,&p4,&p5,&p6);
	if (strcmp(functOption,"\"\"")==0) strcpy(functOption,"");

	TF1 *funct;
	
	if (strcmp(functionName,"funct1")==0) {
	  funct = new TF1("funct1","[3]*exp(([4]-x)/2.19703)*(1+[2]*cos(x*[0]+[1]))");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	  funct -> SetParameter(2,p2);
	  funct -> SetParameter(3,p3);
	  funct -> FixParameter(4,xMin);
	}
	else if (strcmp(functionName,"funct2")==0) {
	  funct = new TF1("funct2","[3]*exp(([4]-x)/2.19703)*(1+[2]*cos(x*[0]+[1]))+[5]");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	  funct -> SetParameter(2,p2);
	  funct -> SetParameter(3,p3);
	  funct -> FixParameter(4,xMin);
	  funct -> SetParameter(5,p4);
	}
	else if (strcmp(functionName,"funct3")==0) {
	  funct = new TF1("funct3","[3]*exp(([4]-x)/2.19703)*(1+[2]*cos(x*[0]+[1]))+[5]");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	  funct -> SetParameter(2,p2);
	  funct -> SetParameter(3,p3);
	  funct -> FixParameter(4,xMin);
	  funct -> FixParameter(5,p4);
	}
	else if (strcmp(functionName,"funct4")==0) {
	  funct = new TF1("funct4","[3]*exp(-x/2.19703)*(1+[2]*cos(x*[0]+[1]))+[4]");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	  funct -> SetParameter(2,p2);
	  funct -> SetParameter(3,p3);
	  funct -> SetParameter(4,p4);
	}
	else if (strcmp(functionName,"pol0")==0) {
	  funct = new TF1("pol0","pol0");
	}
	else if (strcmp(functionName,"simpleExpoPLUSconst")==0) {
	  funct = new TF1("simpleExpoPLUSconst","[0]*exp(-x/2.19703)+[1]");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	}
	else if (strcmp(functionName,"TFieldCosPLUSbg")==0) {
	  funct = new TF1("TFieldCosPLUSbg","[3]*(1+[2]*cos(x*[0]+[1]))+[4]*exp(x/2.19703)");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	  funct -> SetParameter(2,p2);
	  funct -> SetParameter(3,p3);
	  funct -> SetParameter(4,p4);
	}
	else if (strcmp(functionName,"TFieldCos")==0) {
	  funct = new TF1("TFieldCos","[3]*(1+[2]*cos(x*[0]+[1]))");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	  funct -> SetParameter(2,p2);
	  funct -> SetParameter(3,p3);
	}
	else if (strcmp(functionName,"rotFrameTime20")==0) {
	  //	  funct = new TF1("rotFrameTime20","[2]*exp(-x/2.19703)*cos(x*[0]+[1]) ");
	  funct = new TF1("rotFrameTime20","[2]*cos(x*[0]+[1]) ");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	  funct -> SetParameter(2,p2);
	}
	else if (strcmp(functionName,"gaus")==0) {
	  funct = new TF1("gaus","gaus");
	  funct -> SetParameter(0,p0);
	  funct -> SetParameter(1,p1);
	  funct -> SetParameter(2,p2);
	}
	else {
	  std::cout<<"musrAnalysis::ReadInInputParameters:  function \""<<functionName<<"\" not defined! ==> S T O P"<<std::endl;
	  exit(1);
	}
	
	musrTH* tmpHistograms=NULL;
	for(std::list<musrTH*>::const_iterator it = listOfAllHistograms1D.begin(); it != listOfAllHistograms1D.end(); ++it) {
	  if ((*it)->IsThisHistoNamed(histoName)) {
	    tmpHistograms = *it;
	  }
	}
	if (tmpHistograms==NULL) {
	  std::cout<<"\n\nmusrAnalysis::ReadInInputParameters: Fit required, but the corresponding histogram "
		   <<"not found!"<<std::endl;
	  std::cout<<"                 "<<line<<std::endl;
	  std::cout<<"  ==> S T O P "<<std::endl;
	  exit(1);
	}

	tmpHistograms->AssignFunction(funct, functOption, xMin, xMax);
      }
      
      else { 
	//------------------------------------------------------------------
	// Second part of the configuration file
	//
	// Replace spaces between quotation marks by '_' 
	// and remove quatation marks and semicolon by ' '. 
	// Remember position of the beginning and end of the defintion of the coincidence counters
	int nrOfSemicolons=0, beginningOfCoincidenceArray=0, endOfCoincidenceArray =0;
	for (Int_t i=0; i<(int)strlen(line); i++) {
	  if (line[i]=='"') {
	    line[i] = ' ';
	    Int_t ii=i;
	    do {
	      ii++;
	      if (isspace(line[ii])) line[ii] = '_';
	    } while (line[ii]!='"');
	    line[ii]= ' ';
	  }
	  else if (line[i] == ';')   {
	    line[i] = ' ';
	    nrOfSemicolons++; 
	    if (nrOfSemicolons==5) beginningOfCoincidenceArray=i;
	    if (nrOfSemicolons==6)       endOfCoincidenceArray=i;
	  }
	}

	// Read in the values from the configuration file 
	int tmpChannel=-1, tmpTimeShift=0; 
	char tmpName[200], tmpType[200];
	float tmpThreshold =0.;
	Bool_t tmp_keep=(musrMode=='P'); // JSL: keep the small pulses in case different events add up to a big one
	sscanf(&line[0],"%d %s %s %g %d",&tmpChannel,tmpName,tmpType,&tmpThreshold,&tmpTimeShift);
	if(commonThreshold>0.0) { tmpThreshold=commonThreshold; }
	musrCounter* counter = new musrCounter(tmpChannel,tmpName,tmpType[0],tmpThreshold,tmpTimeShift, detRecoveryTime / tdcresolution, tmp_keep);
	if      (tmpType[0]=='M') {mCounter               =counter; allCounterMap[tmpChannel]=counter; overallBinDelay=tmpTimeShift;}
	else if (tmpType[0]=='P') {pCounterMap[tmpChannel]=counter; allCounterMap[tmpChannel]=counter;}
	else if (tmpType[0]=='K') {kCounterMap[tmpChannel]=counter; allCounterMap[tmpChannel]=counter;}
	else if (tmpType[0]=='V') {vCounterMap[tmpChannel]=counter; allCounterMap[tmpChannel]=counter;}

	// Read in the values for the coincidence detectors
	// (Tricky while the number of detectors in coincidence is not known)
	if ((endOfCoincidenceArray>0)&&(endOfCoincidenceArray!=beginningOfCoincidenceArray)) {
	  char tmpCoinc[100]; strcpy(tmpCoinc,"987654321");
	  int jj=beginningOfCoincidenceArray;
	  do {
	    do {jj++;} while (isspace(line[jj]));  // skip space characters;
	    if (jj>endOfCoincidenceArray) {break;}
	    sscanf(&line[jj],"%s",tmpCoinc);
	    jj+=strlen(tmpCoinc);
	    int iCoinc=atoi(tmpCoinc);
	    if (iCoinc==987654321) continue;
	    tmpCoincidenceMultimap.insert(std::pair<int, int>(tmpChannel,iCoinc));
	    //	    tmpCoincidenceMultimap[tmpChannel][abs(iCoinc)]=iCoinc;
	  } while (jj<endOfCoincidenceArray);
	  // Read in the last part of the configuration line (after the coincidence definition)
	  if (tmpType[0]=='P') {
	    char tmpHistoName[500];     strcpy(tmpHistoName,   "Unset");
	    char tmpHistoNameAdd[500];  strcpy(tmpHistoNameAdd,"Unset");
	    int tmpT0=0, tmpT1=0, tmpT2=0, tmpHistoaddNr=0;
	    sscanf(&line[endOfCoincidenceArray],"%s %d %d %d %d %s",tmpHistoName,&tmpT0,&tmpT1,&tmpT2,&tmpHistoaddNr,tmpHistoNameAdd);
	    counter->SetTDChistogram(tmpHistoName,tmpT0,tmpT1,tmpT2,tmpHistoaddNr,tmpHistoNameAdd);
	  }
	}
      }
      //      std::cout << line;
    }
  }
  std::cout << "INSTRUMENT=" << instrument << std::endl;
  std::cout << "DESCRIPTION=" << description << std::endl;
  std::cout << "TYPE=" << tdchwtype << std::endl;
  std::cout << "RESOLUTION="<<tdcresolution<<" ps"<<std::endl;
  std::cout << "MDELAY="<<mdelay<<std::endl;
  std::cout << "PDELAY="<<pdelay<<std::endl;
  std::cout << "MCOINCIDENCEW="<<mcoincwin<<std::endl;
  std::cout << "PCOINCIDENCEW="<<pcoincwin<<std::endl;
  std::cout << "VCOINCIDENCEW="<<vcoincwin<<std::endl;
  std::cout << "DATAWINDOWMIN="<<dataWindowMin<<std::endl;
  std::cout << "DATAWINDOWMAX="<<dataWindowMax<<std::endl;
  std::cout << "PILEUPWINDOWMIN="<<pileupWindowMin<<std::endl;
  std::cout << "PILEUPWINDOWMAX="<<pileupWindowMax<<std::endl;
  std::cout << "REWINDTIMEBINS="<<rewindTimeBins<<std::endl;


  // Loop over all counters and set maximum coincidence window (maxCoincidenceTimeWindow):
  // The "K" counters are tricky - they have to be set according to the detector, in which they
  // are in coincidence.
 
  pileupWindowBinMin = Long64_t(pileupWindowMin/tdcresolution);
  pileupWindowBinMax = Long64_t(pileupWindowMax/tdcresolution);
  dataWindowBinMin   = Long64_t(  dataWindowMin/tdcresolution);
  dataWindowBinMax   = Long64_t(  dataWindowMax/tdcresolution);

  for (counterMapType::const_iterator it = allCounterMap.begin(); it!=allCounterMap.end(); ++it) {
    int iChanNr = (it->second)->GetCounterNr();
    for (std::multimap<int,int>::iterator itCoinc = tmpCoincidenceMultimap.begin(); itCoinc != tmpCoincidenceMultimap.end();  ++itCoinc)  {
      if ((*itCoinc).first == iChanNr) {
	int iChn2 = itCoinc->second;
	counterMapType::const_iterator itTwo = allCounterMap.find(abs(iChn2));
        if (itTwo==allCounterMap.end()) { 
	  std::cout<<" Pointer to coincidence counter ("<<iChn2<<") not found!    Perhaps the counter nr. "<<abs(iChn2)
		   <<" was not defined ?"<<std::endl;
	  std::cout<<" Serious problem !!! ==> S T O P"<<std::endl;
	  exit(1);
	}
	else {
	  (it->second)->SetCoincidenceCounter(itTwo->second,iChn2);
	}
      }
    }
  }

  for (counterMapType::const_iterator it = allCounterMap.begin(); it!=allCounterMap.end(); ++it) {
    char DetectorType = (it->second)->GetCounterType();
    if      (DetectorType=='M') {
      (it->second)->SetMaxCoincidenceTimeWindow(pileupWindowBinMin);
      //      (it->second)->SetCoincidenceTimeWindow_M(pileupWindowBinMin,pileupWindowBinMax);
      (it->second)->SetCoincidenceTimeWindowOfAllCoincidenceDetectors('M',-mcoincwin+pileupWindowBinMin,-mcoincwin,mcoincwin);
      (it->second)->SetCoincidenceTimeWindowOfAllVetoDetectors(-mcoincwin+pileupWindowBinMin,-vcoincwin,vcoincwin);
    }
    else if (DetectorType=='P') {
      (it->second)->SetMaxCoincidenceTimeWindow(dataWindowBinMin);
      //      (it->second)->SetCoincidenceTimeWindow_P(dataWindowBinMin,dataWindowBinMax);
      (it->second)->SetCoincidenceTimeWindowOfAllCoincidenceDetectors('P',-pcoincwin+dataWindowBinMin,-pcoincwin,pcoincwin);
      (it->second)->SetCoincidenceTimeWindowOfAllVetoDetectors(-pcoincwin+dataWindowBinMin,-vcoincwin,vcoincwin);
    }
    //    else if (DetectorType=='V') {
    //      (it->second)->SetMaxCoincidenceTimeWindow(-vcoincwin);
    //      (it->second)->SetCoincidenceTimeWindow(-vcoincwin,vcoincwin);
    //    }
  }

    //    for (int j=0; j<maxChannels; j++) {
    //    for (multimap<int,int>::iterator itCoinc = tmpCoincidenceMultimap.begin(); itCoinc != tmpCoincidenceMultimap.end();  ++itCoinc)  {
    //      std::cout << "   houby  [" << (*itCoinc).first << ", " << (*itCoinc).second << "]" << std::endl;
 
    //
    //      if (tmpCoincidenceMultimap[iChanNr][j]!=0) {
//      int j = (*itCoinc).first;
//        counterMapType::const_iterator itTwo = allCounterMap.find(j);
//        if (itTwo==allCounterMap.end()) { 
//	  std::cout<<" Pointer to coincidence counter ("<<j<<") not found!  This should never happen!!! ==> S T O P"<<std::endl;
//	  exit(1);
//	}
//	else {
//	  //	  int iChn = tmpCoincidenceMultimap[iChanNr][j];
//	  int iChn = itCoinc->second;
//	  (it->second)->SetCoincidenceCounter(itTwo->second,iChn);
//	}
//	//      }
//    }
//  }


  
 //      
  //
  //      char tmpString1[100]="Unset",tmpString2[100]="Unset",tmpString3[100]="Unset";
  //      sscanf(&line[0],"%*s %s %s",tmpString1,tmpString2);

}

//================================================================

void musrAnalysis::CreateHistograms() {
  std::cout<<"musrAnalysis::CreateHistograms()"<<std::endl;
  // safeTimeWindow=2*(std::max(datawindow,pileupwindow));
  safeTimeWindow = (dataWindowMax>pileupWindowMax)? 3*dataWindowMax: 3*pileupWindowMax;
  std::cout<<"musrAnalysis::CreateHistograms():  safeTimeWindow = "<<safeTimeWindow<<" (bins="<<Long64_t(safeTimeWindow/tdcresolution)<<")"<<std::endl;
  currentTime=0.;
  //  nextEventTime=0.;
  nextUnfilledEventTime=0.;
  numberOfRewinds=0;
  numberOfGoodMuons=0;
  lastPreprocessedEntry=-1;
  firstPreprocessedEntry=0x7FFFFFFFFFFFFFFFLL;
  //  currentEventID =0;
  fChain->GetEntry(1); InitialiseEvent(); Double_t fieldValue = fieldNomVal[0]; if (nFieldNomVal>1) fieldValue += fieldNomVal[1];
  //  omega = 851.372*fabs(fieldValue);
  //  omega = 851.372*fieldValue;     // value set originally in this program
  //  omega = 850.62*fieldValue;  // value used in Geant ?
  omega = 851.610*fieldValue;       // value from the fits of the data

  hInfo->Fill(1, fieldValue);
  hInfo->Fill(6, runID);
  hInfo->Fill(7, hGeantParameters->GetBinContent(7));
  std::cout<<"musrAnalysis::CreateHistograms():   fieldValue="<<fieldValue<<"T,  omega="<<omega<<std::endl;
  // zero the condition counters - could use a histogram for this as well?
  for (Int_t i=0; i<nrConditions; i++) { conditionCounter[i]=0; }
}
//================================================================

void musrAnalysis::SaveHistograms(char* runChar, char* v1190FileName) {

  for(std::list<musrTH*>::const_iterator it = listOfAllHistograms1D.begin(); it != listOfAllHistograms1D.end(); ++it) {
    //    (*it)->ListHistograms();
    (*it)->FitHistogramsIfRequired(omega);
    (*it)->DrawTH1DdrawList("");
    //    (*it)->DrawTH1D("hist",0);
    //    (*it)->DrawTH1D("hist",1);
  }
  for(std::list<musrTH*>::const_iterator it = listOfAllHistograms2D.begin(); it != listOfAllHistograms2D.end(); ++it) {
    (*it)->DrawTH2DdrawList("hist");
  }
  if(musrMode == 'D') {
    for (counterMapType::const_iterator it = pCounterMap.begin(); it!=pCounterMap.end(); ++it) {
      (it->second)->DrawTDChistogram();
    }
  }
  if (humanDecayHistograms) {
    //    humanDecayHistograms->FillHumanDecayArray(motherOfHumanDecayHistograms,indexHuman,nHumanDecayArray);
    humanDecayHistograms->FillHumanDecayArray(motherOfHumanDecayHistograms,humanDecayMap,humanDecayMultimap);
    humanDecayHistograms->DrawTH1DdrawList("text");
    humanDecayPileupHistograms->FillHumanDecayArray(motherOfHumanDecayPileupHistograms,humanDecayMap,humanDecayMultimap);
    humanDecayPileupHistograms->DrawTH1DdrawList("text");
  }

  // need to re-define these for Pulsed acquisition (and anything else)
  Long64_t numberOfMuonCandidates;
  Long64_t numberOfMuonCandidatesAfterVK;
  Long64_t numberOfMuonCandidatesAfterVKandDoubleHitRemoval;
  Double_t durationOfExperiment;
	if(musrMode=='D') {
  numberOfMuonCandidates = mCounter->GetNumberOfMuonCandidates();
  numberOfMuonCandidatesAfterVK = mCounter->GetNumberOfMuonCandidatesAfterVK();
  numberOfMuonCandidatesAfterVKandDoubleHitRemoval = mCounter->GetNumberOfMuonCandidatesAfterVKandDoubleHitRemoval();
  durationOfExperiment = (numberOfRewinds*rewindTimeBins*tdcresolution+currentTime)/1000000;
  hInfo->Fill(5,(Double_t) numberOfMuonCandidates);  // number of "triggers"
  hInfo->Fill(4,(Double_t) numberOfMuonCandidatesAfterVK);  // number of "triggers"
  hInfo->Fill(3,durationOfExperiment);
  hInfo->Fill(8,(Double_t) numberOfMuonCandidatesAfterVKandDoubleHitRemoval);
	}
  //==============================
  //  Write out the histograms
  std::cout<<"musrAnalysis::SaveHistograms()"<<std::endl;
  TDirectory *dir = gDirectory;
  char HistogramOutputFileName[500];
  sprintf(HistogramOutputFileName,"data/his_%s_%s.root",runChar,v1190FileName);
  TFile *fHistograms = new TFile(HistogramOutputFileName,"recreate");
  TIter next(dir->GetList());
  while ( TObject *obj = next() ) {
    if (obj->InheritsFrom(TH1::Class()) ) {obj->Write();}
  }
  fHistograms->Close();
  delete fHistograms;

  //  if (musrCounter::bool_WriteDataToDumpFile==true) {
  //    musrCounter::dumpFile.close();
  //  }

  if (musrCounter::bool_WriteDataToDumpFile) {
    if (myWriteDump!=NULL) {
      delete myWriteDump;
    }
  }

  // condition counts
  std::cout << "Number of times each condition was recorded" << std::endl;
  for(int i=0; i<nrConditions; i++) {
	  std::cout << i << "   " << conditionCounter[i] << "   " << std::endl;
  }

  //==============================
  std::cout<<"==================================================================="<<std::endl;
  if(musrMode=='D') {
  std::cout<<"Number of raw trigger counts (ignoring pileup gate, vetos and coincidences): "<<numberOfMuonCandidates<<std::endl;
  std::cout<<"Number of trigger counts (after vetos and coincidences but not pile-up rejected): "<<numberOfMuonCandidatesAfterVK<<std::endl;
  std::cout<<"Number of triggered events (i.e. only good \"muons\"): "<<numberOfGoodMuons<<std::endl;
  std::cout<<"Duration of the \"experiment\": "<<durationOfExperiment<<" second"<<std::endl;
  //  std::cout<<"                        (numberOfRewinds="<<numberOfRewinds<<")"<<std::endl;
  std::cout<<"MUONRATEFACTOR was set to "<<muonRateFactor<<std::endl;
  std::cout<<"   Raw trigger rate = "<<numberOfMuonCandidates/durationOfExperiment<<" muons/second";
  std::cout<<" (to get 30000 events/second, set MUONRATEFACTOR = "
	   << muonRateFactor*( (numberOfMuonCandidates/durationOfExperiment)/30000 )<<")"<<std::endl;
  std::cout<<"   Trigger rate after V & K = "<<numberOfMuonCandidatesAfterVK/durationOfExperiment<<" muons/second";
  std::cout<<" (to get 30000 events/second, set MUONRATEFACTOR = "
	   << muonRateFactor*( (numberOfMuonCandidatesAfterVK/durationOfExperiment)/30000 )<<")"<<std::endl;
  } else if (musrMode=='P') {
	  Double_t duration_tmp;
	  if(numberOfRewinds==0) {
		  duration_tmp = nextUnfilledEventTime;
		 std::cout <<"One frame only, partial filling = " <<nextUnfilledEventTime/frameInterval << std::endl;
	  } else if(nextUnfilledEventTime>0 && (doPartialFrameAtEnd)) {
		  duration_tmp = numberOfRewinds*frameInterval + nextUnfilledEventTime;
		 std::cout <<"Number of frames: "<<numberOfRewinds<<" whole frames and a " <<nextUnfilledEventTime/frameInterval << "partial one" << std::endl;
	  } else {
		  duration_tmp = (numberOfRewinds)*frameInterval;
		  std::cout << "Number of whole frames: "<<numberOfRewinds<<std::endl;
	  }
	  std::cout <<"Number of muon counts (inc. doubles, minus dead): " << numberOfGoodMuons << std::endl;
	  std::cout << "Duration of the \"experiment\": " << duration_tmp/1000000.0 <<" seconds"<<std::endl;
	  std::cout << "Displayed count rate: " << numberOfGoodMuons / (duration_tmp/1000000.0) * 3600/1.E6 <<" Mevents/hour"<<std::endl;
  }
  std::cout<<"========================== E N D =================================="<<std::endl;
}

//================================================================

void musrAnalysis::AnalyseEvent(Long64_t iiiEntry) {
	Bool_t pulsedFinishing = false;
  //  std::cout<<"\n______________________________________________________________________________________________\n";
  //  std::cout<<"musrAnalysis::AnalyseEvent("<<iiiEntry<<")   currentTime="<<currentTime<<" (bin="<<Long64_t(currentTime/tdcresolution)<<")"<<std::endl;
  

  // Reset some initial variables

  // Loop over several next event and preprocess them (i.e. fill
  // them into the lists/maps of the class musrCounter).
	// JSL: pulsed mode, loop over and preprocess a whole frame worth of events
  if (bool_debugingRequired) {
    if (debugEventMap[eventID]>0) {std::cout<<"DEBUGEVENT "<<eventID<<"_________________ \"Preprocessing Event\"_________"<<std::endl;}
  }
  if(musrMode=='P') {
	  while (((iiiEntry>lastPreprocessedEntry)||((nextUnfilledEventTime<frameInterval)&&(!boolInfinitelyLowMuonRate))) && (lastPreprocessedEntry+1<nentries)) {
		 // std::cout << "preprocessing event no. " << lastPreprocessedEntry+1 << std::endl;
		Double_t deltaT = PreprocessEvent(lastPreprocessedEntry+1);
		nextUnfilledEventTime+=deltaT;
	};

	  // avoid a partial frame at the end with lower than usual dead time effects
	  // but process everything if less than one frame worth of muons to work with
	  pulsedFinishing = ((lastPreprocessedEntry+1>=nentries) && (nextUnfilledEventTime<frameInterval) ); // flag it for later
	  // if(pulsedFinishing) { std::cout << "set Finishing flag" << std::endl; }
  } else { // Continuous 'D'
  while (((iiiEntry>lastPreprocessedEntry)||(((nextUnfilledEventTime-currentTime)<safeTimeWindow))&&(!boolInfinitelyLowMuonRate)) && (lastPreprocessedEntry+1<nentries)) {
    Double_t deltaT = PreprocessEvent(lastPreprocessedEntry+1);
    nextUnfilledEventTime+=deltaT;
  };
  };
  fChain->GetEntry(iiiEntry);  InitialiseEvent();
  if (bool_debugingRequired) {
    if (debugEventMap[eventID]>2)  PrintHitsInAllCounters();
    //    if (debugEventMap[eventID]>1) {std::cout<<"DEBUGEVENT "<<eventID<<"_________________(after \"PreprocessEvent\"_________"<<std::endl;}
  }

  // Loop over all interesting "moments", which are:
  //    1) any hit in the muon counter
  //    2) any new event (even if nothing was detected in the muon counter)

  Long64_t currentEventID = eventID;
  Double_t timeOfM0Hit;
  Long64_t evID;
  Bool_t thisEventAlreadyFilled = false;

  //  std::cout<<"___________________\n";
  //  for (counterMapType::const_iterator it = allCounterMap.begin(); it!=allCounterMap.end(); ++it) {
  //    (*it).second->myPrintThisCounter(eventID);
  //  }
  if (bool_debugingRequired) {
    if (debugEventMap[eventID]>1) {std::cout<<"DEBUGEVENT "<<eventID<<"_________________(\"Filling Histograms\"_________"<<std::endl;}
  }
  if(musrMode=='P') {
	  // no longer use Not-process-in-bulk option...
	  //if(!processInBulk && (!pulsedFinishing || boolInfinitelyLowMuonRate || doPartialFrameAtEnd || (numberOfRewinds==0))) {
		// std::cout<<"FillHistogramsPulsed("<<iiiEntry<<")"<<std::endl;
	//	FillHistogramsPulsed(iiiEntry,iiiEntry);
	 // }
  } else {
	  FillHistograms(iiiEntry);
  }
  if (bool_debugingRequired) {
    //   if (debugEventMap[eventID]>1) {std::cout<<"DEBUGEVENT "<<eventID<<"_________________(after \"FillHistograms\"_________"<<std::endl;}
    if (debugEventMap[eventID]>1) {std::cout<<"____________________________________________________________________"<<std::endl;}
  }



  if(musrMode=='P') {
	  if(iiiEntry==lastPreprocessedEntry) {
		if(!pulsedFinishing || doPartialFrameAtEnd || (numberOfRewinds==0) || boolInfinitelyLowMuonRate) {
			// std::cout<<"FillHistogramsPulsed(-1000)"<<std::endl;
			FillHistogramsPulsed(firstPreprocessedEntry,lastPreprocessedEntry);
	    }
		if(pulsedFinishing && !doPartialFrameAtEnd && (numberOfRewinds>0)) {
		  nextUnfilledEventTime=0.0;
		  // std::cout << "resetting NextUnfilledEventTime"<<std::endl;
		} else {
			if(!pulsedFinishing) {
			// std::cout << "Rewinding/clearing Frame" << std::endl;
			Long64_t currentTimeBin = Long64_t(frameInterval/tdcresolution);
			RemoveOldHitsFromCounters(currentTimeBin-1);              // Remove all hits as they can not influence the next frame
			firstPreprocessedEntry=0x7FFFFFFFFFFFFFFFLL; // reset
			nextUnfilledEventTime -= frameInterval; // only time variable to rewind, otherwise working from start of "this" frame
			numberOfRewinds++;
			}
		};
	  };
  } else { // 'D' - continuous
  currentTime+=timeToNextEvent*muonRateFactor;
  Long64_t currentTimeBin = Long64_t(currentTime/tdcresolution);
  if (!boolInfinitelyLowMuonRate ) RemoveOldHitsFromCounters(currentTimeBin-1);              // Remove all hits that can not influence the next event
  else                            RemoveOldHitsFromCounters(std::numeric_limits<Long64_t>::max()/2);
  if (currentTimeBin>rewindTimeBins) {  // When the time variables start to be too large, rewind the time info everywhere; 
    //    RewindAllTimeInfo(rewindTimeBins);
    RewindAllTimeInfo();
  }
  }
}

//================================================================

//void musrAnalysis::RemoveOldHitsFromCounters(Double_t timeLimit) {
void musrAnalysis::RemoveOldHitsFromCounters(Long64_t timeBinLimit) {
  // Loop over all Counters and remove hits that happen at or before the "timeBinLimit".
  // std::cout<<"musrAnalysis::RemoveOldHitsFromCounters: timeBinLimit="<<timeBinLimit<<std::endl;
  for (counterMapType::const_iterator it = allCounterMap.begin(); it!=allCounterMap.end(); ++it) {
    (*it).second->RemoveHitsInCounter(timeBinLimit);
  }
}

//================================================================

//void musrAnalysis::RewindAllTimeInfo(Double_t timeToRewind) {
//void musrAnalysis::RewindAllTimeInfo(Long64_t timeBinsToRewind) {
void musrAnalysis::RewindAllTimeInfo() {
  Long64_t timeBinsToRewind = rewindTimeBins;
  if (musrCounter::bool_WriteDataToDumpFile) {
    //    Long64_t currentTimeBin = Long64_t(currentTime/tdcresolution);
    //    mCounter->CheckClockInfo(currentTimeBin);
    mCounter->WriteRewindIntoDumpFile();
    //    musrCounter::previousClock -= timeBinsToRewind;
  }
  currentTime           -= timeBinsToRewind*tdcresolution;
  nextUnfilledEventTime -= timeBinsToRewind*tdcresolution;
  numberOfRewinds++;
  // Loop over all timing information and rewind it by "timeToRewind";
  //  std::cout<<"musrAnalysis::RewindAllTimeInfo()"<<std::endl;
  for (counterMapType::const_iterator it = allCounterMap.begin(); it!=allCounterMap.end(); ++it) {
    //    (*it).second->RewindHitsInCounter(timeBinsToRewind);
    (*it).second->RewindHitsInCounter();
  }
}

//================================================================

void musrAnalysis::PrintHitsInAllCounters() {
  //  std::cout<<"___________________\n";
  for (counterMapType::const_iterator it = allCounterMap.begin(); it!=allCounterMap.end(); ++it) {
    (*it).second->myPrintThisCounter(eventID,0);
  }
}
//================================================================

void musrAnalysis::FillHistograms(Int_t iiiEntry) {
  //  std::cout<<"musrAnalysis::FillHistograms() event="<<eventID<<" , bool="<<generatedInfo<<","<<detectorInfo<<std::endl;
  oncePerEvent = true;
  Bool_t mCounterHitExistsForThisEventID = false;
  Bool_t pCounterHitExistsForThisEventID = false;
  Long64_t timeBinOfThePreviouslyProcessedHit = -100000000;
  Long64_t timeBin0                    = -100000000;
  Long64_t timeBin1                    = -100000000;
  Long64_t timeBin2                    = -100000000;
  Long64_t timeBinDoubleHit;
  Int_t    kEntry                      = 0;
  Int_t    posEntry                      = 0;
  Int_t    idetM                       = 0;
  Int_t    idetM_ID                    = 0;
  Double_t idetM_edep                  = 0.;
  Int_t    idetP                       = 0;
  Int_t    idetP_ID                    = 0;
  Double_t idetP_edep                  = 0.;
  //cks 4.10.2011  Bool_t   doubleHitM                  = false;
  //cks 4.10.2011  Bool_t   doubleHitP                  = false;

  //  std::cout<<" FillHistograms: timeBinOfThePreviouslyProcessedHit = "<<timeBinOfThePreviouslyProcessedHit<<std::endl;
  //  mCounterHitExistsForThisEventID = mCounter->GetNextGoodHitInThisEvent(eventID,timeBinOfThePreviouslyProcessedHit,0,'M',timeBin0,kEntry,idetM,idetM_ID,idetM_edep,doubleHitM);  
  //  mCounterHitExistsForThisEventID = mCounter->GetNextGoodMuon(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep,doubleHitM);
  //  mCounterHitExistsForThisEventID = MuonCounterHit(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep);
  mCounterHitExistsForThisEventID = mCounter->GetNextGoodMuon(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep);
  timeBinOfThePreviouslyProcessedHit = timeBin0;

  //___________________________________________________________ 

  do {
    //    std::cout<<"                                                   timeBin0 ="<<timeBin0<<std::endl;
    // Check whether there was good hit in the Positron counter
    //    Long64_t dataBinMin = (mCounterHitExistsForThisEventID) ? timeBin0+dataWindowBinMin : timeBinOfThePreviouslyProcessedHit-100000000;
    //    Long64_t dataBinMax = (mCounterHitExistsForThisEventID) ? timeBin0+dataWindowBinMax : timeBinOfThePreviouslyProcessedHit+100000000;
    detP_x = -1001.;
    detP_y = -1001.;
    detP_z = -1001.;
    detP_time_start = -1001.;
    detP_time_end   = -1001.;
    detP_theta = -1001.;
    detP_phi = -1001.;
    detP_phi_MINUS_pos_Phi = -1001.;
    detP_phi_MINUS_pos_Phi360 = -1001.;
    detP_theta_MINUS_pos_Theta = -1001.;
    detP_theta_MINUS_pos_Theta360 = -1001.;
    detP_time_start_MINUS_muDecayTime = -1001.;
    pileup_eventID             = -1001;
    pileup_muDecayDetID_double = -1001;
    pileup_muDecayPosX         = -1000000000;
    pileup_muDecayPosY         = -1000000000;
    pileup_muDecayPosZ         = -1000000000;
    pileup_muDecayPosR         = -1000000000;
    det_time31                 = -1001.;
    timeBinDoubleHit           = -100000000;
    doubleHit                  = false;
    pos_detID                  = -1.;
    pos_detID_doubleHit        = -1.;
    Int_t pos_detID_doubleHit_INT    = -1;
    if (mCounterHitExistsForThisEventID) {
      numberOfGoodMuons++;
      Long64_t dataBinMin = timeBin0+dataWindowBinMin;
      Long64_t dataBinMax = timeBin0+dataWindowBinMax;
      //      Long64_t positronBinMax = timeBin0+pileupWindowBinMax+dataWindowBinMin;
      Long64_t positronBinMax = dataBinMax-dataWindowBinMin;  // note that "dataWindowBinMin" is normally negative, i.e. positronBinMax > dataBinMax !!!
      pCounterHitExistsForThisEventID = PositronCounterHit(eventID,dataBinMin,dataBinMax,positronBinMax,timeBin1,timeBin2,timeBinDoubleHit,posEntry,idetP,idetP_ID,idetP_edep,pos_detID_doubleHit_INT);
      pos_detID = Double_t(idetP_ID);
      pos_detID_doubleHit = Double_t(pos_detID_doubleHit_INT);
      if (debugEventMap[eventID]>2) {
	if (pCounterHitExistsForThisEventID) {std::cout<<"FillHistograms: GOOD positron candidate found: timeBin1="<<timeBin1
						       <<",  deltat+800="<<timeBin1-timeBin0+800 <<std::endl;}
	else {std::cout<<"FillHistograms: NO positron candidate found"<<std::endl;}
      }
      if ((!pCounterHitExistsForThisEventID)&&(pos_detID_doubleHit_INT!=-1)&&mCounterHitExistsForThisEventID) {
	doubleHit = true;
	det_time31 = (timeBinDoubleHit-timeBin1)*tdcresolution;
	if (pos_detID_doubleHit==pos_detID) {std::cout<<"DOUBLEHIT: pos_detID_doubleHit="<<pos_detID_doubleHit<<"; pos_detID="<<pos_detID<<std::endl;}

      }
      //cDEL      if (pCounterHitExistsForThisEventID) std::cout<<"        timeBin1-timeBin2 = "<<timeBin1<<"-"<<timeBin2<<"="<<timeBin1-timeBin2 <<std::endl;
      //      
      if (pCounterHitExistsForThisEventID&&(posEntry>0)) {
	// Get variables of the detector hit corresponding to the positron candidate:
	fChain->GetEntry(posEntry); InitialiseEvent();
	Double_t detP_edep = det_edep[idetP];
	if (detP_edep!=idetP_edep) {std::cout<<"KIKS: detP_edep="<<detP_edep<<";   idetP_edep="<<idetP_edep<<";    idetP= "<<idetP<<std::endl; exit(1);}
	detP_x = det_x[idetP];
	detP_y = det_y[idetP];
	detP_z = det_z[idetP];
	detP_time_start = det_time_start[idetP];
	detP_time_end   = det_time_end[idetP];
	detP_phi = myAtan2(detP_y,detP_x) * 180./pi;
	detP_theta = myAtan2(sqrt(detP_y*detP_y+detP_x+detP_x),detP_z) * 180./pi;
	//
	if (pCounterHitExistsForThisEventID && (kEntry>0)&&(posEntry>0)&&(kEntry!=posEntry)) {
	  // This must be a pileup event (positron counter hit comes from the different event than the muon counter hit)
	  //	fChain->GetEntry(posEntry);
	  pileup_eventID             = eventID;
	  pileup_muDecayDetID_double = muDecayDetID;
	  pileup_muDecayPosX         = muDecayPosX;
	  pileup_muDecayPosY         = muDecayPosY;
	  pileup_muDecayPosZ         = muDecayPosZ;
	  pileup_muDecayPosR         = sqrt(muDecayPosX*muDecayPosX+muDecayPosY*muDecayPosY);
	  //	fChain->GetEntry(iiiEntry);
	}
	fChain->GetEntry(iiiEntry); InitialiseEvent();
      }
    }
    //    if (doubleHitP) {
    //      std::cout<<"yyyyyyyyyyyyyyyyy Total double hit nr="<<testIVar1++<<std::endl;
    //    }

    // CALCULATE VARIABLES
    // Initial muon
    muIniPosR = sqrt(muIniPosX*muIniPosX+muIniPosY*muIniPosY);
    muIniMomTrans = sqrt(muIniMomX*muIniMomX+muIniMomY*muIniMomY);
    muTargetPol_Theta = myAtan2(sqrt(muTargetPolX*muTargetPolX+muTargetPolY*muTargetPolY),muTargetPolZ) * 180./pi;
    muTargetPol_Theta360 = (muTargetPol_Theta<0) ? muTargetPol_Theta+360. : muTargetPol_Theta;
    muTargetPol_Phi   = myAtan2(muTargetPolY,muTargetPolX) * 180./pi;
    muTargetPol_Phi360=  (muTargetPol_Phi<0) ? muTargetPol_Phi+360. : muTargetPol_Phi;
    muDecayPol_Theta = myAtan2(sqrt(muDecayPolX*muDecayPolX+muDecayPolY*muDecayPolY),muDecayPolZ) * 180./pi;
    muDecayPol_Theta360 = (muDecayPol_Theta<0) ? muDecayPol_Theta+360. : muDecayPol_Theta;
    muDecayPol_Phi   = myAtan2(muDecayPolY,muDecayPolX) * 180./pi;
    muDecayPol_Phi360=  (muDecayPol_Phi<0) ? muDecayPol_Phi+360. : muDecayPol_Phi;
    // Initial positron
    pos_Trans_Momentum = sqrt(posIniMomX*posIniMomX+posIniMomY*posIniMomY);
    pos_Momentum = sqrt(pos_Trans_Momentum*pos_Trans_Momentum+posIniMomZ*posIniMomZ);
    pos_Radius = pos_Trans_Momentum/(-BFieldAtDecay_Bz)/0.3;
    pos_Theta = myAtan2(pos_Trans_Momentum,posIniMomZ) * 180./pi;
    pos_Theta360 = (pos_Theta<0) ? pos_Theta+360. : pos_Theta;
    pos_Phi      = myAtan2(posIniMomY,posIniMomX) * 180./pi;
    pos_Phi360   = (pos_Phi<0) ? pos_Phi+360. : pos_Phi;
    pos_Theta_MINUS_muDecayPol_Theta = deltaAngle(pos_Theta,muDecayPol_Theta);
    pos_Theta_MINUS_muDecayPol_Theta360 = (pos_Theta_MINUS_muDecayPol_Theta<0) ? pos_Theta_MINUS_muDecayPol_Theta+360 : pos_Theta_MINUS_muDecayPol_Theta;
    pos_Phi_MINUS_muDecayPol_Phi = deltaAngle(pos_Phi,muDecayPol_Phi);
    pos_Phi_MINUS_muDecayPol_Phi360 = (pos_Phi_MINUS_muDecayPol_Phi<0) ? pos_Phi_MINUS_muDecayPol_Phi+360 : pos_Phi_MINUS_muDecayPol_Phi;
    // Detector info
    det_m0edep = (mCounterHitExistsForThisEventID) ? idetM_edep : -1000.;
    //   det_time0 = timeBin0*tdcresolution;
    //    det_time1 = timeBin1*tdcresolution;
    det_time10 = ((timeBin0==-100000000)|| (timeBin1==-100000000)) ? -100000000 : (timeBin1-timeBin0)*tdcresolution;
    det_time20 = ((timeBin0==-100000000)|| (timeBin2==-100000000)) ? -100000000 : (timeBin2-timeBin0)*tdcresolution;
    //cks    if ((det_time10>-1)&&(det_time10<0.01)) {std::cout<<" eventID="<<eventID<<"   det_time10="<< det_time10<<std::endl;}
    gen_time10 = muDecayTime-muM0Time;
    det_time10_MINUS_gen_time10 = (det_time10 - gen_time10)/picosecond;
    det_posEdep = (pCounterHitExistsForThisEventID) ?  idetP_edep : -1000.;
    det_time1_MINUS_muDecayTime = (timeBin1*tdcresolution-muDecayTime)/picosecond;
    if ((detP_time_start>-998.)&&(muDecayTime>-998.)) detP_time_start_MINUS_muDecayTime = (detP_time_start - muDecayTime)/picosecond;
    detP_phi_MINUS_pos_Phi = deltaAngle(detP_phi,pos_Phi);
    //  std::cout<<"detP_phi_MINUS_pos_Phi="<<detP_phi_MINUS_pos_Phi<<std::endl;
    detP_phi_MINUS_pos_Phi360 = (detP_phi_MINUS_pos_Phi<0) ? detP_phi_MINUS_pos_Phi+360 : detP_phi_MINUS_pos_Phi;
    detP_theta_MINUS_pos_Theta = deltaAngle(detP_theta,pos_Theta);
    detP_theta_MINUS_pos_Theta360 = (detP_theta_MINUS_pos_Theta<0) ? detP_theta_MINUS_pos_Theta+360 : detP_theta_MINUS_pos_Theta;
    //    std::cout<<eventID<<"  det_time10="<<det_time10<<" t1="<<timeBin1*tdcresolution<<" t0="<<timeBin0*tdcresolution<<"  gen_time10="<<gen_time10<<std::endl;
    
    // CALCULATE CONDITIONS
    alwaysTrue = true;
    // Find whether the muon decayed in the sample;  the sample may consist of more than just one volume.  The ID
    // of the sample volume (or volumes) is defined in the *.v1190 file after the keyword "sampleID":
    muonDecayedInSample_gen = (find(SampleDetIDList.begin(), SampleDetIDList.end(), muDecayDetID)) != SampleDetIDList.end() ;
	muonDecayedInSampleOnce_gen = muonDecayedInSample_gen && oncePerEvent;
    muonTriggered_gen = muM0Time > -1000;
    muonTriggered_gen_AND_muonDecayedInSample_gen = muonTriggered_gen && muonDecayedInSample_gen;
    muonTriggered_det = mCounterHitExistsForThisEventID;
    positronHit_det   = pCounterHitExistsForThisEventID;
    goodEvent_det     = muonTriggered_det && positronHit_det;
    goodEvent_gen     = (muDecayTime>-999)&&(muM0Time>-999);
    goodEvent_det_AND_goodEvent_gen = goodEvent_det && goodEvent_gen;
    pileupEventCandidate = ((kEntry>0)&&(posEntry>0)&&(kEntry!=posEntry)) ? true:false;
    pileupEvent       = pileupEventCandidate&&goodEvent_det;
    goodEvent_det_AND_muonDecayedInSample_gen = goodEvent_det && muonDecayedInSample_gen;

     //    posCounterList_Iterator = find(F_posCounterList.begin(), F_posCounterList.end(), idetP_ID);    
     //    goodEvent_F_det   = posCounterList_Iterator != F_posCounterList.end()
    
    goodEvent_F_det = goodEvent_det && ( (find(F_posCounterList.begin(), F_posCounterList.end(), idetP_ID)) != F_posCounterList.end() );
    goodEvent_B_det = goodEvent_det && ( (find(B_posCounterList.begin(), B_posCounterList.end(), idetP_ID)) != B_posCounterList.end() );
    goodEvent_U_det = goodEvent_det && ( (find(U_posCounterList.begin(), U_posCounterList.end(), idetP_ID)) != U_posCounterList.end() );
    goodEvent_D_det = goodEvent_det && ( (find(D_posCounterList.begin(), D_posCounterList.end(), idetP_ID)) != D_posCounterList.end() );
    goodEvent_L_det = goodEvent_det && ( (find(L_posCounterList.begin(), L_posCounterList.end(), idetP_ID)) != L_posCounterList.end() );
    goodEvent_R_det = goodEvent_det && ( (find(R_posCounterList.begin(), R_posCounterList.end(), idetP_ID)) != R_posCounterList.end() );
    //    std::cout<<"goodEvent_F_det="<<goodEvent_F_det<<std::endl;
    if (pileupEvent&&goodEvent_F_det) {
      //      std::cout<<" DEBUG: Pileup Event: eventID = "<<eventID<<"     pileup_eventID = "<<pileup_eventID<<"     det_time10 = "<<det_time10<<std::endl;
      //      debugEventMap.insert(std::pair<int,int>(eventID,10));
    }
    
		// for better assignment of backgrounds versus useful signal
	goodEvent_F_det_AND_muonDecayedInSample_gen = goodEvent_F_det && muonDecayedInSample_gen;
	goodEvent_B_det_AND_muonDecayedInSample_gen = goodEvent_B_det && muonDecayedInSample_gen;
	goodEvent_U_det_AND_muonDecayedInSample_gen = goodEvent_U_det && muonDecayedInSample_gen;
	goodEvent_D_det_AND_muonDecayedInSample_gen = goodEvent_D_det && muonDecayedInSample_gen;
	goodEvent_L_det_AND_muonDecayedInSample_gen = goodEvent_L_det && muonDecayedInSample_gen;
	goodEvent_R_det_AND_muonDecayedInSample_gen = goodEvent_R_det && muonDecayedInSample_gen;

    goodEvent_F_det_AND_pileupEvent = goodEvent_F_det && pileupEvent;
    goodEvent_B_det_AND_pileupEvent = goodEvent_B_det && pileupEvent;
    goodEvent_U_det_AND_pileupEvent = goodEvent_U_det && pileupEvent;
    goodEvent_D_det_AND_pileupEvent = goodEvent_D_det && pileupEvent;
    goodEvent_L_det_AND_pileupEvent = goodEvent_L_det && pileupEvent;
    goodEvent_R_det_AND_pileupEvent = goodEvent_R_det && pileupEvent;

    promptPeak  = goodEvent_det &&  (det_time10>promptPeakWindowMin) && (det_time10<promptPeakWindowMax);
    promptPeakF = promptPeak && goodEvent_F_det;
    promptPeakB = promptPeak && goodEvent_B_det;
    promptPeakU = promptPeak && goodEvent_U_det;
    promptPeakD = promptPeak && goodEvent_D_det;
    promptPeakL = promptPeak && goodEvent_L_det;
    promptPeakR = promptPeak && goodEvent_R_det;


    //    if (bool_debugingRequired && muonTriggered_det) {
    //      std::cout<<"DEBUG: goodEvent_det: eventID="<<eventID<<std::endl;
    //      if (goodEvent_det) std::cout<<"    ___DETECTED___"<<std::endl;
    //      MyPrintTree();
    //      MyPrintConditions();
    //    }
    
    //    if (pileupEvent) {
    //      std::cout<<"\n NEW: pileupEvent: eventID="<<eventID<<",  kEntry="<<kEntry<<",  posEntry="<<posEntry<< std::endl;
    //      std::cout<<"det_time10 = "<<det_time10<<std::endl;
    //    }

    // Fill pileup-variables, but only if positron comes from different muon than the trigger signal
    //      if (mCounterHitExistsForThisEventID&&pCounterHitExistsForThisEventID)
    //      std::cout<<eventID<<" "<<mCounterHitExistsForThisEventID<<":  iiiEntry (trigger) = "<<iiiEntry<<"     kEntry (muon hit)="<<kEntry<<" "<<pCounterHitExistsForThisEventID<<"    posEntry (positron hit)="<<posEntry<<std::endl;    
    //    if (goodEvent_det) {
    //      std::cout<<"wwwwwwwwwwwwww \nEvent Nr: "<<eventID<<" muDecayTime="<< muDecayTime <<"  muM0Time="<<muM0Time<<"  det_time_start[idetP]="<<det_time_start[idetP]<<"   t1-t0="<<(timeBin1-timeBin0)*tdcresolution<<std::endl;
    //    }
    for (Int_t i=0; i<nrConditions; i++) { condition[i]=false; }
    //    MyPrintConditions();
    for (conditionMapType::const_iterator it = conditionMap.begin(); it!=conditionMap.end(); ++it) {
      int i = it->first;
      condition[i]=*(it->second);
	  if(condition[i]) { conditionCounter[i]++; }
    }
    
    // A special condition can me added here:
    // condition[29] = muIniMomZ>27;
    
    // Fill all "Detector Histograms"
    for(std::list<musrTH*>::const_iterator it = listOfAllHistograms1D.begin(); it != listOfAllHistograms1D.end(); ++it) {
      (*it)->FillTH1D(wght,&condition[0]);
    }
    for(std::list<musrTH*>::const_iterator it = listOfAllHistograms2D.begin(); it != listOfAllHistograms2D.end(); ++it) {
      (*it)->FillTH2D(wght,&condition[0]);
    }
    if ((goodEvent_det)&&(pCounterHitExistsForThisEventID)) {
      //	musrCounter* pCounterHitInThisEvent 
      counterMapType::const_iterator itPcounterHitInThisEvent = pCounterMap.find(idetP_ID);
      if (itPcounterHitInThisEvent==pCounterMap.end()) {
	std::cout<<" ERROR!  musrAnalysis:  itPcounterHitInThisEvent not found ==> This should never happen!!!"<<std::endl;
	std::cout<<"      idetP="<<idetP<<" idetP_ID="<<idetP_ID<< std::endl;
      }
      else{
	(itPcounterHitInThisEvent->second)->FillTDChistogram(float(timeBin1-timeBin0+overallBinDelay)+0.5,wght);
      }
    }

    //  Check whether there is a good trigger in the next event
    //    std::cout<<" FillHistograms: timeBinOfThePreviouslyProcessedHit = "<<timeBinOfThePreviouslyProcessedHit<<std::endl;
    //    mCounterHitExistsForThisEventID = mCounter->GetNextGoodHitInThisEvent(eventID,timeBinOfThePreviouslyProcessedHit,0,'M',timeBin0,kEntry,idetM,idetM_ID,idetM_edep,doubleHitM);
    //    mCounterHitExistsForThisEventID = mCounter->GetNextGoodMuon(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep,doubleHitM);
    //    mCounterHitExistsForThisEventID = MuonCounterHit(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep);
    mCounterHitExistsForThisEventID = mCounter->GetNextGoodMuon(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep);
    timeBinOfThePreviouslyProcessedHit = timeBin0;
    //    if (mCounterHitExistsForThisEventID) std::cout<<"   YYYYYYYYYYYYYYYYYYY check this : LOOOPING AGAIN"<<std::endl;
    oncePerEvent=false;				    
  }  while(mCounterHitExistsForThisEventID);

}

//================================================================
void musrAnalysis::FillHistogramsPulsed(Int_t iiiEntry1,Int_t iiiEntry2) {
  //  std::cout<<"musrAnalysis::FillHistograms() event="<<eventID<<" , bool="<<generatedInfo<<","<<detectorInfo<<std::endl;
	// edited for Pulsed Mode acquisition rather than fill "FillHistograms" with lots of if() statements
	// may also be useful for Integral Mode acquisition?
	// search for events iiiEntry1 to iiiEntry2 (maybe whole frame)
	// process "once per event" for all, regardless of whether there are hits
	//
	// det_time_10: time difference between "mean t0" and the positron counter hit (=measured), will include the muon's time of flight to the sample
	// gen_time_20: time from the muon entering the target to the positron event (-1000 for decay in flight, background, etc)
	// det_time_10_MINUS_gen_time_10 = position in pulse, after initial arrival transport
	// det_time_20 and gen_time_20 adjusted for phase shifts
	// pileup* not applicable
  oncePerEvent = true; // each incoming muon event, one randomly chosen detector hit if there are any at all but probably from a low numbered counter
  // Bool_t mCounterHitExistsForThisEventID = false;
  Int_t positronQuality = -1000;
  Long64_t timeBinOfThePreviouslyProcessedHit = -100000000;
  Long64_t timeBin0                    = -100000000;
  Long64_t timeBin1                    = -100000000;
  Long64_t timeBin2                    = -100000000;
  Long64_t timeBinDoubleHit;
  Int_t    kEntry                      = 0;
  Int_t    posEntry                      = 0;
  Int_t    idetM                       = 0;
  Int_t    idetM_ID                    = 0;
  Double_t idetM_edep                  = 0.;
  Int_t    idetP                       = 0;
  Int_t    idetP_ID                    = 0;
  Double_t idetP_edep                  = 0.;
  Int_t		idetFirstMulti				= -1000;
  Int_t bank_this = 0;
  Int_t bank_multi = 0;
  //cks 4.10.2011  Bool_t   doubleHitM                  = false;
  //cks 4.10.2011  Bool_t   doubleHitP                  = false;
//  typedef std::map<Int_t,Bool_t> unrecordedEventMap;
//  unrecordedEventMap unrecordedEvents;
//  unrecordedEventMap::const_iterator it2;

  typedef std::multimap<Long64_t,hitInfo*> sortedByEventMap;
  sortedByEventMap sortedByEvent;
  sortedByEventMap::const_iterator it3;
  hitInfo* theEvent;
  // new improved
  // start with empty multimap of hits (key=entry num)
  // loop through counters
  //  scan time order
  //   "copy" events to map
  // initialise iterator into sorted map
  // loop through event numbers in range
  //  if events to be processed then do them (1st=OnceOnly)
  //  else do OnceOnly with no hit
  //
  // hitInfo needs PositronQuality field (filled in during GetNextGoodPositronPulsed() and timeBin1 )
  // GetNextGoodPositronPulsed() now returns ptr to the hitInfo (or NULL)..?
  // std::cout << "starting FHP; map contents=" << sortedByEvent.size() << std::endl;
  for (counterMapType::const_iterator it = pCounterMap.begin(); it != pCounterMap.end(); ++it) {
	  Long64_t dataBinMin = dataWindowBinMin;
      Long64_t dataBinMax = dataWindowBinMax;
	  timeBin1                    = -100000000;
      timeBin2                    = -100000000;

	  do {
		  positronQuality = (it->second)->GetNextGoodPositronPulsed(iiiEntry1,iiiEntry2,dataBinMin,dataBinMax,timeBin1,timeBin2,posEntry,idetP,idetP_ID,idetP_edep,idetFirstMulti,theEvent);
		  if(positronQuality != -1000) {
			  // std::cout << "pushing event " << posEntry << " det " << idetP_ID << std::endl;
			  sortedByEvent.insert(std::pair<Long64_t,hitInfo*>(posEntry,theEvent));
		  }
	  } while (positronQuality != -1000);
  }

  it3=sortedByEvent.begin();
	// std::cout << "now processing sorted list" << std:: endl;

  //for(Int_t i=iiiEntry1; i<=iiiEntry2; i++) {
  //	  unrecordedEvents.insert(std::pair<Int_t,Bool_t>(i,true));
  //}

  //  std::cout<<" FillHistograms: timeBinOfThePreviouslyProcessedHit = "<<timeBinOfThePreviouslyProcessedHit<<std::endl;
  //  mCounterHitExistsForThisEventID = mCounter->GetNextGoodHitInThisEvent(eventID,timeBinOfThePreviouslyProcessedHit,0,'M',timeBin0,kEntry,idetM,idetM_ID,idetM_edep,doubleHitM);  
  //  mCounterHitExistsForThisEventID = mCounter->GetNextGoodMuon(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep,doubleHitM);
  //  mCounterHitExistsForThisEventID = MuonCounterHit(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep);
  // not needed (JSL) mCounterHitExistsForThisEventID = mCounter->GetNextGoodMuon(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep);
	//  timeBinOfThePreviouslyProcessedHit = timeBin0;
  //___________________________________________________________ 

	// loop through all good P counters and then one extra go with it==pCounterMap.end() to tidy up unrecorded events
    //  for (counterMapType::const_iterator it = pCounterMap.begin(); ; ++it) {
    for(Int_t iec=iiiEntry1; iec<=iiiEntry2; iec++) {
		oncePerEvent=true;
		fChain->GetEntry(iec); InitialiseEvent(); // now opening each event once only and in order! (as well as once for inserting hits in counters)

      //      positronQuality = (it->second)->GetNextGoodPositron(evID,dataBinMin,dataBinMax,tBin1,tBin2,kEntry,idetP,idetP_ID,idetP_edep);

    //    std::cout<<"                                                   timeBin0 ="<<timeBin0<<std::endl;
    // Check whether there was good hit in the Positron counter
    //    Long64_t dataBinMin = (mCounterHitExistsForThisEventID) ? timeBin0+dataWindowBinMin : timeBinOfThePreviouslyProcessedHit-100000000;
    //    Long64_t dataBinMax = (mCounterHitExistsForThisEventID) ? timeBin0+dataWindowBinMax : timeBinOfThePreviouslyProcessedHit+100000000;
    detP_x = -1001.;
    detP_y = -1001.;
    detP_z = -1001.;
    detP_time_start = -1001.;
    detP_time_end   = -1001.;
    detP_theta = -1001.;
    detP_phi = -1001.;
    detP_phi_MINUS_pos_Phi = -1001.;
    detP_phi_MINUS_pos_Phi360 = -1001.;
    detP_theta_MINUS_pos_Theta = -1001.;
    detP_theta_MINUS_pos_Theta360 = -1001.;
    detP_time_start_MINUS_muDecayTime = -1001.;
    pileup_eventID             = -1001;
    pileup_muDecayDetID_double = -1001;
    pileup_muDecayPosX         = -1000000000;
    pileup_muDecayPosY         = -1000000000;
    pileup_muDecayPosZ         = -1000000000;
    pileup_muDecayPosR         = -1000000000;
    det_time31                 = -1001.;
    timeBinDoubleHit           = -100000000;
    doubleHit                  = false;
    pos_detID                  = -1.;
    pos_detID_doubleHit        = -1.;
    Int_t pos_detID_doubleHit_INT    = -1;
    timeBin1                    = -100000000;
    timeBin2                    = -100000000;

	  Long64_t dataBinMin = dataWindowBinMin;
      Long64_t dataBinMax = dataWindowBinMax;
      //      Long64_t positronBinMax = timeBin0+pileupWindowBinMax+dataWindowBinMin;
      Long64_t positronBinMax = dataBinMax-dataWindowBinMin;  // note that "dataWindowBinMin" is normally negative, i.e. positronBinMax > dataBinMax !!!
	  // GetNextGoodPositronPulsed(Int_t evtID, Long64_t timeBinMin, Long64_t timeBinMax, Long64_t& timeBinOfNextGoodHit, Long64_t& timeBinOfNextGoodHit_phaseShifted, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep) {
 
	  while (oncePerEvent || positronQuality != -1000) {
		  // does the next hit in sortedByEvent belong to this?
		  if(it3!=sortedByEvent.end() && it3->first == iec) {
			  positronQuality=it3->second->posQual;
			  timeBin1=it3->second->timeBin1;
			  timeBin2=it3->second->timeBin2;
			  posEntry=it3->second->eventEntry;
			  idetP=it3->second->det_i;
			  idetP_ID=it3->second->det_id;
			  idetP_edep=it3->second->det_edep;
			  if(((it3->second)->multiHitCtr > 1) && (it3->second)->firstMulti) {
				  idetFirstMulti = (it3->second)->firstMulti->det_i;
			  } else {
				  idetFirstMulti = -1000; // not a second or subsequent multi hit, or not recorded for some reason
			  }
			  // std::cout << "processing hit in det " << idetP_ID << " for event " << iec << std::endl;
			  it3++;
		  } else {
			  if(!oncePerEvent) {
				  // std::cout << "run out of hits for event " << iec << std::endl;
				  break;
			  }
			  positronQuality=-1000;
			  posEntry=iec;
			  timeBin1=-100000000;
			  timeBin2=-100000000;
			  idetP=-1000;
			  idetP_ID=-1000;
			  idetP_edep=-1000;
			  idetFirstMulti=-1000;
			  // std::cout << "processing non-hit for event " << iec << std::endl;
		  }

//		  if(it == pCounterMap.end()) {
//			  while((it2 != unrecordedEvents.end()) && (!it2->second)) { it2++; }
//			  if(it2 == unrecordedEvents.end()) { break; }
//			  positronQuality=-1000;
//			  posEntry=it2->first;
//			  timeBin1=-1000;
//			  timeBin2=-1000;
//			  idetP=-1000;
//			  idetP_ID=-1000;
//			  idetP_edep=-1000;
//			  idetFirstMulti=-1000;
//			  // std::cout << "found entry " << posEntry << " with no hits" << std::endl;
//		  } else {
//			  if((positronQuality = (it->second)->GetNextGoodPositronPulsed(iiiEntry1,iiiEntry2,dataBinMin,dataBinMax,timeBin1,timeBin2,posEntry,idetP,idetP_ID,idetP_edep,idetFirstMulti)) == -1000 ) { break; }
//		      // std::cout << "found a hit in detector " << idetP_ID << " from event " << posEntry << " with quality=" << positronQuality << std::endl;
//		  }
//		  // std::cout << "searching entry " << iiiEntry << " and found a hit in detector " << idetP_ID << " with quality=" << positronQuality << std::endl;

//	  fChain->GetEntry(posEntry); InitialiseEvent();
//	  oncePerEvent=unrecordedEvents[posEntry];
//	  unrecordedEvents[posEntry] = false;

	  if (muTargetTime != -1000.0) {
		  timeBin0 = Long64_t(muTargetTime / tdcresolution);
	  } else {
		  timeBin0=-100000000;
	  }
      pos_detID = Double_t(idetP_ID);
      if (debugEventMap[eventID]>2) {
		std::cout<<"FillHistograms: GOOD positron candidate found: timeBin1="<<timeBin1
						       <<",  deltat+800="<<timeBin1-timeBin0+800 <<std::endl;}
	// Get variables of the detector hit corresponding to the positron candidate:
	  if(positronQuality != -1000) {
	Double_t detP_edep = det_edep[idetP];
	if (detP_edep!=idetP_edep) {std::cout<<"KIKS: detP_edep="<<detP_edep<<";   idetP_edep="<<idetP_edep<<";    idetP= "<<idetP<<std::endl; exit(1);}
	detP_x = det_x[idetP];
	detP_y = det_y[idetP];
	detP_z = det_z[idetP];
	detP_time_start = det_time_start[idetP];
	detP_time_end   = det_time_end[idetP];
	detP_phi = myAtan2(detP_y,detP_x) * 180./pi;
	detP_theta = myAtan2(sqrt(detP_y*detP_y+detP_x+detP_x),detP_z) * 180./pi;
	  } else {
		  detP_x = -1000;
		  detP_y = -1000;
		  detP_z = -1000;
		  detP_time_start = -1000;
		  detP_time_end   = -1000;
		  detP_phi = -1000;
		  detP_theta = -1000;
	  }
	//
    //    if (doubleHitP) {
    //      std::cout<<"yyyyyyyyyyyyyyyyy Total double hit nr="<<testIVar1++<<std::endl;
    //    }

    // CALCULATE VARIABLES
    // Initial muon
    muIniPosR = sqrt(muIniPosX*muIniPosX+muIniPosY*muIniPosY);
    muIniMomTrans = sqrt(muIniMomX*muIniMomX+muIniMomY*muIniMomY);
    muTargetPol_Theta = myAtan2(sqrt(muTargetPolX*muTargetPolX+muTargetPolY*muTargetPolY),muTargetPolZ) * 180./pi;
    muTargetPol_Theta360 = (muTargetPol_Theta<0) ? muTargetPol_Theta+360. : muTargetPol_Theta;
    muTargetPol_Phi   = myAtan2(muTargetPolY,muTargetPolX) * 180./pi;
    muTargetPol_Phi360=  (muTargetPol_Phi<0) ? muTargetPol_Phi+360. : muTargetPol_Phi;
    muDecayPol_Theta = myAtan2(sqrt(muDecayPolX*muDecayPolX+muDecayPolY*muDecayPolY),muDecayPolZ) * 180./pi;
    muDecayPol_Theta360 = (muDecayPol_Theta<0) ? muDecayPol_Theta+360. : muDecayPol_Theta;
    muDecayPol_Phi   = myAtan2(muDecayPolY,muDecayPolX) * 180./pi;
    muDecayPol_Phi360=  (muDecayPol_Phi<0) ? muDecayPol_Phi+360. : muDecayPol_Phi;
    // Initial positron
    pos_Trans_Momentum = sqrt(posIniMomX*posIniMomX+posIniMomY*posIniMomY);
    pos_Momentum = sqrt(pos_Trans_Momentum*pos_Trans_Momentum+posIniMomZ*posIniMomZ);
    pos_Radius = pos_Trans_Momentum/(-BFieldAtDecay_Bz)/0.3;
    pos_Theta = myAtan2(pos_Trans_Momentum,posIniMomZ) * 180./pi;
    pos_Theta360 = (pos_Theta<0) ? pos_Theta+360. : pos_Theta;
    pos_Phi      = myAtan2(posIniMomY,posIniMomX) * 180./pi;
    pos_Phi360   = (pos_Phi<0) ? pos_Phi+360. : pos_Phi;
    pos_Theta_MINUS_muDecayPol_Theta = deltaAngle(pos_Theta,muDecayPol_Theta);
    pos_Theta_MINUS_muDecayPol_Theta360 = (pos_Theta_MINUS_muDecayPol_Theta<0) ? pos_Theta_MINUS_muDecayPol_Theta+360 : pos_Theta_MINUS_muDecayPol_Theta;
    pos_Phi_MINUS_muDecayPol_Phi = deltaAngle(pos_Phi,muDecayPol_Phi);
    pos_Phi_MINUS_muDecayPol_Phi360 = (pos_Phi_MINUS_muDecayPol_Phi<0) ? pos_Phi_MINUS_muDecayPol_Phi+360 : pos_Phi_MINUS_muDecayPol_Phi;
    // Detector info
    // det_m0edep = (mCounterHitExistsForThisEventID) ? idetM_edep : -1000.;
    //   det_time0 = timeBin0*tdcresolution;
    //    det_time1 = timeBin1*tdcresolution;
    det_time10 = (timeBin1==-100000000) ? -1000.0 : timeBin1*tdcresolution;
    det_time20 = (timeBin2==-100000000) ? -1000.0 : timeBin2*tdcresolution;
    //cks    if ((det_time10>-1)&&(det_time10<0.01)) {std::cout<<" eventID="<<eventID<<"   det_time10="<< det_time10<<std::endl;}
	  if (muTargetTime != -1000.0) {
		  gen_time10 = muDecayTime-muTargetTime;
		  det_time10_MINUS_gen_time10 = (det_time10 - gen_time10)/picosecond;
	  } else {
		  gen_time10 = -1000.0;
		  det_time10_MINUS_gen_time10 = -1000.0;
	  }
    det_posEdep =   idetP_edep ;
    det_time1_MINUS_muDecayTime = (timeBin1*tdcresolution-muDecayTime)/picosecond;
    if ((detP_time_start>-998.)&&(muDecayTime>-998.)) detP_time_start_MINUS_muDecayTime = (detP_time_start - muDecayTime)/picosecond;

	detP_phi_MINUS_pos_Phi = deltaAngle(detP_phi,pos_Phi);
    //  std::cout<<"detP_phi_MINUS_pos_Phi="<<detP_phi_MINUS_pos_Phi<<std::endl;
    detP_phi_MINUS_pos_Phi360 = (detP_phi_MINUS_pos_Phi<0) ? detP_phi_MINUS_pos_Phi+360 : detP_phi_MINUS_pos_Phi;
    detP_theta_MINUS_pos_Theta = deltaAngle(detP_theta,pos_Theta);
    detP_theta_MINUS_pos_Theta360 = (detP_theta_MINUS_pos_Theta<0) ? detP_theta_MINUS_pos_Theta+360 : detP_theta_MINUS_pos_Theta;
    //    std::cout<<eventID<<"  det_time10="<<det_time10<<" t1="<<timeBin1*tdcresolution<<" t0="<<timeBin0*tdcresolution<<"  gen_time10="<<gen_time10<<std::endl;
    
    // CALCULATE CONDITIONS
    alwaysTrue = true;
    // Find whether the muon decayed in the sample;  the sample may consist of more than just one volume.  The ID
    // of the sample volume (or volumes) is defined in the *.v1190 file after the keyword "sampleID":
    muonDecayedInSample_gen = (find(SampleDetIDList.begin(), SampleDetIDList.end(), muDecayDetID)) != SampleDetIDList.end() ;
	muonDecayedInSampleOnce_gen = muonDecayedInSample_gen && oncePerEvent;
    // muonTriggered_gen = muM0Time > -1000;
    // muonTriggered_gen_AND_muonDecayedInSample_gen = muonTriggered_gen && muonDecayedInSample_gen;
    // muonTriggered_det = mCounterHitExistsForThisEventID;
    // positronHit_det   = pCounterHitExistsForThisEventID;
    goodEvent_det     = (positronQuality >= -1);
	if(goodEvent_det) numberOfGoodMuons++;
    goodEvent_gen     = (positronQuality== -3 || positronQuality== -2 || positronQuality==0 || positronQuality==1);
    goodEvent_det_AND_goodEvent_gen = goodEvent_det && goodEvent_gen;
    // pileupEventCandidate = ((kEntry>0)&&(posEntry>0)&&(kEntry!=posEntry)) ? true:false;
    // pileupEvent       = pileupEventCandidate&&goodEvent_det;
    goodEvent_det_AND_muonDecayedInSample_gen = goodEvent_det && muonDecayedInSample_gen;

     //    posCounterList_Iterator = find(F_posCounterList.begin(), F_posCounterList.end(), idetP_ID);    
     //    goodEvent_F_det   = posCounterList_Iterator != F_posCounterList.end()
    
	Bool_t event_F = ( (find(F_posCounterList.begin(), F_posCounterList.end(), idetP_ID)) != F_posCounterList.end() );
	Bool_t event_B = ( (find(B_posCounterList.begin(), B_posCounterList.end(), idetP_ID)) != B_posCounterList.end() );
	Bool_t event_U = ( (find(U_posCounterList.begin(), U_posCounterList.end(), idetP_ID)) != U_posCounterList.end() );
	Bool_t event_D = ( (find(D_posCounterList.begin(), D_posCounterList.end(), idetP_ID)) != D_posCounterList.end() );
	Bool_t event_L = ( (find(L_posCounterList.begin(), L_posCounterList.end(), idetP_ID)) != L_posCounterList.end() );
	Bool_t event_R = ( (find(R_posCounterList.begin(), R_posCounterList.end(), idetP_ID)) != R_posCounterList.end() );

	bank_this = (event_F ? 1:0) + (event_B ? 2:0) + (event_U ? 3:0) + (event_D ? 4:0) + (event_L ? 5:0) + (event_R ? 6:0) ;

	if(idetFirstMulti >= 0) {
		det_multi_interval = det_time_start[idetP]-det_time_start[idetFirstMulti];
		pos_detID_doubleHit = det_ID[idetFirstMulti];
		// pos_doubleHit_dPhi: difference in detector phases between this and the first multi hit, plus 10*first-bank + 60*second-bank
		bank_multi =	(( (find(F_posCounterList.begin(), F_posCounterList.end(), det_ID[idetFirstMulti])) != F_posCounterList.end() ) ? 1:0) +
						(( (find(B_posCounterList.begin(), B_posCounterList.end(), det_ID[idetFirstMulti])) != B_posCounterList.end() ) ? 2:0) +
						(( (find(U_posCounterList.begin(), U_posCounterList.end(), det_ID[idetFirstMulti])) != U_posCounterList.end() ) ? 3:0) +
						(( (find(D_posCounterList.begin(), D_posCounterList.end(), det_ID[idetFirstMulti])) != D_posCounterList.end() ) ? 4:0) +
						(( (find(L_posCounterList.begin(), L_posCounterList.end(), det_ID[idetFirstMulti])) != L_posCounterList.end() ) ? 5:0) +
						(( (find(R_posCounterList.begin(), R_posCounterList.end(), det_ID[idetFirstMulti])) != R_posCounterList.end() ) ? 6:0);
		pos_doubleHit_dPhi= deltaAngle(phaseShiftMap[det_ID[idetFirstMulti]]*180/pi,phaseShiftMap[idetP_ID]*180/pi) + bank_this*1000 + bank_multi*6000;
	} else {
		det_multi_interval = -1000;
		pos_detID_doubleHit = -1000;
		pos_doubleHit_dPhi= -1000;
	}

    goodEvent_F_det = goodEvent_det && event_F;
    goodEvent_B_det = goodEvent_det && event_B;
    goodEvent_U_det = goodEvent_det && event_U;
    goodEvent_D_det = goodEvent_det && event_D;
    goodEvent_L_det = goodEvent_det && event_L;
    goodEvent_R_det = goodEvent_det && event_R;
	// for better assignment of backgrounds versus useful signal
	goodEvent_F_det_AND_muonDecayedInSample_gen = goodEvent_F_det && muonDecayedInSample_gen;
	goodEvent_B_det_AND_muonDecayedInSample_gen = goodEvent_B_det && muonDecayedInSample_gen;
	goodEvent_U_det_AND_muonDecayedInSample_gen = goodEvent_U_det && muonDecayedInSample_gen;
	goodEvent_D_det_AND_muonDecayedInSample_gen = goodEvent_D_det && muonDecayedInSample_gen;
	goodEvent_L_det_AND_muonDecayedInSample_gen = goodEvent_L_det && muonDecayedInSample_gen;
	goodEvent_R_det_AND_muonDecayedInSample_gen = goodEvent_R_det && muonDecayedInSample_gen;

	//    std::cout<<"goodEvent_F_det="<<goodEvent_F_det<<std::endl;

    promptPeak  = goodEvent_det &&  (det_time10>promptPeakWindowMin) && (det_time10<promptPeakWindowMax);
    promptPeakF = promptPeak && goodEvent_F_det;
    promptPeakB = promptPeak && goodEvent_B_det;
    promptPeakU = promptPeak && goodEvent_U_det;
    promptPeakD = promptPeak && goodEvent_D_det;
    promptPeakL = promptPeak && goodEvent_L_det;
    promptPeakR = promptPeak && goodEvent_R_det;

	pileupEvent = (positronQuality > -1000 && positronQuality<=-2); // a good positron missed because of dead time
    goodEvent_F_det_AND_pileupEvent = pileupEvent && event_F;
    goodEvent_B_det_AND_pileupEvent = pileupEvent && event_B;
    goodEvent_U_det_AND_pileupEvent = pileupEvent && event_U;
    goodEvent_D_det_AND_pileupEvent = pileupEvent && event_D;
    goodEvent_L_det_AND_pileupEvent = pileupEvent && event_L;
    goodEvent_R_det_AND_pileupEvent = pileupEvent && event_R;

	doubleHitEvent_gen = (positronQuality>1 || (positronQuality>-1000 && positronQuality<-3)); // any double hits
	doubleHit = (positronQuality>1); // counted double hits only
	goodEvent_F_det_AND_doubleHit = doubleHit && event_F;
	goodEvent_B_det_AND_doubleHit = doubleHit && event_B;
	goodEvent_U_det_AND_doubleHit = doubleHit && event_U;
	goodEvent_D_det_AND_doubleHit = doubleHit && event_D;
	goodEvent_L_det_AND_doubleHit = doubleHit && event_L;
	goodEvent_R_det_AND_doubleHit = doubleHit && event_R;

	singleHitEvent_gen = (positronQuality==0);

	stackedEvent_gen = (positronQuality == -1);

    //    if (bool_debugingRequired && muonTriggered_det) {
    //      std::cout<<"DEBUG: goodEvent_det: eventID="<<eventID<<std::endl;
    //      if (goodEvent_det) std::cout<<"    ___DETECTED___"<<std::endl;
    //      MyPrintTree();
    //      MyPrintConditions();
    //    }
    
    //    if (pileupEvent) {
    //      std::cout<<"\n NEW: pileupEvent: eventID="<<eventID<<",  kEntry="<<kEntry<<",  posEntry="<<posEntry<< std::endl;
    //      std::cout<<"det_time10 = "<<det_time10<<std::endl;
    //    }

    // Fill pileup-variables, but only if positron comes from different muon than the trigger signal
    //      if (mCounterHitExistsForThisEventID&&pCounterHitExistsForThisEventID)
    //      std::cout<<eventID<<" "<<mCounterHitExistsForThisEventID<<":  iiiEntry (trigger) = "<<iiiEntry<<"     kEntry (muon hit)="<<kEntry<<" "<<pCounterHitExistsForThisEventID<<"    posEntry (positron hit)="<<posEntry<<std::endl;    
    //    if (goodEvent_det) {
    //      std::cout<<"wwwwwwwwwwwwww \nEvent Nr: "<<eventID<<" muDecayTime="<< muDecayTime <<"  muM0Time="<<muM0Time<<"  det_time_start[idetP]="<<det_time_start[idetP]<<"   t1-t0="<<(timeBin1-timeBin0)*tdcresolution<<std::endl;
    //    }
    for (Int_t i=0; i<nrConditions; i++) { condition[i]=false; }
    //    MyPrintConditions();
    for (conditionMapType::const_iterator it = conditionMap.begin(); it!=conditionMap.end(); ++it) {
      int i = it->first;
      condition[i]=*(it->second);
	  if(condition[i]) { conditionCounter[i]++; }
    }
    
    // A special condition can me added here:
    // condition[29] = muIniMomZ>27;
    
    // Fill all "Detector Histograms"
    for(std::list<musrTH*>::const_iterator it = listOfAllHistograms1D.begin(); it != listOfAllHistograms1D.end(); ++it) {
      (*it)->FillTH1D(wght,&condition[0]);
    }
    for(std::list<musrTH*>::const_iterator it = listOfAllHistograms2D.begin(); it != listOfAllHistograms2D.end(); ++it) {
      (*it)->FillTH2D(wght,&condition[0]);
    }
	// JSL don't plot the individual histograms - too many of them!
//    if ((goodEvent_det)&&(pCounterHitExistsForThisEventID)) {
      //	musrCounter* pCounterHitInThisEvent 
//      counterMapType::const_iterator itPcounterHitInThisEvent = pCounterMap.find(idetP_ID);
//      if (itPcounterHitInThisEvent==pCounterMap.end()) {
//	std::cout<<" ERROR!  musrAnalysis:  itPcounterHitInThisEvent not found ==> This should never happen!!!"<<std::endl;
//	std::cout<<"      idetP="<<idetP<<" idetP_ID="<<idetP_ID<< std::endl;
//      }
//      else{
//	(itPcounterHitInThisEvent->second)->FillTDChistogram(float(timeBin1-timeBin0+overallBinDelay)+0.5,wght);
//      }
//    }

    //  Check whether there is a good trigger in the next event
    //    std::cout<<" FillHistograms: timeBinOfThePreviouslyProcessedHit = "<<timeBinOfThePreviouslyProcessedHit<<std::endl;
    //    mCounterHitExistsForThisEventID = mCounter->GetNextGoodHitInThisEvent(eventID,timeBinOfThePreviouslyProcessedHit,0,'M',timeBin0,kEntry,idetM,idetM_ID,idetM_edep,doubleHitM);
    //    mCounterHitExistsForThisEventID = mCounter->GetNextGoodMuon(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep,doubleHitM);
    //    mCounterHitExistsForThisEventID = MuonCounterHit(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep);
    // mCounterHitExistsForThisEventID = mCounter->GetNextGoodMuon(eventID,timeBinOfThePreviouslyProcessedHit,timeBin0,kEntry,idetM,idetM_ID,idetM_edep);
    timeBinOfThePreviouslyProcessedHit = timeBin1;
    //    if (mCounterHitExistsForThisEventID) std::cout<<"   YYYYYYYYYYYYYYYYYYY check this : LOOOPING AGAIN"<<std::endl;
    oncePerEvent=false;				    
  }; // loop through possible hits
  };// loop through events

}

//================================================================
void musrAnalysis::InitialiseEvent() {
   runID_double        = runID;
   eventID_double      = eventID;
   muDecayDetID_double = muDecayDetID;
   det_n_double        = det_n;
   muDecayPosR         = sqrt(muDecayPosX*muDecayPosX+muDecayPosY*muDecayPosY);

   wght                = ( fChain->GetBranch("weight") ) ? weight : 1.;
}

//================================================================
Double_t musrAnalysis::PreprocessEvent(Long64_t iEn) {
  //  std::cout<<"musrAnalysis::PreprocessEvent()"<<std::endl;
	Int_t multiCtr;
  fChain->GetEntry(iEn); InitialiseEvent();

  // Clone some channels into different one, if requested by user
  // (This is usefull when e.g. user splits a signal from a veto
  // and uses it in two different ways - e.g. once for vetoing 
  // muons, and second (with a different threshold) for validating
  // a positron candidate.  This is initiated by the 
  // keyword "CLONECHANNEL" in the *.v1190 file
  

  if (bool_clonedChannelsMultimap_NotEmpty) {
    //   std::cout<<"det_n="<<det_n<<std::endl;
    Int_t det_n_OLD=det_n;
    for (Int_t i=0; i<det_n_OLD; i++) {
      //      std::cout<<"   det_ID["<<i<<"]="<<det_ID[i]<<"  edep="<<det_edep[i]<<std::endl;
      clonedChannelsMultimapType::const_iterator it = clonedChannelsMultimap.find(det_ID[i]);
      //      std::cout<<"              clonedChannelsMultimap[i]="<<clonedChannelsMultimap[i]<<std::endl;
      if (it!=clonedChannelsMultimap.end()) {
	
	int chNumTMP = it->first;
	//	std::cout<<"DEBUG:  chNumTMP="<<chNumTMP<<"  ";
	std::pair<clonedChannelsMultimapType::const_iterator,clonedChannelsMultimapType::const_iterator> ret = clonedChannelsMultimap.equal_range(chNumTMP);
	for (clonedChannelsMultimapType::const_iterator ittt=ret.first; ittt!=ret.second; ++ittt) {
	  //	  std::cout << "  ittt->second=" << ittt->second;
	  int chNumNewTMP = ittt->second;
	  det_ID[det_n]            = chNumNewTMP;
          det_edep[det_n]          = det_edep[i];
          det_edep_el[det_n]       = det_edep_el[i];
	  det_edep_pos[det_n]      = det_edep_pos[i];
          det_edep_gam[det_n]      = det_edep_gam[i];
          det_edep_mup[det_n]      = det_edep_mup[i];
	  det_nsteps[det_n]        = det_nsteps[i];
          det_length[det_n]        = det_length[i];
          det_time_start[det_n]    = det_time_start[i];
	  det_time_end[det_n]      = det_time_end[i];
          det_x[det_n]             = det_x[i];
          det_y[det_n]             = det_y[i];
          det_z[det_n]             = det_z[i];
	  det_kine[det_n]          = det_kine[i];
          det_VrtxKine[det_n]      = det_VrtxKine[i];
          det_VrtxX[det_n]         = det_VrtxX[i];
	  det_VrtxY[det_n]         = det_VrtxY[i];
          det_VrtxZ[det_n]         = det_VrtxZ[i];
          det_VrtxVolID[det_n]     = det_VrtxVolID[i];
	  det_VrtxProcID[det_n]    = det_VrtxProcID[i];
	  det_VrtxTrackID[det_n]   = det_VrtxTrackID[i];
	  det_VrtxParticleID[det_n]= det_VrtxParticleID[i];
	  det_VvvKine[det_n]       = det_VvvKine[i];
          det_VvvX[det_n]          = det_VvvX[i];
	  det_VvvY[det_n]          = det_VvvY[i];
	  det_VvvZ[det_n]          = det_VvvZ[i];
	  det_VvvVolID[det_n]      = det_VvvVolID[i];
	  det_VvvProcID[det_n]     = det_VvvProcID[i];
	  det_VvvTrackID[det_n]    = det_VvvTrackID[i];
	  det_VvvParticleID[det_n] = det_VvvParticleID[i];
	  det_n++;
	  
	}
      }
    }
  }

  //  std::cout<<"\n musrAnalysis::PreprocessEvent() Filling event "<<eventID<<std::endl;
  //  MyPrintTree();
  Double_t globTime;
  if(musrMode=='P') {
	  globTime=muIniTime * muonPulseWidthFactor; // JSL: use time within pulse
  } else { // continuous
	  globTime= nextUnfilledEventTime;
  }
  multiCtr=0; // JSL: number the above-threshold Good counts as 0 (single) or 1,2 etc (multi); small ones all -1
  // must insert events in time order for correct labelling of multiple hits as "first", "second", etc
  // use multimap to sort them. Sorting by GEANT raw time is OK here
  doubleHitSorterType doubleHitSorter;

  for (Int_t i=0; i<det_n; i++) {
	    doubleHitSorter.insert(std::pair<double,int>(det_time_start[i],i));
  }
  for(doubleHitSorterType::const_iterator dhi = doubleHitSorter.begin(); dhi != doubleHitSorter.end(); dhi++) {
	  Int_t i=dhi->second;
    // //    Int_t detID=det_ID[i];
    std::map<int,musrCounter*>::iterator it;
    it = allCounterMap.find(det_ID[i]);
    if (it==allCounterMap.end()) {
       std::cout<<"Active detector with det_ID="<<det_ID[i]<<" not found !!!"<<std::endl;
    }
    else {
      //      Double_t omega = 851.372*fieldNomVal[0];
      Double_t dPhaseShift = (omega==0) ? 0:phaseShiftMap[det_ID[i]]/omega;

      Double_t t1 = (globTime+det_time_start[i]            )/tdcresolution;
      Double_t t2 = (globTime+det_time_start[i]+dPhaseShift)/tdcresolution;
      //      }
      Long64_t timeBin  = Long64_t(t1);
      Long64_t timeBin2 = Long64_t(t2);

      (*it).second->FillHitInCounter(det_edep[i],timeBin,timeBin2,iEn,eventID,i,det_ID[i],eventID, multiCtr);
	  // std::cout << "pushed event into counter "<<det_ID[i]<<" resulting in multiCtr="<<multiCtr<<std::endl;
    }
  }

  //  std::cout<<"lastPreprocessedEntry+1="<<lastPreprocessedEntry+1<<"   iEn="<<iEn<<"   eventID="<<eventID<<std::endl;
  if ((lastPreprocessedEntry+1)!=iEn) {std::cout<<"ERROR PreprocessEvent - should never happen!!!"<<std::endl; }
  lastPreprocessedEntry = iEn;
  if(iEn<firstPreprocessedEntry) { firstPreprocessedEntry=iEn; } 
  return timeToNextEvent*muonRateFactor;
}

//================================================================
//Bool_t musrAnalysis::MuonCounterHit(Int_t evID, Long64_t timeBinMin, Long64_t& timeBin0, Int_t& kEntry, Int_t& idet, Int_t& idetID, Double_t& idetEdep) {
//  Bool_t mCounterHitCanditateExists = mCounter->GetNextGoodMuon(evID,timeBinMin,timeBin0,kEntry,idet,idetID,idetEdep);
//  if (!mCounterHitCanditateExists) return false;
//  // Check for other muons within the pileup window:
//  if ( mCounter->CheckForPileupMuons(timeBin0,kEntry) )  return false;  // This muon candidate is killed due to a double hit rejection.
//  return true;
//}
//
//================================================================
Bool_t musrAnalysis::PositronCounterHit(Int_t evID, Long64_t dataBinMin, Long64_t dataBinMax, Long64_t positronBinMax, Long64_t& tBin1, Long64_t& tBin2, Long64_t& tBinDoubleHit, Int_t& kEntry, Int_t& idetP, Int_t& idetP_ID, Double_t& idetP_edep, Int_t& idetP_ID_doubleHit) {

  if (bool_debugingRequired) {
    if (debugEventMap[eventID]>4) {std::cout<<"PositronCounterHit:  pCounterMap.size()="<<pCounterMap.size()<<std::endl;}
  }
  if (pCounterMap.empty()) return false;

  //  Bool_t positronHitFound  = false;
  Bool_t goodPositronFound = false;
  Int_t positronQuality = 0;
  //  Int_t prev_counterID = 0;
  //  Long64_t prev_time = 0;
  //  std::cout<<"Debug 10------------------------------"<<std::endl;
  if (musrMode=='D') {
    // Loop over all positron counters
    for (counterMapType::const_iterator it = pCounterMap.begin(); it!=pCounterMap.end(); ++it) {
      //      positronQuality = (it->second)->GetNextGoodPositron(evID,dataBinMin,dataBinMax,tBin1,tBin2,kEntry,idetP,idetP_ID,idetP_edep);
      positronQuality = (it->second)->GetNextGoodPositron(evID,dataBinMin,positronBinMax,tBin1,tBin2,kEntry,idetP,idetP_ID,idetP_edep);
      if (positronQuality==3) {  // double hit was found in the same counter
	if (debugEventMap[eventID]>3) {std::cout<<"PositronCounterHit:  positron candidate killed - double hit in the same counter"<<std::endl;}
	return false;
      }
      if (positronQuality==2) {
	if (goodPositronFound) {
	  if (debugEventMap[eventID]>3) {std::cout<<"PositronCounterHit:  positron candidate killed - double hit in a different counter"<<std::endl;}
	  //	  std::cout<<"("<<evID<<") Double hit in counters "<<idetP_ID<<", "<<prev_counterID<<";  time bins ="<<tBin1<<", "<<prev_time<<std::endl;
	  //	  if (idetP_ID_doubleHit == idetP_ID) std::cout<<"DOUBLEHIT  idetP_ID_doubleHit = "<<idetP_ID_doubleHit<<";  idetP_ID ="<< idetP_ID<<std::endl;
	  return false;  // double hit was found in a different counter
	}
	goodPositronFound = true;
	tBinDoubleHit = tBin1;
	idetP_ID_doubleHit = idetP_ID;
	//	prev_counterID = idetP_ID;
	//	prev_time = tBin1;
      }
    }



    //    if (goodPositronFound)    return true;
    if (goodPositronFound&&(tBin1<dataBinMax))    return true;
  }
  //  std::cout<<"Debug 110"<<std::endl;
  idetP_ID_doubleHit = -1;
  return false;
}

//================================================================
void musrAnalysis::CopySubstring(char* inputChar, int iStart, int iEnd, char* outputChar) {
  int iiiEnd = std::min(iEnd,(int)strlen(inputChar));
  int lastChar=0;
  for (Int_t i=0; i<=(iiiEnd-iStart); i++) {
    outputChar[i]=inputChar[i+iStart];
    lastChar=i;
  }
  outputChar[lastChar+1]='\0';
  //  std::cout<<"iStart="<<iStart<<"  iiiEnd="<<iiiEnd<<std::endl;
}

//================================================================
void musrAnalysis::MyPrintTree() {
  std::cout<<"==========\n det_n="<<det_n<<std::endl;
  for (Int_t jj=0; jj<det_n; jj++) {
    std::cout<<"\t det_ID="<<det_ID[jj]<<"\t det_edep="<<det_edep[jj]<<"\t det_time_start="<<det_time_start[jj]<<std::endl;
  }
}

//================================================================
void musrAnalysis::MyPrintConditions() {
  std::cout<<"CONDITION NR = ";
  for (conditionMapType::const_iterator it = conditionMap.begin(); it!=conditionMap.end(); ++it) {
    std::cout<<"  "<<it->first;
  }
  std::cout<<std::endl<<"               ";
  for (conditionMapType::const_iterator it = conditionMap.begin(); it!=conditionMap.end(); ++it) {
    std::cout<<"  "<<*(it->second);
  }
  std::cout<<std::endl;
}
//================================================================
Double_t musrAnalysis::myAtan2(Double_t y, Double_t x) {
  if ( (y<-999.99999) && (y>-1000.00001) ) return -1000.;
  if ( (x<-999.99999) && (x>-1000.00001) ) return -1000.;
  if ( (x==0) && (y==0) ) return -1000.;
  return atan2(y,x);
}
//================================================================
Double_t musrAnalysis::deltaAngle(Double_t alpha, Double_t beta) {
  // Calculates the difference between angle alpha and beta.
  // The angles alpha and beta are in degrees.
  // The difference will be in the range of (-180,180> degrees.
  if ((alpha<-998.)||(beta<-998.)) {return -1001.;}   // one of the angles was undefined;
  Double_t delta = alpha - beta;
  if (delta<=-180) {delta+=360;}
  else {if (delta>180)  delta-=360;}
  return delta;
}
//================================================================
