# HuffmanCodec
##Introduction

The project objective was to realize a text file compressor/decompressor using the [Huffman coding](https://en.wikipedia.org/wiki/Huffman_coding).
With this program you can:
- **compress** one or more text file inside a *.hca (Huffman Compressed Archive) file;
- **decompress** one *.hca file to get the original text files.

The functionalities are both available from GUI and command line.

To compile the program use the Makefile inside the repo.

To compress use the GUI or use this command from the console:
```
./HuffmanCodec -c File1.txt ... FileN.txt -o ArchiveFile.hca
```

To decompress use the GUI or use this command from the console:
```
./HuffmanCodec -d ArchiveFile.hca -o DecompressFolderPath/
```

##Requirements
- Gtk 3.0


