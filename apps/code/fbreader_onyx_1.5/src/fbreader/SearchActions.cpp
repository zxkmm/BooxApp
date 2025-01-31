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

#include <ZLDialogManager.h>
#include <ZLDialog.h>
#include <ZLStringUtil.h>
#include <ZLOptionEntry.h>

#include <ZLTextView.h>

#include "FBReader.h"
#include "FBView.h"
#include "FBReaderActions.h"
#include "../options/FBOptions.h"

#include <set>

static const std::string SEARCH = "Search";
static const std::string PATTERN = "Pattern";

class SearchPatternEntry : public ZLComboOptionEntry {

public:
	SearchPatternEntry(SearchPatternAction &action);

	const std::string &initialValue() const;
	const std::vector<std::string> &values() const;
	void onAccept(const std::string &value);

private:
	SearchPatternAction &myAction;
	mutable std::vector<std::string> myValues;
};

SearchPatternEntry::SearchPatternEntry(SearchPatternAction &action) : ZLComboOptionEntry(true), myAction(action) {
}

const std::string &SearchPatternEntry::initialValue() const {
	return values()[0];
}

const std::vector<std::string> &SearchPatternEntry::values() const {
	if (myValues.empty()) {
		// TODO: implement
		myValues.push_back(myAction.SearchPatternOption.value());
		for (int i = 1; i < 6; ++i) {
			std::string pattern = PATTERN;
			ZLStringUtil::appendNumber(pattern, i);
			std::string value = ZLStringOption(FBCategoryKey::SEARCH, SEARCH, pattern, "").value();
			if (!value.empty()) {
				myValues.push_back(value);
			}
		}
	}
	return myValues;
}

void SearchPatternEntry::onAccept(const std::string &value) {
	std::string v = value;
	ZLStringUtil::stripWhiteSpaces(v);
	if (v != values()[0]) {
		myAction.SearchPatternOption.setValue(v);
		int index = 1;
		for (std::vector<std::string>::const_iterator it = myValues.begin(); (index < 6) && (it != myValues.end()); ++it) {
			if (*it != v) {
				std::string pattern = PATTERN;
				ZLStringUtil::appendNumber(pattern, index++);
				ZLStringOption(FBCategoryKey::SEARCH, SEARCH, pattern, "").setValue(*it);
			}
		}
	}
}

SearchAction::SearchAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool SearchAction::isVisible() const {
	fb::shared_ptr<ZLView> view = fbreader().currentView();
    return !view.isNull() && ((FBView&)*view).hasContents();
}

ClearSearchResultAction::ClearSearchResultAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ClearSearchResultAction::isEnabled() const
{
    fb::shared_ptr<ZLView> view = fbreader().currentView();
    return !view.isNull() && ((FBView&)*view).hasContents();
}

void ClearSearchResultAction::run()
{
    ZLTextView &textView = (ZLTextView&)*fbreader().currentView();
    textView.model()->removeAllMarks();
    fbreader().clearTextCaches();
    fbreader().refreshWindow();
}

SearchPatternAction::SearchPatternAction(FBReader &fbreader) : SearchAction(fbreader),
	SearchBackwardOption(FBCategoryKey::SEARCH, SEARCH, "Backward", false),
	SearchIgnoreCaseOption(FBCategoryKey::SEARCH, SEARCH, "IgnoreCase", true),
	SearchInWholeTextOption(FBCategoryKey::SEARCH, SEARCH, "WholeText", false),
	SearchThisSectionOnlyOption(FBCategoryKey::SEARCH, SEARCH, "ThisSectionOnly", false),
	SearchPatternOption(FBCategoryKey::SEARCH, SEARCH, PATTERN, "") {
}

void SearchPatternAction::run() {
    ZLTextView &textView = (ZLTextView&)*fbreader().currentView();
    textView.search(
        fbreader().SearchPatternOption.value(),
        true,
        false,
        fbreader().SearchForwardOption.value(),
        false);

}

FindNextAction::FindNextAction(FBReader &fbreader) : SearchAction(fbreader) {
}

bool FindNextAction::isEnabled() const {
	fb::shared_ptr<ZLView> view = fbreader().currentView();
	return !view.isNull() && ((ZLTextView&)*view).canFindNext();
}

void FindNextAction::run() {
	((ZLTextView&)*fbreader().currentView()).findNext();
}

FindPreviousAction::FindPreviousAction(FBReader &fbreader) : SearchAction(fbreader) {
}

bool FindPreviousAction::isEnabled() const {
	fb::shared_ptr<ZLView> view = fbreader().currentView();
    return !view.isNull() && ((ZLTextView&)*view).canFindPrevious();
}

void FindPreviousAction::run() {
	((ZLTextView&)*fbreader().currentView()).findPrevious();
}
