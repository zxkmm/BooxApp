/*
 * Copyright (C) 2008-2009 Geometer Plus <contact@geometerplus.com>
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

#include "FBReaderActions.h"

const std::string ActionCode::SHOW_READING = "showReading";
const std::string ActionCode::SHOW_COLLECTION = "showLibrary";
const std::string ActionCode::SHOW_NET_LIBRARY = "showNetLibrary";
const std::string ActionCode::SHOW_OPTIONS = "preferences";
const std::string ActionCode::UNDO = "undo";
const std::string ActionCode::REDO = "redo";
const std::string ActionCode::SHOW_CONTENTS = "toc";
const std::string ActionCode::SEARCH = "search";
const std::string ActionCode::FIND_PREVIOUS = "findPrevious";
const std::string ActionCode::FIND_NEXT = "findNext";
const std::string ActionCode::LARGE_SCROLL_FORWARD = "largeScrollForward";
const std::string ActionCode::LARGE_SCROLL_BACKWARD = "largeScrollBackward";
const std::string ActionCode::SMALL_SCROLL_FORWARD = "smallScrollForward";
const std::string ActionCode::SMALL_SCROLL_BACKWARD = "smallScrollBackward";
const std::string ActionCode::MOUSE_SCROLL_FORWARD = "mouseScrollForward";
const std::string ActionCode::MOUSE_SCROLL_BACKWARD = "mouseScrollBackward";
const std::string ActionCode::TAP_SCROLL_FORWARD = "tapScrollForward";
const std::string ActionCode::TAP_SCROLL_BACKWARD = "tapScrollBackward";
const std::string ActionCode::SCROLL_TO_HOME = "gotoHome";
const std::string ActionCode::SCROLL_TO_START_OF_TEXT = "gotoSectionStart";
const std::string ActionCode::SCROLL_TO_END_OF_TEXT = "gotoSectionEnd";
const std::string ActionCode::CANCEL = "cancel";
const std::string ActionCode::INCREASE_FONT = "increaseFont";
const std::string ActionCode::DECREASE_FONT = "decreaseFont";
const std::string ActionCode::UPDATE_OPTIONS = "updateOptions";
const std::string ActionCode::SHOW_HIDE_POSITION_INDICATOR = "toggleIndicator";
const std::string ActionCode::TOGGLE_FULLSCREEN = "toggleFullscreen";
const std::string ActionCode::FULLSCREEN_ON = "onFullscreen";
const std::string ActionCode::ADD_BOOK = "addBook";
const std::string ActionCode::SHOW_BOOK_INFO = "bookInfo";
const std::string ActionCode::SHOW_HELP = "showHelp";
const std::string ActionCode::ROTATE_SCREEN = "rotate";
const std::string ActionCode::QUIT = "quit";
const std::string ActionCode::OPEN_PREVIOUS_BOOK = "previousBook";
const std::string ActionCode::GOTO_NEXT_TOC_SECTION = "nextTOCSection";
const std::string ActionCode::GOTO_PREVIOUS_TOC_SECTION = "previousTOCSection";
const std::string ActionCode::COPY_SELECTED_TEXT_TO_CLIPBOARD = "copyToClipboard";
const std::string ActionCode::CLEAR_SELECTION = "clearSelection";
const std::string ActionCode::OPEN_SELECTED_TEXT_IN_DICTIONARY = "openInDictionary";
const std::string ActionCode::GOTO_PAGE_NUMBER = "gotoPageNumber";
const std::string ActionCode::GOTO_PAGE_NUMBER_WITH_PARAMETER = "gotoPageNumberWithParameter";
const std::string ActionCode::SEARCH_ON_NETWORK = "searchOnNetwork";
const std::string ActionCode::ADVANCED_SEARCH_ON_NETWORK = "advancedSearchOnNetwork";
const std::string ActionCode::ORGANIZE_BOOKS_BY_AUTHOR = "byAuthor";
const std::string ActionCode::ORGANIZE_BOOKS_BY_TAG = "byTag";
