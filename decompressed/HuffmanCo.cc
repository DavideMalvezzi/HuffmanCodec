
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

  return FALSE;
}

gboolean saveFileHeader(ofstream& outputFile, FileInfo* fileInfo){
  const gchar* path = getFilePath(fileInfo);
  gint charSetFreq[CHAR_SET_SIZE];
  Trie* keywords = getFileKeywords(fileInfo);

  //Work out the frequencies
  if(getFileFrequencies(path, charSetFreq)){
    /*
    cout << "File " << getFileName(fileInfo) << endl;
    cout << "Frequencies:" << endl;
    for(int i = 0; i < CHAR_SET_SIZE; i++){
      if(charSetFreq[i] != 0){
        cout << (int)i << " " << charSetFreq[i] << endl;
      }
    }
    */

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

  return FALSE;
}

word computeFileKeywords(const gchar* path, gint* charSetFreq, Trie*& keywordsTrie){
  word compressedSize;
  Trie* trie;
  TrieItem* trieItem;
  PriorityList* pList = createNewPriorityList(CHAR_SET_SIZE);

  //If there isn't a key word trie associated to the file
  if(keywordsTrie == NULL){
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

  //Put the deleteFun param to NULL because we don't want to destroy the trieItem in the list
  //They will be deleted with the Trie from the FileInfo destruction function
  deletePriorityList(pList, NULL);

  return compressedSize;
}

gboolean getFileFrequencies(const gchar* path, int* charSetFreq){
  int c;

  //Open input file
  ifstream inputFile(path, ios::binary | ios::in);

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

  for(int i = 0; i < CHAR_SET_SIZE; i++){
    if(charSetFreq[i] > 0){
      size += charSetFreq[i] * keywords[i].size;
    }
  }

  return size / 8 + 1;
}

void keywordsTrieToKeywordsArray(Trie* trie, CharKey* arr, hword path, hword deep){
  TrieItem* item;
  if(isTrieALeaf(trie)){
    item = (TrieItem*)getTrieData(trie);
    arr[item->c].key = path;
    arr[item->c].size = deep;
  }
  else{
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
  return FALSE;
}

void saveTrieKeywordsOnFile(ofstream& outputFile, Trie* keywords){
  byte *trieChild, *trieElem;
  int count = 0, bytesCount, i = 0;
  getTrieElementCount(keywords, &count);

  bytesCount = ceil((float)count / 8);
  trieChild = new byte[bytesCount];
  trieElem = new byte[count];

  //Reset child bits
  for(int i = 0; i < bytesCount; i++){
    trieChild[i] = 0;
  }

  //Get elements and child bits
  trieToFile(keywords, trieChild, trieElem, &i);

  outputFile.write(MEM_BUFFER(count), sizeof(int));

  for(int i = 0; i < bytesCount; i++){
    outputFile.write(MEM_BUFFER(trieChild[i]), sizeof(byte));
  }

  for(int i = 0; i < count; i++){
    outputFile.write(MEM_BUFFER(trieElem[i]), sizeof(byte));
  }

  delete[] trieChild;
  delete[] trieElem;
}

void trieToFile(Trie* root, byte* trieChild, byte* trieElem, int* count){
  //Seve the n-th element in the trieElem array
  trieElem[*count] = ((TrieItem*)getTrieData(root))->c;

  //If not a leaf call recursively on the children
  if(!isTrieALeaf(root)){
    int byte = (*count) / 8;
    int bit = (*count) % 8;

    //Update the trieChild header
    trieChild[byte] = setBit(trieChild[byte], 7 - bit);

    //Update the element count
    (*count)++;
    trieToFile(navigateTrie(root, LEFT), trieChild, trieElem, count);
    (*count)++;
    trieToFile(navigateTrie(root, RIGHT), trieChild, trieElem, count);
  }
}
