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

#include "stdafx.h"
#include "fader.h"
#include "faderDlg.h"
#include ".\faderdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef int (__cdecl *GENERICCOMPAREFN)(const void * elem1, const void * elem2);

// CfaderDlg dialog

CfaderDlg::CfaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CfaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CfaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_StartButton);
	DDX_Control(pDX, IDC_EDIT1, m_FramesNumber);
	DDX_Control(pDX, IDC_PROGRESS1, m_Progress);
	DDX_Control(pDX, IDC_BUTTON1, m_InputImagesSelectButton);
	DDX_Control(pDX, IDC_BUTTON2, m_OutputDirSelectButton);
	DDX_Control(pDX, IDC_PROGRESS2, m_ProgressMini);
	DDX_Control(pDX, IDC_STATIC2, m_StatusText);
	DDX_Control(pDX, IDC_STATIC3, m_StatusText2);
	DDX_Control(pDX, IDC_CHECK1, m_LeaveCheckBox);
	DDX_Control(pDX, IDC_SPIN1, m_SpinButton);
	DDX_Control(pDX, IDC_RADIO1, m_JPEGOut);
	DDX_Control(pDX, IDC_RADIO2, m_BMPOut);
}

BEGIN_MESSAGE_MAP(CfaderDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( WU_STATUS, StatusMsgDispatcher )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
END_MESSAGE_MAP()


// CfaderDlg message handlers

BOOL CfaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_FramesNumber.SetWindowText( "1" );
	CString tmp;
	tmp.Format( "fader v%s", VERSION );
	this->SetWindowText( tmp );
	tmp = tmp + " by Nonoo";
	m_StatusText.SetWindowText( tmp );
	m_pRenderThread = NULL;
	m_pOneSecTickThread = NULL;
	m_LeaveCheckBox.SetCheck( 1 );
	m_nLastFrameCount = 0;
	m_JPEGOut.SetCheck( 1 );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CfaderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CfaderDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CfaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CfaderDlg::OnBnClickedButton1()
{
	TCHAR szDir[MAX_PATH];

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _T("Select the source images directory!");
	LPITEMIDLIST pidl = SHBrowseForFolder( &bi );
	if ( pidl != 0 )
	{
		// get the name of the folder
		SHGetPathFromIDList ( pidl, szDir );

		// free memory used
		IMalloc* imalloc = NULL;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc ) ) )
		{
			imalloc->Free( pidl );
			imalloc->Release();
		}
	}

	if( strlen( szDir ) )
	{
		m_szInPath = szDir;
	}

	CheckThatWeCanStart();
}

void CfaderDlg::OnBnClickedButton2()
{
	TCHAR szDir[MAX_PATH];

	BROWSEINFO bi = { 0 };
	bi.lpszTitle = _T("Select the output directory!");
	LPITEMIDLIST pidl = SHBrowseForFolder( &bi );
	if ( pidl != 0 )
	{
		// get the name of the folder
		SHGetPathFromIDList ( pidl, szDir );

		// free memory used
		IMalloc* imalloc = NULL;
		if ( SUCCEEDED( SHGetMalloc ( &imalloc ) ) )
		{
			imalloc->Free( pidl );
			imalloc->Release();
		}
	}

	if( strlen( szDir ) )
	{
		m_szOutPath = szDir;
	}

	CheckThatWeCanStart();
}

void CfaderDlg::CheckThatWeCanStart()
{
	CString szTmp;
	int i = -1;
	m_FramesNumber.GetWindowText( szTmp );
	if( szTmp.GetLength() )
	{
		i = atoi( szTmp );
	}

	if( ( m_szOutPath.GetLength() > 0 ) && ( m_szInPath.GetLength() > 0 ) && ( i >= 0 ) )
	{
		m_StartButton.EnableWindow( true );
	}
	else
	{
		m_StartButton.EnableWindow( false );
	}
}
void CfaderDlg::OnEnChangeEdit1()
{
	CheckThatWeCanStart();
}

void CfaderDlg::OnBnClickedOk()
{
	if( m_pRenderThread == NULL )
	{
		m_pRenderThread = (CRenderThread *)AfxBeginThread( RUNTIME_CLASS( CRenderThread ), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED );
		m_pRenderThread->Create();
	}

	if( m_pOneSecTickThread == NULL )
	{
		m_pOneSecTickThread = (COneSecTickThread *)AfxBeginThread( RUNTIME_CLASS( COneSecTickThread ), THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED );
		m_pOneSecTickThread->Create();
		m_pOneSecTickThread->Init( this->m_hWnd );
	}

	if( m_pRenderThread->IsRunning() )
	{
		// stop has been pressed
		ResetDialog();
	}
	else
	{
		// start has been pressed
		m_szaSelectedFiles.RemoveAll();
		bool b1 = FindFilesInDir( m_szInPath, "*.jpg" );
		bool b2 = FindFilesInDir( m_szInPath, "*.jpeg" );
		bool b3 = FindFilesInDir( m_szInPath, "*.bmp" );
		if( !b1 && !b2 && !b3 )
		{
			MessageBox( CString( "No image files has been found in \"" ) + m_szInPath + "\"!", "Error", MB_ICONERROR | MB_OK );
			return;
		}

		m_szaSelectedFiles.Sort();
		m_StartButton.SetWindowText( "Stop" );
		m_InputImagesSelectButton.EnableWindow( false );
		m_OutputDirSelectButton.EnableWindow( false );
		m_FramesNumber.EnableWindow( false );
		m_LeaveCheckBox.EnableWindow( false );
		m_SpinButton.EnableWindow( false );
		m_JPEGOut.EnableWindow( false );
		m_BMPOut.EnableWindow( false );

		m_nLastFrameCount = 0;

		// creating render thread
		CString szTmp;
		m_FramesNumber.GetWindowText( szTmp );
		int i = atoi( szTmp );
		m_pRenderThread->Init( &m_szaSelectedFiles, m_szOutPath, i, m_LeaveCheckBox.GetCheck(), m_JPEGOut.GetCheck(), this->m_hWnd );
		m_pRenderThread->Start();
		m_pOneSecTickThread->Start();
	}
}

