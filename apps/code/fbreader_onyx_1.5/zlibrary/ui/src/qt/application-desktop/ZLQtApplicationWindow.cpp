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

#include <qapplication.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qmenubar.h>
#include <qaction.h>
#include <qlayout.h>
#include <qobjectlist.h>
#include <qtooltip.h>

#include <ZLibrary.h>
#include <ZLPopupData.h>

#include "ZLQtApplicationWindow.h"
#include "ZLQtPopupMenu.h"
#include "../dialogs/ZLQtDialogManager.h"
#include "../view/ZLQtViewWidget.h"
#include "../util/ZLQtKeyUtil.h"

void ZLQtDialogManager::createApplicationWindow(ZLApplication *application) const {
	new ZLQtApplicationWindow(application);
}

static const std::string OPTIONS = "Options";

class MyIconFactory : public QIconFactory {

public:
	QPixmap *createPixmap(const QIconSet &set, QIconSet::Size size, QIconSet::Mode mode, QIconSet::State state);
};

inline QRgb grayRgb(QRgb rgb) {
	int gray = (qRed(rgb) + qGreen(rgb) + qBlue(rgb)) / 3;
	return qRgba(gray, gray, gray, qAlpha(rgb) / 2);
}

QPixmap *MyIconFactory::createPixmap(const QIconSet &set, QIconSet::Size size, QIconSet::Mode mode, QIconSet::State state) {
	if (mode != QIconSet::Disabled) {
		return 0;
	}
	QImage image;
	image = set.pixmap(size, QIconSet::Normal, state);
	const int numColors = image.numColors();
	if (numColors > 0) {
		for (int i = 0; i < numColors; ++i) {
			image.setColor(i, grayRgb(image.color(i)));
		}
	} else {
		const int width = image.width();
		const int height = image.height();
		for (int i = 0; i < width; ++i) {
			for (int j = 0; j < height; ++j) {
				image.setPixel(i, j, grayRgb(image.pixel(i, j)));
			}
		}
	}

	return new QPixmap(image);
}

ZLQtToolButton::ZLQtToolButton(ZLQtApplicationWindow &window, ZLToolbar::AbstractButtonItem &item) : QToolButton(window.myToolBar), myWindow(window), myItem(item) {
	static std::string imagePrefix = ZLibrary::ApplicationImageDirectory() + ZLibrary::FileNameDelimiter;
	QPixmap icon((imagePrefix + myItem.iconName() + ".png").c_str());
	setIconSet(QIconSet(icon));
	QSize size = icon.size();
	QIconSet::setIconSize(QIconSet::Large, size);
	QIconSet::setIconSize(QIconSet::Small, size);
	if (item.type() == ZLToolbar::Item::TOGGLE_BUTTON) {
		setToggleButton(true);
	} else if (item.type() == ZLToolbar::Item::MENU_BUTTON) {
		ZLToolbar::MenuButtonItem &menuButtonItem = (ZLToolbar::MenuButtonItem&)myItem;
		fb::shared_ptr<ZLPopupData> popupData = menuButtonItem.popupData();
		myWindow.myPopupIdMap[&menuButtonItem] =
			!popupData ? (size_t)-1 : (popupData->id() - 1);
		setPopup(new ZLQtPopupMenu(this));
	}
	QString text = QString::fromUtf8(myItem.tooltip().c_str());
	setTextLabel(text);
	setUsesTextLabel(false);
	QToolTip::add(this, text);
	connect(this, SIGNAL(clicked()), this, SLOT(onActivated()));
}

void ZLQtToolButton::onActivated() {
	myWindow.onButtonPress(myItem);
}

void ZLQtApplicationWindow::setToggleButtonState(const ZLToolbar::ToggleButtonItem &button) {
	((QToolButton*)myItemToWidgetMap[&button])->setOn(button.isPressed());
}

