/*
 * Copyright (C) 2004-2009 Geometer Plus <contact@geometerplus.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __HTMLDESCRIPTIONREADER_H__
#define __HTMLDESCRIPTIONREADER_H__

#include "HtmlReader.h"
#include "../../database/booksdb/DBBook.h"

class HtmlDescriptionReader : public HtmlReader {

public:
	HtmlDescriptionReader(DBBook &book);
	~HtmlDescriptionReader();

protected:
	void startDocumentHandler();
	void endDocumentHandler();

	bool tagHandler(const HtmlTag &tag);
	bool characterDataHandler(const char *text, size_t len, bool convert);

private:
	bool myReadTitle;
	DBBook &myBook;
};

inline HtmlDescriptionReader::~HtmlDescriptionReader() {}

#endif /* __HTMLDESCRIPTIONREADER_H__ */
