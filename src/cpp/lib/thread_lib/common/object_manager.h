#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <vector>
#include <algorithm>

#include "critical_section.h"
#include "common/ref.h"

template<typename Object>
class ObjectManager
{
public:
	bool AddObject(const Object& object)
	{
		bool res = false;
		CSLocker locker(&m_cs);
		typename std::vector<Object>::iterator it = std::upper_bound(m_objects.begin(), m_objects.end(), object);
		if(it != m_objects.end() && *it != object)
		{
			res = true;
			m_objects.insert(it, object);
		}
		else if(it == m_objects.end())
		{
			res = true;
			m_objects.push_back(object);
		}
		return res;
	}

	bool RemoveObject(const Object& object)
	{
		CSLocker locker(&m_cs);
		typename std::vector<Object>::iterator it = std::lower_bound(m_objects.begin(), m_objects.end(), object);
		if(it != m_objects.end() && (*it) == object)
		{
			m_objects.erase(it);
			return true;
		}
		return false;
	}

	template<typename Func>
	bool DestroyObject(const Object& object, Func func)
	{
		CSLocker locker(&m_cs);
		typename std::vector<Object>::iterator it = std::lower_bound(m_objects.begin(), m_objects.end(), object);
		if(it != m_objects.end() && (*it) == object)
		{
			func(*it);
			m_objects.erase(it);
			return true;
		}
		return false;
	}

	bool GetObject(const Object& object, Object* getObject)
	{
		bool res = false;
		CSLocker locker(&m_cs);
		typename std::vector<Object>::iterator it = std::lower_bound(m_objects.begin(), m_objects.end(), object);
		if(it != m_objects.end() && (*it) == object)
		{
			*getObject = *it;
			res = true;
		}

		return res;
	}
	
	bool UpdateObject(const Object& object)
	{
		bool res = false;
		CSLocker locker(&m_cs);
		typename std::vector<Object>::iterator it = std::lower_bound(m_objects.begin(), m_objects.end(), object);
		if(it != m_objects.end() && (*it) == object)
		{
			*it = object;
			res = true;
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
		for(int i = 0; i < m_objects.size(); i++)
		{
			if(func(m_objects[i]))
			{
				m_objects.erase(m_objects.begin() + i);
				i--;			  
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