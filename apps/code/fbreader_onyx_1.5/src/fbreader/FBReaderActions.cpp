// -*- mode: c++; c-basic-offset: 4; -*-

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

#include <algorithm>

#include <ZLDialogManager.h>
#include <ZLDialog.h>
#include <ZLOptionsDialog.h>
#include <optionEntries/ZLSimpleOptionEntry.h>
#include <ZLibrary.h>

#include <ZLTextView.h>

#include "FBReader.h"
#include "FBReaderActions.h"
#include "BookTextView.h"
#include "ContentsView.h"
#include "FBFileHandler.h"

#include "../bookmodel/BookModel.h"
#include "../database/booksdb/BooksDBUtil.h"

FBAction::FBAction(FBReader &fbreader) : myFBReader(fbreader) {
}

ModeDependentAction::ModeDependentAction(FBReader &fbreader, int visibleInModes) : FBAction(fbreader), myVisibleInModes(visibleInModes) {
}

bool ModeDependentAction::isVisible() const {
    return fbreader().mode() & myVisibleInModes;
}

SetModeAction::SetModeAction(FBReader &fbreader, FBReader::ViewMode modeToSet, int visibleInModes) : ModeDependentAction(fbreader, visibleInModes), myModeToSet(modeToSet) {
}

void SetModeAction::run() {
    fbreader().setMode(myModeToSet);
}

ShowHelpAction::ShowHelpAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowHelpAction::run() {
	fb::shared_ptr<DBBook> book = BooksDBUtil::getBook(fbreader().helpFileName(ZLibrary::Language()));
	if (book.isNull()) {
		book = BooksDBUtil::getBook(fbreader().helpFileName("en"));
	}
	if (!book.isNull()) {
		fbreader().openBook(book);
        fbreader().setMode(FBReader::BOOK_TEXT_MODE);
        fbreader().refreshWindow();
    } else {
        ZLDialogManager::instance().errorBox(ZLResourceKey("noHelpBox"));
    }
}

ShowOptionsDialogAction::ShowOptionsDialogAction(FBReader &fbreader) : FBAction(fbreader) {
}

void ShowOptionsDialogAction::run() {
}

ShowContentsAction::ShowContentsAction(FBReader &fbreader) : SetModeAction(fbreader, FBReader::CONTENTS_MODE, FBReader::BOOK_TEXT_MODE) {
}

bool ShowContentsAction::isVisible() const {
    return ModeDependentAction::isVisible() && !((ContentsView&)*fbreader().myContentsView).isEmpty();
}

ScrollToHomeAction::ScrollToHomeAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

bool ScrollToHomeAction::isEnabled() const {
    if (!isVisible()) {
        return false;
    }
    ZLTextWordCursor cursor = fbreader().bookTextView().startCursor();
    return cursor.isNull() || !cursor.isStartOfParagraph() || !cursor.paragraphCursor().isFirst();
}

void ScrollToHomeAction::run() {
    fbreader().bookTextView().scrollToHome();
}

ScrollToStartOfTextAction::ScrollToStartOfTextAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

bool ScrollToStartOfTextAction::isEnabled() const {
    if (!isVisible()) {
        return false;
    }
    ZLTextWordCursor cursor = fbreader().bookTextView().startCursor();
    return cursor.isNull() || !cursor.isStartOfParagraph() || !cursor.paragraphCursor().isFirst();
}

void ScrollToStartOfTextAction::run() {
    fbreader().bookTextView().scrollToStartOfText();
}

ScrollToEndOfTextAction::ScrollToEndOfTextAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

bool ScrollToEndOfTextAction::isEnabled() const {
    if (!isVisible()) {
        return false;
    }
    ZLTextWordCursor cursor = fbreader().bookTextView().endCursor();
    return cursor.isNull() || !cursor.isEndOfParagraph() || !cursor.paragraphCursor().isLast();
}

void ScrollToEndOfTextAction::run() {
    fbreader().bookTextView().scrollToEndOfText();
}

UndoAction::UndoAction(FBReader &fbreader, int visibleInModes) : ModeDependentAction(fbreader, visibleInModes) {
}

bool UndoAction::isEnabled() const {
    return (fbreader().mode() != FBReader::BOOK_TEXT_MODE) ||
            fbreader().bookTextView().canUndoPageMove();
}

