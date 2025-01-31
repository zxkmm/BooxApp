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

#ifndef __FBREADERACTIONS_H__
#define __FBREADERACTIONS_H__

#include <ZLApplication.h>

#include "FBReader.h"

class ZLTextView;

class ActionCode {

public:
	static const std::string SHOW_READING;
	static const std::string SHOW_COLLECTION;
	static const std::string SHOW_NET_LIBRARY;
	static const std::string SHOW_OPTIONS;
	static const std::string UNDO;
	static const std::string REDO;
	static const std::string SHOW_CONTENTS;
	static const std::string SEARCH;
	static const std::string FIND_PREVIOUS;
	static const std::string FIND_NEXT;
	static const std::string LARGE_SCROLL_FORWARD;
	static const std::string LARGE_SCROLL_BACKWARD;
	static const std::string SMALL_SCROLL_FORWARD;
	static const std::string SMALL_SCROLL_BACKWARD;
	static const std::string MOUSE_SCROLL_FORWARD;
	static const std::string MOUSE_SCROLL_BACKWARD;
	static const std::string TAP_SCROLL_FORWARD;
	static const std::string TAP_SCROLL_BACKWARD;
	static const std::string SCROLL_TO_HOME;
	static const std::string SCROLL_TO_START_OF_TEXT;
	static const std::string SCROLL_TO_END_OF_TEXT;
	static const std::string CANCEL;
	static const std::string INCREASE_FONT;
	static const std::string DECREASE_FONT;
    static const std::string UPDATE_OPTIONS;
	static const std::string SHOW_HIDE_POSITION_INDICATOR;
	static const std::string TOGGLE_FULLSCREEN;
	static const std::string FULLSCREEN_ON;
	static const std::string ADD_BOOK;
	static const std::string SHOW_BOOK_INFO;
	static const std::string SHOW_HELP;
	static const std::string ROTATE_SCREEN;
	static const std::string QUIT;
	static const std::string OPEN_PREVIOUS_BOOK;
	static const std::string GOTO_NEXT_TOC_SECTION;
	static const std::string GOTO_PREVIOUS_TOC_SECTION;
	static const std::string COPY_SELECTED_TEXT_TO_CLIPBOARD;
	static const std::string CLEAR_SELECTION;
	static const std::string OPEN_SELECTED_TEXT_IN_DICTIONARY;
	static const std::string GOTO_PAGE_NUMBER;
	static const std::string GOTO_PAGE_NUMBER_WITH_PARAMETER;
	static const std::string SEARCH_ON_NETWORK;
	static const std::string ADVANCED_SEARCH_ON_NETWORK;
	static const std::string ORGANIZE_BOOKS_BY_AUTHOR;
	static const std::string ORGANIZE_BOOKS_BY_TAG;

private:
	ActionCode();
};

class FBAction : public ZLApplication::Action {

protected:
	FBAction(FBReader &fbreader);
	FBReader &fbreader();
	const FBReader &fbreader() const;

private:
	FBReader &myFBReader;
};

class ModeDependentAction : public FBAction {

protected:
	ModeDependentAction(FBReader &fbreader, int visibleInModes);

public:
	bool isVisible() const;

private:
	int myVisibleInModes;
};

class SetModeAction : public ModeDependentAction {

public:
	SetModeAction(FBReader &fbreader, FBReader::ViewMode modeToSet, int visibleInModes);
	void run();

private:
	FBReader::ViewMode myModeToSet;
};

class ShowHelpAction : public FBAction {

public:
	ShowHelpAction(FBReader &fbreader);
	void run();
};

class ShowOptionsDialogAction : public FBAction {

public:
	ShowOptionsDialogAction(FBReader &fbreader);
	void run();
};

class ShowContentsAction : public SetModeAction {

public:
	ShowContentsAction(FBReader &fbreader);
	bool isVisible() const;
};

class ScrollingAction : public FBAction {

public:
	ScrollingAction(FBReader &fbreader, const FBReader::ScrollingOptions &options, bool forward);
	bool isEnabled() const;
	bool useKeyDelay() const;
	void run();

private:
	const FBReader::ScrollingOptions &myOptions;
	const bool myForward;
};

class ScrollToHomeAction : public ModeDependentAction {

public:
	ScrollToHomeAction(FBReader &fbreader);
	bool isEnabled() const;
	void run();
};

class ScrollToStartOfTextAction : public ModeDependentAction {

public:
	ScrollToStartOfTextAction(FBReader &fbreader);
	bool isEnabled() const;
	void run();
};

class ScrollToEndOfTextAction : public ModeDependentAction {

public:
	ScrollToEndOfTextAction(FBReader &fbreader);
	bool isEnabled() const;
	void run();
};

