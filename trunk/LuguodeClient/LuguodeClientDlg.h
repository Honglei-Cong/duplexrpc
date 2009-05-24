
// LuguodeClientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include <boost/thread.hpp>
#include "../rpc/rpc.hpp"
#define DRI_IS_CLIENT
#include "../LuguodeServer/LuguodeInterface.h"
#include <queue>


#define MY_WM_NEW_MESSAGE (WM_USER + 1125)
// CLuguodeClientDlg �Ի���
class CLuguodeClientDlg : public CDialog
{
// ����
public:
	CLuguodeClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_LUGUODECLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	//·���� �ĺ���
	void notify( const std::wstring& msg );
	void talk( const std::wstring& msg );
	void stranger_come_in();
	void on_connected( const boost::system::error_code& err, const boost::shared_ptr<LuguodeInterface>& conn );
	void on_connection_closed();
	void close_on_error( const rpc::remote_call_error& err );

	boost::mutex mtx;
	bool isTalking;
	boost::shared_ptr<LuguodeInterface> conn;
	boost::shared_ptr<boost::thread> networkThread;
	std::queue<std::wstring> unshownMessages;

	void AppendToRichEdit( const std::wstring& msg, int extraFlags = SF_UNICODE );
	void NetworkThread();

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNewMessage( WPARAM, LPARAM );
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_Host;
	CEdit m_Port;
	CEdit m_Words;
	CRichEditCtrl m_Message;
	CButton m_SendOrConnect;
	afx_msg void OnBnClickedButtonSendOrConnect();
};