void UndoAction::run() {
    if (fbreader().mode() == FBReader::BOOK_TEXT_MODE) {
        fbreader().bookTextView().undoPageMove();
    } else {
        fbreader().restorePreviousMode();
    }
}

RedoAction::RedoAction(FBReader &fbreader) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE) {
}

bool RedoAction::isEnabled() const {
    return isVisible() && fbreader().bookTextView().canRedoPageMove();
}

void RedoAction::run() {
    fbreader().bookTextView().redoPageMove();
}

ChangeEncodingAction::ChangeEncodingAction(FBReader &fbreader) : FBAction(fbreader)
{
}

bool ChangeEncodingAction::isEnabled() const
{
    return true;
}

void ChangeEncodingAction::run()
{
    fb::shared_ptr<DBBook> db;
    fbreader().createDescription(fbreader().DocumentPathOption.value(), db);
    if (!db.isNull()) {
        db->setEncoding(fbreader().EncodingOption.value());
        fbreader().openBook(db);
        fbreader().refreshWindow();
    }
}

ScrollingAction::ScrollingAction(FBReader &fbreader, const FBReader::ScrollingOptions &options, bool forward) : FBAction(fbreader), myOptions(options), myForward(forward) {
}

bool ScrollingAction::isEnabled() const {
    return
            (&myOptions != &fbreader().TapScrollingOptions) ||
            fbreader().EnableTapScrollingOption.value();
}

bool ScrollingAction::useKeyDelay() const {
    return false;
}

void ScrollingAction::run() {
    int delay = fbreader().myLastScrollingTime.millisecondsTo(ZLTime());
    fb::shared_ptr<ZLView> view = fbreader().currentView();
	if (!view.isNull() && ((delay < 0) || (delay >= myOptions.DelayOption.value()))) {
        ZLTextView::ScrollingMode oType = (ZLTextView::ScrollingMode)myOptions.ModeOption.value();
        unsigned int oValue = 0;
        switch (oType) {
            case ZLTextView::KEEP_LINES:
                oValue = myOptions.LinesToKeepOption.value();
                break;
            case ZLTextView::SCROLL_LINES:
                oValue = myOptions.LinesToScrollOption.value();
                break;
            case ZLTextView::SCROLL_PERCENTAGE:
                oValue = myOptions.PercentToScrollOption.value();
                break;
            default:
                break;
        }
        ((FBView&)*view).scrollAndUpdatePage(myForward, oType, oValue);
        fbreader().myLastScrollingTime = ZLTime();
    }
}

ChangeFontSizeAction::ChangeFontSizeAction(FBReader &fbreader, int delta) : FBAction(fbreader), myDelta(delta) {
}

bool ChangeFontSizeAction::isEnabled() const {
    ZLIntegerRangeOption &option = ZLTextStyleCollection::instance().baseStyle().FontSizeOption;
    if (myDelta < 0) {
        return option.value() > option.minValue();
    } else {
        return option.value() < option.maxValue();
    }
}

void ChangeFontSizeAction::run() {
    ZLIntegerRangeOption &option = ZLTextStyleCollection::instance().baseStyle().FontSizeOption;
    option.setValue(option.value() + myDelta);
    fbreader().clearTextCaches();
    fbreader().refreshWindow();
}

UpdateOptionsAction::UpdateOptionsAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool UpdateOptionsAction::isEnabled() const
{
    return true;
}

void UpdateOptionsAction::run()
{
    fbreader().clearTextCaches();
    fbreader().refreshWindow();
}


OpenPreviousBookAction::OpenPreviousBookAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool OpenPreviousBookAction::isVisible() const {
    return false;
}

void OpenPreviousBookAction::run() {
    return;
}

CancelAction::CancelAction(FBReader &fbreader) : FBAction(fbreader) {
}

void CancelAction::run() {
    switch (fbreader().myActionOnCancel) {
        case FBReader::UNFULLSCREEN:
            if (fbreader().isFullscreen()) {
                fbreader().setFullscreen(false);
                return;
            } else if (fbreader().mode() != FBReader::BOOK_TEXT_MODE) {
                fbreader().restorePreviousMode();
                return;
            }
            break;
        case FBReader::RETURN_TO_TEXT_MODE:
            if (fbreader().mode() != FBReader::BOOK_TEXT_MODE) {
                fbreader().restorePreviousMode();
                return;
            } else if (fbreader().isFullscreen()) {
                fbreader().setFullscreen(false);
                return;
            }
            break;
    }
    if (fbreader().QuitOnCancelOption.value()) {
        fbreader().quit();
    }
}

