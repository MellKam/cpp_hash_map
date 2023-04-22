Symulacja książki telefonicznej

Napisz klasę Entry zawierającą nazwisko abonenta i numer telefonu (oba pola typu string). Klasa powinna zawierać konstruktor inicjalizujący pola, funkcję print() wypisującą wartości pól oraz gettery.

Klasa Directory reprezentuje książkę telefoniczną. Jej struktura wygląda następująco:

```cpp
class Directory {
public:
    Directory(int max_size); // set up empty directory of entries (allocate array)
    Directory(const Directory &d); // copy constructor
    ~Directory(); // free memory
    void insert(const Entry &entry); // Insert an entry into the directory.
    void lookup(const string &name) const; // display the directory entry for a name
    void remove(const string &name); // remove an entry.
    void display() const; // display the current directory.
private:
    int	max_size; // the maximum allowable number of entries
    int current_size; // the current number of entries
    Entry* entry_list; // pointer to the list of entries
    void grow(); // double the maximum size, when required.
    int find_name(const string &name) const; // return index of an entry, given a name.
};
```
Funkcja main() testuje działanie klasy