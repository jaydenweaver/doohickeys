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

#include <random>
#include <type_traits>
#include <vector>

template <typename T> struct Node {
  T key;
  std::vector<Node<T> *> forward;

  Node(const T &key, int level) : key(key), forward(level, nullptr) {}
};

template <typename T> class SkipList {
  static_assert(std::is_convertible_v<
                    decltype(std::declval<T>() < std::declval<T>()), bool>,
                "type T must support < operator");

private:
  const double P = 0.5;
  const int MAX_LEVEL = 15;

  int level = 0;
  Node<T> *head = new Node<T>(T{}, MAX_LEVEL);

  int assign_level();

public:
  SkipList();
  ~SkipList();

  void insert(const T &key);
  void remove(const T &key);
  Node<T> *search(const T &key);
};

template <typename T> int SkipList<T>::assign_level() {
  int level = 1;

  std::random_device rand;
  std::mt19937 gen(rand());
  std::uniform_real_distribution<double> uniform(0.0, 1.0);

  while (uniform(gen) < P && level < MAX_LEVEL)
    level++;

  return level;
}

template <typename T> void SkipList<T>::insert(const T &key) {
  int level = assign_level();

  if (head == nullptr) {
    head = new Node<T>(key, level);
    return;
  }

  Node<T> *curr = head;
  std::vector<Node<T> *> list(this->level, nullptr);

  for (int i = level - 1; i >= 0; i--) {
    while (curr->forward[i] && key > curr->forward[i]->key) {
      curr = curr->forward[i];
    }
    list[i] = curr;
  }

  Node<T> *node = new Node<T>(key, level);

  for (int i = 0; i < level; i++) {
    node->forward[i] = list[i]->forward[i];
    list[i]->forward[i] = node;
  }

  if (level > this->level)
    this->level = level;
}
