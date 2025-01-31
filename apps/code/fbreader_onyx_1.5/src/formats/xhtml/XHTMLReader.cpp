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

#include <iostream>

#include <cstring>

#include <ZLFile.h>
#include <ZLFileImage.h>
#include <ZLUnicodeUtil.h>
#include <ZLStringUtil.h>

#include "XHTMLReader.h"
#include "../util/EntityFilesCollector.h"
#include "../util/MiscUtil.h"
#include "../css/StyleSheetParser.h"

#include "../../bookmodel/BookReader.h"
#include "../../bookmodel/BookModel.h"

static const bool USE_CSS = false;

std::map<std::string,XHTMLTagAction*> XHTMLReader::ourTagActions;

XHTMLTagAction::~XHTMLTagAction() {
}

BookReader &XHTMLTagAction::bookReader(XHTMLReader &reader) {
	return reader.myModelReader;
}

const std::string &XHTMLTagAction::pathPrefix(XHTMLReader &reader) {
	return reader.myPathPrefix;
}

void XHTMLTagAction::beginParagraph(XHTMLReader &reader) {
	reader.beginParagraph();
}

void XHTMLTagAction::endParagraph(XHTMLReader &reader) {
	reader.endParagraph();
}

class XHTMLTagLinkAction : public XHTMLTagAction {

public:
	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagParagraphAction : public XHTMLTagAction {

public:
	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagBodyAction : public XHTMLTagAction {

public:
	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagRestartParagraphAction : public XHTMLTagAction {

public:
	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagImageAction : public XHTMLTagAction {

public:
	XHTMLTagImageAction(const std::string &nameAttribute);

	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);

private:
	const std::string myNameAttribute;
};

class XHTMLTagItemAction : public XHTMLTagAction {

public:
	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);
};

class XHTMLTagHyperlinkAction : public XHTMLTagAction {

public:
	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);

private:
	std::stack<FBTextKind> myHyperlinkStack;
};

class XHTMLTagControlAction : public XHTMLTagAction {

public:
	XHTMLTagControlAction(FBTextKind control);

	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);

private:
	FBTextKind myControl;
};

class XHTMLTagParagraphWithControlAction : public XHTMLTagAction {

public:
	XHTMLTagParagraphWithControlAction(FBTextKind control);

	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);

private:
	FBTextKind myControl;
};

class XHTMLTagPreAction : public XHTMLTagAction {

public:
	void doAtStart(XHTMLReader &reader, const char **xmlattributes);
	void doAtEnd(XHTMLReader &reader);
};

void XHTMLTagLinkAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
	static const std::string REL = "stylesheet";
	const char *rel = reader.attributeValue(xmlattributes, "rel");
	if ((rel == 0) || (REL != rel)) {
		return;
	}
	static const std::string TYPE = "text/css";

	const char *type = reader.attributeValue(xmlattributes, "type");
	if ((type == 0) || (TYPE != type)) {
		return;
	}

	const char *href = reader.attributeValue(xmlattributes, "href");
	if (href == 0) {
		return;
	}

	fb::shared_ptr<ZLInputStream> cssStream = ZLFile(reader.myPathPrefix + href).inputStream();
	if (cssStream.isNull()) {
		return;
	}
	StyleSheetTableParser parser(reader.myStyleSheetTable);
	parser.parse(*cssStream);
	//reader.myStyleSheetTable.dump();
}

void XHTMLTagLinkAction::doAtEnd(XHTMLReader&) {
}

void XHTMLTagParagraphAction::doAtStart(XHTMLReader &reader, const char**) {
	if (!reader.myNewParagraphInProgress) {
		beginParagraph(reader);
		reader.myNewParagraphInProgress = true;
	}
}

void XHTMLTagParagraphAction::doAtEnd(XHTMLReader &reader) {
	endParagraph(reader);
}

void XHTMLTagBodyAction::doAtStart(XHTMLReader &reader, const char**) {
	reader.myInsideBody = true;
}

void XHTMLTagBodyAction::doAtEnd(XHTMLReader &reader) {
	endParagraph(reader);
	reader.myInsideBody = false;
}

void XHTMLTagRestartParagraphAction::doAtStart(XHTMLReader &reader, const char**) {
	if (reader.myCurrentParagraphIsEmpty) {
		bookReader(reader).addData(" ");
	}
	endParagraph(reader);
	beginParagraph(reader);
}

void XHTMLTagRestartParagraphAction::doAtEnd(XHTMLReader&) {
}

void XHTMLTagItemAction::doAtStart(XHTMLReader &reader, const char**) {
	endParagraph(reader);
	// TODO: increase left indent
	beginParagraph(reader);
	// TODO: replace bullet sign by number inside OL tag
	const std::string bullet = "\xE2\x80\xA2\xC0\xA0";
	bookReader(reader).addData(bullet);
}

