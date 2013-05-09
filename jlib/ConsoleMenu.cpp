// Title: Jeff Benson's Console Library
// File: ConsoleMenu.cpp
// Author: Jeff Benson
// Date: 7/28/11
// Last Updated: 8/2/2011
// Contact: pepsibot@hotmail.com
//
// Copyright (C) 2011
// This file is part of JLib.
// 
// JLib is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// JLib is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with JLib.  If not, see <http://www.gnu.org/licenses/>.
#include "ConsoleMenu.h"
#include "ConsoleCore.h"


ConsoleMenu::ConsoleMenu(COORD origin, ConsoleFormat format
	, ConsoleFormat selectionFormat)
	:m_origin(origin)
	,m_format(format)
	,m_selectionFormat(selectionFormat)
	,m_longestItem(0)
{
}

ConsoleMenu::ConsoleMenu(const ConsoleMenu& rhs)
	:m_origin(rhs.m_origin)
	,m_format(rhs.m_format)
	,m_selectionFormat(rhs.m_selectionFormat)
	,m_items(rhs.m_items)
	,m_longestItem(rhs.m_longestItem)
{
}
	
COORD ConsoleMenu::Origin() const
{
	return m_origin;
}
COORD ConsoleMenu::Origin(COORD origin)
{
	COORD old = m_origin;
	m_origin = origin;
	return old;
}
ConsoleFormat ConsoleMenu::DisplayFormat() const
{
	return m_format;
}
ConsoleFormat ConsoleMenu::DisplayFormat(ConsoleFormat format)
{
	ConsoleFormat old = m_format;
	m_format = format;
	return old;
}
ConsoleFormat ConsoleMenu::SelectionFormat() const
{
	return m_selectionFormat;
}
ConsoleFormat ConsoleMenu::SelectionFormat(ConsoleFormat format)
{
	ConsoleFormat old = m_selectionFormat;
	m_selectionFormat = format;
	return old;
}

unsigned ConsoleMenu::Count() const
{
	return m_items.size();
}
void ConsoleMenu::Append(ConsoleMenuItem item)
{
	if(item.Text().length() > m_longestItem)
		m_longestItem = item.Text().length();
	m_items.push_back(item);
}

void ConsoleMenu::Append(string text, DWORD value)
{
	if(text.length() > m_longestItem)
		m_longestItem = text.length();
	m_items.push_back(ConsoleMenuItem(text,value));
}
void ConsoleMenu::InsertAfter(ConsoleMenuItem what, Iterator where)
{
	if(what.Text().length() > m_longestItem)
		m_longestItem = what.Text().length();
	m_items.insert(where,what);
}
void ConsoleMenu::Clear()
{
	m_items.clear();
}

ConsoleMenu::ConstIterator ConsoleMenu::GetIterator(BOOL back) const
{
	ConstIterator it = m_items.begin();
	if(back)
		advance(it,m_items.size()-1);
	return it;
}

ConsoleMenu::Iterator ConsoleMenu::GetIterator(BOOL back)
{
	Iterator it = m_items.begin();
	if(back)
		advance(it,m_items.size()-1);
	return it;
}

ConsoleMenu::ConstIterator ConsoleMenu::GetEnd() const
{
	return m_items.end();
}

ConsoleMenu::Iterator ConsoleMenu::GetEnd()
{
	return m_items.end();
}

unsigned ConsoleMenu::LongestItem() const
{
	return m_longestItem;
}

DWORD ConsoleMenu::Show()
{
	DWORD result = BADMENU;
	if(m_items.size() == 0)
		return result;
	ConsoleCore* pCore = ConsoleCore::GetInstance();
	ConsoleFormat oldFormat = pCore->Color();
	COORD bufferOrigin = m_origin;

	COORD bufferSize = {m_longestItem,m_items.size()};
	PCHAR_INFO menuBuffer = new CHAR_INFO[bufferSize.X*bufferSize.Y];
	memset(menuBuffer,0,sizeof(CHAR_INFO)*bufferSize.X*bufferSize.Y);
	
	m_selected = GetIterator();
	do
	{
		// restore the screen.
		COORD cursorOffset = m_origin;
		pCore->LoadScreen(menuBuffer,bufferSize,cursorOffset);
		pCore->CursorPosition(&cursorOffset);
		ConsoleMenu::Iterator it = GetIterator()
			,end = GetEnd();
		cursorOffset.Y = m_origin.Y;
		for(;it != end; ++it)
		{
			if(it == m_selected)
				pCore->Prints(it->Text(),FALSE,&SelectionFormat(),cursorOffset.X,cursorOffset.Y);
			else
				pCore->Prints(it->Text(),FALSE,&DisplayFormat(),cursorOffset.X,cursorOffset.Y);								
			cursorOffset.X = m_origin.X;
			cursorOffset.Y++;
			pCore->CursorPosition(&cursorOffset);				
		}
		// Draw the menu
		pCore->Color(&oldFormat);
		// reset the old format
		pCore->SaveScreen(menuBuffer,bufferSize,bufferOrigin);
		switch(_getch())
		{
		case UP_KEY:
			if(m_selected == GetIterator())
				m_selected = GetIterator(TRUE);
			else
				m_selected--;
			break;
		case DOWN_KEY:
			if(m_selected == GetIterator(TRUE))
				m_selected = GetIterator();
			else							
				m_selected++;
			break;
		case RETURN:
			delete [] menuBuffer;
			pCore->Color(&oldFormat);
			return 0;
			break;
		case ESCAPE:
			delete [] menuBuffer;
			pCore->Color(&oldFormat);
			result = USERESC;
			return result;
		}
	}
	while(TRUE);
}

