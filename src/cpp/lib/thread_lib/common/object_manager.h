#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>

#include "critical_section.h"

template<typename Object>
class ObjectManager
{
public:
	bool AddObject(const Object& object)
	{
		bool res = false;
		Object object_;
		CSLocker locker(&m_cs);
		if(!GetObject(object, &object_))
		{
			res = true;
			m_objects.push_back(object);
		}
		return res;
	}

	bool RemoveObject(const Object& object)
	{
		CSLocker locker(&m_cs);
		for (typename std::vector<Object>::iterator it = m_objects.begin();
			it != m_objects.end();
			it++)
		{
			if((*it) == object)
			{
				m_objects.erase(it);
				return true;
			}
		}

		return false;
	}

	bool GetObject(const Object& object, Object* getObject)
	{
		bool res = false;
		CSLocker locker(&m_cs);
		for (typename std::vector<Object>::iterator it = m_objects.begin();
			it != m_objects.end();
			it++)
		{
			if((*it) == object)
			{
				*getObject = *it;
				res = true;
				break;
			}
		}
		return res;
	}
	
	bool UpdateObject(const Object& object)
	{
		bool res = false;
		CSLocker locker(&m_cs);
		for (typename std::vector<Object>::iterator it = m_objects.begin();
			it != m_objects.end();
			it++)
		{
			if((*it) == object)
			{
				*it = object;
				res = true;
				break;
			}
		}
		return res;
	}

	std::vector<Object> GetObjectList()
	{
		CSLocker locker(&m_cs);
		return m_objects;
	}

	void Clear()
	{
		CSLocker locker(&m_cs);
		m_objects.clear();
	}

	void operator = (const ObjectManager<Object>& manager)
	{
		CSLocker locker(&m_cs);
		m_objects = const_cast<ObjectManager<Object>&>(manager).GetObjectList();
	}

	template<typename Func>
	void CheckObjects(Func func)
	{
		CSLocker locker(&m_cs);
		for (typename std::vector<Object>::iterator it = m_objects.begin();
			it != m_objects.end();)
		{
			bool erase;
			func(*it, erase);
			if(erase)
			{
				it = m_objects.erase(it);
			}
			else 
			{
				it++;
			}
		}	
	}

	template<typename Func>
	void ProcessingObjects(Func func)
	{
		CSLocker locker(&m_cs);
		for (typename std::vector<Object>::iterator it = m_objects.begin();
			it != m_objects.end();
			it++)
		{
			func(*it);
		}	
	}
private:
	std::vector<Object> m_objects;
	CriticalSection m_cs;
};

#endif/*OBJECT_MANAGER_H*/