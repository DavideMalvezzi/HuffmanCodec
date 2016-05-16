
#include "HuffmanCo.h"

struct CharKey{
  hword key, size;
};

gboolean compressToFile(const gchar* path, GSList* fileList){
  int fileNum = g_slist_length(fileList);
  GSList* current = fileList;
  //Open output file
  ofstream outputFile(path, ios::binary | ios::out);
  //If open is ok
  if(outputFile.is_open()){
    //Write files count
    outputFile.write(MEM_BUFFER(fileNum), sizeof(int));
    //Save each file header
    DPRINTLN("Saving files header...");
    while(current != NULL){

      if(!saveFileHeader(outputFile, (FileInfo*)current->data)){
        //If return false the write failed, so close and abort
        outputFile.close();
        return FALSE;
      }

      current = g_slist_next(current);
    }

    current = fileList;
    //Compress each file
    DPRINTLN("Compressing files...");
    while(current != NULL){
      if(!compressFile(outputFile, (FileInfo*)current->data)){
        //If return false the write failed, so close and abort
        outputFile.close();
        return FALSE;
      }
      current = g_slist_next(current);
    }

    outputFile.close();

    DPRINTLN("All files compressed");
    DLINE('=');

    return TRUE;
  }

  DPRINTLN("Compression failed...");
  DLINE('=');

  return FALSE;
}

gboolean saveFileHeader(ofstream& outputFile, FileInfo* fileInfo){
  const gchar* path = getFilePath(fileInfo);
  gint charSetFreq[CHAR_SET_SIZE];
  Trie* keywords = getFileKeywords(fileInfo);

  //Work out the frequencies
  if(getFileFrequencies(path, charSetFreq)){
    //Set the file frequencies
    setFileCharsFrequencies(fileInfo, charSetFreq);

    //Get file header info
    char nameLen = strlen(getFileName(fileInfo));
    word fileSize = getFileSize(fileInfo);
    word fileCompSize = computeFileKeywords(path, charSetFreq, keywords);

    //if fileCompSize is -1 -> error on create the keywords
    if(fileCompSize != (word)-1){
      //Write header info
      outputFile.write(MEM_BUFFER(nameLen), sizeof(char));
      outputFile.write(getFileName(fileInfo), nameLen);
      outputFile.write(MEM_BUFFER(fileSize), sizeof(word));
      outputFile.write(MEM_BUFFER(fileCompSize), sizeof(word));

      //Save the keywords trie on the file
      saveTrieKeywordsOnFile(outputFile, keywords);

      //Set the file compressed size and keywords trie
      setFileCompressedSize(fileInfo, fileCompSize);
      setFileKeywords(fileInfo, keywords);

      return TRUE;
    }
  }

  DPRINTLN("Failed to save file header...");

  return FALSE;
}

word computeFileKeywords(const gchar* path, gint* charSetFreq, Trie*& keywordsTrie){
  word compressedSize;
  Trie* trie;
  TrieItem* trieItem;
  PriorityList* pList = createNewPriorityList(CHAR_SET_SIZE);

  //If there isn't a key word trie associated to the file
  if(keywordsTrie == NULL){
    DPRINTLN("Generating keywords...");

    //Insert elements in the PriorityList
    for(int i = 0; i < CHAR_SET_SIZE; i++){
      if(charSetFreq[i] > 0){
        trieItem = new TrieItem;
        trieItem->c = i;
        trieItem->frequency = charSetFreq[i];

        //Create the trie
        trie = createNewTrieRoot(NULL, NULL, trieItem);

        //Insert in the list
        insertListItem(pList, trie, trieItem->frequency);
      }
    }

    //Get the keywords trie
    keywordsTrie = getKeywordsTrie(pList);
  }

  //Get the file compressedSize from the keywordsTrie and the charSetFreq
  compressedSize = getFileCompressedSize(keywordsTrie, charSetFreq);

  DPRINTLN("File compressed size is " << compressedSize << " bytes");

  //Put the deleteFun param to NULL because we don't want to destroy the trieItem in the list
  //They will be deleted with the Trie from the FileInfo destruction function
  deletePriorityList(pList, NULL);

  return compressedSize;
}

gboolean getFileFrequencies(const gchar* path, int* charSetFreq){
  int c;

  //Open input file
  ifstream inputFile(path, ios::binary | ios::in);

  DPRINTLN("Calcolating characters frequencies...");

  //Reset the chars count
  for(int i = 0; i < CHAR_SET_SIZE; i++){
    charSetFreq[i] = 0;
  }

  //If open is ok
  if(inputFile.is_open()){
    //Count each char frequency
    while((c = inputFile.get()) != EOF){
      charSetFreq[c]++;
    }

    inputFile.close();
    return TRUE;
  }

  DPRINTLN("Failed to open the file...");

  return FALSE;
}

