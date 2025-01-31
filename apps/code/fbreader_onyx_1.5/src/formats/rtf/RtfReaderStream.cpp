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

#include <string>
#include <string.h>
#include <stdlib.h>
#include <iostream>


#include "RtfReader.h"
#include "RtfReaderStream.h"

class RtfTextOnlyReader : public RtfReader {

public:
	RtfTextOnlyReader(char *buffer, size_t maxSize);
	~RtfTextOnlyReader();
	size_t readSize() const;

protected:
	void addCharData(const char *data, size_t len, bool convert);
	void insertImage(const std::string &mimeType, const std::string &fileName, size_t startOffset, size_t size);
	void setEncoding(int code);
	void switchDestination(DestinationType destination, bool on);
	void setAlignment();
	void setFontProperty(FontProperty property);
	void newParagraph();

	void interrupt();

private:
	struct RtfTextOnlyReaderState {
        bool ReadText;
    };

    RtfTextOnlyReaderState myCurrentState;

private:
	char* myBuffer;
	const size_t myMaxSize;
	size_t myFilledSize;
};

RtfTextOnlyReader::RtfTextOnlyReader(char *buffer, size_t maxSize) : RtfReader(std::string()), 
																	myBuffer(buffer), 
																	myMaxSize(maxSize), 
																	myFilledSize(0) {
	myCurrentState.ReadText = true;
}

RtfTextOnlyReader::~RtfTextOnlyReader() {
}

void RtfTextOnlyReader::addCharData(const char *data, size_t len, bool) {
    if (myBuffer == 0) {
        return;
    }
    if (myCurrentState.ReadText) {
		if (myFilledSize < myMaxSize) {
        	len = std::min((size_t)len, myMaxSize - myFilledSize);
        	memcpy(myBuffer + myFilledSize, data, len);
       		myFilledSize += len;
			myBuffer[myFilledSize++]=' ';
    	} else {
        	interrupt();
    	}
	}
}

size_t RtfTextOnlyReader::readSize() const {
	return myFilledSize;
}

void RtfTextOnlyReader::insertImage(const std::string&, const std::string&, size_t, size_t) {
}

void RtfTextOnlyReader::setEncoding(int) {
}

void RtfTextOnlyReader::switchDestination(DestinationType destination, bool on) {
	switch (destination) {
        case DESTINATION_NONE:
            break;
        case DESTINATION_SKIP:
        case DESTINATION_INFO:
        case DESTINATION_TITLE:
        case DESTINATION_AUTHOR:
        case DESTINATION_STYLESHEET:
            myCurrentState.ReadText = !on;
            break;
        case DESTINATION_PICTURE:
            myCurrentState.ReadText = !on;
            break;
        case DESTINATION_FOOTNOTE:
            if (on) {
				myCurrentState.ReadText = true;
			}
			break;
	}
}

void RtfTextOnlyReader::setAlignment() {
}

void RtfTextOnlyReader::setFontProperty(FontProperty) {
}

void RtfTextOnlyReader::newParagraph() {
}

void RtfTextOnlyReader::interrupt() {
}

RtfReaderStream::RtfReaderStream(const std::string& fileName, size_t maxSize) : myFileName(fileName), myBuffer(0), mySize(maxSize) {
}

RtfReaderStream::~RtfReaderStream() {
    close();
}

bool RtfReaderStream::open() {
    if (mySize != 0) {
		myBuffer = new char[mySize];
	}
    RtfTextOnlyReader reader(myBuffer, mySize);
    reader.readDocument(myFileName);
    mySize = reader.readSize();
    myOffset = 0;
    return true;
}

size_t RtfReaderStream::read(char *buffer, size_t maxSize) {
    maxSize = std::min(maxSize, mySize - myOffset);
    if ((buffer != 0) && (myBuffer !=0)) {
        memcpy(buffer, myBuffer, maxSize);
    }
    myOffset += maxSize;
    return maxSize;
}

void RtfReaderStream::close() {
    if (myBuffer != 0) {
        delete[] myBuffer;
        myBuffer = 0;
    }
}

void RtfReaderStream::seek(int offset, bool absoluteOffset) {
    if (!absoluteOffset) {
        offset += myOffset;
    }
    myOffset = std::min(mySize, (size_t)std::max(0, offset));
}

size_t RtfReaderStream::offset() const {
    return myOffset;
}

size_t RtfReaderStream::sizeOfOpened() {
    return mySize;
}

