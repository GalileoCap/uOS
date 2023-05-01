#ifndef __LIST_H__
#define __LIST_H__

#include <defines.h>

namespace std {

template<typename T>
struct list {
  using value_type = T;
  struct node;
  struct iterator;

  list();
  list(const list&);
  ~list();

  bool empty() const;
  size_t size() const;

  value_type& front();
  value_type& back();

  void push_front(const value_type&);
  void push_back(const value_type&);

  value_type pop_front();
  value_type pop_back();

  void clear();

  iterator begin();
  iterator end();
  //TODO: Reverse iterator

  void _deleteNodes();

  size_t _size;
  struct node *_front, *_back; 

  struct node {
    using value_type = T;
    node(const value_type&, struct node*, struct node*);
    node(const struct node&);
    node& operator=(const struct node&);

    value_type value;
    struct node *prev, *next;
  };

  struct iterator {
    using value_type = T;

    iterator(struct list<T>::node*);
    iterator& operator=(const struct iterator&);

    value_type& operator*() const;

    iterator& operator++();

    bool operator==(const iterator&) const;
    bool operator!=(const iterator&) const;

    struct list<T>::node *_node;
  };
};

}

#include "../list.ipp"

#endif // __LIST_H__