void XHTMLTagItemAction::doAtEnd(XHTMLReader &reader) {
	endParagraph(reader);
}

XHTMLTagImageAction::XHTMLTagImageAction(const std::string &nameAttribute) : myNameAttribute(nameAttribute) {
}

void XHTMLTagImageAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
	const char *fileName = reader.attributeValue(xmlattributes, myNameAttribute.c_str());
	if (fileName == 0) {
		return;
	}

	const std::string fullfileName = pathPrefix(reader) + fileName;
	if (!ZLFile(fullfileName).exists()) {
		return;
	}

	bool flag = bookReader(reader).paragraphIsOpen();
	if (flag) {
		endParagraph(reader);
	}
	if ((strlen(fileName) > 2) && strncmp(fileName, "./", 2) == 0) {
		fileName +=2;
	}
	bookReader(reader).addImageReference(fullfileName);
	bookReader(reader).addImage(fullfileName, new ZLFileImage("image/auto", fullfileName, 0));
	if (flag) {
		beginParagraph(reader);
	}
}

void XHTMLTagImageAction::doAtEnd(XHTMLReader&) {
}

XHTMLTagControlAction::XHTMLTagControlAction(FBTextKind control) : myControl(control) {
}

void XHTMLTagControlAction::doAtStart(XHTMLReader &reader, const char**) {
	bookReader(reader).pushKind(myControl);
	bookReader(reader).addControl(myControl, true);
}

void XHTMLTagControlAction::doAtEnd(XHTMLReader &reader) {
	bookReader(reader).addControl(myControl, false);
	bookReader(reader).popKind();
}

void XHTMLTagHyperlinkAction::doAtStart(XHTMLReader &reader, const char **xmlattributes) {
	const char *href = reader.attributeValue(xmlattributes, "href");
	if (href != 0) {
		const std::string link = (*href == '#') ? (reader.myReferenceName + href) : href;
		const FBTextKind hyperlinkType = MiscUtil::referenceType(link);
		myHyperlinkStack.push(hyperlinkType);
		bookReader(reader).addHyperlinkControl(hyperlinkType, link);
	} else {
		myHyperlinkStack.push(REGULAR);
	}
	const char *name = reader.attributeValue(xmlattributes, "name");
	if (name != 0) {
		bookReader(reader).addHyperlinkLabel(reader.myReferenceName + "#" + name);
	}
}

void XHTMLTagHyperlinkAction::doAtEnd(XHTMLReader &reader) {
	FBTextKind kind = myHyperlinkStack.top();
	if (kind != REGULAR) {
		bookReader(reader).addControl(kind, false);
	}
	myHyperlinkStack.pop();
}

XHTMLTagParagraphWithControlAction::XHTMLTagParagraphWithControlAction(FBTextKind control) : myControl(control) {
}

void XHTMLTagParagraphWithControlAction::doAtStart(XHTMLReader &reader, const char**) {
	if ((myControl == TITLE) && (bookReader(reader).model().bookTextModel()->paragraphsNumber() > 1)) {
		bookReader(reader).insertEndOfSectionParagraph();
	}
	bookReader(reader).pushKind(myControl);
	beginParagraph(reader);
}

void XHTMLTagParagraphWithControlAction::doAtEnd(XHTMLReader &reader) {
	endParagraph(reader);
	bookReader(reader).popKind();
}

void XHTMLTagPreAction::doAtStart(XHTMLReader &reader, const char**) {
	reader.myPreformatted = true;
	beginParagraph(reader);
	bookReader(reader).addControl(CODE, true);
}

void XHTMLTagPreAction::doAtEnd(XHTMLReader &reader) {
	bookReader(reader).addControl(CODE, false);
	endParagraph(reader);
	reader.myPreformatted = false;
}

XHTMLTagAction *XHTMLReader::addAction(const std::string &tag, XHTMLTagAction *action) {
	XHTMLTagAction *old = ourTagActions[tag];
	ourTagActions[tag] = action;
	return old;
}

