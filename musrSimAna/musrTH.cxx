#include "musrTH.hh"
#include "musrAnalysis.hh"
#include "TCanvas.h"

musrTH::musrTH(const char* dimension, const char* histoName, const char* histoTitle, Int_t nrOfBinsX, Double_t minBoundaryX, Double_t maxBoundaryX, Int_t nrOfBinsY, Double_t minBoundaryY, Double_t maxBoundaryY, Double_t* variableName1, Double_t* variableName2) {
//musrTH::musrTH(char* dimension, char* histoName, char* histoTitle, Int_t nrOfBins, Double_t minBoundary, Double_t maxBoundary, Double_t* variableName1, Double_t* varibleName2) {
  std::cout<<" Defining "<<dimension<<" histogram "<<histoName<<" \""<<histoTitle<<"\" "
           <<nrOfBinsX<<" "<<minBoundaryX<<" "<<maxBoundaryX<<" "
	   <<nrOfBinsY<<" "<<minBoundaryY<<" "<<maxBoundaryY<<" "
	   <<variableName1<<" "<<variableName2<<std::endl;

  strcpy(histogramName,histoName);
  variableToBeFilled_X = variableName1;
  variableToBeFilled_Y = variableName2;
  funct = NULL;
  bool_rotating_reference_frame=false;
  bool_exp_decay_correction=false;
  rot_ref_frequency=0;
  rot_ref_phase=0;
  strcpy(funct_option,"");
  //  std::cout<<"hojhoj    variableName1="<<variableName1<<"   variableToBeFilled_X="<<variableToBeFilled_X<<std::endl;

  char nameHist[500];
  if (dimension[0]=='2') {
    histArray2D=new TH2D*[musrAnalysis::nrConditions];
    for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
      sprintf(nameHist,"%s_%d",histoName,i);
      TH2D *hTemp = new TH2D(nameHist,histoTitle,nrOfBinsX,minBoundaryX,maxBoundaryX,nrOfBinsY,minBoundaryY,maxBoundaryY);
      hTemp->Sumw2();
      histArray2D[i] = hTemp;
      //           std::cout<<"histogram hTemp defined, name="<<nameHist<<"\t pointer="<<hTemp<<std::endl;
    }
  }
  else {
    histArray1D=new TH1D*[musrAnalysis::nrConditions];
    for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
      sprintf(nameHist,"%s_%d",histoName,i);
      TH1D *hTemp = new TH1D(nameHist,histoTitle,nrOfBinsX,minBoundaryX,maxBoundaryX);
      hTemp->Sumw2();
      histArray1D[i] = hTemp;
      //            std::cout<<"histogram hTemp defined, name="<<nameHist<<"\t pointer="<<hTemp<<std::endl;
    }
  }
}

//==============================================================================================
void musrTH::FillTH1D(Double_t vaha, Bool_t* cond){
  //  std::cout<<"Filling histograms  histArray1D="<<histArray1D<<std::endl;
  for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
    if (bool_rotating_reference_frame) {
      //      Double_t var = *variableToBeFilled_X;
      Double_t waha = vaha*exp((*variableToBeFilled_X)/2.19703)*cos(2*pi*rot_ref_frequency*(*variableToBeFilled_X)+rot_ref_phase);
      //      std::cout<<"rot_ref_frequency="<<rot_ref_frequency<<std::endl;
      if (cond[i])  histArray1D[i]->Fill(*variableToBeFilled_X,waha);
    }
    else if (bool_exp_decay_correction) {
      Double_t waha = vaha*exp((*variableToBeFilled_X)/2.19703);
      if (cond[i])  histArray1D[i]->Fill(*variableToBeFilled_X,waha);
    }
    else {
      if (cond[i])  histArray1D[i]->Fill(*variableToBeFilled_X,vaha);
    }
  }
}
//==============================================================================================
void musrTH::FillTH2D(Double_t vaha, Bool_t* cond){
  //  std::cout<<"Filling histograms  histArray2D="<<histArray2D<<" variableToBeFilled_X="<<variableToBeFilled_X<< " variableToBeFilled_Y="<<variableToBeFilled_Y<<std::endl;
  for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
    if (cond[i])  histArray2D[i]->Fill(*variableToBeFilled_X,*variableToBeFilled_Y,vaha);
  }
}
//==============================================================================================
void musrTH::DrawTH1D(Option_t* option, Int_t idHist) {
  std::cout<<"canvas created in DrawTH1D"<<std::endl;
  char canvasName[501]; sprintf(canvasName,"%s_%d",histogramName,idHist); canvasName[0]='c';
  TCanvas* tempC = new TCanvas(canvasName,canvasName);
  tempC->cd(); tempC->Update();
  char chopt[1000];
  strcpy(chopt,option);
  Int_t i_first=0, i_last=musrAnalysis::nrConditions;  if (idHist>=0) {i_first=idHist; i_last=idHist+1;}
  for (Int_t i=i_first; i<i_last; i++) {
    if ((i==1)&&(i_first!=1)) strcat(chopt,"same");
    //    std::cout<<"histo nr."<<i<<"\t chopt="<<chopt<<std::endl;
    //    if ((idHist!=-1)||(drawCond[i])) histArray1D[i]->Draw(chopt);
    if (idHist!=-1) {
      //  std::cout<<"\t\t\t musrTH::DrawTH1D: i="<<i<<", pointer="<<histArray1D[i]<<std::endl; 
      histArray1D[i]->Draw(chopt);
    }
  }
}
//==============================================================================================
void musrTH::DrawTH2D(Option_t* option, Int_t idHist) {
  std::cout<<"canvas created in DrawTH2D"<<std::endl;
  TCanvas* tempC = new TCanvas();
  char chopt[1000];
  strcpy(chopt,option);
  Int_t i_first=0, i_last=musrAnalysis::nrConditions;  if (idHist>=0) {i_first=idHist; i_last=idHist+1;}
  for (Int_t i=i_first; i<i_last; i++) {
    if ((i==1)&&(i_first!=1)) strcat(chopt,"same");
    //    std::cout<<"histo nr."<<i<<"\t chopt="<<chopt<<std::endl;
    if (idHist!=-1) histArray2D[i]->Draw(chopt);
  }
}

