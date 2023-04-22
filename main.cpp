#include <iostream>
#include <cstring>
#include <optional>

struct Entry
{
private:
  std::string name;
  std::string phone_number;

public:
  Entry(const std::string &name, const std::string &phone_number)
      : name(name),
        phone_number(phone_number) {}

  Entry(const Entry &entry)
      : name(entry.name), phone_number(entry.phone_number) {}

  const std::string &get_name() const
  {
    return this->name;
  }

  const std::string &get_phone_number() const
  {
    return this->phone_number;
  }

  void print() const
  {
    std::cout << "{ name: \"" << name << "\", phone_number: \"" << phone_number << "\" }" << std::endl;
  }
};

struct DirectoryNode
{
  std::string key;
  Entry value;
  std::size_t hash;
  DirectoryNode *next;

  DirectoryNode(const std::string &key, const Entry &value, std::size_t hash)
      : key(key),
        value(value),
        hash(hash),
        next(nullptr) {}

  DirectoryNode(const DirectoryNode &node) : key(node.key), value(node.value), hash(node.hash)
  {
    next = node.next ? new DirectoryNode(*node.next) : nullptr;
  }

  ~DirectoryNode()
  {
    delete next;
    next = nullptr;
  }
};

class Directory
{
private:
  DirectoryNode **table;
  std::hash<std::string> hasher;

  int capacity;
  int size;
  int threshold;
  float loadFactor;

  int calculate_threshold()
  {
    return capacity * loadFactor;
  }

  void grow(int new_capacity)
  {
    DirectoryNode **new_table = new DirectoryNode *[new_capacity];

    for (int i = 0; i < capacity; i++)
    {
      DirectoryNode *node = table[i];

      while (node != nullptr)
      {
        table[i] = node->next;

        int new_index = node->hash % new_capacity;
        // if node is no empty
        if (new_table[new_index])
        {
          node->next = new_table[new_index];
          new_table[new_index] = node;
        }
        else
        {
          node->next = nullptr;
          new_table[new_index] = node;
        }
        node = table[i];
      }
    }

    capacity = new_capacity;
    threshold = calculate_threshold();

    delete[] table;
    table = new_table;
  }

public:
  Directory(
      int unsigned capacity = 4,
      float loadFactor = 1)
      : capacity(capacity),
        size(0),
        loadFactor(loadFactor)
  {
    threshold = calculate_threshold();

    if (capacity == 0)
    {
      throw std::invalid_argument("Capacity can't be zero");
    }
    table = new DirectoryNode *[capacity];
    memset(table, 0, capacity * sizeof(DirectoryNode *));
  }

  Directory(const Directory &d)
      : capacity(d.capacity), size(d.size), threshold(d.threshold), loadFactor(d.loadFactor)
  {
    table = new DirectoryNode *[capacity];
    memset(table, 0, capacity * sizeof(DirectoryNode *));

    for (int i = 0; i < capacity; i++)
    {
      if (d.table[i] != nullptr)
      {
        table[i] = new DirectoryNode(*d.table[i]);
      }
    }
  }

  ~Directory()
  {
    for (int i = 0; i < capacity; ++i)
    {
      delete table[i];
    }
    delete[] table;
  }

  void insert(const Entry &value)
  {
    std::size_t hash = hasher(value.get_name());
    int index = hash % capacity;
    DirectoryNode *node = table[index];

    while (node != nullptr)
    {
      // if record with this key already exists
      if (node->key == value.get_name())
      {
        node->value = value;
        return;
      }
      node = node->next;
    }

    DirectoryNode *newNode = new DirectoryNode(value.get_name(),
                                               value,
                                               hash);

    // insert newNode at the beginning of the linked list
    newNode->next = table[index];
    table[index] = newNode;
    ++size;

    if (size >= threshold)
    {
      grow(capacity * 2);
    }
  }

  std::optional<Entry> get(const std::string &key) const
  {
    DirectoryNode *node = table[hasher(key) % capacity];

    while (node != nullptr)
    {
      if (node->key == key)
      {
        return std::make_optional(node->value);
      }
      node = node->next;
    }

    return std::nullopt;
  }

  void lookup(const std::string &key)
  {
    auto value = this->get(key);
    if (value.has_value())
    {
      value.value().print();
      return;
    }

    std::cout << "Key \"" << key << "\" not found" << std::endl;
  }

  void remove(const std::string &key)
  {
    int index = hasher(key) % capacity;
    DirectoryNode *node = table[index];
    DirectoryNode *prev = nullptr;
    while (node != nullptr)
    {
      if (node->key == key)
      {
        if (prev == nullptr)
        {
          table[index] = node->next;
        }
        else
        {
          prev->next = node->next;
        }

        // untie the node (if it has next) so as not to remove the whole chain recursively
        if (node->next)
        {
          node->next = nullptr;
        }
        delete node;

        --size;
        return;
      }
      prev = node;
      node = node->next;
    }
  }

  void display() const
  {
    for (int i = 0; i < capacity; i++)
    {
      DirectoryNode *node = table[i];
      while (node != nullptr)
      {
        node->value.print();
        node = node->next;
      }
    }
  }

  int get_size() const
  {
    return size;
  }

  unsigned int get_capacity()
  {
    return capacity;
  }
};

int main()
{
  // Create a new directory
  Directory dir;

  // Create some entries to insert
  Entry entry1("Alice", "123-456-7890");
  Entry entry2("Bob", "456-789-0123");
  Entry entry3("Charlie", "789-012-3456");

  // Insert the entries into the directory
  dir.insert(entry1);
  dir.insert(entry2);
  dir.insert(entry3);

  // Get an entry by its key and print its details
  std::optional<Entry> retrieved_entry = dir.get("Alice");
  if (retrieved_entry.has_value())
  {
    retrieved_entry.value().print();
  }
  else
  {
    std::cout << "No entry found for Alice" << std::endl;
  }

  // Remove an entry from the directory
  dir.remove("Bob");

  // Look up an entry by its key and print its details (using the lookup function)
  dir.lookup("Charlie");
}