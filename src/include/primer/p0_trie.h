//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// p0_trie.h
//
// Identification: src/include/primer/p0_trie.h
//
// Copyright (c) 2015-2022, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/exception.h"
#include "common/rwlatch.h"
#include "common/logger.h"
// typedef std::unique_ptr<std::unordered_map<char, std::unique_ptr<TrieNode>>> childMapPtr;
namespace bustub {

/**
 * TrieNode is a generic container for any node in Trie.
 */
class TrieNode {
 public:
  /**
   * TODO(P0): Add implementation
   *
   * @brief Construct a new Trie Node object with the given key char.
   * is_end_ flag should be initialized to false in this constructor.
   *
   * @param key_char Key character of this trie node
   */
  explicit TrieNode(char key_char) {
    is_end_ = false;
    key_char_ = key_char;
    children_.clear();
  }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Move constructor for trie node object. The unique pointers stored
   * in children_ should be moved from other_trie_node to new trie node.
   *
   * @param other_trie_node Old trie node.
   */
  TrieNode(TrieNode &&other_trie_node) noexcept {
    is_end_ = other_trie_node.is_end_;
    key_char_ = other_trie_node.key_char_;
    children_.swap(other_trie_node.children_);
  }

  // TrieNode(char key_char,std::unordered_map<char, std::unique_ptr<TrieNode>>&& chidren){
  //   key_char_ = key_char;
  //   is_end_ = false;
  //   children_.std::swap(chidren.children_);
  // }
  /**
   * @brief Destroy the TrieNode object.
   */
  virtual ~TrieNode() = default;

  /**
   * TODO(P0): Add implementation
   *
   * @brief Whether this trie node has a child node with specified key char.
   *
   * @param key_char Key char of child node.
   * @return True if this trie node has a child with given key, false otherwise.
   */
  bool HasChild(char key_char) const {
      return children_.count(key_char) != 0;
  }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Whether this trie node has any children at all. This is useful
   * when implementing 'Remove' functionality.
   *
   * @return True if this trie node has any child node, false if it has no child node.
   */
  bool HasChildren() const {
    return !children_.empty();
  }

  bool HasMultiChildren() const {
    return children_.size() > 1;
  }
  // std::unique_ptr<TrieNode>* createNewTrieNode() {
  //   std::unique_ptr<TrieNode> newTrieNode = std::make_unique<TrieNode>();
  // }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Whether this trie node is the ending character of a key string.
   *
   * @return True if is_end_ flag is true, false if is_end_ is false.
   */
  bool IsEndNode() const { return is_end_; }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Return key char of this trie node.
   *
   * @return key_char_ of this trie node.
   */
  char GetKeyChar() const { return key_char_; }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Insert a child node for this trie node into children_ map, given the key char and
   * unique_ptr of the child node. If specified key_char already exists in children_,
   * return nullptr. If parameter `child`'s key char is different than parameter
   * `key_char`, return nullptr.
   *
   * Note that parameter `child` is rvalue and should be moved when it is
   * inserted into children_map.
   *
   * The return value is a pointer to unique_ptr because pointer to unique_ptr can access the
   * underlying data without taking ownership of the unique_ptr. Further, we can set the return
   * value to nullptr when error occurs.
   *
   * @param key Key of child node
   * @param child Unique pointer created for the child node. This should be added to children_ map.
   * @return Pointer to unique_ptr of the inserted child node. If insertion fails, return nullptr.
   */
  std::unique_ptr<TrieNode>* InsertChildNode(char key_char, std::unique_ptr<TrieNode> &&child) {
    if (HasChild(key_char) || key_char != child -> key_char_) { return nullptr; }
    children_[key_char] = std::forward<std::unique_ptr<TrieNode>>(child);
    std::unique_ptr<TrieNode>* ret = & (children_[key_char]);
    return ret;
  }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Get the child node given its key char. If child node for given key char does
   * not exist, return nullptr.
   *
   * @param key Key of child node
   * @return Pointer to unique_ptr of the child node, nullptr if child
   *         node does not exist.
   */
  std::unique_ptr<TrieNode>* GetChildNode(char key_char) {
    if (children_.count(key_char) == 0) { return nullptr; }
    return &children_[key_char];
  }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Remove child node from children_ map.
   * If key_char does not exist in children_, return immediately.
   *
   * @param key_char Key char of child node to be removed
   */
  void RemoveChildNode(char key_char) {
    if (children_.count(key_char) == 0) { return; }
    children_[key_char] = nullptr;
    children_.erase(key_char);
  }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Set the is_end_ flag to true or false.
   *
   * @param is_end Whether this trie node is ending char of a key string
   */
  void SetEndNode(bool is_end) {
    is_end_ = is_end;
  }

