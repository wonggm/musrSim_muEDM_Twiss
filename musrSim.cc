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
#include "G4UIterminal.hh"
#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh" //JSL: should be commented on windows ?
#endif

#ifndef G4VIS_USE
#define G4VIS_USE
// #einfif (#endif ?)
#endif
// #include <TApplication.h>
// #include <TSystem.h>

// The following two lines are needed to cope with the problem of
// "Error in <TPluginManager::FindHandler>: Cannot find plugin handler for TVirtualStreamerInfo!
//               Does $ROOTSYS/etc/plugins/TVirtualStreamerInfo exist?"
#include "TROOT.h"
#include "TPluginManager.h"

#include "Randomize.hh"

// #include <X11/Xlib.h>  //JSL

#ifdef G4VIS_USE
// #include "musrVisManager.hh"
#include "G4VisExecutive.hh"
#include "G4TrajectoryDrawByCharge.hh" // TS Trajectory drawing by ID or charge
#endif

#include "musrRootOutput.hh"
#include "musrParameters.hh"
#include "musrErrorMessage.hh"
// #include "F04GlobalField.hh"

void printHelp(G4double beam_offset, G4int rngSeed, G4double Rcut, G4double beam_zpos, G4double beam_angle
// , G4bool doBacktrack
);

int main(int argc, char **argv)
{

  //  XInitThreads();
  G4cout << "\n\n*************************************************************" << G4endl;
  G4cout << " musrSim version 1.0.5 for Geant4.10.3, released on 20 Mar 2017" << G4endl;
  G4cout << "      WWW:  https://www.psi.ch/lmu/geant4-simulations" << G4endl;
  // choose the Random engine
  //  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);   // the /musr/run/randomOption 2 does not work with RanecuEngine
  CLHEP::HepRandom::setTheEngine(new CLHEP::HepJamesRandom);

  // my Verbose output class
  G4VSteppingVerbose::SetInstance(new musrSteppingVerbose);

  // Run manager
  G4RunManager *runManager = new G4RunManager;

  // Create class "myParameters", which is a collection of many different parameters
  G4String steeringFileName = argv[1];
  musrParameters *myParameters = new musrParameters(steeringFileName);
  G4double beam_offset = 0.0;
  G4int rngSeed = 0;
  G4double Rcut = 0.0;
  G4double beam_zpos = 0.0;
  G4double beam_angle = 0.0;
  // G4bool doBacktrack = false;

  // printHelp(beam_offset, rngSeed);

  // Create class "musrErrorMessage"
  musrErrorMessage *myErrorMessage = new musrErrorMessage();

  //  TApplication* myapp=new TApplication("myapp",0,0);

  // Create Root class for storing the output of the Geant simulation
  musrRootOutput *myRootOutput = new musrRootOutput();

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
  if (argc > 1)
  {
    G4int myRunNr = atoi(argv[1]); // Get the run number from the name of the
                                   // parameter file, if it starts with a number.
    if (myRunNr > 0)
    {
      runManager->SetRunIDCounter(myRunNr);
    }
    //    musrdetector->SetInputParameterFileName(argv[1]);
  }
  musrDetectorConstruction *musrdetector = new musrDetectorConstruction(steeringFileName);
  // musrPrimaryGeneratorAction *musrpga = new musrPrimaryGeneratorAction(beam_offset,
  //                                                                      rngSeed);
  runManager->SetUserInitialization(musrdetector);
  runManager->SetUserInitialization(new musrPhysicsList);

#ifdef G4VIS_USE
  // Visualization, if you choose to have it!
  //  G4VisManager* visManager = new musrVisManager;
  G4VisManager *visManager = new G4VisExecutive; // TS Trajectory drawing by ID or charge
  visManager->Initialize();
#endif

  // UserAction classes
  runManager->SetUserAction(new musrPrimaryGeneratorAction(musrdetector, beam_offset, rngSeed, Rcut, beam_zpos, beam_angle
  // ,doBacktrack
  )
  );
  // runManager->SetUserAction(musrpga);
  runManager->SetUserAction(new musrRunAction);
  runManager->SetUserAction(new musrEventAction);
  // Initiate musrStackingAction only if optical photons are required (otherwise not needed)
  if (musrParameters::boolG4OpticalPhotons)
    runManager->SetUserAction(new musrStackingAction);
  runManager->SetUserAction(new musrSteppingAction);

  // Initialize G4 kernel
  runManager->Initialize();

  // get the pointer to the User Interface manager
  G4UImanager *UI = G4UImanager::GetUIpointer();

  if (argc == 1)
  // Define (G)UI terminal for interactive mode
  {
    // G4UIterminal is a (dumb) terminal.
    G4UIsession *session = 0;
#ifdef G4UI_USE_TCSH
    session = new G4UIterminal(new G4UItcsh);
#else
    session = new G4UIterminal();
#endif

    UI->ApplyCommand("/control/execute vis.mac");
    session->SessionStart();
    delete session;
  }
  else
  // Batch mode
  {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UI->ApplyCommand(command + fileName);
    if (argc > 2)
    {
      G4String SecondArgument = argv[2];
      if (SecondArgument == "idle")
      {
        G4UIsession *session = 0;
#ifdef G4UI_USE_TCSH
        session = new G4UIterminal(new G4UItcsh);
#else
        session = new G4UIterminal();
#endif
        G4cout << "Go to idle state now:" << G4endl;
        session->SessionStart();
        delete session;
      }
    }
  }

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