ToggleIndicatorAction::ToggleIndicatorAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool ToggleIndicatorAction::isVisible() const {
    ZLIntegerRangeOption &option = FBView::commonIndicatorInfo().TypeOption;
    switch (option.value()) {
        case FBIndicatorStyle::FB_INDICATOR:
        case FBIndicatorStyle::NONE:
            return true;
    }
    return false;
}

void ToggleIndicatorAction::run() {
    ZLIntegerRangeOption &option = FBView::commonIndicatorInfo().TypeOption;
    switch (option.value()) {
        case FBIndicatorStyle::OS_SCROLLBAR:
            break;
        case FBIndicatorStyle::FB_INDICATOR:
            option.setValue(FBIndicatorStyle::NONE);
            fbreader().refreshWindow();
            break;
        case FBIndicatorStyle::NONE:
            option.setValue(FBIndicatorStyle::FB_INDICATOR);
            fbreader().refreshWindow();
            break;
    }
}

QuitAction::QuitAction(FBReader &fbreader) : FBAction(fbreader) {
}

void QuitAction::run() {
    fbreader().closeView();
}

GotoNextTOCSectionAction::GotoNextTOCSectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool GotoNextTOCSectionAction::isVisible() const {
    if (fbreader().mode() != FBReader::BOOK_TEXT_MODE) {
        return false;
    }
    const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
    fb::shared_ptr<ZLTextModel> model = contentsView.model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoNextTOCSectionAction::isEnabled() const {
    const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
    fb::shared_ptr<ZLTextModel> model = contentsView.model();
	return !model.isNull() && ((int)contentsView.currentTextViewParagraph() < (int)model->paragraphsNumber() - 1);
}

void GotoNextTOCSectionAction::run() {
    ContentsView &contentsView = (ContentsView&)*fbreader().myContentsView;
    size_t current = contentsView.currentTextViewParagraph();
    const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.model();
    int reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current + 1]));
    if (reference != -1) {
        ((ZLTextView&)*fbreader().myBookTextView).gotoParagraph(reference);
        fbreader().refreshWindow();
    }
}

GotoPreviousTOCSectionAction::GotoPreviousTOCSectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool GotoPreviousTOCSectionAction::isVisible() const {
    if (fbreader().mode() != FBReader::BOOK_TEXT_MODE) {
        return false;
    }
    const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
    fb::shared_ptr<ZLTextModel> model = contentsView.model();
	return !model.isNull() && (model->paragraphsNumber() > 1);
}

bool GotoPreviousTOCSectionAction::isEnabled() const {
    const ContentsView &contentsView = (const ContentsView&)*fbreader().myContentsView;
    fb::shared_ptr<ZLTextModel> model = contentsView.model();
	if (model.isNull()) {
        return false;
    }
    const ContentsModel &contentsModel = (const ContentsModel&)*model;
    int tocIndex = contentsView.currentTextViewParagraph(false);
    if (tocIndex > 0) {
        return true;
    }
    if (tocIndex == 0) {
        const ZLTextWordCursor &cursor = fbreader().bookTextView().startCursor();
        if (cursor.isNull()) {
            return false;
        }
        if (cursor.elementIndex() > 0) {
            return true;
        }
        return
                contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[0])) >
                (int)cursor.paragraphCursor().index();
    }
    return false;
}

void GotoPreviousTOCSectionAction::run() {
    ContentsView &contentsView = (ContentsView&)*fbreader().myContentsView;
    size_t current = contentsView.currentTextViewParagraph(false);
    const ContentsModel &contentsModel = (const ContentsModel&)*contentsView.model();

    int reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current]));
    const ZLTextWordCursor &cursor = fbreader().bookTextView().startCursor();
    if (!cursor.isNull() &&
        (cursor.elementIndex() == 0)) {
        int paragraphIndex = cursor.paragraphCursor().index();
        if (reference == paragraphIndex) {
            reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current - 1]));
        } else if (reference == paragraphIndex - 1) {
            const ZLTextModel &textModel = *fbreader().bookTextView().model();
            const ZLTextParagraph *para = textModel[paragraphIndex];
            if ((para != 0) && (para->kind() == ZLTextParagraph::END_OF_SECTION_PARAGRAPH)) {
                reference = contentsModel.reference(((const ZLTextTreeParagraph*)contentsModel[current - 1]));
            }
        }
    }
    if (reference != -1) {
        ((ZLTextView&)*fbreader().myBookTextView).gotoParagraph(reference);
        fbreader().refreshWindow();
    }
}

