#pragma once

template <class T>
class CTlsData
{
private:
	DWORD tlsindex;
public:
	T* GetPtr() const
	{
		if(tlsindex == 0xffffffff)
			return NULL;

		T* pT = reinterpret_cast<T*>(::TlsGetValue(tlsindex));
		if(pT)
			return pT;

		pT = new T;
		if(!pT)
			return NULL;

		if(!::TlsSetValue(tlsindex, pT)) {
			delete pT;
			return NULL;
		}
		return pT;
	}
	bool ProcessInit()
	{
		tlsindex = ::TlsAlloc();
		return (tlsindex != 0xffffffff);
	}
	void ProcessTerm()
	{
		if(tlsindex == 0xffffffff)
			return;
		ThreadTerm();	//Ç±ÇÍì¸ÇÍÇ»Ç¢Ç∆ÉäÅ[ÉNÇ∑ÇÈ
		::TlsFree(tlsindex);
		tlsindex = 0xffffffff;
	}
//	bool ThreadInit()
//	{
//	}
	void ThreadTerm()
	{
		T* pT = GetPtr();
		if(pT)
			delete pT;
		::TlsSetValue(tlsindex, NULL);
	}
};
