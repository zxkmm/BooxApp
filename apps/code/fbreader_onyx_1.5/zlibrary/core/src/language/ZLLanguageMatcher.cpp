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

#include <ZLFile.h>
#include <ZLInputStream.h>
#include <iostream>
#include <typeinfo>

#include "ZLLanguageMatcher.h"
#include "ZLStatistics.h"
#include "ZLStatisticsXMLReader.h"

ZLLanguageMatcher::ZLLanguageMatcher(fb::shared_ptr<ZLLanguageDetector::LanguageInfo> info) : myInfo(info) {
}

ZLLanguageMatcher::~ZLLanguageMatcher() {
}

fb::shared_ptr<ZLLanguageDetector::LanguageInfo> ZLLanguageMatcher::info() const {
	return myInfo;
}

ZLStatisticsBasedMatcher::ZLStatisticsBasedMatcher(const std::string &fileName, fb::shared_ptr<ZLLanguageDetector::LanguageInfo> info) : ZLLanguageMatcher(info) {
	myStatisticsPtr = ZLStatisticsXMLReader().readStatistics(fileName);
	//if (myStatisticsPtr == 0) {
		//std::cerr << "pattern reading failed\n";
	//}
}

ZLStatisticsBasedMatcher::~ZLStatisticsBasedMatcher() {
}

int ZLStatisticsBasedMatcher::criterion(const ZLStatistics &otherStatistics) {
    if (!myStatisticsPtr.isNull())
    {
        return ZLStatistics::correlation(otherStatistics, *myStatisticsPtr);
    }
    return -1;
}
