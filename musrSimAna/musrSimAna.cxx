#include "musrAnalysis.hh"
// ROOT libraries:
#include <TApplication.h>
#include <TSystem.h>
#include <iostream>
#include <fstream>

int main(int argc,char** argv) {
  //  if (strcmp(argv[2],"nographics")!=0) {
    //  TApplication class is used for the ROOT graphical interface
  TApplication* myapp=new TApplication("myapp",0,0);
  //  std::cout<<"1 argc = "<<argc<<std::endl;
  // cks 2009.09.08  TApplication* myapp=new TApplication("myapp",&argc,argv);
    //  }
  //  std::cout<<"2 argc = "<<argc<<std::endl;
  if (argc==1) {
    std::cout<<"\n This program analyses the Root tree and creates some histograms as an output."<<std::endl;
    std::cout<<" Usage:\n       musrSimAna N [numberOfEventsToAnalyse] [nographics] [pileup]"<<std::endl;
    std::cout<<"\n where: N is the string indicating the input root tree (i.e. hifidata/musr_N.root)"<<std::endl;
    std::cout<<"        nographics indicates that the histograms will be not displayed."<<std::endl;
    std::cout<<std::endl;
    //    system("ls");
    std::ifstream check("file.ext");
    
    if ( ! check ) { std::cout << "file does not exist"<<std::endl;}
    return 0;
  }

  //   Open the Root file  
  char rootFileName[500];
  //  sprintf(rootFileName,"hifidata/musr_%s.root",argv[1]);
  sprintf(rootFileName,"data/musr_%s.root",argv[1]);
  //  std::cout<<"rootFileName="<<rootFileName<<std::endl;

  TFile* f = new TFile(rootFileName);
  if (f==NULL) {
    std::cout<<"\n Error!!!  File \""<<rootFileName<<"\" not found!    ==>  Stop!"<<std::endl;
    return 1;
  }

  char v1190FileName[500];
  sprintf(v1190FileName,"%s.v1190",argv[2]);
  //  TFile* fff = new TFile(v1190FileName);
  //  if (fff==NULL) {
  //    std::cout<<"\n Error!!!  File \""<<v1190FileName<<"\" not found!    ==>  Stop!"<<std::endl;
  //    return 1;
  //  }
  
  //   Load the tree
  TTree* tree = (TTree*)gDirectory->Get("t1");
  if (tree==NULL) {
    std::cout<<"\n Error!!!  Tree \"t1\" not found!    ==>  Stop!"<<std::endl;
    return 1;
  }


  //   Prepare the directory for the pictures
  //  char pictureDirectoryName[500]; char mkdircommand[506];
  //  sprintf(pictureDirectoryName,"/home/sedlak/muSR_Gen/pict_%s",argv[1]);
  //  sprintf(mkdircommand,"mkdir /home/sedlak/muSR_Gen/pict_%s",argv[1]);
  //  std::ifstream check(pictureDirectoryName);
  //  if ( ! check ) { 
  //    system(mkdircommand);
  //    std::cout << "file "<<pictureDirectoryName<<" created"<<std::endl;
  //  }

  musrAnalysis m(tree);
  //  m.displayGraphs = false;
  if (argc>4) {
    for (int i=3; i<argc; i++) {
      //      if      (strcmp(argv[i],"pileup")==0)          m.includePileupBgr = true;
      //      else if (strcmp(argv[i],"graphs")==0)          m.displayGraphs = true;
      //      //      else if (strcmp(argv[i],"noactivecollim")==0)  m.useActiveCollimator = false;     
    }
  }
  //  std::cout<<"m.includePileupBgr = "<<m.includePileupBgr
  //	   <<"   m.displayGraphs = "<<m.displayGraphs
  //    //	   <<"  m.useActiveCollimator = "<<m.useActiveCollimator
  //	   <<std::endl;


  //  m.fMusrRoot=f;
  //  m.boolTimingTreeExists = bTimingTreeExists;
  //  m.SetFriendTreeTimingFileName(friendTreeTimingFileNameTMP);
  //  m.SetFriendTreePileupFileName(friendTreePileupFileNameTMP);
  Int_t nrEvents=0; if (argc>3) nrEvents=atoi(argv[3]);
  std::cout<<" before loop"<<std::endl;
  m.Loop(argv[1],v1190FileName,nrEvents);     ////////,mPileupPos);
  std::cout<<" after loop"<<std::endl;

  //  //  if (argc<4) {
  //  std::cout<<" before myapp->Run()"<<std::endl;
  //  //  if (m.displayGraphs)  
  //  myapp->Run(kTRUE);
  //  std::cout<<" after myapp->Run()"<<std::endl;


  if ((argc>=4)&&(strcmp(argv[3],"nographic")==0))  {;}
  else {
    std::cout<<" argc = "<<argc<<"   argv[3] = \""<<argv[3]<<"\""<<std::endl;
    myapp->Run(kTRUE);
  }
  std::cout<<" after myapp->Run()"<<std::endl;
  //  }
  return 0;
}
