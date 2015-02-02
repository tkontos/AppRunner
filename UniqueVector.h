#pragma once

template<class Key, class Data, template<class, class> class KeyGen, template<class> class Pred = std::equal_to>
class unique_vector_t
{
public:
   unique_vector_t(KeyGen<Key, Data> g = KeyGen<Key, Data>(), Pred<Key> p = Pred<Key>())
      : m_keygen(g), m_pred(p) { }

   void push_back(const Data &d)
   {
      if (Find(m_keygen(d)) == m_data.end())
      {
         m_data.push_back(d);
      }
   }

   const Data *operator[](const Key key) const
   {
      auto it = Find(key);
      return it == m_data.end() ? nullptr : &*it;
   }

   using DataList = std::vector < Data > ;
   const DataList &GetData() const { return m_data; }

   using const_iterator = typename std::vector<Data>::const_iterator;
   const_iterator   begin() const { return m_data.cbegin(); }
   const_iterator   end()   const { return m_data.cend(); }

private:
   bool                 m_sorted = false;
   KeyGen<Key, Data>    m_keygen;
   Pred<Key>            m_pred;
   DataList             m_data;

   template<class T>
   typename std::enable_if<!std::is_pointer<T>::value, const_iterator>::type Find(const T &key) const
   {
      return std::find_if(m_data.begin(), m_data.end(), [this, &key](const Data &d) { return m_pred(key, m_keygen(d)); });
   }

   template<class T>
   typename std::enable_if<std::is_pointer<T>::value, const_iterator>::type Find(const T key) const
   {
      return std::find_if(m_data.begin(), m_data.end(), [this, &key](const Data &d) { return m_pred(key, m_keygen(d)); });
   }
};
