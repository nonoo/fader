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

class COneSecTickThread : public CWinThread
{
	DECLARE_DYNCREATE(COneSecTickThread)

private:
	bool					m_bStopped;
	bool					m_bEndThread;
	HWND					m_hParentWnd;

	void					Process( WPARAM wp, LPARAM lp );

protected:
	COneSecTickThread();           // protected constructor used by dynamic creation
	virtual ~COneSecTickThread();

public:
	virtual BOOL			InitInstance();
	virtual int				ExitInstance();

	bool					IsRunning();
	void					Start();
	void					Stop();
	void					Create();
	void					Init( HWND ParentWnd );

protected:
	DECLARE_MESSAGE_MAP()
};
