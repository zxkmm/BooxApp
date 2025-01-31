/*
 * Copyright (C) 2007-2009 Geometer Plus <contact@geometerplus.com>
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

#ifndef __ZLLANGUAGEDETECTOR_H__
#define __ZLLANGUAGEDETECTOR_H__

#include <vector>
#include <string>

//#include <shared_ptr.h>

class ZLStatisticsBasedMatcher;

class ZLLanguageDetector {

public:
	struct LanguageInfo {
		LanguageInfo(const std::string &language, const std::string &encoding);
		const std::string Language;
		const std::string Encoding;
	};

public:
	ZLLanguageDetector();
	~ZLLanguageDetector();

	fb::shared_ptr<LanguageInfo> findInfo(const char *buffer, size_t length, int matchingCriterion = 0);

private:
    typedef std::vector<fb::shared_ptr<ZLStatisticsBasedMatcher> > SBVector;
	SBVector myMatchers;
};

#endif /* __ZLLANGUAGEDETECTOR_H__ */
