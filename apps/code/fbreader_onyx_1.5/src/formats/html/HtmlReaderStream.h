/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __HTMLREADERSTREAM_H__
#define __HTMLREADERSTREAM_H__

#include <shared_ptr.h>
#include <ZLInputStream.h>

class HtmlReaderStream : public ZLInputStream {

public:
	HtmlReaderStream(fb::shared_ptr<ZLInputStream> base, size_t maxSize);
	~HtmlReaderStream();

private:
	bool open();
	size_t read(char *buffer, size_t maxSize);
	void close();

	void seek(int offset, bool absoluteOffset);
	size_t offset() const;
	size_t sizeOfOpened();

private:
	fb::shared_ptr<ZLInputStream> myBase;
	char *myBuffer;
	size_t mySize;
	size_t myOffset;
};

#endif /* __HTMLREADERSTREAM_H__ */
