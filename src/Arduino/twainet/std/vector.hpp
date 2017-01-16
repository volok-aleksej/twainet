#ifndef VECTOR_HPP
#define VECTOR_HPP

namespace twnstd {

template<typename Object>
class vector
{
public:
    vector()
    : m_objectsLen(0), m_objectsBuf(4)
    {
        m_objects = new Object[m_objectsBuf];
    }
    
    ~vector()
    {
        delete [] m_objects;
    }
    
    void push_back(const Object& object)
    {
        if(m_objectsBuf >= m_objectsLen){
            m_objectsBuf += 4;
            Object* newContainer = new Object[m_objectsBuf];
            for(unsigned int i = 0; i < m_objectsLen; i++)
            {
                newContainer[i] = m_objects[i];
            }
            delete m_objects;
            m_objects = newContainer;
        }
        m_objects[m_objectsLen] = object;
        m_objectsLen++;
    }
    
    bool resize(unsigned int len)
    {
        if(m_objectsLen <= len)
        {
            m_objectsBuf = len + 4;
            Object* newContainer = new Object[m_objectsBuf];
            for(unsigned int i = 0; i < m_objectsLen; i++)
            {
                newContainer[i] = m_objects[i];
            }
            delete m_objects;
            m_objects = newContainer;
            m_objectsLen = len;
        }
        return false;
    }
    
    void pop_back()
    {
        m_objectsLen--;
    }
    
    Object& back()
    {
        return m_objects[m_objectsLen - 1];
    }
    
    unsigned int length()
    {
        return m_objectsLen;
    }
    
    Object& operator [] (unsigned int i)
    {
        return m_objects[i];
    }
    
private:
    Object* m_objects;
    unsigned int m_objectsLen;
    unsigned int m_objectsBuf;
};

}

#endif/*VECTOR_HPP*/