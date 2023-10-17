﻿#pragma once

/**
 * \brief Linked list
 * \tparam T - type_value
 * todo:
 *  pop_back(), pop_front(), clear(), reverse(), sort(), back(), front(), empty()
 */
template<typename T>
class List
{
  template<typename T>
  struct Node
  {
    Node() = default;
    Node( T val ) noexcept : value( val ) {}
    T value = T();
    Node * next = nullptr;
    Node * prev = nullptr;
  };

  Node<T>* head_ = nullptr;
  Node<T>* tail_ = nullptr;
  size_t size_ = 0;

public:

  List() = default;
  List( size_t size, T& value = T() )
  {
    //todo
  }
  List( const List& other )
  {
    //todo
  }
  ~List()
  {
    //todo
  }
  void push_back( const T& value )
  {
    if ( size_++ == 0 )
      head_ = tail_ = new Node<T>( value );
    else
    {
      auto p = new Node<T>( value );
      tail_->next = p;
      p->prev = tail_;
      tail_ = p;
    }
  }
  void push_front( const T & value )
  {
    if ( size_++ == 0 )
      head_ = tail_ = new Node<T>( value );
    else
    {
      auto p = new Node<T>( value );
      head_->prev = p;
      p->next = head_;
      head_ = p;
    }    
  }
  [[nodiscard]] size_t size() const noexcept
  {
    return size_;
  }
};