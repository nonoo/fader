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
#include "OneSecTickThread.h"

IMPLEMENT_DYNCREATE(COneSecTickThread, CWinThread)

COneSecTickThread::COneSecTickThread()
{
}

COneSecTickThread::~COneSecTickThread()
{
}

BOOL COneSecTickThread::InitInstance()
{
	// TODO:  perform and per-thread initialization here
	return TRUE;
}

int COneSecTickThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(COneSecTickThread, CWinThread)
	ON_THREAD_MESSAGE( WU_THREAD_PROCESS, Process )
END_MESSAGE_MAP()

void COneSecTickThread::Process( WPARAM wp, LPARAM lp )
{
loop:
	if( !m_bStopped )
	{
		SendMessageW( m_hParentWnd, WU_STATUS, STATUSMSG_TICK, 0 );
	}
	Sleep( 1000 );
	goto loop;
}

void COneSecTickThread::Create()
{
	m_bAutoDelete = false;
	m_bStopped = true;

	// starting thread
	//
	this->ResumeThread();
	this->PumpMessage();
	this->PostThreadMessage( WU_THREAD_PROCESS, 0, 0 );
}

void COneSecTickThread::Init( HWND ParentWnd )
{
	m_hParentWnd = ParentWnd;
}

void COneSecTickThread::Start()
{
	m_bStopped = false;
}

void COneSecTickThread::Stop()
{
	m_bStopped = true;
}

bool COneSecTickThread::IsRunning()
{
	return !m_bStopped;
}