GotoPageNumber::GotoPageNumber(FBReader &fbreader, const std::string &parameter) : ModeDependentAction(fbreader, FBReader::BOOK_TEXT_MODE), myParameter(parameter) {
}

bool GotoPageNumber::isVisible() const {
    return
            ModeDependentAction::isVisible() &&
            !fbreader().bookTextView().hasMultiSectionModel();
}

bool GotoPageNumber::isEnabled() const {
    return ModeDependentAction::isEnabled() && (fbreader().bookTextView().pageNumber() > 1);
}

void GotoPageNumber::run() {
    int pageIndex = 0;
    const int pageNumber = fbreader().bookTextView().pageNumber();

    if (!myParameter.empty()) {
        const std::string value = fbreader().visualParameter(myParameter);
        if (value.empty()) {
            return;
        }
        pageIndex = atoi(value.c_str());
    } else {
        fb::shared_ptr<ZLDialog> gotoPageDialog = ZLDialogManager::instance().createDialog(ZLResourceKey("gotoPageDialog"));

        ZLIntegerRangeOption pageIndexOption(ZLCategoryKey::CONFIG, "gotoPageDialog", "Index", 1, pageNumber, pageIndex);
        gotoPageDialog->addOption(ZLResourceKey("pageNumber"), new ZLSimpleSpinOptionEntry(pageIndexOption, 1));
        gotoPageDialog->addButton(ZLDialogManager::OK_BUTTON, true);
        gotoPageDialog->addButton(ZLDialogManager::CANCEL_BUTTON, false);
        if (gotoPageDialog->run()) {
            gotoPageDialog->acceptValues();
            pageIndex = pageIndexOption.value();
        } else {
            return;
        }
    }

    fbreader().bookTextView().gotoPage(std::max(1, std::min(pageIndex, pageNumber)));
    fbreader().refreshWindow();
}

SelectionAction::SelectionAction(FBReader &fbreader) : FBAction(fbreader) {
}

bool SelectionAction::isVisible() const {
	return !fbreader().currentView().isNull();
}

bool SelectionAction::isEnabled() const {
    if (!isVisible()) {
        return false;
    }
    const ZLTextSelectionModel &selectionModel = textView().selectionModel();
	return !selectionModel.text().empty() || !selectionModel.image().isNull();
}

ZLTextView &SelectionAction::textView() {
    return (ZLTextView&)*fbreader().currentView();
}

const ZLTextView &SelectionAction::textView() const {
    return (ZLTextView&)*fbreader().currentView();
}

CopySelectedTextAction::CopySelectedTextAction(FBReader &fbreader) : SelectionAction(fbreader) {
}

bool CopySelectedTextAction::isVisible() const {
    return SelectionAction::isVisible() && ZLDialogManager::instance().isClipboardSupported(ZLDialogManager::CLIPBOARD_MAIN);
}

void CopySelectedTextAction::run() {
    textView().copySelectedTextToClipboard(ZLDialogManager::CLIPBOARD_MAIN);
}

OpenSelectedTextInDictionaryAction::OpenSelectedTextInDictionaryAction(FBReader &fbreader) : SelectionAction(fbreader) {
}

bool OpenSelectedTextInDictionaryAction::isVisible() const {
    return SelectionAction::isVisible() && fbreader().isDictionarySupported();
}

void OpenSelectedTextInDictionaryAction::run() {
    fbreader().openInDictionary(textView().selectionModel().text());
}

ClearSelectionAction::ClearSelectionAction(FBReader &fbreader) : SelectionAction(fbreader) {
}

void ClearSelectionAction::run() {
    textView().selectionModel().clear();
    fbreader().refreshWindow();
}

FBFullscreenAction::FBFullscreenAction(FBReader &fbreader) : ZLApplication::FullscreenAction(fbreader), myFBReader(fbreader) {
}

void FBFullscreenAction::run() {
    if (!myFBReader.isFullscreen()) {
        myFBReader.myActionOnCancel = FBReader::UNFULLSCREEN;
    }
    FullscreenAction::run();
}
