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
    std::cout << "grow to " << new_capacity << std::endl;
    DirectoryNode **new_table = new DirectoryNode *[new_capacity];

    for (int i = 0; i < capacity; i++)
    {
      DirectoryNode *node = table[i];
      while (node != nullptr)
      {
        int new_index = node->hash % new_capacity;
        if (new_table[new_index])
        {
          node->next = new_table[new_index];
          new_table[new_index] = node;
        }
        else
        {
          new_table[new_index] = node;
        }
        node = node->next;
      }
    }

    capacity = new_capacity;
    threshold = calculate_threshold();

    delete[] table;
    table = new_table;
  }

public:
  Directory(
      int capacity,
      float loadFactor = 0.75)
      : capacity(capacity),
        size(0),
        loadFactor(loadFactor)
  {
    threshold = calculate_threshold();
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
      if (node->key == value.get_name())
      {
        node->value = value;
        return;
      }
      node = node->next;
    }
    DirectoryNode *newNode = new DirectoryNode(value.get_name(), value, hash);
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
};

int main()
{
  Directory dictionary(8);

  dictionary.insert({"John Snow", "+485327095"});
  dictionary.insert({"Mike Tayson", "+235236231"});
  dictionary.insert({"gsdgsdgsd", "+235236231"});
  dictionary.insert({"532fwef32fe", "+235236231"});
  dictionary.insert({"f23fs23fsefs", "+235236231"});

  dictionary.remove("532fwef32fe");
  dictionary.lookup("532fwef32fe");

  dictionary.display();
}