ZLQtApplicationWindow::ZLQtApplicationWindow(ZLApplication *application) :
	ZLDesktopApplicationWindow(application),
	myFullScreen(false),
	myWasMaximized(false),
	myCursorIsHyperlink(false) {

	QIconFactory::installDefaultFactory(new MyIconFactory());

	const std::string iconFileName = ZLibrary::ImageDirectory() + ZLibrary::FileNameDelimiter + ZLibrary::ApplicationName() + ".png";
	QPixmap icon(iconFileName.c_str());
	setIcon(icon);

	setWFlags(getWFlags() | WStyle_Customize);

	myToolBar = new QToolBar(this);
	myToolBar->setFocusPolicy(NoFocus);
	myToolBar->boxLayout()->setMargin(5);
	myToolBar->boxLayout()->setSpacing(3);
	setToolBarsMovable(false);

	resize(myWidthOption.value(), myHeightOption.value());
	move(myXOption.value(), myYOption.value());

	qApp->setMainWidget(this);
	menuBar()->hide();
	show();
}

void ZLQtApplicationWindow::init() {
	ZLDesktopApplicationWindow::init();
	switch (myWindowStateOption.value()) {
		case NORMAL:
			break;
		case FULLSCREEN:
			setFullscreen(true);
			break;
		case MAXIMIZED:
			showMaximized();
			break;
	}
}

void ZLQtApplicationWindow::processAllEvents() {
	qApp->processEvents();
}

ZLQtApplicationWindow::~ZLQtApplicationWindow() {
	if (isFullscreen()) {
		myWindowStateOption.setValue(FULLSCREEN);
	} else if (isMaximized()) {
		myWindowStateOption.setValue(MAXIMIZED);
	} else {
		myWindowStateOption.setValue(NORMAL);
		if (x() != -1) {
			myXOption.setValue(x());
		}
		if (y() != -1) {
			myYOption.setValue(y());
		}
		myWidthOption.setValue(width());
		myHeightOption.setValue(height());
	}
}

void ZLQtApplicationWindow::setFullscreen(bool fullscreen) {
	if (fullscreen == myFullScreen) {
		return;
	}
	myFullScreen = fullscreen;
	if (myFullScreen) {
		myWasMaximized = isMaximized();
		if (!myWasMaximized) {
			if (x() != -1) {
				myXOption.setValue(x());
			}
			if (y() != -1) {
				myYOption.setValue(y());
			}
			myWidthOption.setValue(width());
			myHeightOption.setValue(height());
		}
		myToolBar->hide();
		showFullScreen();
	} else {
		myToolBar->show();
		showNormal();
		if (myWasMaximized) {
			showMaximized();
		} else {
			resize(myWidthOption.value(), myHeightOption.value());
			move(myXOption.value(), myYOption.value());
		}
	}
}

bool ZLQtApplicationWindow::isFullscreen() const {
	return myFullScreen;
}

void ZLQtApplicationWindow::keyReleaseEvent(QKeyEvent *event) {
	application().doActionByKey(ZLQtKeyUtil::keyName(event));
}

void ZLQtApplicationWindow::wheelEvent(QWheelEvent *event) {
	if (event->orientation() == Vertical) {
		if (event->delta() > 0) {
			application().doActionByKey(ZLApplication::MouseScrollUpKey);
		} else {
			application().doActionByKey(ZLApplication::MouseScrollDownKey);
		}
	}
}

void ZLQtApplicationWindow::closeEvent(QCloseEvent *event) {
	if (application().closeView()) {
		event->accept();
	} else {
		event->ignore();
	}
}

ZLQtApplicationWindow::LineEditParameter::LineEditParameter(QToolBar *toolbar, ZLQtApplicationWindow &window, const ZLToolbar::TextFieldItem &textFieldItem) : QLineEdit(toolbar), myWindow(window), myActionId(textFieldItem.actionId()) {
	setAlignment(Qt::AlignHCenter);
	setFocusPolicy(ClickFocus);
	setMaxLength(textFieldItem.maxWidth());
	setMaximumWidth(textFieldItem.maxWidth() * 12 + 12);
	QToolTip::add(this, QString::fromUtf8(textFieldItem.tooltip().c_str()));
	myWindow.addVisualParameter(textFieldItem.parameterId(), this);
}

void ZLQtApplicationWindow::LineEditParameter::keyReleaseEvent(QKeyEvent *event) {
	event->accept();
	const std::string key = ZLQtKeyUtil::keyName(event);
	if (key == "<Return>") {
		myWindow.application().doAction(myActionId);
		myWindow.setFocusToMainWidget();
	} else if (key == "<Esc>") {
		restoreOldValue();
		myWindow.setFocusToMainWidget();
	}
}

