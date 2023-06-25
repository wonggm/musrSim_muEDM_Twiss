#define musrWriteDump_cxx
#include "musrWriteDump.hh"


FILE* musrWriteDump::fpw = NULL;
std::ofstream musrWriteDump::textDumpFile;
unsigned int musrWriteDump::get_lwords = 0;
unsigned int musrWriteDump::blt_data[MAX_NUMBER_LWORDS]; 

musrWriteDump::musrWriteDump(char* name, int clock_ch, int maxTimeBinJitter) {
  char  filename[255];
  sprintf(filename,"data/TDC_V1190_200_dump_%s.bin",name);
  fpw = fopen(filename,"wb");
  if (fpw==NULL) {
    std::cout<<"musrWriteDump::musrWriteDump: ERROR!!!  output dump file "<<filename<<" can not be opened !!!"<<std::endl;
    std::cout<<"     ===>  S T O P     F O R C E D !!!"<<std::endl;
    exit(1);
  }

  sprintf(filename,"data/TDC_V1190_200_dump_%s.txt",name);
  textDumpFile.open(filename);
  if (! (textDumpFile.is_open()) ) {
    std::cout<<"musrWriteDump::musrWriteDump: ERROR!!!  output text dump file "<<filename<<" can not be opened !!!"<<std::endl;
    std::cout<<"     ===>  S T O P     F O R C E D !!!"<<std::endl;
    exit(1);
  }
  clock_channel = clock_ch;
  MAX_TIME_JITTER = maxTimeBinJitter;
  tdcBin0 = 0;
  kamilHitMultimap.insert(std::pair<Long64_t, int>(0,-1));
}

//----------------------------------------------------------------------------
musrWriteDump::~musrWriteDump() {
  // write out remaining ~20 evnets into the dump file
  flush_to_dump();

  if (fpw != NULL) {
    fclose(fpw);
    fpw = NULL;
  }
  if (textDumpFile.is_open()) textDumpFile.close();
}
//----------------------------------------------------------------------------
void musrWriteDump::send_to_dump(unsigned int channel, Long64_t tdcBin, int flush) {
  // First do some specific changes:
  if (channel==51) channel=11;
  if (channel==52) channel=12;
  if (channel==102) channel=0;
  
  if (channel>=0) {
    kamilHitMultimap.insert(std::pair<Long64_t, int>(tdcBin,channel) );
  }
  
  if (kamilHitMultimap.size()>20) {
    it = kamilHitMultimap.begin();
    if (MAX_TIME_JITTER>0) {
      // randomise the hits in time in the dump file - usefull for the tests
      // of Andrea's analysis program.
      it1 = kamilHitMultimap.begin();
      it2 = it1; it2++;
      it3 = it2; it3++;
      it4 = it3; it4++;
      it5 = it4; it5++;
      it6 = it5; it6++;
      double prob = (float)rand()/RAND_MAX;
      //     std::cout<<"IIIIIIIIIIIIIIII prob= "<<prob<<";       MAX_TIME_JITTER= "<<MAX_TIME_JITTER<<std::endl;
      if ( ((it2->first)-(it1->first)) <= MAX_TIME_JITTER) { if (prob<0.6) {it = it2;} }
      if ( ((it3->first)-(it1->first)) <= MAX_TIME_JITTER) { if (prob<0.5) {it = it3;} }
      if ( ((it4->first)-(it1->first)) <= MAX_TIME_JITTER) { if (prob<0.4) {it = it4;} }
      if ( ((it5->first)-(it1->first)) <= MAX_TIME_JITTER) { if (prob<0.3) {it = it5;} }
      if ( ((it6->first)-(it1->first)) <= MAX_TIME_JITTER) { if (prob<0.2) {it = it6;} }
    }

    Long64_t tdcBin = it->first;
    int ch_num = it->second;
    if (ch_num==-1) { // clock channel
      kamilHitMultimap.insert(std::pair<Long64_t, int>(tdcBin + CLOCK_INTERVAL,-1));
      ch_num = clock_channel;
    }
    int tdcDifference = tdcBin - tdcBin0;
    while (tdcDifference>=TDC_BIT_RANGE) {
      tdcBin0 += TDC_BIT_RANGE;
      tdcDifference = tdcBin - tdcBin0;
    }
    write_to_dump(ch_num, tdcDifference, false);
    //	    std::cout<<"Setrideno: ch="<<it->first<<", "<<it->second<<std::endl;
    kamilHitMultimap.erase(it);
    if (tdcDifference<=MAX_TIME_JITTER) tdcBin0 -= TDC_BIT_RANGE;
  }
}

