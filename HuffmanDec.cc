
#include "HuffmanDec.h"

static unsigned long compressedFileHeaderSize = 0;

gboolean getCompressedFileInfo(const gchar* fileToDecompress, GSList*& fileList){
  ifstream inputFile(fileToDecompress, ios::binary | ios::in);
  compressedFileHeaderSize = 0;

  if(inputFile.is_open()){
    int fileCount = parseInt(inputFile);

    DPRINTLN("Compressed files found " << fileCount);
    DPRINTLN("Name\tOriginal size\tCompressed size");

    for(int i = 0; i < fileCount; i++){
      fileList = g_slist_append(fileList, parseFileHeader(inputFile));
    }

    compressedFileHeaderSize = inputFile.tellg();

    DLINE('=');

    inputFile.close();

    return TRUE;
  }

  return FALSE;
}

FileInfo* parseFileHeader(ifstream& inputFile){
  gchar* name = parseString(inputFile);
  word size = parseWord(inputFile);
  word compSize = parseWord(inputFile);
  Trie* keywords = parseKeywordsTrie(inputFile);

  DPRINTLN(name << "\t" << size << "\t" << compSize);

  return createNewFileInfo(name, size, compSize, keywords);
}

gchar* parseString(ifstream& inputFile){
  byte size;
  gchar* string;

  inputFile.read(MEM_BUFFER(size), 1);

  string = new gchar[size + 1];
  inputFile.read(string, size);
  string[size] = '\0';

  return string;
}

int parseInt(ifstream& inputFile){
  int i;
  inputFile.read(MEM_BUFFER(i), sizeof(int));
  return i;
}

word parseWord(ifstream& inputFile){
  word i;
  inputFile.read(MEM_BUFFER(i), sizeof(word));
  return i;
}

Trie* parseKeywordsTrie(ifstream& inputFile){
  byte *trieChild, *trieElem;
  int bytesCount;
  int count = parseInt(inputFile);
  Trie* keywords;

  bytesCount = ceil((float)count / 8);
  trieChild = new byte[bytesCount];
  trieElem = new byte[count];

  for(int i = 0; i < bytesCount; i++){
    inputFile.read(MEM_BUFFER(trieChild[i]), sizeof(byte));
  }

  for(int i = 0; i < count; i++){
    inputFile.read(MEM_BUFFER(trieElem[i]), sizeof(byte));
  }

  count = 0;
  keywords = trieFromFile(trieChild, trieElem, &count);

  delete[] trieChild;
  delete[] trieElem;

  return keywords;
}

Trie* trieFromFile(byte* trieChild, byte* trieElem, int* count){
  TrieItem* item = new TrieItem;
  item->c = trieElem[*count];
  Trie* root = createNewTrieRoot(NULL, NULL, item);
  int byte = (*count) / 8;
  int bit = (*count) % 8;

  if(readBit(trieChild[byte], 7 - bit)){
    (*count)++;
    setTrieLeftChild(root, trieFromFile(trieChild, trieElem, count));
    (*count)++;
    setTrieRightChild(root, trieFromFile(trieChild, trieElem, count));
  }

  return root;
}

gboolean decompressFileToDir(const gchar* fileToDecompress, const gchar* decompressPath, GSList* fileList){
  ifstream inputFile(fileToDecompress, ios::binary | ios::in);
  int i = 0;

  if(inputFile.is_open()){
    gchar* destPath;
    FileInfo* info;
    GSList* current = fileList;

    inputFile.seekg(compressedFileHeaderSize);

    while(current != NULL){
      info = (FileInfo*)current->data;
      destPath = g_strconcat(decompressPath, "/", getFileName(info), NULL);

      DPRINTLN("Decompressing " << getFileName(info));

      if(!decompressFile(inputFile, destPath, info)){
        DPRINTLN("Decompression failed for " << destPath);
        DLINE('=');
        g_free(destPath);

        inputFile.close();
        return FALSE;
      }
      g_free(destPath);

      current = g_slist_next(current);
      i++;
    }

    DPRINTLN("All files decompressed to " << decompressPath);
    DLINE('=');

    inputFile.close();
    return TRUE;
  }
  return FALSE;
}

gboolean decompressFile(ifstream& inputFile, const gchar* destPath, FileInfo* info){
  ofstream outputFile(destPath, ios::binary | ios::out);

    if(outputFile.is_open()){
      Trie* keywordsTrie = getFileKeywords(info);

      word i = 0, j = 0;
      word originalSize = getFileSize(info);
      int buffer = inputFile.get();

      while(i < originalSize && buffer != EOF){

          if(j == 8){
            j = 0;
            buffer = inputFile.get();
          }

          if(buffer != EOF){
            keywordsTrie = navigateTrie(keywordsTrie, readBit(buffer, 7 - j) ? RIGHT : LEFT);
            if(isTrieALeaf(keywordsTrie)){
              outputFile.write(MEM_BUFFER(((TrieItem*)getTrieData(keywordsTrie))->c), sizeof(byte));
              keywordsTrie = getFileKeywords(info);
              i++;
            }
          }

          j++;
      }

      outputFile.close();

      return i == originalSize;
    }
  return FALSE;
}
