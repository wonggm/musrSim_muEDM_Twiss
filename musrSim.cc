#include "musrDetectorConstruction.hh"
#include "musrPhysicsList.hh"
#include "musrPrimaryGeneratorAction.hh"
#include "musrRunAction.hh"
#include "musrEventAction.hh"
#include "musrStackingAction.hh"
#include "musrSteppingAction.hh"
#include "musrSteppingVerbose.hh"

#include "G4RunManager.hh"
#include "G4UImanager.hh"
// #include "G4UIterminal.hh"
// #ifdef G4UI_USE_TCSH
// #include "G4UItcsh.hh"  //JSL: should be commented on windows ?
// #endif
#include "G4UIExecutive.hh"

// why need this ? 
// I can delete all the #ifndef...#define... around the visManager related codes
#ifndef G4VIS_USE
#define G4VIS_USE
#endif
//#include <TApplication.h>
//#include <TSystem.h


// The following two lines are needed to cope with the problem of 
// "Error in <TPluginManager::FindHandler>: Cannot find plugin handler for TVirtualStreamerInfo! 
//               Does $ROOTSYS/etc/plugins/TVirtualStreamerInfo exist?"
#include "TROOT.h"
#include "TPluginManager.h"

#include "Randomize.hh"

// #include <X11/Xlib.h>  //JSL

#ifdef G4VIS_USE
  #include "G4VisExecutive.hh"
  #include "G4TrajectoryDrawByCharge.hh"    // TS Trajectory drawing by ID or charge
#endif

#include "musrRootOutput.hh"
#include "musrParameters.hh"
#include "musrErrorMessage.hh"
//#include "F04GlobalField.hh"

