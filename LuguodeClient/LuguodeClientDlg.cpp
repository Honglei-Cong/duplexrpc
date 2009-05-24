
// LuguodeClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LuguodeClient.h"
#include "LuguodeClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <sstream>
#include <limits>
using namespace std;
using namespace boost;
// CLuguodeClientDlg �Ի���




CLuguodeClientDlg::CLuguodeClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLuguodeClientDlg::IDD, pParent), isTalking(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLuguodeClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HOST, m_Host);
	DDX_Control(pDX, IDC_EDIT_PORT, m_Port);
	DDX_Control(pDX, IDC_EDIT_WORDS, m_Words);
	DDX_Control(pDX, IDC_RICHEDIT2_MESSAGE, m_Message);
	DDX_Control(pDX, IDC_BUTTON_SEND_OR_CONNECT, m_SendOrConnect);
}

BEGIN_MESSAGE_MAP(CLuguodeClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SEND_OR_CONNECT, &CLuguodeClientDlg::OnBnClickedButtonSendOrConnect)
	ON_MESSAGE(MY_WM_NEW_MESSAGE, OnNewMessage)
END_MESSAGE_MAP()


// CLuguodeClientDlg ��Ϣ�������

BOOL CLuguodeClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	m_SendOrConnect.SetWindowText( L"����" );
	m_Host.SetWindowText( L"localhost" );
	m_Port.SetWindowText( L"9125" );

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLuguodeClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CLuguodeClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD CALLBACK MyStreamInCallback( DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb )
{
   wistringstream* pStrm = (wistringstream*) dwCookie;

   *pcb = sizeof(wchar_t) * pStrm->readsome( (wchar_t*)pbBuff, cb/(sizeof(wchar_t)) );

   return 0;
}

void CLuguodeClientDlg::AppendToRichEdit( const wstring& msg, int extraFlags )
{
	int size = m_Message.GetTextLength();
	m_Message.SetSel( size, size );
	
	EDITSTREAM es;

	wistringstream strm( msg );
	es.dwCookie = (DWORD_PTR) &strm;
	es.pfnCallback = &MyStreamInCallback;

	m_Message.StreamIn( SF_TEXT|SFF_SELECTION|extraFlags, es );
}

void CLuguodeClientDlg::notify( const wstring& msg )
{
	mutex::scoped_lock lck(mtx);
	unshownMessages.push( msg + L"\r\n" );
	PostMessage( MY_WM_NEW_MESSAGE ) ;
}

void CLuguodeClientDlg::talk( const wstring& msg )
{
	mutex::scoped_lock lck(mtx);
	unshownMessages.push( L"·�˼ף�" + msg + L"\r\n" );
	PostMessage( MY_WM_NEW_MESSAGE ) ;
}

void CLuguodeClientDlg::stranger_come_in()
{
	isTalking = true;
	AppendToRichEdit( L"�Ѿ���������һλ·�˼ס��ȶ� ��/�� ˵����ðɡ�\r\n" );
}

void CLuguodeClientDlg::NetworkThread()
{
	CString host, portStr;
	m_Host.GetWindowText( host );
	m_Port.GetWindowText( portStr );
	int port;
	wistringstream( (wstring)portStr ) >> port;
	LuguodeInterface::connect( (LPCSTR)CStringA(host), port, this );
}

void CLuguodeClientDlg::OnBnClickedButtonSendOrConnect()
{
	if ( conn )
	{
		CString words;
		m_Words.GetWindowText( words );
		m_Words.SetWindowText( L"" );
		conn->talk( (LPCTSTR)words, bind( &CLuguodeClientDlg::close_on_error, this, _1 ) );
		AppendToRichEdit( (LPCTSTR) (L"�㣺" + words + L"\r\n") );
		m_Message.SendMessage(WM_VSCROLL,SB_BOTTOM,0);
	}
	else
	{
		networkThread.reset( new thread( bind( &CLuguodeClientDlg::NetworkThread, this ) ) );
	}
}

void CLuguodeClientDlg::on_connected( const boost::system::error_code& err, const shared_ptr<LuguodeInterface>& conn_ )
{
	if (err)
	{
		AppendToRichEdit( L"����ʧ�ܣ�" );
		AppendToRichEdit( (LPCTSTR)CStringW(err.message().c_str()) );
		AppendToRichEdit( L"\r\n" );
	}
	else
	{
		conn = conn_;
		conn->on_connection_closed = bind( &CLuguodeClientDlg::on_connection_closed, this );
		AppendToRichEdit( L"�������Ϸ�������\r\n" );
		m_SendOrConnect.SetWindowText( L"������Ϣ" );
	}
}

void CLuguodeClientDlg::on_connection_closed()
{
	isTalking = false;
	conn.reset();
	AppendToRichEdit( L"�����жϡ�\r\n" );
	m_SendOrConnect.SetWindowText( L"����" );
}

void CLuguodeClientDlg::close_on_error( const rpc::remote_call_error& err )
{
	if ( err )
	{
		if ( err.code != rpc::remote_call_error::connection_error )
		{
			conn->close();
		}
		MessageBoxA( GetSafeHwnd(), err.what().c_str(), "����", MB_OK|MB_ICONERROR );
	}
}

LRESULT CLuguodeClientDlg::OnNewMessage( WPARAM, LPARAM )
{
	mutex::scoped_lock lck(mtx);
	AppendToRichEdit( unshownMessages.front() );
	unshownMessages.pop();
	m_Message.SendMessage(WM_VSCROLL,SB_BOTTOM,0);
	return 0;
}