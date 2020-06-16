#include <list>
using namespace std;
// Skeleton for the ExpandableHashMap class template.  You must implement the first six
// member functions.

template<typename KeyType, typename ValueType>
class ExpandableHashMap
{
public:
    ExpandableHashMap(double maximumLoadFactor = 0.5);
    ~ExpandableHashMap();
    void reset();
    int size() const;
    void associate(const KeyType& key, const ValueType& value);

      // for a map that can't be modified, return a pointer to const ValueType
    const ValueType* find(const KeyType& key) const;

    // for a modifiable map, ret*u**rn a pointer to modifiable ValueType
    ValueType* find(const KeyType& key)
    {
        return const_cast<ValueType*>(const_cast<const ExpandableHashMap*>(this)->find(key));
    }

      // C++11 syntax for preventing copying and assignment
    ExpandableHashMap(const ExpandableHashMap&) = delete;
    ExpandableHashMap& operator=(const ExpandableHashMap&) = delete;

private:
    struct Node
    {
        KeyType key;
        ValueType val;
    };

    double m_maxLF;
    int m_nBuckets;
    int m_nItem;
    list<Node*>* m_hash;  // an array of list of Node pointers
    
    void clean() // delete all the lists
    {
        for(int i = 0; i < m_nBuckets; i++)
        {
            list<Node*> p = m_hash[i];
            p.clear();
        }
    }
    unsigned int getBucketNumber(const KeyType& key) const{
        unsigned int hasher(const KeyType& key);
        return hasher(key) % m_nBuckets; //use modulus to generate hash key
    }
};

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType,ValueType>::ExpandableHashMap(double maximumLoadFactor)
{
    if(maximumLoadFactor <= 0)
        m_maxLF = 0.5;
    m_maxLF = maximumLoadFactor;
    m_nBuckets = 8;
    m_hash = new list<Node*>[m_nBuckets];
}

template<typename KeyType, typename ValueType>
ExpandableHashMap<KeyType,ValueType>::~ExpandableHashMap()
{
    clean();
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType,ValueType>::reset()
{
    clean();
    m_nBuckets = 8;
    m_hash = new list<Node*>[m_nBuckets];
}

template<typename KeyType, typename ValueType>
int ExpandableHashMap<KeyType,ValueType>::size() const
{
    return m_nBuckets;
}

template<typename KeyType, typename ValueType>
void ExpandableHashMap<KeyType,ValueType>::associate(const KeyType& key, const ValueType& value)
{
    // if already in the hash table, upadate the value
    if(find(key) != nullptr)
    {
        *find(key) = value;
        return;
    }
    
    // if above the load factor
    if((m_nItem + 1) * 1.0 / (m_nBuckets * 1.0) > m_maxLF)
    {
        // copy over old hash table
        list<Node*>* prevHash = m_hash;
        int prevSize = m_nBuckets;
        
        // create new double-sized one
        m_nBuckets *= 2;
        m_hash = new list<Node*>[m_nBuckets];
        
        for(int i = 0; i < prevSize; i++)
        {
            list<Node*> li = prevHash[i];
            if(!li.empty())
            {
                // auto <==> typename list<Node*>::iterator
                for(auto it = li.begin(); it != li.end(); )
                {
                    // hashing the old key and create new node
                    Node* newNode = new Node;
                    newNode->key = (*it)->key;
                    newNode->val = (*it)->val;
                    unsigned int m = getBucketNumber(newNode->key);
                    int num = m % m_nBuckets;
                    // push the new node with old value into the slot
                    m_hash[num].push_back(newNode);
                    // delete the old
                    it = li.erase(it);
                }
            }
        }
        delete [] prevHash;
    }
    
    // if not above, just insert
    unsigned int m = getBucketNumber(key);
    int num = m % m_nBuckets;
    Node* newNode = new Node;
    newNode->key = key;
    newNode->val = value;
    m_hash[num].push_back(newNode);
    
    m_nItem++;
}

template<typename KeyType, typename ValueType>
const ValueType* ExpandableHashMap<KeyType,ValueType>::find(const KeyType& key) const
{
    unsigned int m = getBucketNumber(key);
    int num = m % m_nBuckets;
    
    list<Node*> li = m_hash[num];
    for(auto it = li.begin(); it!= li.end(); it++)
        if((*it)->key == key)
            return &((*it)->val);
    return nullptr;
}
