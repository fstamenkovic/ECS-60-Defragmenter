defrag.out : defragmenter.o DefragRunner.o mynew.o linearprobing.o
	g++ -ansi -Wall -g -o defrag.out defragmenter.o DefragRunner.o mynew.o linearprobing.o

defragmenter.o : defragmenter.cpp defragmenter.h linearprobing.h
	g++ -ansi -Wall -g -c defragmenter.cpp

DefragRunner.o : DefragRunner.cpp DefragRunner.h mynew.h CPUTimer.h defragmenter.h
	g++ -ansi -Wall -g -c DefragRunner.cpp

linearprobing.o : linearprobing.cpp linearprobing.h DefragRunner.h defragmenter.h
	g++ -ansi -Wall -g -c linearprobing.cpp

mynew.o : mynew.cpp mynew.h
	g++ -ansi -Wall -g -c mynew.cpp

clean :
	rm -f defrag.out defragmenter.o  DefragRunner.o  mynew.o