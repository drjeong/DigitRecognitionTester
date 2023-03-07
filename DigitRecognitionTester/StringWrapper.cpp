#include "pch.h"
#include "StringWrapper.h"


CStringWrapper::CStringWrapper(void)
{
	data=NULL;
}

CStringWrapper::CStringWrapper(const CString &cs)
{
	data=NULL;
	data=ConvertToChar(cs);
	__super::SetString(cs);
};

CStringWrapper::~CStringWrapper(void)
{
	if (data) delete [] data;
	data=NULL;
	__super::Empty();
}

char* CStringWrapper::ConvertToChar(const CString &s)
{
	int nSize = s.GetLength();
	char *pAnsiString = new char[nSize+1];
	memset(pAnsiString,0,nSize+1);
	//wcstombs(pAnsiString, s, nSize+1);
	// Conversion
	size_t i;
	wcstombs_s(&i, pAnsiString, nSize+1, s, nSize+1);

	return pAnsiString;
};