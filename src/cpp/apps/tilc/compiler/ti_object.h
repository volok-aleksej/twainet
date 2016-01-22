#ifndef TI_COBJECT_H
#define TI_COBJECT_H

#include <vector>
#include <string>

class TIObject
{
public:
    enum typeObject
    {
        Root = 0,
        Plugin,
        Module,
        Application,
        Variable,
        Function
    };
    
    TIObject(typeObject type, const std::string& name)
    : m_type(type), m_name(name), m_parent(0){}
    virtual ~TIObject()
    {
        for(std::vector<TIObject*>::iterator it = m_childs.begin();
            it != m_childs.end(); it++)
        {
            delete *it;
        }
    }
    
    void operator = (const TIObject& object)
    {
        m_type = object.m_type;
        m_childs = object.m_childs;
        m_name = object.m_name;
    }
    
    bool operator == (const TIObject& object)
    {
        return m_type == object.m_type &&
               m_name == object.m_name;
    }
    
    void AddChild(TIObject* object)
    {
        object->m_parent = this;
        m_childs.push_back(object);
    }
    
    std::vector<TIObject*> GetChilds()
    {
        return m_childs;
    }
    
    TIObject* GetParent()
    {
        return m_parent;
    }
    
    typeObject GetType()
    {
        return m_type;
    }
    
    std::string GetName()
    {
        return m_name;
    }
    
private:
    typeObject m_type;
    std::vector<TIObject*> m_childs;
    TIObject* m_parent;
    std::string m_name;
};

#endif/*TI_COBJECT_H*/