std::string ZLQtApplicationWindow::LineEditParameter::internalValue() const {
	return (const char*)text().utf8();
}

void ZLQtApplicationWindow::LineEditParameter::internalSetValue(const std::string &value) {
	setText(QString::fromUtf8(value.c_str()));
}

void ZLQtApplicationWindow::addToolbarItem(ZLToolbar::ItemPtr item) {
	switch (item->type()) {
		case ZLToolbar::Item::TEXT_FIELD:
			myItemToWidgetMap[&*item] = new LineEditParameter(myToolBar, *this, (ZLToolbar::TextFieldItem&)*item);
			break;
		case ZLToolbar::Item::PLAIN_BUTTON:
		case ZLToolbar::Item::MENU_BUTTON:
		case ZLToolbar::Item::TOGGLE_BUTTON:
			myItemToWidgetMap[&*item] = new ZLQtToolButton(*this, (ZLToolbar::AbstractButtonItem&)*item);
			break;
		case ZLToolbar::Item::SEPARATOR:
			myToolBar->addSeparator();
			myItemToWidgetMap[&*item] = (QWidget*)myToolBar->children()->getLast();
			break;
	}
}

void ZLQtApplicationWindow::setToolbarItemState(ZLToolbar::ItemPtr item, bool visible, bool enabled) {
	QWidget *widget = myItemToWidgetMap[&*item];
	if (widget == 0) {
		return;
	}
	widget->setEnabled(enabled);
	widget->setShown(visible);

	switch (item->type()) {
		default:
			break;
		case ZLToolbar::Item::MENU_BUTTON:
		{
			ZLToolbar::MenuButtonItem &menuButtonItem = (ZLToolbar::MenuButtonItem&)*item;
			fb::shared_ptr<ZLPopupData> data = menuButtonItem.popupData();
			if (data && (data->id() != myPopupIdMap[&menuButtonItem])) {
				myPopupIdMap[&menuButtonItem] = data->id();
				((ZLQtPopupMenu*)((QToolButton*)widget)->popup())->reset(data);
			}
			break;
		}
	}
}

ZLQtViewWidgetPositionInfo::ZLQtViewWidgetPositionInfo(const ZLQtApplicationWindow &window) : myWindow(window) {
}

int ZLQtViewWidgetPositionInfo::x() const {
	return 0;
}

int ZLQtViewWidgetPositionInfo::y() const {
	return ((myWindow.myToolBar != 0) && myWindow.myToolBar->isVisible()) ?
		myWindow.myToolBar->height() : 0;
}

int ZLQtViewWidgetPositionInfo::width() const {
	return myWindow.width();
}

int ZLQtViewWidgetPositionInfo::height() const {
	return myWindow.height() - y();
}

ZLViewWidget *ZLQtApplicationWindow::createViewWidget() {
	ZLQtViewWidgetPositionInfo positionInfo(*this);
	ZLQtViewWidget *viewWidget = new ZLQtViewWidget(this, &application(), positionInfo);
	setCentralWidget(viewWidget->widget());
	viewWidget->widget()->show();
	return viewWidget;
}

void ZLQtApplicationWindow::close() {
	QMainWindow::close();
}

void ZLQtApplicationWindow::grabAllKeys(bool) {
}

void ZLQtApplicationWindow::setCaption(const std::string &caption) {
	QMainWindow::setCaption(QString::fromUtf8(caption.c_str()));
}

void ZLQtApplicationWindow::setHyperlinkCursor(bool hyperlink) {
	if (hyperlink == myCursorIsHyperlink) {
		return;
	}
	myCursorIsHyperlink = hyperlink;
	if (hyperlink) {
		myStoredCursor = cursor();
		setCursor(Qt::pointingHandCursor);
	} else if (&myStoredCursor != &Qt::waitCursor) {
		setCursor(myStoredCursor);
	}
}

void ZLQtApplicationWindow::setFocusToMainWidget() {
	centralWidget()->setFocus();
}
