#ifndef musrWriteDump_h
#define musrWriteDump_h 1
#include <iostream>
#include <fstream>
#include <map>
#include <TSystem.h>

class musrWriteDump {
  public :
  musrWriteDump(char* name, int clock_ch, int maxTimeBinJitter);
  ~musrWriteDump();
  void send_to_dump(unsigned int channel, Long64_t tdcBin, int flush);
  void flush_to_dump();

  private:
  int write_to_dump(unsigned int channel, unsigned int tdctime, int flush);

  static const int N_TDC_CHANNELS = 32;
  static const int TDC_BIT_RANGE  = 524288;
  static const int CLOCK_INTERVAL = 512000;
  static const int MAX_NUMBER_LWORDS = 1024;
  int MAX_TIME_JITTER;
  int clock_channel;
  Long64_t tdcBin0;

  // 32 bit unsigned integer
  static unsigned int get_lwords;                       // number of lwords in blt_data
  static unsigned int blt_data[MAX_NUMBER_LWORDS];      // data buffer

  static  FILE *fpw;    // handle to write TDC dump file
  static std::ofstream textDumpFile;

  typedef unsigned short int WORD;
  typedef unsigned long int DWORD;
  typedef struct _v1190data {
  //  TDC V1290
  // DWORD data:21;     // TDC V1290A measurement (21bit@25ps, 19bit @100ps & @200ps,
  // WORD  channel:5;   //                         17bit@800ps) 32 channel for TDC V1290A
  //                    //                                      16 channel for TDC V1290N
  //  TDC V1190
    DWORD data:19;     // TDC V1190 measurement (19bit@100 & @200, 17bit@800ps)
    WORD  channel:7;   // 128 channel for TDC V1190A, 64 channel for TDC V1190B
    //
    WORD  edge:1;      // 0   -> leading edge, 1 -> trailing edge
    WORD  buffer:3;    // 000 -> TDC measurement, 100 -> TDC error
    WORD  filler:2;    // 00  -> TDC data
  } V1190_DATA;

  std::multimap <Long64_t, int>  kamilHitMultimap;
  std::multimap <Long64_t, int>::iterator it;
  std::multimap <Long64_t, int>::iterator it1;
  std::multimap <Long64_t, int>::iterator it2;
  std::multimap <Long64_t, int>::iterator it3;
  std::multimap <Long64_t, int>::iterator it4;
  std::multimap <Long64_t, int>::iterator it5;
  std::multimap <Long64_t, int>::iterator it6;
};

#endif
