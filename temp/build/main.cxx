#include <iostream>
#include <fstream>
#include <string>
#include "TString.h"
#include "MyAna.h"
#include <TCanvas.h>
#include <cstdlib>

int main (int argc, char **argv) {

    // TCanvas *mydummycanvas = new TCanvas(); 

    string temp1, temp2; // temp1 to store filename, temp2 to store run number
    cout << " " << endl;
    cout << "## Reading rootfiles to be analyzed........." << endl;
    cout << " " << endl;


    TChain *chain = new TChain ("t1");

    string fname;
    string outfname;
    ifstream inData (argv[1]); 
    outfname = argv[2]; 

    while ( !inData.eof() ) {

	getline (inData, fname); //get contents of infile 
	cout << fname << endl; //output contents of infile   

	if(fname!=""){ // if this is not the last file

	    temp1 = fname;
	    int n = temp1.rfind ("/"); // find the position in string of the last "/"
	    temp1.erase (0, n + 1);	// erase substring in front of it including itself

	    temp2 = fname;
	    n = temp2.rfind ("run0"); // find the position in string of the last "/"
	    temp2.erase (0, n + 4);	// erase substring in front of it including itself

		chain->Add (TString(fname));
	}

    } 

    MyAna ana(chain);
    ana.Loop(outfname);

    return 0;
}
