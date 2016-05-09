
#include "FileInfo.h"

struct FileInfo{
  const gchar* name;
  const gchar* path;
  int charSetFreq[CHAR_SET_SIZE];
  word size, comprSize;
  Trie* keywords;
};


FileInfo* createNewFileInfo(const gchar* path){
  assert(path != NULL);

  ifstream in(path, ios::binary | ios::ate);

  if(in.is_open()){
    FileInfo* info = new FileInfo;

    info->name = g_path_get_basename(path);
    info->path = path;
    info->size = in.tellg();
    info->comprSize = 0;
    info->keywords = NULL;

    in.close();
    return info;
  }

  return NULL;
}

FileInfo* createNewFileInfo(const gchar* name, word size, word compSize, Trie* keywords){
  FileInfo* info = new FileInfo;

  info->name = name;
  info->path = NULL;
  info->size = size;
  info->comprSize = compSize;
  info->keywords = keywords;

  return info;

}

void deleteTrieItem(gpointer ptr){
  TrieItem* item = (TrieItem*)ptr;
  delete item;
}

void deleteFileInfo(gpointer ptr){
  FileInfo* info = (FileInfo*)ptr;
  delete[] info->name;
  delete[] info->path;
  if(info->keywords != NULL){
    deleteTrie(info->keywords, deleteTrieItem);
  }
  delete info;
}

gint findFileByName(gconstpointer a, gconstpointer b){
  FileInfo* info = (FileInfo*)a;
  gchar* name = (gchar*)b;

  return strcmp(info->name, name);
}


const gchar* getFileName(FileInfo* info){
  return info->name;
}

const gchar* getFilePath(FileInfo* info){
  return info->path;
}

const gchar* getFileFormat(FileInfo* info){
  gboolean isCertain;
  return g_content_type_guess(info->name, NULL, 0, &isCertain);
}

word getFileSize(FileInfo* info){
  return info->size;
}

word getFileCompressedSize(FileInfo* info){
  return info->comprSize;
}

void setFileCompressedSize(FileInfo* info, word size){
  info->comprSize = size;
}

void setFileKeywords(FileInfo* info, Trie* keywords){
  info->keywords = keywords;
}

Trie* getFileKeywords(FileInfo* info){
  return info->keywords;
}

gint* getFileCharsFrequencies(FileInfo* info){
    return info->charSetFreq;
}

void setFileCharsFrequencies(FileInfo* info, int* charSetFreq){
  memcpy(info->charSetFreq, charSetFreq, sizeof(int) * CHAR_SET_SIZE);
}
