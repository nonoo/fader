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
#include "RenderThread.h"

IMPLEMENT_DYNCREATE(CRenderThread, CWinThread)

CRenderThread::CRenderThread()
{
}

CRenderThread::~CRenderThread()
{
}

BOOL CRenderThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int CRenderThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CRenderThread, CWinThread)
	ON_THREAD_MESSAGE( WU_THREAD_PROCESS, Process )
END_MESSAGE_MAP()


// CRenderThread message handlers
void CRenderThread::Process( WPARAM wp, LPARAM lp )
{
loop:
	if( !m_bStopped )
	{
		int nTotalFileCount;

		m_nFileNameCounter = 0;

		if( m_fLeaveOutIntermediateFrames )
		{
			nTotalFileCount = nTotalFileCount = 2 + ( (int)m_szaInFiles->GetCount()-1 ) * m_nFramesToInsert;

			// copying left image to the destination dir
			OutPutFile( m_szaInFiles->GetAt( 0 ) );
		}
		else
		{
			nTotalFileCount = nTotalFileCount = (int)m_szaInFiles->GetCount() + ( (int)m_szaInFiles->GetCount()-1 ) * m_nFramesToInsert;
		}

		m_ImageL.Load( m_szaInFiles->GetAt( 0 ), CXIMAGE_FORMAT_JPG );

		for( int i = 0; i < m_szaInFiles->GetSize() - 1; i++ )
		{
			if( m_bStopped )
			{
				goto loop;
			}
			ProgressBarUpdate( m_nFileNameCounter, nTotalFileCount );

			m_ImageR.Load( m_szaInFiles->GetAt( i+1 ), CXIMAGE_FORMAT_JPG );

			if( ( m_ImageL.GetWidth() != m_ImageR.GetWidth() ) || ( m_ImageL.GetHeight() != m_ImageR.GetHeight() ) )
			{
				SendMessageW( m_hParentWnd, WU_STATUS, STATUSMSG_SIZE_MISMATCH, 0 );
				m_bStopped = true;
				goto loop;
			}

			if( !m_fLeaveOutIntermediateFrames )
			{
				OutPutFile( m_szaInFiles->GetAt( i ) );

				ProgressBarUpdate( m_nFileNameCounter, nTotalFileCount );
			}

			// rendering the intermediate frames
			for( int j = 1; j < m_nFramesToInsert + 1; j++ )
			{
				if( m_bStopped )
				{
					goto loop;
				}

				m_ImageN = m_ImageL;

				// blending L and R images together
				for( DWORD y = 0; y < m_ImageN.GetHeight(); y++ )
				{
					for( DWORD x = 0; x < m_ImageN.GetWidth(); x++ )
					{
						RGBQUAD c1 = m_ImageN.GetPixelColor( x, y );
						RGBQUAD c2 = m_ImageR.GetPixelColor( x, y );
						float r2 = (float)j / ( (float)m_nFramesToInsert + 1 );
						float r1 = 1 - r2;

						c1.rgbRed = (BYTE)floor( (float)c1.rgbRed * r1 + (float)c2.rgbRed * r2 + 0.5 );
						c1.rgbGreen = (BYTE)floor( (float)c1.rgbGreen * r1 + (float)c2.rgbGreen * r2 + 0.5 );
						c1.rgbBlue = (BYTE)floor( (float)c1.rgbBlue * r1 + (float)c2.rgbBlue * r2 + 0.5 );
						m_ImageN.SetPixelColor( x, y, c1 );
					}
					// updating the mini progress bar
					SendMessageW( m_hParentWnd, WU_STATUS, STATUSMSG_PROGRESSMINI, (LPARAM)floor( ( (double)y / (double)m_ImageN.GetHeight() ) * 100 + 0.5 ) );
				}

				// updating the mini progress bar
				SendMessageW( m_hParentWnd, WU_STATUS, STATUSMSG_PROGRESSMINI, 0 );

				if( m_fJPEGOut )
				{
					m_ImageN.SetJpegQuality( 100 );
					m_ImageN.Save( GenerateFilenameFromInt( "jpg", m_nFileNameCounter++ ), CXIMAGE_FORMAT_JPG );
				}
				else
				{
					m_ImageN.Save( GenerateFilenameFromInt( "bmp", m_nFileNameCounter++ ), CXIMAGE_FORMAT_BMP );
				}

				ProgressBarUpdate( m_nFileNameCounter, nTotalFileCount );
			}

			m_ImageL = m_ImageR;
		}

		OutPutFile( m_szaInFiles->GetAt( i ) );

		ProgressBarUpdate( m_nFileNameCounter, nTotalFileCount );

		// work completed
		SendMessageW( m_hParentWnd, WU_STATUS, STATUSMSG_DONE, 0 );
		m_bStopped = true;
	}
	Sleep(1);
	goto loop;
}