//----------------------------------------------------------------------------
//  At the end of analysis, just before closing everything, flush the last few events
//  to the dump file:
void musrWriteDump::flush_to_dump() {
  while (kamilHitMultimap.size()>1) {
    it  = kamilHitMultimap.begin();
    Long64_t tdcBin = it->first;
    int ch_num = it->second;
    if (ch_num==-1) { // clock channel
      kamilHitMultimap.insert(std::pair<Long64_t, int>(tdcBin + CLOCK_INTERVAL,-1));
      ch_num = clock_channel;
    }
    int tdcDifference = tdcBin - tdcBin0;
    while (tdcDifference>=TDC_BIT_RANGE) {
      tdcBin0 += TDC_BIT_RANGE;
      tdcDifference = tdcBin - tdcBin0;
    }

    if (kamilHitMultimap.size()==2)  write_to_dump(ch_num, tdcDifference, true);
    else                             write_to_dump(ch_num, tdcDifference, false);

    kamilHitMultimap.erase(it);
  }    
}
//----------------------------------------------------------------------------

/*
 *  write_to_dump  write TDC event to dump file
 *
 *  channel           TDC channel
 *  tdctime           TDC event time
 *  flush             <> 0  flush buffer to file 
 *
 *  status returned:
 *  > 0 : number of events flushed to file
 *    0 : event written to bu                     ffer
 *   -1 : invalid channel or tdctime
 *   -2 : invalid file handle
 *   -3 : error writing to file 
 */

int musrWriteDump::write_to_dump(unsigned int channel, unsigned int tdctime, int flush) {
  int status;

  //  std::cout<<channel<<"\t"<<tdctime<<std::endl;
  textDumpFile<<channel<<"\t"<<tdctime<<"\n";
  status = -1; // assume error
 
  // add valid event to buffer
  if ((channel >= 0) && (channel < N_TDC_CHANNELS)) {
    if ((tdctime >= 0) && (tdctime < TDC_BIT_RANGE)) {
#ifdef OS_LINUX
      V1190_DATA *v1190_data;
#endif

#ifdef OS_LINUX
      v1190_data = (V1190_DATA *) &blt_data[get_lwords];

      v1190_data->channel = channel;
      v1190_data->data    = tdctime;

#else // bit fields do not work for windows

#ifdef TDCV1290
      blt_data[get_lwords] = tdctime + (channel << 21);
#else
      blt_data[get_lwords] = tdctime + (channel << 19);
#endif

#endif

      get_lwords++;
      status = 0;
    }
  }

  // buffer filled or flush buffer to file
  if ((get_lwords >= MAX_NUMBER_LWORDS) || (flush && (get_lwords > 0))) {

     if (fpw != NULL) {
       status = (int) get_lwords;

/*
 * TDC control register: BERR EN bit set -> Bus error to finish block transfer or during
 *                                          the empty buffer readout
 *
 *  Possible values of get_lwords:
 *     0          flag previous TDC buffer was empty
 *     32*1024    flag previous TDC buffer was completely filled
 *
 *     1-1023     TDC buffer is empty now      
 *                   generates  return status code 0x211 : VME buffer empty now flag
 *     1024       TDC buffer data available    
 *                   generates  return status code 0x000 : Status OK 
 */

       // write number of events currently in buffer to file
       if (fwrite(&get_lwords, sizeof(unsigned int), 1, fpw) != 1) {
         printf("ERROR %d writing # of data words to dump file\n",ferror(fpw));
         status = -3;
       }

       // write buffer contents to file
       if (fwrite(blt_data, sizeof(unsigned int), get_lwords, fpw) != get_lwords) {
         printf("ERROR %d writing blt_data to dump file\n",ferror(fpw));
         status = -3;
       }
     } else {
       status = -2;
     }

     get_lwords = 0;
  }

  return status;
}

//----------------------------------------------------------------------------
