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

#ifndef __ZLTEXTELEMENT_H__
#define __ZLTEXTELEMENT_H__

#include <ZLImageManager.h>

#include <ZLTextKind.h>
#include <ZLTextParagraph.h>

class ZLTextElement {

protected:
	ZLTextElement();

public:
	virtual ~ZLTextElement();

	enum Kind {
		WORD_ELEMENT,
		IMAGE_ELEMENT,
		CONTROL_ELEMENT,
		FORCED_CONTROL_ELEMENT,
		INDENT_ELEMENT,
		HSPACE_ELEMENT,
		NB_HSPACE_ELEMENT,
		FIXED_HSPACE_ELEMENT,
		BEFORE_PARAGRAPH_ELEMENT,
		AFTER_PARAGRAPH_ELEMENT,
		EMPTY_LINE_ELEMENT,
		START_REVERSED_SEQUENCE_ELEMENT,
		END_REVERSED_SEQUENCE_ELEMENT,
	};

	virtual Kind kind() const = 0;

private:
	// assignment and copy constructor are disabled
	ZLTextElement(const ZLTextElement&);
	ZLTextElement &operator = (const ZLTextElement&);
};

class ZLTextImageElement : public ZLTextElement {

public:
	ZLTextImageElement(const std::string &id, fb::shared_ptr<ZLImageData> image);
	~ZLTextImageElement();
	fb::shared_ptr<ZLImageData> image() const;
	const std::string &id() const;

private:
	Kind kind() const;

private:
	const std::string myId;
	fb::shared_ptr<ZLImageData> myImage;
};

class ZLTextSpecialElement : public ZLTextElement {

public:
	ZLTextSpecialElement(Kind kind);
	~ZLTextSpecialElement();

private:
	Kind kind() const;

private:
	Kind myKind;
};

class ZLTextStyleElement : public ZLTextElement {

public:
	ZLTextStyleElement(fb::shared_ptr<ZLTextParagraphEntry> entry);
	~ZLTextStyleElement();
	const ZLTextStyleEntry &entry() const;

private:
	Kind kind() const;

private:
	const fb::shared_ptr<ZLTextParagraphEntry> myEntry;
};

class ZLTextFixedHSpaceElement : public ZLTextElement {

public:
	ZLTextFixedHSpaceElement(unsigned char length);
	unsigned char length() const;

private:
	Kind kind() const;

private:
	const unsigned char myLength;
};

class ZLTextControlElement : public ZLTextElement {

private:
	ZLTextControlElement(fb::shared_ptr<ZLTextParagraphEntry> entry);
	~ZLTextControlElement();

public:
	const ZLTextControlEntry &entry() const;
	ZLTextKind textKind() const;
	bool isStart() const;

private:
	Kind kind() const;

private:
	const fb::shared_ptr<ZLTextParagraphEntry> myEntry;

friend class ZLTextElementPool;
};

inline ZLTextElement::ZLTextElement() {}
inline ZLTextElement::~ZLTextElement() {}

inline ZLTextImageElement::ZLTextImageElement(const std::string &id, const fb::shared_ptr<ZLImageData> image) : myId(id), myImage(image) {}
inline ZLTextImageElement::~ZLTextImageElement() {}
inline fb::shared_ptr<ZLImageData> ZLTextImageElement::image() const { return myImage; }
inline const std::string &ZLTextImageElement::id() const { return myId; }

inline ZLTextSpecialElement::ZLTextSpecialElement(Kind kind) : myKind(kind) {}
inline ZLTextSpecialElement::~ZLTextSpecialElement() {}

inline ZLTextStyleElement::ZLTextStyleElement(const fb::shared_ptr<ZLTextParagraphEntry> entry) : myEntry(entry) {}
inline ZLTextStyleElement::~ZLTextStyleElement() {}
inline const ZLTextStyleEntry &ZLTextStyleElement::entry() const { return (const ZLTextStyleEntry&)*myEntry; }

inline ZLTextControlElement::ZLTextControlElement(const fb::shared_ptr<ZLTextParagraphEntry> entry) : myEntry(entry) {}
inline ZLTextControlElement::~ZLTextControlElement() {}
inline const ZLTextControlEntry &ZLTextControlElement::entry() const { return (const ZLTextControlEntry&)*myEntry; }
inline ZLTextKind ZLTextControlElement::textKind() const { return entry().kind(); }
inline bool ZLTextControlElement::isStart() const { return entry().isStart(); }

inline ZLTextFixedHSpaceElement::ZLTextFixedHSpaceElement(unsigned char length) : myLength(length) {}
inline unsigned char ZLTextFixedHSpaceElement::length() const { return myLength; }

#endif /* __ZLTEXTELEMENT_H__ */
