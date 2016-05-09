OBJS = main.o HuffmanDec.o HuffmanCo.o PriorityList.o Trie.o FileInfo.o BitUtils.o
EXNAME = HuffmanCodec
INSTALLDIR = /bin
CXXFLAGS = -Wall `pkg-config --cflags gtk+-3.0` #-D NDEBUG
LDFLAGS = -export-dynamic `pkg-config --libs gtk+-3.0`

.PHONY: dependencies clean cleanAll

install: compile
		rm -rf $(INSTALLDIR)/gui
		mkdir -p $(INSTALLDIR)/gui
		cp -f $(EXNAME) $(INSTALLDIR)
		cp -rf gui/ $(INSTALLDIR)
		chmod +x $(INSTALLDIR)/$(EXNAME)

compile: dependencies $(OBJS)
	g++ -o $(EXNAME) $(OBJS) $(LDFLAGS)

-include depend.txt

dependencies:
	g++ -MM *.cc > depend.txt

clean:
	rm -f *.o *~

cleanAll: clean
	rm -f $(EXNAME)