  // void moveChildrenFromOther(TrieNode &&other_trie_node) {
  //   std::unique_ptr<TrieNode> otherTrieNode = std::make_unique<TrieNode>(other_trie_node);
  //   children_ = std::move(otherTrieNode -> );
  // }

//  protected:
  /** Key character of this trie node */
  char key_char_;
  /** whether this node marks the end of a key */
  bool is_end_{false};
  /** A map of all child nodes of this trie node, which can be accessed by each
   * child node's key char. */
  std::unordered_map<char, std::unique_ptr<TrieNode>> children_;
};

/**
 * TrieNodeWithValue is a node that marks the ending of a key, and it can
 * hold a value of any type T.
 */
template <typename T>
class TrieNodeWithValue : public TrieNode {
 private:
  /* Value held by this trie node. */
  T value_;

 public:
  /**
   * TODO(P0): Add implementation
   *
   * @brief Construct a new TrieNodeWithValue object from a TrieNode object and specify its value.
   * This is used when a non-terminal TrieNode is converted to terminal TrieNodeWithValue.
   *
   * The children_ map of TrieNode should be moved to the new TrieNodeWithValue object.
   * Since it contains unique pointers, the first parameter is a rvalue reference.
   *
   * You should:
   * 1) invoke TrieNode's move constructor to move data from TrieNode to
   * TrieNodeWithValue.
   * 2) set value_ member variable of this node to parameter `value`.
   * 3) set is_end_ to true
   *
   * @param trieNode TrieNode whose data is to be moved to TrieNodeWithValue
   * @param value
   */
  TrieNodeWithValue(TrieNode &&trieNode, T value): TrieNode(std::forward<TrieNode>(trieNode)) {
    SetEndNode(true);
    this -> value_ = value;
  }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Construct a new TrieNodeWithValue. This is used when a new terminal node is constructed.
   *
   * You should:
   * 1) Invoke the constructor for TrieNode with the given key_char.
   * 2) Set value_ for this node.
   * 3) set is_end_ to true.
   *
   * @param key_char Key char of this node
   * @param value Value of this node
   */
  TrieNodeWithValue(char key_char, T value):TrieNode(key_char), value_(value) {
    SetEndNode(true);
  }

  /**
   * @brief Destroy the Trie Node With Value object
   */
  ~TrieNodeWithValue() override = default;

  /**
   * @brief Get the stored value_.
   *
   * @return Value of type T stored in this node
   */
  T GetValue() const { return value_; }
};

/**
 * Trie is a concurrent key-value store. Each key is a string and its corresponding
 * value can be any type.
 */
class Trie {
 private:
  /* Root node of the trie */
  std::unique_ptr<TrieNode> root_;
  /* Read-write lock for the trie */
  ReaderWriterLatch latch_;

 public:
  /**
   * TODO(P0): Add implementation
   *
   * @brief Construct a new Trie object. Initialize the root node with '\0'
   * character.
   */
  Trie() {
    auto root = std::make_unique<TrieNode>('\0');
    root_ = std::move(root);
  }

