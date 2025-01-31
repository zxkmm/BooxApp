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

#include <ZLStringUtil.h>
#include <ZLFile.h>
#include <ZLInputStream.h>

#include "HtmlPlugin.h"
#include "HtmlDescriptionReader.h"
#include "HtmlBookReader.h"
#include "HtmlReaderStream.h"
#include "../txt/PlainTextFormat.h"
#include "../util/MiscUtil.h"

#include "../../database/booksdb/DBBook.h"

bool HtmlPlugin::acceptsFile(const ZLFile &file) const {
	const std::string &extension = file.extension();
	return ZLStringUtil::stringEndsWith(extension, "html") || (extension == "htm");
}

bool HtmlPlugin::readDescription(const std::string &path, DBBook &book) const {
	ZLFile file(path);
	fb::shared_ptr<ZLInputStream> stream = file.inputStream();
	if (stream.isNull()) {
		return false;
	}

	fb::shared_ptr<ZLInputStream> htmlStream = new HtmlReaderStream(stream, 50000);
	detectEncodingAndLanguage(book, *htmlStream);
	if (book.encoding().empty()) {
		return false;
	}
	HtmlDescriptionReader(book).readDocument(*stream);

	return true;
}

bool HtmlPlugin::readModel(const DBBook &book, BookModel &model) const {
	std::string fileName = book.fileName();
	fb::shared_ptr<ZLInputStream> stream = ZLFile(fileName).inputStream();
	if (stream.isNull()) {
		return false;
	}

	PlainTextFormat format(fileName);
	if (!format.initialized()) {
		PlainTextFormatDetector detector;
		detector.detect(*stream, format);
	}

	std::string directoryPrefix = MiscUtil::htmlDirectoryPrefix(fileName);
	HtmlBookReader reader(directoryPrefix, model, format, book.encoding());
	reader.setFileName(MiscUtil::htmlFileName(fileName));
	reader.readDocument(*stream);

	return true;
}

const std::string &HtmlPlugin::iconName() const {
	static const std::string ICON_NAME = "html";
	return ICON_NAME;
}

FormatInfoPage *HtmlPlugin::createInfoPage(ZLOptionsDialog &dialog, const std::string &fileName) {
	return new PlainTextInfoPage(dialog, fileName, ZLResourceKey("<PRE>"), false);
}
