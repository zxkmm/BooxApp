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

#ifndef __RTFDESCRIPTIONREADER_H__
#define __RTFDESCRIPTIONREADER_H__

#include <string>

#include "RtfReader.h"
#include "../../database/booksdb/DBBook.h"

class RtfDescriptionReader : public RtfReader {

public:
	RtfDescriptionReader(DBBook &book);
	~RtfDescriptionReader();

	bool readDocument(const std::string &fileName);

	void setEncoding(int code);
	void setAlignment();
	void switchDestination(DestinationType destination, bool on);
	void addCharData(const char *data, size_t len, bool convert);
	void insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size);

	void setFontProperty(FontProperty property);
	void newParagraph();

private:
	DBBook &myBook;

	bool myDoRead;
	std::string myBuffer;
};

inline RtfDescriptionReader::~RtfDescriptionReader() {}

#endif /* __RTFDESCRIPTIONREADER_H__ */
