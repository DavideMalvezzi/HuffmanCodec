
#include "Trie.h"

struct Trie{
  void* data;
  Trie* left;
  Trie* right;
};

Trie* createNewTrieRoot(Trie* left, Trie* right, void* data){
  Trie* root = new Trie;
  root->left = left;
  root->right = right;
  root->data = data;
  return root;
}

void deleteTrie(Trie* trie, GDestroyNotify deleteFun){
  if(deleteFun != NULL){
    deleteFun(trie->data);
  }

  if(trie->left != NULL){
    deleteTrie(trie->left, deleteFun);
  }

  if(trie->right != NULL){
    deleteTrie(trie->right, deleteFun);
  }

  delete trie;
}

Trie* navigateTrie(Trie* root, TrieDirection direction){
  assert(root != NULL);

  if(direction == LEFT){
    return root->left;
  }

  return root->right;
}

gboolean isTrieALeaf(Trie* root){
  assert(root != NULL);

  return root->left == NULL && root->right == NULL;
}

void setTrieLeftChild(Trie* root, Trie* child){
  assert(root != NULL);

  root->left = child;
}

void setTrieRightChild(Trie* root, Trie* child){
  assert(root != NULL);

  root->right = child;
}

void getTrieElementCount(Trie* root, gint* count){
  assert(root != NULL);

  (*count)++;
  if(!isTrieALeaf(root)){
    getTrieElementCount(root->left, count);
    getTrieElementCount(root->right, count);
  }
}

void* getTrieData(Trie* root){
  assert(root != NULL);

  return root->data;
}
