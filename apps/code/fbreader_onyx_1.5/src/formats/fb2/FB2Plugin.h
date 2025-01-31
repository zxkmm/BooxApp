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

#ifndef __FB2PLUGIN_H__
#define __FB2PLUGIN_H__

#include "../FormatPlugin.h"

class FB2Plugin : public FormatPlugin {

public:
	FB2Plugin();
	~FB2Plugin();
	bool providesMetaInfo() const;
	bool acceptsFile(const ZLFile &file) const;
	bool readDescription(const std::string &path, DBBook &book) const;
	bool readModel(const DBBook &book, BookModel &model) const;
	const std::string &iconName() const;
};

inline FB2Plugin::FB2Plugin() {}
inline FB2Plugin::~FB2Plugin() {}
inline bool FB2Plugin::providesMetaInfo() const { return true; }

#endif /* __FB2PLUGIN_H__ */