class UndoAction : public ModeDependentAction {

public:
	UndoAction(FBReader &fbreader, int visibleInModes);
	bool isEnabled() const;
	void run();
};

class RedoAction : public ModeDependentAction {

public:
	RedoAction(FBReader &fbreader);
	bool isEnabled() const;
	void run();
};

class SearchAction : public FBAction {

public:
	SearchAction(FBReader &fbreader);
	bool isVisible() const;
};

class ClearSearchResultAction : public FBAction {

public:
    ClearSearchResultAction(FBReader &fbreader);
    bool isEnabled() const;
    void run();
};


class ChangeEncodingAction : public FBAction {

public:
    ChangeEncodingAction(FBReader &fbreader);
    bool isEnabled() const;
    void run();
};


class SearchPatternAction : public SearchAction {

public:
	SearchPatternAction(FBReader &fbreader);
	void run();

private:
	ZLBooleanOption SearchBackwardOption;
	ZLBooleanOption SearchIgnoreCaseOption;
	ZLBooleanOption SearchInWholeTextOption;
	ZLBooleanOption SearchThisSectionOnlyOption;
	ZLStringOption SearchPatternOption;

friend class SearchPatternEntry;
};

class FindNextAction : public SearchAction {

public:
	FindNextAction(FBReader &fbreader);
	bool isEnabled() const;
	void run();
};

class FindPreviousAction : public SearchAction {

public:
	FindPreviousAction(FBReader &fbreader);
	bool isEnabled() const;
	void run();
};

class ChangeFontSizeAction : public FBAction {

public:
	ChangeFontSizeAction(FBReader &fbreader, int delta);
	bool isEnabled() const;
	void run();

private:
	const int myDelta;
};

class UpdateOptionsAction: public FBAction {

public:
    UpdateOptionsAction(FBReader &fbreader);
    bool isEnabled() const;
    void run();
};

class CancelAction : public FBAction {

public:
	CancelAction(FBReader &fbreader);
	void run();
};

class ToggleIndicatorAction : public FBAction {

public:
	ToggleIndicatorAction(FBReader &fbreader);
	bool isVisible() const;
	void run();
};

class QuitAction : public FBAction {

public:
	QuitAction(FBReader &fbreader);
	void run();
};

class OpenPreviousBookAction : public FBAction {

public:
	OpenPreviousBookAction(FBReader &fbreader);
	bool isVisible() const;
	void run();
};

class GotoNextTOCSectionAction : public FBAction {

public:
	GotoNextTOCSectionAction(FBReader &fbreader);
	bool isVisible() const;
	bool isEnabled() const;
	void run();
};

class GotoPreviousTOCSectionAction : public FBAction {

public:
	GotoPreviousTOCSectionAction(FBReader &fbreader);
	bool isVisible() const;
	bool isEnabled() const;
	void run();
};

class GotoPageNumber : public ModeDependentAction {

public:
	GotoPageNumber(FBReader &fbreader, const std::string &parameter);
	bool isVisible() const;
	bool isEnabled() const;
	void run();

private:
	const std::string myParameter;
};

class SelectionAction : public FBAction {

public:
	SelectionAction(FBReader &fbreader);
	bool isVisible() const;
	bool isEnabled() const;

protected:
	ZLTextView &textView();
	const ZLTextView &textView() const;
};

class CopySelectedTextAction : public SelectionAction {

public:
	CopySelectedTextAction(FBReader &fbreader);
	bool isVisible() const;
	void run();
};

class OpenSelectedTextInDictionaryAction : public SelectionAction {

public:
	OpenSelectedTextInDictionaryAction(FBReader &fbreader);
	bool isVisible() const;
	void run();
};

class ClearSelectionAction : public SelectionAction {

public:
	ClearSelectionAction(FBReader &fbreader);
	void run();
};

class SearchOnNetworkAction : public ModeDependentAction {

public:
	SearchOnNetworkAction(FBReader &fbreader);
	void run();
};

class AdvancedSearchOnNetworkAction : public ModeDependentAction {

public:
	AdvancedSearchOnNetworkAction(FBReader &fbreader);
	void run();
};

class FBFullscreenAction : public ZLApplication::FullscreenAction {

public:
	FBFullscreenAction(FBReader &fbreader);
	void run();

private:
	FBReader &myFBReader;
};

class BooksOrderAction : public ModeDependentAction {

public:
	BooksOrderAction(FBReader &fbreader);
	void run();
};

inline FBReader &FBAction::fbreader() {
	return myFBReader;
}

inline const FBReader &FBAction::fbreader() const {
	return myFBReader;
}

#endif /* __FBREADERACTIONS_H__ */
