#ifndef REF_H
#define REF_H

template<typename TClass, typename TFunc, typename TReturn = bool>
class Reference
{
public:
	Reference(TClass* ref, TFunc f)
		: m_ref(ref), m_func(f){}

	template<typename TObject>
	TReturn operator ()(const TObject& obj)
	{
		return (m_ref->*m_func)(obj);
	}
private:
	TClass* m_ref;
	TFunc m_func;
};

template<typename TClass, typename TFunc, typename TObject, typename TReturn = bool>
class ReferenceObject
{
public:
	ReferenceObject(TClass* ref, TFunc f, const TObject& obj)
		: m_ref(ref), m_func(f), m_obj(obj){}

	template<typename TRefObject>
	TReturn operator ()(const TRefObject& obj)
	{
		return (m_ref->*m_func)(m_obj, obj);
	}
	
private:
	TClass* m_ref;
	TFunc m_func;
	const TObject& m_obj;
};

template<typename TFunc, typename TReturn = bool>
class OwnReference
{
public:
	OwnReference(TFunc f)
		: m_func(f){}

	template<typename TObject>
	TReturn operator ()(const TObject& obj)
	{
		return (const_cast<TObject&>(obj).*m_func)();
	}

private:
	TFunc m_func;
};

template<typename TClass, typename TFunc>
Reference<TClass, TFunc> Ref(TClass* ref, TFunc func)
{
	return Reference<TClass, TFunc>(ref, func);
}

template<typename TClass, typename TFunc, typename TObject>
ReferenceObject<TClass, TFunc, TObject> Ref(TClass* ref, TFunc func, const TObject& obj)
{
	return ReferenceObject<TClass, TFunc, TObject>(ref, func, obj);
}

template<typename TFunc>
OwnReference<TFunc> Ref(TFunc func)
{
	return OwnReference<TFunc>(func);
}

#endif/*REF_H*/