string ConsoleMenu::SelectedText() const
{
	if(m_selected == m_items.end())
		return "";
	return m_selected->Text();
}

DWORD ConsoleMenu::SelectedValue() const
{
	if(m_selected == m_items.end())
		return -1;
	return m_selected->Value();
}

ConsoleMenuItem ConsoleMenu::SelectedItem()
{
	if(m_selected == m_items.end())
		return ConsoleMenuItem();
	return *m_selected;
}

void ConsoleMenu::Selection(ConsoleMenu::Iterator it)
{
	m_selected = it;
}
ConsoleMenu::Iterator ConsoleMenu::Selection()
{
	return m_selected;
}
////////////////////////////////

ScrollingMenu::ScrollingMenu(COORD origin, unsigned maxToShow
	,ConsoleFormat format, ConsoleFormat selectionFormat)
	:ConsoleMenu(origin, format,selectionFormat)
	,m_maxToShow(maxToShow)
{
}

ScrollingMenu::ScrollingMenu(const ScrollingMenu& rhs)
	:ConsoleMenu(rhs)
{
}

unsigned ScrollingMenu::MaxToShow() const
{
	return m_maxToShow;
}

void ScrollingMenu::MaxToShow(unsigned max)
{
	m_maxToShow = max;
}

DWORD ScrollingMenu::Show()
{
	DWORD result = BADMENU;
	if(Count() == 0)
		return result;
	if(m_maxToShow > Count())
		return BADMENU;
	ConsoleCore* pCore = ConsoleCore::GetInstance();
	unsigned menuOffset = 0;
	COORD bufferOrigin = {Origin().X,Origin().Y}
		,cursorOffset
		,bufferSize = {LongestItem(),m_maxToShow};
	PCHAR_INFO menuBuffer = new CHAR_INFO[bufferSize.X*bufferSize.Y];
	memset(menuBuffer,0,sizeof(CHAR_INFO)*bufferSize.X*bufferSize.Y);
	ConsoleFormat oldFormat = pCore->Color();
	pCore->SaveScreen();
	Selection(GetIterator());
	// The "anchor" is the top-most displayed element.
	// It is used to guide drawing relative items and to scroll the menu.
	Iterator menuAnchor = Selection();
	do
	{
		cursorOffset = Origin();
		pCore->LoadScreen(menuBuffer,bufferSize,cursorOffset);
		pCore->CursorPosition(&cursorOffset);
		// Draw only as many items as needed.
		ConstIterator toDraw;
		unsigned shown = 0;
		for(toDraw = menuAnchor; shown < (menuOffset+m_maxToShow); ++shown,++toDraw)
		{
			if(toDraw == Selection())
				pCore->Prints(toDraw->Text(),FALSE,&SelectionFormat(),cursorOffset.X,cursorOffset.Y);
			else
				pCore->Prints(toDraw->Text(),FALSE,&DisplayFormat(),cursorOffset.X,cursorOffset.Y);

			cursorOffset.X = Origin().X;
			cursorOffset.Y += 1;
		}
		pCore->Color(&oldFormat);
		pCore->SaveScreen(menuBuffer,bufferSize,bufferOrigin);
		switch(_getch())
		{
		case UP_KEY: //Up Arrow
			{
				// Move up when highlighting the anchor
				Iterator selected = Selection();
				if(selected == menuAnchor)
				{
					// Not at the top of the menu?  Just go back one element
					if(menuAnchor != GetIterator())
					{
						menuAnchor--;
						selected--;
					}
					else	// At the top?  Selection becomes bottom, anchor becomes bottom-maxToShow
					{
						selected = GetIterator(TRUE);
						menuAnchor = selected;
						advance(menuAnchor,-(int)(m_maxToShow-1));
					}	
				}
				else
					selected--;
				Selection(selected);
			}
			break;
		case 80: //Down Arrow
			{
				Iterator selected = Selection();
				Iterator bottomAnchor = menuAnchor;
				advance(bottomAnchor,m_maxToShow-1);
				// Moving down when at bottom of list
				if(selected == bottomAnchor)
				{
					// Not at bottom end of list?
					if(bottomAnchor != GetIterator(TRUE))
					{
						menuAnchor++;
						selected++;
					}
					else // At bottom? Selection becomes top, anchor becomes top.
					{
						selected = GetIterator();
						menuAnchor = selected;
					}
				}
				else
					selected++;
				Selection(selected);
			}
			break;
		case RETURN: //\n
			delete [] menuBuffer;
			return 0;
		case ESCAPE: // ESC
			delete [] menuBuffer;
			return USERESC;
		}
	}
	while(TRUE);
}

