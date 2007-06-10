//  This file is part of fader.
//
//  fader is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  fader is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with fader; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#pragma once

#include "RenderThread.h"
#include "SortableStringArray.h"
#include "OneSecTickThread.h"
#include "afxcmn.h"

// CfaderDlg dialog
class CfaderDlg : public CDialog
{
// Construction
public:
	CfaderDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FADER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	CButton m_StartButton;
	CEdit m_FramesNumber;
	CProgressCtrl m_Progress;
	CButton m_InputImagesSelectButton;
	CButton m_OutputDirSelectButton;
	CProgressCtrl m_ProgressMini;
	CStatic m_StatusText;
	CStatic m_StatusText2;
	CButton m_LeaveCheckBox;
	CSpinButtonCtrl m_SpinButton;
	CButton m_JPEGOut;
	CButton m_BMPOut;

	int m_nCurrentFrameCount;
	int m_nTotalFrameCount;
	int m_nLastFrameCount;

	LRESULT StatusMsgDispatcher( WPARAM wp, LPARAM lp );
	void CheckThatWeCanStart();
	void ResetDialog();
	bool FindFilesInDir( CString szDir, CString szMask );

// Implementation
protected:
	HICON m_hIcon;
	CSortableStringArray m_szaSelectedFiles;
	TCHAR m_szPath[MAX_PATH];
	CRenderThread* m_pRenderThread;
	COneSecTickThread* m_pOneSecTickThread;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnBnClickedOk();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
};
