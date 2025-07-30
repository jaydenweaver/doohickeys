/*

   SKIP LIST IMPLEMENTATION
   -------------------------------------------------------
   probabilistic data structure allowing for O(log(n)) traversals of sorted
   linked lists. does however require more memory than a standard sorted linked
   list.

   a p value of 0.5 is used in this implementation (probability for node to be
   added to i+1 level).

*/

#pragma once

#include <vector>

struct Node {
  int key;
  std::vector<Node *> forward;

  Node(int key, int level) : key(key), forward(level, nullptr) {}
};

class SkipList {
private:
  const double P = 0.5;
  const int MAX_LEVEL = 15;

  int level = 0;
  Node *head;

  int assign_level();

public:
  SkipList();
  ~SkipList();

  void insert(int key);
  void remove(int key);
  Node *search(int key);
};