  /**
   * TODO(P0): Add implementation
   *
   * @brief Insert key-value pair into the trie.
   *
   * If the key is an empty string, return false immediately.
   *
   * If the key already exists, return false. Duplicated keys are not allowed and
   * you should never overwrite value of an existing key.
   *
   * When you reach the ending character of a key:
   * 1. If TrieNode with this ending character does not exist, create new TrieNodeWithValue
   * and add it to parent node's children_ map.
   * 2. If the terminal node is a TrieNode, then convert it into TrieNodeWithValue by
   * invoking the appropriate constructor.
   * 3. If it is already a TrieNodeWithValue,
   * then insertion fails and returns false. Do not overwrite existing data with new data.
   *
   * You can quickly check whether a TrieNode pointer holds TrieNode or TrieNodeWithValue
   * by checking the is_end_ flag. If is_end_ == false, then it points to TrieNode. If
   * is_end_ == true, it points to TrieNodeWithValue.
   *
   * @param key Key used to traverse the trie and find the correct node
   * @param value Value to be inserted
   * @return True if insertion succeeds, false if the key already exists
   */
  template <typename T>
  bool Insert(const std::string &key, T value) {
    if (key.length() == 0) { return false; }
    latch_.WLock();
    TrieNode* ptr = SearchForKey(key, 0, &root_);
    if (ptr) {
      // LOG_DEBUG(" Insert key: %s FAILED, already Exists ", key.c_str());
      latch_.WUnlock();
      return false;
    }
    // LOG_DEBUG(" Insert key: %s BEGIN ", key.c_str());
    std::unique_ptr<TrieNode>* current = &root_;
    // std::unique_ptr<T> valuePtr = std::unique_ptr<T>(std::move(value));
    for (unsigned int i = 0; i < key.length(); i++) {
      char ch = key[i];
      std::unique_ptr<TrieNode> *search = (*current) -> GetChildNode(ch);

      // 找不到, 需要在此处开始新建Node
      if (search == nullptr) {
        // newNode = std::make_shared<TrieNode>(*children);
        // todo： 从该节点开始 一直新建结点插入；
        InsertFromNode(key, i, current, value);
        // LOG_DEBUG(" Insert key: %s DONE with insert NewNodes ", key.c_str());
        latch_.WUnlock();
        return true;
      }
      if (i == key.length() - 1 && !(*search) -> IsEndNode()) {
        // 但not value node 需要修改node类型
          // LOG_DEBUG(" Insert key: %s DONE with modify Node ", key.c_str());
        std::unique_ptr<TrieNodeWithValue<T>> new_node(new TrieNodeWithValue<T>(std::move(**search), value));
        (*current) -> RemoveChildNode(ch);
        (*current) -> InsertChildNode(ch, std::move(new_node));
        latch_.WUnlock();
        return true;
      }
      current = search;
    }
    // LOG_DEBUG("Insert key: %s DONE with false", key.c_str());
    latch_.WUnlock();
    return false;
  }
  template <typename T>
  void InsertFromNode(const std::string &key, unsigned int start, std::unique_ptr<TrieNode>* current, T value) const {
    // auto children = getChildren(current);
    // LOG_DEBUG("InsertFromNode key: %s BEGIN with idx %d", key.c_str(), start);
    if (start == key.length() - 1) {
      auto new_node_with_val = std::make_unique<TrieNodeWithValue<T>> (key[start], value);
      (*current) -> InsertChildNode(key[start], std::move(new_node_with_val));
      // LOG_DEBUG(" InsertFromNode key: %s END ", key.c_str());
      return;
    }
    std::unique_ptr<TrieNode> new_node(new TrieNode(key[start]));
    std::unique_ptr<TrieNode>* next_node = (*current) -> InsertChildNode(key[start], std::move(new_node));
    InsertFromNode(key, start + 1, next_node, value);
  }
  /**
   * TODO(P0): Add implementation
   *
   * @brief Remove key value pair from the trie.
   * This function should also remove nodes that are no longer part of another
   * key. If key is empty or not found, return false.
   *
   * You should:
   * 1) Find the terminal node for the given key.
   * 2) If this terminal node does not have any children, remove it from its
   * parent's children_ map.
   * 3) Recursively remove nodes that have no children and are not terminal node
   * of another key.
   *
   * @param key Key used to traverse the trie and find the correct node
   * @return True if the key exists and is removed, false otherwise
   */
  bool Remove(const std::string &key) {
    if (key.empty()) {
      return false;
    }
    latch_.WLock();
    std::unique_ptr<TrieNode>* pre = nullptr;
    std::unique_ptr<TrieNode>* cur = &root_;
    for (auto ch : key) {
      std::unique_ptr<TrieNode>* child_node = (*cur) -> GetChildNode(ch);
      if (child_node == nullptr) {
        latch_.WUnlock();
        return false;
      }
      pre = cur;
      cur = child_node;
    }
    if ((!(*cur) -> HasChildren()) && cur != &root_) {
      DeleteNodeIfNeeded(key);
    } else {
      // cast to trieNodeWithVal

      std::unique_ptr<TrieNode> new_node = std::move(*cur);
      if (pre != nullptr) {
        (*pre) -> RemoveChildNode(key.back());
        (*pre) -> InsertChildNode(key.back(), std::move(new_node));
      } else {
        root_ = std::move(new_node);
      }
    }
    latch_.WUnlock();
    return true;
  }


