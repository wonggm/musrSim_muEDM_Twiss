#ifndef musrTH_h
#define musrTH_h 1
#include <TApplication.h>
#include <TSystem.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <iostream>
//#include <musrAnalysis.hh>
#include <map>
#include <list>

class musrAnalysis;

class musrTH {
public:
  typedef std::map<Int_t,std::string> humanDecayMapType;
  typedef std::multimap<Int_t,Int_t> humanDecayMultimapType;

  musrTH(const char* dimension, const char* histoName, const char* histoTitle, Int_t nrOfBinsX, Double_t minBoundaryX, Double_t maxBoundaryX, Int_t nrOfBinsY, Double_t minBoundaryY, Double_t maxBoundaryY, Double_t* variableName1, Double_t* variableName2);
  ~musrTH();
  void FillTH1D(Double_t vaha, Bool_t* cond);
  void FillTH2D(Double_t vaha, Bool_t* cond);
  void DrawTH1D(Option_t* option, Int_t idHist);
  void DrawTH1DdrawList(Option_t* option);
  void DrawTH2D(Option_t* option, Int_t idHist);
  void DrawTH2DdrawList(Option_t* option);
  void SetDrawListOfHistograms(int i);
  Bool_t IsThisHistoNamed(char* someName) {return (strcmp(someName,histogramName)==0);}
  Double_t GetBinContent1D(Int_t i, Int_t jBin);
  void SetBinLabel1D(Int_t iBin,std::string slabel);
  //  void FillHumanDecayArray(musrTH* decayMapHistos, const int nBins, const int* iBins);
  void FillHumanDecayArray(musrTH* decayMapHistos, humanDecayMapType myMap, humanDecayMultimapType myMultimap);
  //  TH1D** GetHistArray1D(std::string& varToBeFilled) {varToBeFilled = variableToBeFilled_X; return histArray1D;}
  TH1D** GetHistArray1D() {return histArray1D;}
  //  TH2D** GetHistArray2D() {return histArray2D;}
  Int_t GetXmin1D();
  Int_t GetXmax1D();
  Int_t GetNbinsX1D();
  Int_t GetNbinsX2D();
  Int_t GetNbinsY2D();
  void  AssignFunction(TF1* function, char* functOption, Double_t xMin, Double_t xMax);
  void  FitHistogramsIfRequired(Double_t omega);
  void  SetRotatingReferenceFrame(Double_t frequency, Double_t phase) {bool_rotating_reference_frame=true; 
                                                                       rot_ref_frequency=frequency; rot_ref_phase=phase;}
  void  SetExpDecayCorrection() {bool_exp_decay_correction=true;}
  void  ListHistograms();

private:
  char   histogramName[501];
  TH1D** histArray1D;
  TH2D** histArray2D;
  Double_t* variableToBeFilled_X;
  Double_t* variableToBeFilled_Y;
  std::list<int> drawList;
  TF1*   funct;
  Double_t funct_xMin, funct_xMax;
  char   funct_option[100];
  Bool_t bool_rotating_reference_frame;
  Double_t rot_ref_frequency, rot_ref_phase;
  Bool_t bool_exp_decay_correction;
  //  Double_t N0_FromLastFit;
  //  Double_t BinWidth_FromLastFit;
  //  Double_t funct_p0, funct_p1, funct_p2, funct_p3, funct_p4, funct_p5, funct_p6, funct_p7, funct_p8, funct_p9;
};

#endif