void XHTMLReader::fillTagTable() {
	if (ourTagActions.empty()) {
		//addAction("html",	new XHTMLTagAction());
		addAction("body",	new XHTMLTagBodyAction());
		//addAction("title",	new XHTMLTagAction());
		//addAction("meta",	new XHTMLTagAction());
		//addAction("script",	new XHTMLTagAction());

		//addAction("font",	new XHTMLTagAction());
		//addAction("style",	new XHTMLTagAction());

		addAction("p",	new XHTMLTagParagraphAction());
		addAction("h1",	new XHTMLTagParagraphWithControlAction(H1));
		addAction("h2",	new XHTMLTagParagraphWithControlAction(H2));
		addAction("h3",	new XHTMLTagParagraphWithControlAction(H3));
		addAction("h4",	new XHTMLTagParagraphWithControlAction(H4));
		addAction("h5",	new XHTMLTagParagraphWithControlAction(H5));
		addAction("h6",	new XHTMLTagParagraphWithControlAction(H6));

		//addAction("ol",	new XHTMLTagAction());
		//addAction("ul",	new XHTMLTagAction());
		//addAction("dl",	new XHTMLTagAction());
		addAction("li",	new XHTMLTagItemAction());

		addAction("strong",	new XHTMLTagControlAction(STRONG));
		addAction("b",	new XHTMLTagControlAction(BOLD));
		addAction("em",	new XHTMLTagControlAction(EMPHASIS));
		addAction("i",	new XHTMLTagControlAction(ITALIC));
		addAction("code",	new XHTMLTagControlAction(CODE));
		addAction("tt",	new XHTMLTagControlAction(CODE));
		addAction("kbd",	new XHTMLTagControlAction(CODE));
		addAction("var",	new XHTMLTagControlAction(CODE));
		addAction("samp",	new XHTMLTagControlAction(CODE));
		addAction("cite",	new XHTMLTagControlAction(CITE));
		addAction("sub",	new XHTMLTagControlAction(SUB));
		addAction("sup",	new XHTMLTagControlAction(SUP));
		addAction("dd",	new XHTMLTagControlAction(DEFINITION_DESCRIPTION));
		addAction("dfn",	new XHTMLTagControlAction(DEFINITION));
		addAction("strike",	new XHTMLTagControlAction(STRIKETHROUGH));

		addAction("a",	new XHTMLTagHyperlinkAction());

		addAction("img",	new XHTMLTagImageAction("src"));
		addAction("object",	new XHTMLTagImageAction("data"));

		//addAction("area",	new XHTMLTagAction());
		//addAction("map",	new XHTMLTagAction());

		//addAction("base",	new XHTMLTagAction());
		//addAction("blockquote",	new XHTMLTagAction());
		addAction("br",	new XHTMLTagRestartParagraphAction());
		//addAction("center",	new XHTMLTagAction());
		addAction("div", new XHTMLTagParagraphAction());
		addAction("dt", new XHTMLTagParagraphAction());
		//addAction("head",	new XHTMLTagAction());
		//addAction("hr",	new XHTMLTagAction());
		addAction("link",	new XHTMLTagLinkAction());
		//addAction("param",	new XHTMLTagAction());
		//addAction("q",	new XHTMLTagAction());
		//addAction("s",	new XHTMLTagAction());

		addAction("pre",	new XHTMLTagPreAction());
		//addAction("big",	new XHTMLTagAction());
		//addAction("small",	new XHTMLTagAction());
		//addAction("u",	new XHTMLTagAction());

		//addAction("table",	new XHTMLTagAction());
		addAction("td",	new XHTMLTagParagraphAction());
		addAction("th",	new XHTMLTagParagraphAction());
		//addAction("tr",	new XHTMLTagAction());
		//addAction("caption",	new XHTMLTagAction());
		//addAction("span",	new XHTMLTagAction());
	}
}

XHTMLReader::XHTMLReader(BookReader &modelReader) : myModelReader(modelReader) {
}

bool XHTMLReader::readFile(const std::string &filePath, const std::string &referenceName) {
	myModelReader.addHyperlinkLabel(referenceName);

	fillTagTable();

	myPathPrefix = MiscUtil::htmlDirectoryPrefix(filePath);
	myReferenceName = referenceName;

	myPreformatted = false;
	myNewParagraphInProgress = false;
	myInsideBody = false;

	myCSSStack.clear();
	myStyleEntryStack.clear();
	myStylesToRemove = 0;

	return readDocument(filePath);
}

void XHTMLReader::addStyleEntry(const std::string tag, const std::string aClass) {
	fb::shared_ptr<ZLTextStyleEntry> entry = myStyleSheetTable.control(tag, aClass);
	if (!entry.isNull()) {
		myModelReader.addControl(*entry);
		myStyleEntryStack.push_back(entry);
	}
}

