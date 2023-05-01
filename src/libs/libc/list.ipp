#include <list.h>

namespace std {

/************************************************************/
/* S: List **************************************************/

template<typename T>
list<T>::list() : _size(0), _front(NULL), _back(NULL) {}

template<typename T>
list<T>::list(const list<T> &otherList) : list() {
  *this = otherList;
}

template<typename T>
list<T>::~list() {
  _deleteNodes();
}

template<typename T>
bool list<T>::empty() const {
  return size() == 0;
}

template<typename T>
size_t list<T>::size() const {
  return _size;
}

template<typename T>
list<T>::value_type& list<T>::front() {
  return _front->value;
}

template<typename T>
list<T>::value_type& list<T>::back() {
  return _back->value;
}

template<typename T>
void list<T>::push_front(const list<T>::value_type &value) {
  _front = new list<T>::node(value, NULL, _front);
  if (_back == NULL)
    _back = _front;
}

template<typename T>
void list<T>::push_back(const list<T>::value_type &value) {
  _back = new list<T>::node(value, _back, NULL);
  if (_front == NULL)
    _front = _back;
}

template<typename T>
list<T>::value_type list<T>::pop_front() {
  list<T>::node *prevFront = _front;

  _size -= 1;
  _front = prevFront->next;
  if (_front == NULL)
    _back = NULL;

  T res = prevFront->value;
  delete prevFront;
  return res;
}

template<typename T>
list<T>::value_type list<T>::pop_back() {
  list<T>::node *prevBack = _back;

  _size -= 1;
  _back = prevBack->prev;
  if (_back == NULL)
    _front = NULL;

  T res = prevBack->value;
  delete prevBack;
  return res;
}

template<typename T>
void list<T>::clear() {
  _deleteNodes();
}

template<typename T>
typename list<T>::iterator list<T>::begin() {
  return list<T>::iterator(_front);
}

template<typename T>
typename list<T>::iterator list<T>::end() {
  return list<T>::iterator(NULL);
}

template<typename T>
void list<T>::_deleteNodes() {
  list<T>::node *node = _front, *next;
  while (node != NULL) {
    next = node->next;
    delete node;
    node = next;
  }

  _size = 0;
  _front = NULL;
  _back = NULL;
}

/************************************************************/
/* S: List::Node ********************************************/

template<typename T>
list<T>::node::node(const list<T>::value_type &_value, node *_prev, node *_next)
  : value(_value), prev(_prev), next(_next) {
  if (prev != NULL)
    prev->next = this;
  if (next != NULL)
    next->prev = this;
}

template<typename T>
list<T>::node::node(const node &otherNode) : node() {
  *this = otherNode;
}

template<typename T>
list<T>::node& list<T>::node::operator=(const node &otherNode) {
  value = otherNode.value;
  prev = otherNode.prev;
  next = otherNode.next;

  return *this;
}

/************************************************************/
/* S: List::Iterator ****************************************/

template<typename T>
list<T>::iterator::iterator(list<T>::node *node) : _node(node) {}

template<typename T>
list<T>::iterator& list<T>::iterator::operator=(const iterator &otherIterator) {
  _node = otherIterator._node;
  return *this;
}

template<typename T>
list<T>::iterator::value_type& list<T>::iterator::operator*() const {
  return _node->value;
}

template<typename T>
typename list<T>::iterator& list<T>::iterator::operator++() {
  _node = _node->next;
  return *this;
}

template<typename T>
bool list<T>::iterator::operator==(const iterator &otherIterator) const {
  return _node == otherIterator._node;
}

template<typename T>
bool list<T>::iterator::operator!=(const iterator &otherIterator) const {
  return !(*this == otherIterator);
}

}