LRESULT CfaderDlg::StatusMsgDispatcher( WPARAM wp, LPARAM lp )
{
	CString szTmp;

	if( !m_pRenderThread->IsRunning() )
	{
		return 0;
	}

	switch( wp )
	{
		case STATUSMSG_DONE:
			m_StatusText.SetWindowText( "Done." );
			MessageBox( "Done!", "fader", MB_OK );
			break;
		case STATUSMSG_SIZE_MISMATCH:
			MessageBox( "Image size mismatch!", "Error", MB_ICONERROR|MB_OK );
			break;
		case STATUSMSG_PROGRESS:
			m_nCurrentFrameCount = LOWORD( lp );
			m_nTotalFrameCount = HIWORD( lp );
			m_Progress.SetPos( (int)floor( ( (double)m_nCurrentFrameCount / (double)m_nTotalFrameCount ) * 100 + 0.5 ) );
			szTmp.Format( "%d/%d", LOWORD( lp ), HIWORD( lp ) );
			m_StatusText.SetWindowText( szTmp );
			return 0;
		case STATUSMSG_PROGRESSMINI:
			m_ProgressMini.SetPos( (int)lp );
			return 0;
		case STATUSMSG_TICK: // one sec tick from our tick thread
			if( ( m_nTotalFrameCount > 0 ) && ( m_nCurrentFrameCount > 0 ) )
			{
				if( m_nCurrentFrameCount - m_nLastFrameCount > 0 )
				{
					int nSecsRemaining = ( m_nTotalFrameCount - m_nCurrentFrameCount ) / ( m_nCurrentFrameCount - m_nLastFrameCount );

					int nHoursRemaining = nSecsRemaining / 3600;
					nSecsRemaining -= nHoursRemaining * 3600;
					int nMinutesRemaining = nSecsRemaining / 60;
					nSecsRemaining -= nMinutesRemaining * 60;

					if( nHoursRemaining > 0 )
					{
						szTmp.Format( "%d hrs, %d mins, %d secs remaining", nHoursRemaining, nMinutesRemaining, nSecsRemaining );
					}
					else
					{
						if( nMinutesRemaining > 0 )
						{
							szTmp.Format( "%d minutes, %d seconds remaining", nMinutesRemaining, nSecsRemaining );
						}
						else
						{
							szTmp.Format( "%d seconds remaining", nSecsRemaining );
						}
					}

					m_StatusText2.SetWindowText( szTmp );
					m_nLastFrameCount = m_nCurrentFrameCount;
				}
			}
			return 0;
	}

	ResetDialog();

	return 0;
}

void CfaderDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	CString szTmp;
	int i = -1;
	m_FramesNumber.GetWindowText( szTmp );
	if( szTmp.GetLength() )
	{
		i = atoi( szTmp );
	}
	i += -pNMUpDown->iDelta;
	if( i < 1 )
	{
		i = 1;
	}
	szTmp.Format( "%d", i );
	m_FramesNumber.SetWindowText( szTmp );

	*pResult = 0;
}

void CfaderDlg::ResetDialog()
{
	m_Progress.SetPos( 0 );
	m_ProgressMini.SetPos( 0 );
	m_StartButton.SetWindowText( "Start" );
	m_InputImagesSelectButton.EnableWindow( true );
	m_OutputDirSelectButton.EnableWindow( true );
	m_FramesNumber.EnableWindow( true );
	m_LeaveCheckBox.EnableWindow( true );
	m_StatusText2.SetWindowText( "www.nonoo.hu/projects/fader/" );
	m_SpinButton.EnableWindow( true );
	m_JPEGOut.EnableWindow( true );
	m_BMPOut.EnableWindow( true );

	m_nLastFrameCount = 0;

	m_pRenderThread->Stop();
	m_pOneSecTickThread->Stop();
}

bool CfaderDlg::FindFilesInDir( CString szDir, CString szMask )
{
	struct _finddata_t c_file;
	intptr_t hFile;

	if( ( hFile = _findfirst( szDir + "\\" + szMask, &c_file ) ) > -1 )
	{
		do
		{
			m_szaSelectedFiles.Add( szDir + "\\" + c_file.name );
		} while( _findnext( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}
	else
	{
		return false;
	}
	return true;
}