void CRenderThread::Create()
{
	m_bAutoDelete = false;
	m_bStopped = true;

	// starting thread
	//
	this->ResumeThread();
	this->PumpMessage();
	this->PostThreadMessage( WU_THREAD_PROCESS, 0, 0 );
}

void CRenderThread::Init( CSortableStringArray* szaInFiles, TCHAR* szOutDir, int nFramesToInsert, int fLeaveOutIntermediateFrames, int fJPEGOut, HWND ParentWnd )
{
	m_hParentWnd = ParentWnd;
	m_nFramesToInsert = nFramesToInsert;
	m_szaInFiles = szaInFiles;
	m_szOutDir = szOutDir;
	m_fLeaveOutIntermediateFrames = fLeaveOutIntermediateFrames;
	m_fJPEGOut = fJPEGOut;
}

void CRenderThread::Start()
{
	m_bStopped = false;
}

void CRenderThread::Stop()
{
	m_bStopped = true;
}

bool CRenderThread::IsRunning()
{
	return !m_bStopped;
}

CString CRenderThread::GenerateFilenameFromInt( CString szExtension, int n )
{
	CString szOut;

	if( n < 10 )
	{
		szOut.Format( "%s\\0000%d.%s", m_szOutDir, n, szExtension );
		return szOut;
	}
	if( n < 100 )
	{
		szOut.Format( "%s\\000%d.%s", m_szOutDir, n, szExtension );
		return szOut;
	}
	if( n < 1000 )
	{
		szOut.Format( "%s\\00%d.%s", m_szOutDir, n, szExtension );
		return szOut;
	}
	if( n < 10000 )
	{
		szOut.Format( "%s\\0%d.%s", m_szOutDir, n, szExtension );
		return szOut;
	}
	szOut.Format( "%s\\%d.%s", m_szOutDir, n, szExtension );
	return szOut;
}

void CRenderThread::CopyFile( CString szSrc, CString szDest )
{
	m_fInTmp = fopen( szSrc, "rb" );
	m_fOutTmp = fopen( szDest, "wb" );
	while( !feof( m_fInTmp ) )
	{
		m_nBytesRead = fread( m_pBuf, 1, sizeof( m_pBuf ), m_fInTmp );
		fwrite( m_pBuf, 1, m_nBytesRead, m_fOutTmp );
	}
	fclose( m_fInTmp );
	fclose( m_fOutTmp );
}

void CRenderThread::ProgressBarUpdate( int nFileNameCounter, int nTotalFileCount )
{
	LPARAM l = ( nTotalFileCount << 16 ) + nFileNameCounter;
	SendMessageW( m_hParentWnd, WU_STATUS, STATUSMSG_PROGRESS, l );
}

void CRenderThread::OutPutFile( CString szSrc )
{
	if( m_fJPEGOut )
	{
		if( szSrc.Find( ".jpg", 0 ) > -1 || szSrc.Find( ".jpeg", 0 ) > -1 )
		{
			// the file is already compressed
			CopyFile( szSrc, GenerateFilenameFromInt( "jpg", m_nFileNameCounter++ ) );
		}
		else
		{
			// converting to jpeg
			m_ImageN.Load( szSrc, CXIMAGE_FORMAT_BMP );
			m_ImageN.SetJpegQuality( 100 );
			m_ImageN.Save( GenerateFilenameFromInt( "jpg", m_nFileNameCounter++ ), CXIMAGE_FORMAT_JPG );
		}
	}
	else
	{
		if( szSrc.Find( ".bmp", 0 ) > -1 )
		{
			// the file is already uncompressed
			CopyFile( szSrc, GenerateFilenameFromInt( "bmp", m_nFileNameCounter++ ) );
		}
		else
		{
			// converting to bmp
			m_ImageN.Load( szSrc, CXIMAGE_FORMAT_JPG );
			m_ImageN.Save( GenerateFilenameFromInt( "bmp", m_nFileNameCounter++ ), CXIMAGE_FORMAT_BMP );
		}
	}
}