void XHTMLReader::startElementHandler(const char *tag, const char **attributes) {
	static const std::string HASH = "#";
	const char *id = attributeValue(attributes, "id");
	if (id != 0) {
		myModelReader.addHyperlinkLabel(myReferenceName + HASH + id);
	}

	const std::string sTag = ZLUnicodeUtil::toLower(tag);

	const char *aClass = attributeValue(attributes, "class");
	const std::string sClass = (aClass != 0) ? aClass : "";

	if (myStyleSheetTable.doBreakBefore(sTag, sClass)) {
		myModelReader.insertEndOfSectionParagraph();
	}
	myDoPageBreakAfterStack.push_back(myStyleSheetTable.doBreakAfter(sTag, sClass));

	XHTMLTagAction *action = ourTagActions[sTag];
	if (action != 0) {
		action->doAtStart(*this, attributes);
	}

	const int sizeBefore = myStyleEntryStack.size();
	addStyleEntry(sTag, "");
	addStyleEntry("", sClass);
	addStyleEntry(sTag, sClass);
	const char *style = attributeValue(attributes, "style");
	if (style != 0) {
		fb::shared_ptr<ZLTextStyleEntry> entry = myStyleParser.parseString(style);
		myModelReader.addControl(*entry);
		myStyleEntryStack.push_back(entry);
	}
	myCSSStack.push_back(myStyleEntryStack.size() - sizeBefore);
}

void XHTMLReader::endElementHandler(const char *tag) {
	for (int i = myCSSStack.back(); i > 0; --i) {
		myModelReader.addControl(REGULAR, false);
	}
	myStylesToRemove = myCSSStack.back();
	myCSSStack.pop_back();

	XHTMLTagAction *action = ourTagActions[ZLUnicodeUtil::toLower(tag)];
	if (action != 0) {
		action->doAtEnd(*this);
		myNewParagraphInProgress = false;
	}

	for (; myStylesToRemove > 0; --myStylesToRemove) {
		myStyleEntryStack.pop_back();
	}

	if (myDoPageBreakAfterStack.back()) {
		myModelReader.insertEndOfSectionParagraph();
	}
	myDoPageBreakAfterStack.pop_back();
}

void XHTMLReader::beginParagraph() {
	myCurrentParagraphIsEmpty = true;
	myModelReader.beginParagraph();
	bool doBlockSpaceBefore = false;
	for (std::vector<fb::shared_ptr<ZLTextStyleEntry> >::const_iterator it = myStyleEntryStack.begin(); it != myStyleEntryStack.end(); ++it) {
		myModelReader.addControl(**it);
		doBlockSpaceBefore =
			doBlockSpaceBefore ||
			(*it)->lengthSupported(ZLTextStyleEntry::LENGTH_SPACE_BEFORE);
	}

	if (doBlockSpaceBefore) {
		ZLTextStyleEntry blockingEntry;
		blockingEntry.setLength(
			ZLTextStyleEntry::LENGTH_SPACE_BEFORE,
			0,
			ZLTextStyleEntry::SIZE_UNIT_PIXEL
		);
		myModelReader.addControl(blockingEntry);
	}
}

void XHTMLReader::endParagraph() {
	bool doBlockSpaceAfter = false;
	for (std::vector<fb::shared_ptr<ZLTextStyleEntry> >::const_iterator it = myStyleEntryStack.begin(); it != myStyleEntryStack.end() - myStylesToRemove; ++it) {
		doBlockSpaceAfter =
			doBlockSpaceAfter ||
			(*it)->lengthSupported(ZLTextStyleEntry::LENGTH_SPACE_AFTER);
	}
	if (doBlockSpaceAfter) {
		ZLTextStyleEntry blockingEntry;
		blockingEntry.setLength(
			ZLTextStyleEntry::LENGTH_SPACE_AFTER,
			0,
			ZLTextStyleEntry::SIZE_UNIT_PIXEL
		);
		myModelReader.addControl(blockingEntry);
	}
	for (; myStylesToRemove > 0; --myStylesToRemove) {
		myModelReader.addControl(*myStyleEntryStack.back());
		myStyleEntryStack.pop_back();
	}
	myModelReader.endParagraph();
}

void XHTMLReader::characterDataHandler(const char *text, size_t len) {
	if (myPreformatted) {
		if ((*text == '\r') || (*text == '\n')) {
			myModelReader.addControl(CODE, false);
			endParagraph();
			beginParagraph();
			myModelReader.addControl(CODE, true);
		}
		size_t spaceCounter = 0;
		while ((spaceCounter < len) && isspace((unsigned char)*(text + spaceCounter))) {
			++spaceCounter;
		}
		myModelReader.addFixedHSpace(spaceCounter);
		text += spaceCounter;
		len -= spaceCounter;
	} else if ((myNewParagraphInProgress) || !myModelReader.paragraphIsOpen()) {
		while (isspace((unsigned char)*text)) {
			++text;
			if (--len == 0) {
				break;
			}
		}
	}
	if (len > 0) {
		myCurrentParagraphIsEmpty = false;
		if (myInsideBody && !myModelReader.paragraphIsOpen()) {
			myModelReader.beginParagraph();
		}
		myModelReader.addData(std::string(text, len));
		myNewParagraphInProgress = false;
	}
}

const std::vector<std::string> &XHTMLReader::externalDTDs() const {
	return EntityFilesCollector::instance().externalDTDs("xhtml");
}
