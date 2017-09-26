///
/// @file		Singleton.h
/// @author		Junpyo Hong (jp7.hong@gmail.com)
/// @date		Aug. 30, 2016
/// @version	1.0
///
/// @brief		Singleton template class (Singleton Pattern)
///

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

/// @brief		Singleton template class (Singleton Pattern)
template<typename T>
class TSingleton
{
protected:
	/// constructor
	explicit TSingleton() {}

	/// destructor
	virtual ~TSingleton()
	{
		if (!m_pInstance)
			DestroyInstance();
	}

public:
	/// get singleton instance pointer
	static T * GetInstance()
	{
		if (!m_pInstance)
			m_pInstance = new T;

		return m_pInstance;
	}

	/// destroy singleton instance
	static void DestroyInstance()
    {
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = 0;
		}
	}

private:
	/// singleton instance pointer
	static T * m_pInstance;
};

/// initialize a static member variable
template<typename T> T* TSingleton<T>::m_pInstance = 0;

//------------------------------------------------------------------------------
// How to use TSingleton:
//
// class CObject : public TSingleton<CObject>
// {
// public:
//     explicit CObject();
//     virtual ~CObject();
// };
//------------------------------------------------------------------------------

#endif // _SINGLETON_H_
