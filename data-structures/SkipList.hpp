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
  ~SkipList();

  void insert(const T &key);
  void remove(const T &key);
  Node<T> *search(const T &key);
};

template <typename T> SkipList<T>::~SkipList() {
  Node<T> *curr = head;

  while (curr) {
    Node<T> *next = curr->forward[0];
    delete curr;
    curr = next;
  }
}

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
  if (search(key))
    return;

  int curr_level = assign_level();

  Node<T> *curr = head;
  std::vector<Node<T> *> list(MAX_LEVEL, nullptr);

  for (int i = level - 1; i >= 0; i--) {
    while (curr->forward[i] && key > curr->forward[i]->key) {
      curr = curr->forward[i];
    }
    list[i] = curr;
  }

  Node<T> *node = new Node<T>(key, curr_level);

  if (curr_level > level)
    level = curr_level;

  for (int i = 0; i < curr_level; i++) {
    node->forward[i] = list[i]->forward[i];
    list[i]->forward[i] = node;
  }
}

template <typename T> void SkipList<T>::remove(const T &key) {
  Node<T> *curr = head;
  std::vector<Node<T> *> list(MAX_LEVEL, nullptr);

  for (int i = level - 1; i >= 0; i--) {
    while (curr->forward[i] && curr->forward[i]->key < key) {
      curr = curr->forward[i];
    }
    list[i] = curr;
  }

  curr = curr->forward[0];
  if (!curr || curr->key != key)
    return;

  for (int i = 0; i < level; i++) {
    if (list[i]->forward[i] != curr)
      break;
    list[i]->forward[i] = curr->forward[i];
  }

  delete curr;

  while (level > 1 && head->forward[level - 1] == nullptr)
    level--;
}

template <typename T> Node<T> *SkipList<T>::search(const T &key) {
  Node<T> *curr = head;

  for (int i = level - 1; i >= 0; i--) {
    while (curr->forward[i] && curr->forward[i]->key < key)
      curr = curr->forward[i];
  }

  curr = curr->forward[0];
  if (curr && curr->key == key)
    return curr;
  return nullptr;
}
