
// LuguodeClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLuguodeClientApp:
// �йش����ʵ�֣������ LuguodeClient.cpp
//

class CLuguodeClientApp : public CWinAppEx
{
public:
	CLuguodeClientApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLuguodeClientApp theApp;