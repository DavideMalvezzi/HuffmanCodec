
#include "FileInfo.h"

struct FileInfo{
  const gchar* name;
  const gchar* path;
  int* charSetFreq;
  word size, comprSize;
  Trie* keywords;
};


FileInfo* createNewFileInfo(const gchar* path){
  assert(path != NULL);

  ifstream in(path, ios::binary | ios::ate);

  if(in.is_open()){
    FileInfo* info = new FileInfo;

    //Set file params
    info->name = g_path_get_basename(path);
    info->path = path;
    info->size = in.tellg();
    info->comprSize = 0;
    info->charSetFreq = NULL;
    info->keywords = NULL;

    in.close();

    //Discard the file if it's size is 0
    if(info->size > 0){
      return info;
    }

    delete info;

    return NULL;
  }

  return NULL;
}

FileInfo* createNewFileInfo(const gchar* name, word size, word compSize, Trie* keywords){
  FileInfo* info = new FileInfo;

  //Set file params
  info->name = name;
  info->path = NULL;
  info->size = size;
  info->comprSize = compSize;
  info->charSetFreq = NULL;
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

  if(info->path != NULL){
    delete[] info->path;
  }

  if(info->charSetFreq != NULL){
    delete[] info->charSetFreq;
  }

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

int* getFileCharsFrequencies(FileInfo* info){
    return info->charSetFreq;
}

void setFileCharsFrequencies(FileInfo* info, int* charSetFreq){
  if(info->charSetFreq == NULL){
    info->charSetFreq = new int[CHAR_SET_SIZE];
  }
  memcpy(info->charSetFreq, charSetFreq, sizeof(int) * CHAR_SET_SIZE);
}
