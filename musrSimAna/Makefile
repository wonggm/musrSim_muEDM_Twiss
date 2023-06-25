#include /home/daquser/temp/SctRodDaq/build/Makefile.include
#include /home/daquser/temp/SctRodDaq/build/Makefile.commands
#
#
CPP=g++
#CPPFLAGS= -I$(ROOTSYS)/include -I/userdisk/sedlak/myLCG/CondDBMySQL_new/CondDBMySQL/include -I/usr/include/mysql
#LDFLAGS= $(shell root-config --glibs) -L/userdisk/sedlak/myLCG/CondDBMySQL_new/CondDBMySQL/src/.libs -lconddb -L/usr/lib/mysql -lmysqlclient
CPPFLAGS= -I$(ROOTSYS)/include/root -std=c++11
LDFLAGS= $(shell root-config --glibs) -lMinuit

all:	musrRoot

%.o:%.cxx
	$(CPP) -c $(CPPFLAGS) $^

musrRoot:	musrSimAna.o musrAnalysis.o musrCounter.o musrTH.o musrWriteDump.o
	$(CPP) -o musrSimAna $(LDFLAGS) $^

clean:
	-rm -f *.o musrSimAna
