#ifndef MYANA_h
#define MYANA_h

#include "t1.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

class MyAna : public t1 {
    public:
        MyAna(TTree *tree=0):t1(tree){};
        ~MyAna() {};
        virtual void Loop(string filename);
        void initialize(string filename);
        void execute();
        void finalize();
        void clear();

    private:
        TFile *m_file;
	int total_event;
        TH1D *mup_energy;
};
#endif
