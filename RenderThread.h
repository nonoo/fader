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

class CRenderThread : public CWinThread
{
	DECLARE_DYNCREATE(CRenderThread)

private:
	bool					m_bStopped;

	int						m_fJPEGOut;
	CSortableStringArray*	m_szaInFiles;
	TCHAR*					m_szOutDir;
	int						m_nFramesToInsert;
	int						m_fLeaveOutIntermediateFrames;
	HWND					m_hParentWnd;
	CxImage					m_ImageL, m_ImageR, m_ImageN;
	FILE*					m_fInTmp;
	FILE*					m_fOutTmp;
	CString					m_szOutFileName;
	char					m_pBuf[2000000];
	size_t					m_nBytesRead;
	int						m_nFileNameCounter;

	void					Process( WPARAM wp, LPARAM lp );
	CString					GenerateFilenameFromInt( CString szExtension, int n );
	void					CopyFile( CString szSrc, CString szDest );
	void					ProgressBarUpdate( int nFileNameCounter, int nTotalFileCount );
	void					OutPutFile( CString szSrc );

protected:
	CRenderThread();           // protected constructor used by dynamic creation
	virtual ~CRenderThread();

public:
	virtual BOOL			InitInstance();
	virtual int				ExitInstance();
	bool					IsRunning();
	void					Start();
	void					Stop();
	void					Create();
	void					Init( CSortableStringArray* szaInFiles, TCHAR* szOutDir, int nFramesToInsert, int fLeaveOutIntermediateFrames, int fJPEGOut, HWND ParentWnd );

protected:
	DECLARE_MESSAGE_MAP()
};