Trie* getKeywordsTrie(PriorityList* pList){
  Trie *trie, *left, *right;
  TrieItem *trieItem, *leftData, *rightData;

  //Create the keywords Trie
  while(getListSize(pList) > 1){
    //Get the left and right
    right = (Trie*)getListMinData(pList);
    left = (Trie*)getListMinData(pList);

    //Get children data
    leftData = (TrieItem*)getTrieData(left);
    rightData = (TrieItem*)getTrieData(right);

    //Create the new item
    trieItem = new TrieItem;
    trieItem->frequency = leftData->frequency + rightData->frequency;

    //Create the new trie
    trie = createNewTrieRoot(left, right, trieItem);

    //Insert the new trie inside the list
    insertListItem(pList, trie, trieItem->frequency);
  }

  //Get the last trie that is the keywords
  return (Trie*)getListMinData(pList);
}

word getFileCompressedSize(Trie* keywordsTrie, int* charSetFreq){
  word size = 0;
  CharKey keywords[CHAR_SET_SIZE];
  keywordsTrieToKeywordsArray(keywordsTrie, keywords);

  //Get the size in bits
  for(int i = 0; i < CHAR_SET_SIZE; i++){
    if(charSetFreq[i] > 0){
      size += charSetFreq[i] * keywords[i].size;
    }
  }

  //Return the size in bytes
  return size / 8 + 1;
}

void keywordsTrieToKeywordsArray(Trie* trie, CharKey* arr, hword path, hword deep){
  TrieItem* item;

  if(isTrieALeaf(trie)){
    //Add the item to the array
    item = (TrieItem*)getTrieData(trie);
    arr[item->c].key = path;
    arr[item->c].size = deep;
  }
  else{
    //Navigate the trie
    keywordsTrieToKeywordsArray(navigateTrie(trie, LEFT), arr, path, deep + 1);
    keywordsTrieToKeywordsArray(navigateTrie(trie, RIGHT), arr, setBit(path, KEY_BIT_SIZE - deep - 1), deep + 1);
  }
}

gboolean compressFile(ofstream& outputFile, FileInfo* fileInfo){
  ifstream inputFile(getFilePath(fileInfo), ios::binary | ios::in);

  if(inputFile.is_open()){
    byte buffer = 0;
    int c, bufferSize = 0;

    CharKey keywords[CHAR_SET_SIZE];
    keywordsTrieToKeywordsArray(getFileKeywords(fileInfo), keywords);

    DPRINTLN("Compressing " << getFileName(fileInfo));
    while((c = inputFile.get()) != EOF){

      //Convert the char to the keywords
      for(hword i = 0; i < keywords[c].size; i++){
        if(bufferSize == 8){
          outputFile.write(MEM_BUFFER(buffer), sizeof(byte));
          buffer = 0;
          bufferSize = 0;
        }

        if(readBit(keywords[c].key, KEY_BIT_SIZE - i - 1)){
          buffer = setBit(buffer, 7 - bufferSize);
        }

        bufferSize++;
      }
    }

    outputFile.write(MEM_BUFFER(buffer), sizeof(byte));

    inputFile.close();
    return TRUE;
  }

  DPRINTLN("Error opening file " << getFileName(fileInfo));

  return FALSE;
}

void saveTrieKeywordsOnFile(ofstream& outputFile, Trie* keywords){
  byte *trieChild, *trieElem;
  int leafCount = 0, arcCount = 0, bytesCount;

  //Get counts
  getTrieLeafCount(keywords, &leafCount);
  getTrieElementCount(keywords, &arcCount);

  //Need max because if only 1 node return 0
  bytesCount = max(1.0f, ceil((float)(arcCount - 1) / 8));
  trieChild = new byte[bytesCount];
  trieElem = new byte[leafCount];

  //Reset child bits
  for(int i = 0; i < bytesCount; i++){
    trieChild[i] = 0;
  }

  //Get elements and child bits
  leafCount = 0;
  arcCount = 0;
  trieToFile(keywords, trieChild, trieElem, &arcCount, &leafCount);

  outputFile.write(MEM_BUFFER(bytesCount), sizeof(int));

  for(int i = 0; i < bytesCount; i++){
    outputFile.write(MEM_BUFFER(trieChild[i]), sizeof(byte));
  }

  for(int i = 0; i < leafCount; i++){
    outputFile.write(MEM_BUFFER(trieElem[i]), sizeof(byte));
  }

  delete[] trieChild;
  delete[] trieElem;
}

void trieToFile(Trie* root, byte* trieChild, byte* trieElem, int* arcCount, int* leafCount){

  int byte = (*arcCount) / 8;
  int bit = (*arcCount) % 8;

  //Update the element count
  (*arcCount)++;

  //If not a leaf call recursively on the children
  if(!isTrieALeaf(root)){
    //Update the trieChild header
    trieChild[byte] = setBit(trieChild[byte], 7 - bit);
    trieToFile(navigateTrie(root, LEFT), trieChild, trieElem, arcCount, leafCount);
    trieToFile(navigateTrie(root, RIGHT), trieChild, trieElem, arcCount, leafCount);
  }
  else{
    //Save the n-th element in the trieElem array
    trieElem[*leafCount] = ((TrieItem*)getTrieData(root))->c;
    //Update the element count
    (*leafCount)++;
  }
}
