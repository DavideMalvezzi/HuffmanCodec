OBJS = main.o HuffmanDec.o HuffmanCo.o PriorityList.o Trie.o FileInfo.o BitUtils.o
EXNAME = HuffmanCodec
CXXFLAGS = -Wall `pkg-config --cflags gtk+-3.0` #-D NDEBUG
LDFLAGS = -export-dynamic `pkg-config --libs gtk+-3.0`

.PHONY: depend clean cleanAll

compile: depend $(OBJS)
	g++ -o $(EXNAME) $(OBJS) $(LDFLAGS)

-include depend.txt

depend:
	g++ -MM *.cc > depend.txt

clean:
	rm -f *.o *~

cleanAll: clean
	rm -f $(EXNAME)
