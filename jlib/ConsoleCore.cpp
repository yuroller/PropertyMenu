// Title: Jeff Benson's Console Library
// File: ConsoleCore.cpp
// Author: Jeff Benson
// Date: 6/26/03
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
#include "ConsoleCore.h"
#include "CodeFinder.h"

ConsoleCore* ConsoleCore::m_theOnlyInstance = new ConsoleCore();

ConsoleCore* ConsoleCore::GetInstance()
{
	return m_theOnlyInstance;
}

ConsoleCore::ConsoleCore()
{
	// The console begins by placing the cursor in the upper left
	// console corner.  The default fore and back ground are 
	// white and black respectivley.
	m_cursorPosition.X = 0;
	m_cursorPosition.Y = 0;
	m_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	m_currentFormat.Color(ConsoleFormat::SYSTEM);

	GetConsoleScreenBufferInfo(m_consoleHandle,&m_csbi);
	// Get the screen handle and screen buffer info
	ClearScreen();
	SaveScreen();
	// Set the saved screen to blank.
}

ConsoleCore::~ConsoleCore()
{
	ClearScreen();
	CloseHandle(m_consoleHandle);
	delete m_theOnlyInstance;
}

void ConsoleCore::ClearScreen()
{
	LPWORD lpAttributes = new WORD[MAXSCREENX*MAXSCREENY];
	memset((void*)lpAttributes,0x00,sizeof(WORD)*MAXSCREENX*MAXSCREENY);
	DWORD whatWasWritten;
	COORD origin = {0,0};
	FillConsoleOutputCharacter( m_consoleHandle, ' ', m_csbi.dwSize.X * m_csbi.dwSize.Y, m_cursorPosition, &whatWasWritten );
	WriteConsoleOutputAttribute(m_consoleHandle, lpAttributes,
		MAXSCREENX*MAXSCREENY,origin,&whatWasWritten);
	delete [] lpAttributes;
	CursorPosition(&origin);
	// Place the cursor back on 0,0.
}

void ConsoleCore::SaveScreen()
{
	COORD bufferSize = {MAXSCREENX,MAXSCREENY},
		bufferOrigin = {0,0};
	SMALL_RECT rectToRead = {0,0,MAXSCREENX,MAXSCREENY};
	ReadConsoleOutput(m_consoleHandle,m_screenBuffer,
		bufferSize,bufferOrigin,&rectToRead);
}

void ConsoleCore::SaveScreen(PCHAR_INFO buffer, COORD bufferSize, COORD saveOrigin)
{
	COORD bufferOrigin = {0,0};
	SMALL_RECT rectToRead = {saveOrigin.X,saveOrigin.Y,
		saveOrigin.X + bufferSize.X, saveOrigin.Y + bufferSize.Y};
	ReadConsoleOutput(m_consoleHandle,buffer,
		bufferSize,bufferOrigin,&rectToRead);
}
void ConsoleCore::LoadScreen()
{
	COORD bufferSize = {MAXSCREENX,MAXSCREENY},
		bufferOrigin = {0,0};
	SMALL_RECT rectToWrite = {0,0,MAXSCREENX,MAXSCREENY};
	WriteConsoleOutput(m_consoleHandle,m_screenBuffer,
		bufferSize,bufferOrigin,&rectToWrite);
}

void ConsoleCore::LoadScreen(PCHAR_INFO buffer, COORD bufferSize, COORD loadOrigin)
{
	COORD bufferOrigin = {0,0};
	SMALL_RECT rectToWrite = {loadOrigin.X,loadOrigin.Y,
		loadOrigin.X + bufferSize.X, loadOrigin.Y + bufferSize.Y};
	WriteConsoleOutput(m_consoleHandle,buffer,
		bufferSize,bufferOrigin,&rectToWrite);
}

// Mutator/Accessor combo for the default format
// Pass NULL to get the current value
// Pass a pointer to a CONSOLECHARFROMAT to change
// the color, and get the old color in return
ConsoleFormat ConsoleCore::Color(const ConsoleFormat* newFormat )
{
	if(newFormat != NULL)
	{
		ConsoleFormat oldFormat = m_currentFormat;
		m_currentFormat = *newFormat;
		return oldFormat;
	}
	else
		return m_currentFormat;
}


// Mutator/Accessor combo for the cursor position
// Pass NULL to get the current value
// Pass a pointer to a COORD to change the position
// and get the old position back in return
COORD ConsoleCore::CursorPosition(PCOORD lpPosition )
{
	if(lpPosition != NULL)
	{
		COORD oldPosition = m_cursorPosition;
		m_cursorPosition = *lpPosition;
		m_cursorPosition.X %= MAXSCREENX;
		m_cursorPosition.Y %= MAXSCREENY;
		SetConsoleCursorPosition(m_consoleHandle,m_cursorPosition);
		return oldPosition;
	}
	else
		return m_cursorPosition;
}

