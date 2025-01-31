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

#include <ZLFile.h>
#include <iostream>

#include "PdbReader.h"

void PdbUtil::readUnsignedShort(ZLInputStream &stream, unsigned short &N) {
	unsigned char data[2];
	stream.read((char*)data, 2);
	N = (((unsigned short)data[0]) << 8) + data[1];
}

void PdbUtil::readUnsignedLongBE(ZLInputStream &stream, unsigned long &N) {
	unsigned char data[4];
	stream.read((char*)data, 4);
	N = (((unsigned long)data[0]) << 24) +
			(((unsigned long)data[1]) << 16) +
			(((unsigned long)data[2]) << 8) +
			(unsigned long)data[3];
}

void PdbUtil::readUnsignedLongLE(ZLInputStream &stream, unsigned long &N) {
	unsigned char data[4];
	stream.read((char*)data, 4);
	N = (((unsigned long)data[3]) << 24) +
			(((unsigned long)data[2]) << 16) +
			(((unsigned long)data[1]) << 8) +
			(unsigned long)data[0];
}

bool PdbHeader::read(fb::shared_ptr<ZLInputStream> stream) {
	const size_t startOffset = stream->offset();
	DocName.erase();
	DocName.append(32, '\0');
	stream->read((char*)DocName.data(), 32); 			// stream offset: +32

	PdbUtil::readUnsignedShort(*stream, Flags); 		// stream offset: +34

	stream->seek(26, false);							// stream offset: +60
	
	Id.erase();
	Id.append(8, '\0');
	stream->read((char*)Id.data(), 8);					// stream offset: +68

	stream->seek(8, false);								// stream offset: +76
	Offsets.clear();
	unsigned short numRecords;
	PdbUtil::readUnsignedShort(*stream, numRecords); 	// stream offset: +78
	Offsets.reserve(numRecords);

	for (int i = 0; i < numRecords; ++i) {				// stream offset: +78 + 8 * records number  
		unsigned long recordOffset;
		PdbUtil::readUnsignedLongBE(*stream, recordOffset);
		Offsets.push_back(recordOffset);
		stream->seek(4, false);
	}
	return stream->offset() == startOffset + 78 + 8 * numRecords;
}

/*bool PdbRecord0::read(fb::shared_ptr<ZLInputStream> stream) {
	size_t startOffset = stream->offset();
	
	PdbUtil::readUnsignedShort(*stream, CompressionType);    
	PdbUtil::readUnsignedShort(*stream, Spare);          
	PdbUtil::readUnsignedLongBE(*stream, TextLength);     
	PdbUtil::readUnsignedShort(*stream, TextRecords);    
	PdbUtil::readUnsignedShort(*stream, MaxRecordSize);     
	PdbUtil::readUnsignedShort(*stream, NontextOffset);  
	PdbUtil::readUnsignedShort(*stream, NontextOffset2); 

	PdbUtil::readUnsignedLongBE(*stream, MobipocketID);
	PdbUtil::readUnsignedLongBE(*stream, MobipocketHeaderSize);
	PdbUtil::readUnsignedLongBE(*stream, Unknown24);
	PdbUtil::readUnsignedShort(*stream, FootnoteRecs);
	PdbUtil::readUnsignedShort(*stream, SidebarRecs);

	PdbUtil::readUnsignedShort(*stream, BookmarkOffset);
	PdbUtil::readUnsignedShort(*stream, Unknown34);
	PdbUtil::readUnsignedShort(*stream, NontextOffset3);
	PdbUtil::readUnsignedShort(*stream, Unknown38);
	PdbUtil::readUnsignedShort(*stream, ImagedataOffset);
	PdbUtil::readUnsignedShort(*stream, ImagedataOffset2);
	PdbUtil::readUnsignedShort(*stream, MetadataOffset);
	PdbUtil::readUnsignedShort(*stream, MetadataOffset2);
	PdbUtil::readUnsignedShort(*stream, FootnoteOffset);
	PdbUtil::readUnsignedShort(*stream, SidebarOffset);
	PdbUtil::readUnsignedShort(*stream, LastDataOffset);
	PdbUtil::readUnsignedShort(*stream, Unknown54);
	
	return stream->offset() == startOffset + 56;
}*/
