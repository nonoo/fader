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
#include "SortableStringArray.h"

int CSortableStringArray::Compare(const CString * pstr1, const CString * pstr2)
{
	ASSERT(pstr1);
	ASSERT(pstr2);
	return pstr1->Compare(*pstr2);
}

void CSortableStringArray::Sort(STRINGCOMPAREFN pfnCompare /*= CSortedStringArray::Compare */)
{
	CString * prgstr = GetData();
	qsort(prgstr,GetSize(),sizeof(CString),(GENERICCOMPAREFN)pfnCompare);
}