COORD ConsoleCore::CursorPosition(SHORT x, SHORT y)
{
	COORD old = m_cursorPosition;
	if(x != -1)
		m_cursorPosition.X = x; 
	if(y != -1)
		m_cursorPosition.Y = y;
	m_cursorPosition.X %= MAXSCREENX;
	m_cursorPosition.Y %= MAXSCREENY;
	return old;
}

void ConsoleCore::Printn(DWORD number, BOOL endLine, ConsoleFormat* color, SHORT x, SHORT y)
{
	char numberAsText[12];
	_itoa_s(number,numberAsText,12,10);
	Prints(numberAsText,endLine,color,x,y);
}
void ConsoleCore::Printd(DOUBLE number, int characterLength, BOOL endLine, ConsoleFormat* color, SHORT x, SHORT y)
{
	char numberAsText[_CVTBUFSIZE];
	_gcvt_s(numberAsText,_CVTBUFSIZE,number,characterLength);
	Prints(numberAsText,endLine,color,x,y);
}

void ConsoleCore::Prints(string text, BOOL endLine, const ConsoleFormat* color, SHORT x, SHORT y)
{
	_Prints(text, endLine, color, x, y);
}

void ConsoleCore::_Prints(string text, BOOL endLine, const ConsoleFormat* color, SHORT x, SHORT y)
{
	const UINT stringLength = text.length();
	if(stringLength == 0)
		return;
	DWORD whatWasWritten = 0;
	CursorPosition(x,y);
	Color(color);
	vector<string> textVector;
	textVector.push_back(text);
	CodeFinder<> codes;
	codes = for_each(textVector.begin(), textVector.end(),codes);
	
	CodeFinder<>::PhraseLocationVector::iterator it = codes.m_phrasesAndLocations.begin()
		,end = codes.m_phrasesAndLocations.end();
	for(;it != end; ++it)
	{
		CodeFinder<>::CodePhraseVector codesAndPairs = RemoveCodes(it);
		CodeFinder<>::CodePhraseVector::reverse_iterator rIt = codesAndPairs.rbegin()
			,rEnd = codesAndPairs.rend();
		for(; rIt != rEnd; ++rIt)
		{
			ConsoleFormat colorCode;
				
			unsigned stringLength = rIt->second.length();
			
			if(rIt->first.length() != 0)
				colorCode = ConsoleFormat(rIt->first.c_str()+1);
			else
				colorCode = m_currentFormat;

			WORD* lpAttributes = new WORD[stringLength];
			memset((void*)lpAttributes,colorCode.Color(),sizeof(WORD)*stringLength);
			WriteConsoleOutputAttribute(m_consoleHandle, lpAttributes,
				stringLength,m_cursorPosition,&whatWasWritten);

			delete [] lpAttributes;
			WriteConsoleOutputCharacter(m_consoleHandle,rIt->second.c_str(),stringLength,
				m_cursorPosition,&whatWasWritten);

			AdvanceCursor((SHORT)whatWasWritten);			
		}		
	}

	if(endLine)
		EndLine();
}

void ConsoleCore::EndLine()
{
	m_cursorPosition.Y++;
	m_cursorPosition.X = 0;
}

void ConsoleCore::AdvanceCursor(SHORT length)
{
	m_cursorPosition.Y %= MAXSCREENY;
	if((m_cursorPosition.X += length) >= MAXSCREENX)
	{
		m_cursorPosition.Y += m_cursorPosition.X / MAXSCREENX;
		m_cursorPosition.X %= MAXSCREENX;
	}
}

void ConsoleCore::ScanNumber(COORD origin, LPDWORD lpNumber, int digitCount)
{
	if(lpNumber == NULL)
		return;
	if(digitCount < 1 || digitCount > 10)
		return;
	const int MAXLENGTH = 12;
	char ch;
	char buffer[MAXLENGTH];
	int x,
		charactersEntered = 0;
	COORD tempOrigin;
	for(x = 0; x < MAXLENGTH; x++)
		buffer[x] = ' ';		
	buffer[x-1] = '\0';
	SaveScreen();
	CursorPosition(&origin);
	Prints(buffer);
	CursorPosition(&origin);
	do
	{	
		ch = _getch();
		switch(ch)
		{
		case 13: // enter
			{
				*lpNumber = atol(buffer);
			} break;
		case 8: // back space
			{
				if(charactersEntered > 0)
				{
					charactersEntered--;
					if(buffer[charactersEntered] != '-')
						digitCount++;
					buffer[charactersEntered] = ' ';
				}
			} break;
		case '-':
			{
				if(charactersEntered == 0)
				{
					buffer[charactersEntered] = '-';
					charactersEntered++;
				}
			} break;
		case '0': // digit
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			{
				if(charactersEntered < MAXLENGTH-1)
				{
					if(digitCount > 0)
					{
						buffer[charactersEntered] = ch;
						charactersEntered++;
						digitCount--;
					}
				}
			} break;
		};
		ClearScreen();
		LoadScreen();
		CursorPosition(&origin);
		Prints(buffer,FALSE);
		tempOrigin = origin;
		tempOrigin.X = origin.X + charactersEntered;
		CursorPosition(&tempOrigin);
	}
	while(ch != 13);
	tempOrigin.X = 0;
	tempOrigin.Y++;
	CursorPosition(&tempOrigin);
}