//==============================================================================================
void musrTH::SetDrawListOfHistograms(int i) {
  if ((i>=0)&&(i<musrAnalysis::nrConditions)) {
    drawList.push_back(i);
  }
  if (i<0) {
    for (int j=0; j<musrAnalysis::nrConditions; j++) {
      drawList.push_back(j);
    }
  }
}

//==============================================================================================
void musrTH::DrawTH1DdrawList(Option_t* option) {
  //  std::cout<<"========== BEGINNING OF DRAW ==========="<<std::endl;
  //  ListHistograms();
  for(std::list<int>::const_iterator it = drawList.begin(); it != drawList.end(); ++it) {
    int iii = *it;
    std::cout<<"     Drawing histog"<<iii<<std::endl;
    DrawTH1D(option, iii);
  }  
  //  std::cout<<"========== END OF DRAW ==========="<<std::endl;
}

//==============================================================================================
void musrTH::DrawTH2DdrawList(Option_t* option) {
  for(std::list<int>::const_iterator it = drawList.begin(); it != drawList.end(); ++it) {
    int iii = *it;
    DrawTH2D(option, iii);
  }  
}
//==============================================================================================
void musrTH::SetBinLabel1D(Int_t iBin,std::string slabel) {
  for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
    histArray1D[i]->GetXaxis()->SetBinLabel(iBin,slabel.c_str());
  }
}
//==============================================================================================
Double_t musrTH::GetBinContent1D(Int_t i, Int_t jBin) {
  // std::cout<<"musrTH::GetBinContent1D: i="<<i<<"  jBin="<<jBin<<std::endl;
  TH1D* tmpHist = histArray1D[i];
  Int_t iBinNr = tmpHist->FindBin(float(jBin));
  Double_t value = tmpHist->GetBinContent(iBinNr);
  //  std::cout<<"value="<<value<<std::endl;
  return value;
}
//==============================================================================================
Int_t musrTH::GetXmin1D() {
  return int(histArray1D[0]->GetXaxis()->GetXmin());
}
//==============================================================================================
Int_t musrTH::GetXmax1D() {
  return int(histArray1D[0]->GetXaxis()->GetXmax());
}
//==============================================================================================
Int_t musrTH::GetNbinsX1D() {
  return histArray1D[0]->GetNbinsX();
}
//==============================================================================================
Int_t musrTH::GetNbinsX2D() {
  return histArray2D[0]->GetNbinsX();
}
//==============================================================================================
Int_t musrTH::GetNbinsY2D() {
  return histArray2D[0]->GetNbinsY();
}
//==============================================================================================
void musrTH::FillHumanDecayArray(musrTH* decayMapHistos, humanDecayMapType myMap, humanDecayMultimapType myMultimap) {
  //  Int_t iHumanBinForAllTheRest=-1;
  for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
    //    oldString="blaBlaoldStringUndefiNED";
    //    Int_t k=0;
    for (humanDecayMultimapType::const_iterator it = myMultimap.begin(); it!=myMultimap.end(); ++it) {
      Int_t iHumanBin      = it->first;
      Int_t iDecayHistoBin = it->second;
      //      if (iDecayHistoBin==-123456789) {iHumanBinForAllTheRest=iHumanBin; continue;}
      Double_t value  = decayMapHistos->GetBinContent1D(i,iDecayHistoBin);
      Double_t value2 = histArray1D[i]-> GetBinContent(iHumanBin);
      //decayMapHistos->GetBinContent1D(i,iDecayHistoBin);
      histArray1D[i]-> SetBinContent(iHumanBin,value+value2);
    }

    //    //    if (iHumanBinForAllTheRest != -1) {
    //    for (Int_t j=1; j<= (decayMapHistos->GetNbinsX1D()); j++) {
    for (Int_t j=(decayMapHistos->GetXmin1D()); j<= (decayMapHistos->GetXmax1D()); j++) {
      Double_t value  = decayMapHistos->GetBinContent1D(i,j);
      Bool_t thisBinWasAssigned=false;
      if (value!=0) {
    	for (humanDecayMultimapType::const_iterator it = myMultimap.begin(); it!=myMultimap.end(); ++it) {
    	  Int_t iDecayHistoBin = it->second;
    	  if (iDecayHistoBin==j)  thisBinWasAssigned=true;
    	}
      }
      if ((!thisBinWasAssigned)&&(value!=0)&&(j!=-1001)) {     // in case of pileup histo, -1001 is assigned 
                                            // if there was no pileup muon.  Avoid printing error in this case.
	std::cout<<"musrHT.cxx:  "<<value<<" muons stoped and decayed in detector no. "<<j
		 <<", but this bin is not assigned to humanDecayHistogram "<<i<<std::endl;
      //    	Double_t value2 = histArray1D[i]-> GetBinContent(XXXX);
      }      
    }


  }
}
//==============================================================================================
void  musrTH::AssignFunction(TF1* function, char* functOption, Double_t xMin, Double_t xMax) {
  funct = function;
  strcpy(funct_option,functOption);
  funct_xMin = xMin;
  funct_xMax = xMax;
  std::cout<<"musrTH::AssignFunction: "<<funct->GetName()<<", option="<<funct_option<<", xMin="<<xMin<<", xMax="<<xMax<<std::endl;

}
//==============================================================================================
void  musrTH::ListHistograms() {
  std::cout<<"musrTH::ListHistograms():"<<std::endl;
  for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
    std::cout<<"\t NAME="<<histogramName<<"\t POINTER="<<histArray1D[i]<<std::endl;
  }
}
//==============================================================================================
void  musrTH::FitHistogramsIfRequired(Double_t omega) {
  if (funct==NULL) return;
  if (bool_rotating_reference_frame) omega = fabs(omega) - 2*pi*fabs(rot_ref_frequency);
  std::cout<<"============================================================================================================"<<std::endl;
  std::cout<<"Fitting \""<<funct->GetName()<<"\",   funct_xMin="<<funct_xMin<<"   funct_xMax="<<funct_xMax<<"    omega="<<omega<<std::endl;
  if (strcmp(funct->GetName(),"funct1")==0) {funct->FixParameter(0,omega);}
  if (strcmp(funct->GetName(),"funct2")==0) {funct->FixParameter(0,omega);}
  if (strcmp(funct->GetName(),"funct3")==0) {funct->FixParameter(0,omega);}
  if (strcmp(funct->GetName(),"funct4")==0) {funct->FixParameter(0,omega);}
  if (strcmp(funct->GetName(),"TFieldCosPLUSbg")==0) {funct->FixParameter(0,omega);}
  if (strcmp(funct->GetName(),"TFieldCos")==0) {funct->FixParameter(0,omega);}
  if (strcmp(funct->GetName(),"rotFrameTime20")==0)  {
    if (funct->GetParameter(0)==0) {
      funct->SetParameter(0,omega); std::cout<<" FUNKCE rotFrameTime20, omega initialy at "<<omega<<std::endl;
    }
  }

  Double_t ppp[100];
  std::cout<<"    Initial parameter setting: ";
  //  Int_t n_ppp = funct->GetNumberFreeParameters();
  Int_t n_ppp = funct->GetNpar();
  for (Int_t i=0; i<n_ppp; i++) {ppp[i]=funct->GetParameter(i); std::cout<<ppp[i]<<", ";}
  std::cout<<std::endl;
      
  for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
    //    std::cout<<"fitted histogram pointer="<<histArray1D[i]<<std::endl;
    for (Int_t j=0; j<n_ppp; j++) {funct->SetParameter(j,ppp[j]);}
    histArray1D[i]->Fit(funct,funct_option,"",funct_xMin,funct_xMax);
    //    histArray1D[i]->Fit(funct,"LL","",funct_xMin,funct_xMax);
  }
  //  if (strcmp(funct->GetName(),"simpleExpoPLUSconst")==0) {
  //    N0_FromLastFit=funct->GetParameter(0);  std::cout<<"N0_FromLastFit="<<N0_FromLastFit<<std::endl;
  //    BinWidth_FromLastFit = histArray1D[i]->GetBinWidth(2); std::cout<<"BinWidth_FromLastFit="<<BinWidth_FromLastFit<<std::endl;
  //  }
  //  funct->SetLineWidth(2);
  //  funct->SetLineColor(2);
}
//==============================================================================================
//void musrTH::FillHumanDecayArray(musrTH* decayMapHistos, const int nBins, const int* iBins) {
//  for (Int_t i=0; i<musrAnalysis::nrConditions; i++) {
//    for (Int_t j=0; j<nBins; j++) {
//      Int_t jBin = iBins[j];
//      Double_t value = decayMapHistos->GetBinContent1D(i,jBin);
//      histArray1D[i]-> SetBinContent(j+1,value);
//    }
//  }
//}
//==============================================================================================