  void DeleteNodeIfNeeded(const std::string &key);
  void RemoveNodeR(const std::string &key, unsigned int start, std::unique_ptr<TrieNode>* current) {
    if (current == nullptr) { return; }
    if (start == key.length()) { return; }
    auto next_child = (*current) -> GetChildNode(key[start]);
    if (next_child != nullptr) {
      RemoveNodeR(key, start + 1, next_child);
      (*current) -> RemoveChildNode(key[start]);
    }
  }
  TrieNode* SearchForKey(const std::string &key, unsigned int start, std::unique_ptr<TrieNode>* current) const;
  /**
   * TODO(P0): Add implementation
   *
   * @brief Get the corresponding value of type T given its key.
   * If key is empty, set success to false.
   * If key does not exist in trie, set success to false.
   * If the given type T is not the same as the value type stored in TrieNodeWithValue
   * (ie. GetValue<int> is called but terminal node holds std::string),
   * set success to false.
   *
   * To check whether the two types are the same, dynamic_cast
   * the terminal TrieNode to TrieNodeWithValue<T>. If the casted result
   * is not nullptr, then type T is the correct type.
   *
   * @param key Key used to traverse the trie and find the correct node
   * @param success Whether GetValue is successful or not
   * @return Value of type T if type matches
   */
  template <typename T>
  T GetValue(const std::string &key, bool *success) {
    *success = false;
    latch_.RLock();
    TrieNode* ptr = SearchForKey(key, 0, &root_);
    auto ret = dynamic_cast<TrieNodeWithValue<T> *> (ptr);
    *success = !(ret == nullptr);
    if (*success) {
      auto val = ret -> GetValue();
      latch_.RUnlock();
      return val;
    }
    latch_.RUnlock();
    return T();
  }
};



TrieNode* Trie::SearchForKey(const std::string &key, unsigned int start, std::unique_ptr<TrieNode>* current) const {
  if ((*current) -> IsEndNode() && key.length() == start) {
    return current -> get();
  }
  if (key.length() == start) {
    return nullptr;
  }
  std::unique_ptr<TrieNode>* next_node = nullptr;
  // 没找到
  if ((*current) -> HasChild(key[start])) {
    next_node = (*current) -> GetChildNode(key[start]);
    return SearchForKey(key, start + 1, next_node);
  }
  return nullptr;
}

// Below are explicit instantiation of template functions.
//
// Generally people would write the implementation of template classes and functions in the header file. However, we
// separate the implementation into a .cpp file to make things clearer. In order to make the compiler know the
// implementation of the template functions, we need to explicitly instantiate them here, so that they can be picked up
// by the linker.

void Trie::DeleteNodeIfNeeded(const std::string &key) {
  std::unique_ptr<TrieNode>* mark = nullptr;   // 分叉节点,该节点的children有多个nextNode
  std::unique_ptr<TrieNode>* cur = &root_;
  unsigned int idx = 0;
  for (unsigned int i = 0; i < key.length(); i++) {
    char ch = key[i];
    if ((*cur) -> HasMultiChildren() || (*cur) -> IsEndNode()) {
      mark = cur;
      idx = i;
    }
    cur = (*cur) -> GetChildNode(ch);
  }
  // RemoveNodeR(key, idx, mark);
  (*mark) -> RemoveChildNode(key[idx]);
  // for (unsigned int i = idx; i < key.length(); i++) {
  //   char ch = key[i];
  //   auto next_node = (*mark) -> GetChildNode(ch);
  //   (*mark) -> RemoveChildNode(ch);
  //   mark = next_node;
  // }
}
}  // namespace bustub
