#ifndef RECEIVER_FUNC_H
#define RECEIVER_FUNC_H

class SignalReceiver;

class IReceiverFunc
{
public:
	virtual ~IReceiverFunc(){}
	virtual bool isSignal(const DataMessage& msg) = 0;
	virtual void onSignal(const DataMessage& msg) = 0;
	virtual SignalReceiver* GetReciever() const  = 0;
};

template<class TReciever>
class ReceiverFunc : public IReceiverFunc
{
	friend class Signal;
public:
	typedef void (TReciever::*SignalFunction)(const DataMessage& msg);

	ReceiverFunc(TReciever* reciever, const char* typeSignal, SignalFunction func)
		: m_receiver(reciever), m_func(func), m_typeSignal(typeSignal)
	{
	}

protected:	
	bool isSignal(const DataMessage& msg)
	{
		return m_typeSignal == msg.GetName();
	}

	void onSignal(const DataMessage& msg)
	{
		if(m_typeSignal != msg.GetName())
		{
			return;
		}

		(m_receiver->*m_func)(msg);
	}

	SignalReceiver* GetReciever() const
	{
		return m_receiver;
	}

private:
	SignalFunction m_func;
	TReciever* m_receiver;
	const char* m_typeSignal;
};

#endif/*RECEIVER_FUNC_H*/