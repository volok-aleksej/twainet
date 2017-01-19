#ifndef LIST_HPP
#define LIST_HPP

namespace twnstd {
    
template<typename Object>
class list
{
public:
    class iterator
    {
    public:
        iterator ()
        : m_next(0), m_prev(0), m_object(0){}
        
        explicit iterator(const Object& object)
        : m_next(0), m_prev(0)
        {
            m_object = new Object(object);
        }
        
        iterator(const iterator& it)
        {
            operator = (it);
        }
        
        ~iterator() {
        }
        
        bool operator != (const iterator& obj_iter)
        {
            return m_object != obj_iter.m_object;
        }
        
        void operator = (const iterator& obj_iter)
        {
            m_next = obj_iter.m_next;
            m_prev = obj_iter.m_prev;
            m_object = obj_iter.m_object;
        }
        
        Object& operator * ()
        {
            return *m_object;
        }
        
        Object* operator -> ()
        {
            return m_object;
        }
        
        void operator ++ ()
        {
            *this = *m_next;
        }
        
        void operator -- ()
        {
            *this = *m_prev;
        }
        
        bool operator == (const iterator& it) const
        {
            return m_object == it.m_object;
        }
    private:
        friend class list<Object>;
        iterator* m_next;
        iterator* m_prev;
        Object* m_object;
    };    
    
    list()
    : m_objectsLen(0)
    {
        m_begin = m_end = new iterator;
    }
    
    ~list()
    {
        iterator* it = m_begin;
        while(it != m_end)
        {
            delete it->m_object;
            it = it->m_next;
            delete it->m_prev;
        }
        
        delete m_end;
    }
    
    iterator insert(const iterator& it, const Object& object)
    {
        iterator* newIter = new iterator(object);
        if(it == *m_end && !m_end->m_prev)
        {
            m_begin = newIter;
            newIter->m_next = m_end;
            m_end->m_prev = newIter;
        }
        else
        {
            iterator* iter;
            if(it.m_prev)
                iter = it.m_prev->m_next;
            else 
                iter = it.m_next->m_prev;
            newIter->m_next = iter;
            newIter->m_prev = iter->m_prev;
            if(iter->m_prev)
                iter->m_prev->m_next = newIter;
            iter->m_prev = newIter;
            
            if(it == *m_begin)
            {
                m_begin = newIter;
            }
        }
        m_objectsLen++;
        return *newIter;
    }
    
    iterator erase(const iterator& it)
    {
        if(*m_end == it)
        {
            return *m_end;
        }
        iterator* ret = it.m_next;
        
        if(*m_begin == it)
        {
            m_begin = it.m_next;
        }
        
        iterator* iter = it.m_next->m_prev;
        it.m_next->m_prev = it.m_prev;
        if(it.m_prev)
            it.m_prev->m_next = it.m_next;
        delete iter->m_object;
        delete iter;
        m_objectsLen--;
        return *ret;
    }
    
    iterator end()
    {
        return *m_end;
    }
    
    iterator begin()
    {
        return *m_begin;
    }
    
    unsigned int size()
    {
	return m_objectsLen;
    }
    
private:
    iterator* m_begin;
    iterator* m_end;
    unsigned int m_objectsLen;
};

}

#endif/*LIST_HPP*/