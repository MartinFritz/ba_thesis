#include <cassert>
#include <optional>
#include <random>
#include <unordered_map>
#include <vector>
#include <iostream>


/**
 * Insert your simple HT here - no fancy optimizations needed
 * @tparam V Type of the value
 */
template <typename V> class myHashTable {

  // https://burtleburtle.net/bob/hash/integer.html
  /**
   * Simple 32 bit hash function
   * @param key
   * @return
   */
  static constexpr uint32_t hash(uint32_t key) {
    key = (key ^ 61) ^ (key >> 16);
    key = key + (key << 3);
    key = key ^ (key >> 4);
    key = key * 0x27d4eb2d;
    key = key ^ (key >> 15);
    //return key;
    return key%16777216;
  }

  // Possible storage structure for Chaining HT - Change it if you want
  struct Elem {
    uint32_t key;
    V value;
    Elem *next;

    ~Elem(){

    }
  };
  
  std::vector<Elem*> hashTable;

public:
  /**
   * Constructor
   */
  myHashTable() {
    // ## INSERT CODE HERE
    hashTable.resize(16777216); //2^24
  }

  /**
   * Destructor
   */
  ~myHashTable() { clear(); }

  /**
   * insert value to HT if not exists
   *
   * used for non-overwriting inserts
   *
   * @param key
   * @param value
   */
  void insert(uint32_t key, V value) {
    if (!count(key)) {
      operator[](key) = value;
    }
  }

  /**
   * remove element from HT
   *
   * used for remove
   *
   * @param key
   */
  void erase(uint32_t key) {
    // ## INSERT CODE HERE
    if (count(key)) {   
      uint32_t hashKey = hash(key);
      Elem *head = hashTable[hashKey];
    // Case 1: Element at the beginning
      if (head->key == key) {
        if (head->next == NULL) {
          delete(hashTable[hashKey]);
          hashTable[hashKey] = NULL;
          return;
        } else {
          hashTable[hashKey] = head->next;
          delete(head);
          head = NULL;
          return;
        }
      }
      // Case 2: Element in the middle / end
      Elem *nextup = head->next;
      while (nextup != NULL) {
        if (nextup->key == key) {
          head->next = nextup->next;
          delete nextup;
          nextup = NULL;
          return;
        }
        head = nextup;
        nextup = nextup->next;
      }
    }
  }

  /**
   * get element from HT wrapped in optional
   *
   * used for const lookup
   *
   * @param key
   * @return optional containing the element or nothing if not exists
   */
  std::optional<V> get(uint32_t key) const {
    if (count(key)) {
      uint32_t hashKey = hash(key);
      Elem *current = hashTable[hashKey];
      while (current != NULL) {
        if (current->key == key) {
          return current->value;
        } else {
          current = current->next;
        }
      }
    }
    return std::nullopt;
  }

  /**
   * get reference to existing HT element or insert new at key
   *
   * used for lookups, inserts, and editing the HT elements
   *
   * @param key
   * @return reference to HT element
   */
  V &operator[](uint32_t key) {
    // ## INSERT CODE HERE
    // return 0;

    uint32_t hashKey = hash(key);
    
    if (hashTable[hashKey] == NULL) {
      Elem *current = new Elem();
      hashTable[hashKey] = current;
      current->key = key;
      return hashTable[hashKey]->value;
    }

    Elem *current = hashTable[hashKey];
    Elem *previous;
    while (current != NULL) {
      if (current->key == key) {
        return current->value;
      }
      if (current->next == NULL) {
        previous = current;
      }
      current = current->next;
    }
    Elem *inserter = new Elem();
    inserter->key = key;
    previous->next = inserter;
    return inserter->value;
    
  }

  /**
   * get count of contained elements
   *
   * used for containment check
   *
   * @param key
   * @return 0 if not contained, 1 if contained
   */
  uint64_t count(uint32_t key) const {
    uint32_t hashKey = hash(key);
    Elem *current = hashTable[hashKey];
    while (current != NULL) {
      if (current->key == key) {
        return 1;
      } else {
        current = current->next;
      }
    }
    return 0;
  }

  /**
   * get number of stored element
   *
   * @return HT size
   */
  std::size_t size() const {
    // ## INSERT CODE HERE
    size_t counter = 0;
    for(auto i : hashTable) {
      Elem *current = i;
      while (current != NULL) {
        counter++;
        current = current->next;
      }
    }
    return counter;
  }

  /**
   * is HT empty
   *
   * @return true if HT empty
   */
  bool empty() const {
    // ## INSERT CODE HERE
    return (size() == 0);
  }

  /**
   * empty the HT
   */
  void clear() {
    for (size_t i = 0; i < hashTable.size(); i++) {
      if (hashTable[i] != NULL) {
        Elem *current = hashTable[i];
        while (current != NULL) {
          Elem *tmp = current->next;
          delete current;
          current = tmp;
          tmp = NULL;
        }
      }
    }
    hashTable.clear();
    hashTable.resize(16777216);
  }
};