////////////////////////////////

WindowedMenu::WindowedMenu (COORD origin, unsigned maxToShow, string title
		, ConsoleFormat format
		, ConsoleFormat selectionFormat
		, ConsoleFormat windowColor
		, ConsoleFormat clientColor
		, char fill)
	:ScrollingMenu(origin, maxToShow, format,selectionFormat)
	,m_title(title)
	,m_windowColor(windowColor)
	,m_clientColor(clientColor)
	,m_fill(fill)
	,m_scrollable(maxToShow != 0)
{
}
WindowedMenu ::WindowedMenu (const WindowedMenu & rhs)
	:ScrollingMenu(rhs)
	,m_title(rhs.m_title)
	,m_windowColor(rhs.m_windowColor)
	,m_clientColor(rhs.m_clientColor)
	,m_fill(rhs.m_fill)
{
}
string WindowedMenu ::Title() const
{
	return m_title;
}
void WindowedMenu ::Title(const string title)
{
	m_title = title;
}
BOOL WindowedMenu::Scrollable() const
{
	return m_scrollable;
}
void WindowedMenu::Scrollable(BOOL allow)
{
	m_scrollable = allow;
}
ConsoleFormat WindowedMenu::WindowColor() const
{
	return m_windowColor;
}
ConsoleFormat WindowedMenu::WindowColor(ConsoleFormat color)
{
	ConsoleFormat old = m_windowColor;
	m_windowColor = color;
	return old;
}
ConsoleFormat WindowedMenu::ClientColor() const
{
	return m_clientColor;
}

ConsoleFormat WindowedMenu::ClientColor(ConsoleFormat color)
{
	ConsoleFormat old = m_clientColor;
	m_clientColor = color;
	return old;
}

char WindowedMenu::Fill() const
{
	return m_fill;
}
void WindowedMenu::Fill(char fill)
{
	m_fill = fill;
}
DWORD WindowedMenu::Show()
{
	DWORD result = BADMENU;
	if(Count() == 0)
		return result;
	if(!m_scrollable)
		return ShowNoScroll();
	
	UINT lengthOfLongestString = LongestItem() > Title().length() ? LongestItem() : Title().length();
	COORD ulWindow = Origin(),
		brWindow = {Origin().X + lengthOfLongestString+2,Origin().Y + MaxToShow()+4}
		,menuItemOrigin = {Origin().X+1,Origin().Y+3}
		,oldOrigin = Origin();

	CharacterWindow wnd(ulWindow,brWindow,m_title,WindowColor(),ClientColor(),Fill());
	wnd.Draw();
	oldOrigin = Origin(menuItemOrigin);
	result = ScrollingMenu::Show();
	Origin(oldOrigin);
	return result;
}
DWORD WindowedMenu::ShowNoScroll()
{
	DWORD result = BADMENU;
	if(Count() == 0)
		return result;
	COORD oldOrigin = Origin()
		,menuItemOrigin = {Origin().X+1,Origin().Y+3};

	string longestString = Title();
	UINT lengthOfLongestString = LongestItem() > longestString.length() 
		? LongestItem() : longestString.length();

	COORD ulWindow = Origin(),
		brWindow = {Origin().X + lengthOfLongestString+2,Origin().Y + Count()+4};
	CharacterWindow wnd(ulWindow,brWindow,m_title,WindowColor(),ClientColor(),Fill());
	wnd.Draw();
	
	Origin(menuItemOrigin);
	result = ConsoleMenu::Show();
	Origin(oldOrigin);
	return result;
}