int main(int argc,char** argv) {

  //  XInitThreads();
  G4cout<<"\n\n**************************My musrSim + (my+)GM PGA **************************"<<G4endl;
  // G4cout<<" musrSim version 1.0.5 for Geant4.10.3, released on 20 Mar 2017"<<G4endl;
  // G4cout<<"      WWW:  https://www.psi.ch/lmu/geant4-simulations"<<G4endl;
  // G4cout<<"\nnote: this musrSim is modified and compiled with: " << G4endl;
  // G4cout<<" geant4 10.6.2 (conda-forge) \ngcc (conda-forge gcc 10.4.0-19) 10.4.0 on WSL2-Ubuntu 20.04"<<G4endl;
  G4cout<<" \nnote: this version is under test for twiss parameters"<<G4endl;
  G4cout<<" \n       this version should be taken with care, alot of bugs"<<G4endl;
  G4cout<<" \n       pls read printout from the PGA"<<G4endl;
  G4cout<<" \n       1. when setting twiss to 0 0 0 Tmatrix give not pos-def error"<<G4endl;
  G4cout<<" \n       2. what if my gun direction is along x or y ??? "<<G4endl;
  G4cout<<" \n       3. the musrSim contains GM's stuff, not yet understood like rnd, beam_offset etc."<<G4endl;
  G4cout<<" \n       solved - 4. use twiss interferes with vertex x,y."<<G4endl;
  G4cout<<" \n       solved - we use geometrical emmitance so getting momentum is skipped 5. p0 has to be given before /gun/use/twiss"<<G4endl;
  G4cout<<" \n       solved - 6. unexplained scaling factors at setting position and momentum !!!!"<<G4endl;
  // choose the Random engine
  //  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);   // the /musr/run/randomOption 2 does not work with RanecuEngine
  CLHEP::HepRandom::setTheEngine(new CLHEP::HepJamesRandom);

  //my Verbose output class
  G4VSteppingVerbose::SetInstance(new musrSteppingVerbose);
  
  // Run manager
  G4RunManager * runManager = new G4RunManager;

  // Create class "myParameters", which is a collection of many different parameters
  G4String steeringFileName=argv[1];
  musrParameters* myParameters = new musrParameters(steeringFileName);
  
  // GM stuff
  G4double beam_offset = 0.0;
  G4int rngSeed = 0;
  G4double Rcut = 0.0;
  G4double beam_zpos = 0.0;
  G4double beam_angle = 0.0;
    // GM stuff

  // Create class "musrErrorMessage"
  musrErrorMessage* myErrorMessage = new musrErrorMessage();

  //  TApplication* myapp=new TApplication("myapp",0,0);
  
  // Create Root class for storing the output of the Geant simulation
  musrRootOutput* myRootOutput = new musrRootOutput();

// The following command is needed to cope with the problem of 
// "Error in <TPluginManager::FindHandler>: Cannot find plugin handler for TVirtualStreamerInfo! 
//               Does $ROOTSYS/etc/plugins/TVirtualStreamerInfo exist?"
//  /* magic line from Rene - for future reference! */
          gROOT->GetPluginManager()->AddHandler("TVirtualStreamerInfo",
                                                "*",
                                                "TStreamerInfo",
                                                "RIO",
                                                "TStreamerInfo()");


  // UserInitialization classes (mandatory)
  //  musrDetectorConstruction* musrdetector = new musrDetectorConstruction;
  if (argc>1) {
    G4int myRunNr=atoi(argv[1]);       // Get the run number from the name of the 
                                       // parameter file, if it starts with a number.
    if (myRunNr>0)  {runManager->SetRunIDCounter(myRunNr);}
    //    musrdetector->SetInputParameterFileName(argv[1]);
  }
  musrDetectorConstruction* musrdetector = new musrDetectorConstruction(steeringFileName);
  runManager->SetUserInitialization(musrdetector);
  runManager->SetUserInitialization(new musrPhysicsList);
  
#ifdef G4VIS_USE
  // Visualization, if you choose to have it!
  G4VisManager* visManager = new G4VisExecutive;    // TS Trajectory drawing by ID or charge
  visManager->Initialize();
#endif
   
  // UserAction classes
  // GM Stuff: ,beam_offset ...
  runManager->SetUserAction(new musrPrimaryGeneratorAction(musrdetector, beam_offset, rngSeed, Rcut, beam_zpos, beam_angle));
  runManager->SetUserAction(new musrRunAction);  
  runManager->SetUserAction(new musrEventAction);
  // Initiate musrStackingAction only if optical photons are required (otherwise not needed)
  if (musrParameters::boolG4OpticalPhotons) runManager->SetUserAction(new musrStackingAction);
  runManager->SetUserAction(new musrSteppingAction);

  //Initialize G4 kernel
  runManager->Initialize();
      
  //get the pointer to the User Interface manager 
  G4UImanager * UI = G4UImanager::GetUIpointer();   
  
  // run the macro
  G4String command = "/control/execute ";
  G4String fileName = argv[1]; 
  // batch mode by default. 
  // the visualizations are not interactive (eg: VRML)
  // caveat: using batch mode with OGL in the macro will give unexpected results.
  if (argc==2) { 
    UI->ApplyCommand(command+fileName);
    }
    
  else if (argc==3) { 
      // use this if you want interactive visualization
      // the VRML still work, but wont output anything on the screen (this is how it works)
    G4String SecondArgument = argv[2];
    if (SecondArgument=="vis") {
        G4cout<<" Using OGL event display (with GUI) ..."<<G4endl;
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        UI->ApplyCommand(command+fileName);
        ui->SessionStart();
        delete ui;
      }
    else { G4cout<< "invalid argument. we only construct your detector." << G4endl;}
    }
    
  else { G4cout<< "invalid number of arguments. we only construct your detector." << G4endl;}
    
// preview version, you have to give the visOGL.mac as another input, which is not so convenient
//   // batch mode by default. 
//   // the visualizations are not interactive (eg: VRML)
//   G4String command = "/control/execute ";
//   G4String fileName = argv[1];
//   UI->ApplyCommand(command+fileName);

//   // if you want interactive event display ...
//   if (argc==3) { 
//     G4String SecondArgument = argv[2];
//     if (SecondArgument=="visOGL.mac") {
//         G4cout<<"using OGL event display..."<<G4endl;
//         G4UIExecutive* ui = new G4UIExecutive(argc, argv);
//         UI->ApplyCommand(command+SecondArgument);
//         ui->SessionStart();
//         delete ui;
//       }
//     };


  //  myapp->Run(kTRUE);

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete myRootOutput;
  delete myErrorMessage;
  delete myParameters;
  // cks  runManager->SetVerboseLevel(2);   // This line can help debug crashes during the runManager delete
  delete runManager;
  //  F04GlobalField* myGlobalField = F04GlobalField::getObject();
  //  if (myGlobalField!=NULL)  {delete myGlobalField;}

  return 0;
}



