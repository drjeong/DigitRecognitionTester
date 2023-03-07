#pragma once

class CStringWrapper: public CString	 
{
public:
	CStringWrapper(void);
	CStringWrapper(const CString &cs);
	virtual~CStringWrapper(void);

	void setString(const CString &cs){data=ConvertToChar(cs);};
	char *getchardata(){return data;};

	char* ConvertToChar(const CString &s);
	
private:
	char *data;
};