void ConsoleCore::ScanDouble(COORD origin, DOUBLE* pDouble)
{
	if(pDouble == NULL)
		return;
	const int MAXLENGTH = 24;
	bool decimalPassed = false;
	bool exponentPassed = false;
	bool signPassed = false;
	char ch;
	char buffer[MAXLENGTH];
	int x,
		charactersEntered = 0;
	COORD tempOrigin;
	for(x = 0; x < MAXLENGTH; x++)
		buffer[x] = ' ';		
	buffer[x-1] = '\0';
	SaveScreen();
	CursorPosition(&origin);
	Prints(buffer);
	CursorPosition(&origin);
	do
	{	
		ch = _getch();
		switch(ch)
		{
		case 13: // enter
			{
				*pDouble = atof(buffer);
			} break;
		case 8: // back space
			{
				if(charactersEntered > 0)
				{
					charactersEntered--;
					if(tolower(buffer[charactersEntered]) == 'e')
					{
						exponentPassed = false;
					} 
					else if((buffer[charactersEntered] == '-') || (buffer[charactersEntered] == '+'))
					{
						signPassed = false;
					}
					else if(buffer[charactersEntered] == '.')
						decimalPassed = false;
					buffer[charactersEntered] = ' ';
				}
			} break;
		case 'e':
		case 'E':
			{
				if(!exponentPassed)
				{
					if(charactersEntered < MAXLENGTH-3)
					{
						buffer[charactersEntered] = ch;
						charactersEntered++;
					}
					exponentPassed = true;
				}
			} break;
		case '.':
			{
				if(!decimalPassed && !exponentPassed)
				{
					if(charactersEntered < MAXLENGTH-1)
					{
						buffer[charactersEntered] = ch;
						charactersEntered++;
						decimalPassed = true;
					}
				}
			} break;
		case '+':
		case '-':
			{
				if(exponentPassed && !signPassed)
				{
					exponentPassed = true;
					if(charactersEntered < MAXLENGTH-2)
					{
						buffer[charactersEntered] = ch;
						charactersEntered++;
						signPassed = true;
					}
				}
			} break;
		case '0': // digit
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			{
				if(charactersEntered < MAXLENGTH-1)
				{
					if(exponentPassed == signPassed)
					{
						buffer[charactersEntered] = ch;
						charactersEntered++;
					}
				}
			} break;
		};
		ClearScreen();
		LoadScreen();
		CursorPosition(&origin);
		Prints(buffer,FALSE);
		tempOrigin = origin;
		tempOrigin.X = origin.X + charactersEntered;
		CursorPosition(&tempOrigin);
	}
	while(ch != 13);
	tempOrigin.X = 0;
	tempOrigin.Y++;
	CursorPosition(&tempOrigin);
}

void ConsoleCore::ScanString(COORD origin, char* buffer, UINT maxLength)
{
	if(buffer == NULL)
		return;
	char ch;
	UINT x,
		charactersEntered = 0;
	COORD tempOrigin;
	for(x = 0; x < maxLength; x++)
		buffer[x] = ' ';		
	buffer[x-1] = '\0';
	SaveScreen();
	CursorPosition(&origin);
	Prints(buffer);
	CursorPosition(&origin);
	do
	{	
		ch = _getch();
		switch(ch)
		{
		case 13: // enter
			{
			} break;
		case 8: // back space
			{
				if(charactersEntered > 0)
				{
					charactersEntered--;
					buffer[charactersEntered] = ' ';							
				}
			} break;
		default: //A char or digit
			{
				if(charactersEntered < (maxLength-1))
				{
					buffer[charactersEntered] = ch;
					charactersEntered++;
				}
			} break;
		};
		ClearScreen();
		LoadScreen();
		CursorPosition(&origin);
		Prints(buffer,FALSE);
		tempOrigin = origin;
		tempOrigin.X = origin.X + charactersEntered;
		CursorPosition(&tempOrigin);
	}
	while(ch != 13);
	tempOrigin.X = 0;
	tempOrigin.Y++;
	CursorPosition(&tempOrigin);
}