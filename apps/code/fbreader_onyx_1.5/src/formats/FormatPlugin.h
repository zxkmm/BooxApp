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

#ifndef __FORMATPLUGIN_H__
#define __FORMATPLUGIN_H__

#include <string>
#include <vector>

#include <ZLOptions.h>

#include "../database/booksdb/DBBook.h"

class BookModel;
class ZLOptionsDialog;
class ZLOptionsDialogTab;
class ZLFile;
class ZLInputStream;

class FormatInfoPage {

protected:
	FormatInfoPage();

public:
	virtual ~FormatInfoPage();
};

class FormatPlugin {

protected:
	FormatPlugin();
	
public:
	virtual ~FormatPlugin();

	virtual bool providesMetaInfo() const = 0;
	virtual bool acceptsFile(const ZLFile &file) const = 0;
	virtual const std::string &iconName() const = 0;
	virtual FormatInfoPage *createInfoPage(ZLOptionsDialog &dialog, const std::string &path);

	virtual const std::string &tryOpen(const std::string &path) const;
	virtual bool readDescription(const std::string &path, DBBook &book) const = 0;
	virtual bool readModel(const DBBook &book, BookModel &model) const = 0;

protected:
	static void detectEncodingAndLanguage(DBBook &book, ZLInputStream &stream);
	static void detectLanguage(DBBook &book, ZLInputStream &stream);
};

class PluginCollection {

public:
	ZLBooleanOption LanguageAutoDetectOption;
	ZLStringOption DefaultLanguageOption;
	ZLStringOption DefaultEncodingOption;
	
public:
	static PluginCollection &instance();
	static void deleteInstance();

private:
	PluginCollection();
	~PluginCollection();
	
public:
	FormatPlugin *plugin(const ZLFile &file, bool strong);

private:
	static PluginCollection *ourInstance;

	std::vector<FormatPlugin*> myPlugins;
};

inline FormatInfoPage::FormatInfoPage() {}
inline FormatInfoPage::~FormatInfoPage() {}
inline FormatPlugin::FormatPlugin() {}
inline FormatPlugin::~FormatPlugin() {}
inline FormatInfoPage *FormatPlugin::createInfoPage(ZLOptionsDialog&, const std::string&) { return 0; }

#endif /* __FORMATPLUGIN_H__ */
