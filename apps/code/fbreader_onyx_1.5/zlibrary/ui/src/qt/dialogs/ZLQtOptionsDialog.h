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

#ifndef __ZLQTOPTIONSDIALOG_H__
#define __ZLQTOPTIONSDIALOG_H__

#include <vector>

#include <qwidget.h>
#include <qtabdialog.h>

#include "../../../../core/src/desktop/dialogs/ZLDesktopOptionsDialog.h"

class ZLQtOptionsDialog : public QTabDialog, public ZLDesktopOptionsDialog {
	Q_OBJECT

public:
	ZLQtOptionsDialog(const ZLResource &resource, fb::shared_ptr<ZLRunnable> applyAction, bool showApplyButton);
	ZLDialogContent &createTab(const ZLResourceKey &key);

protected:
	const std::string &selectedTabKey() const;
	void selectTab(const ZLResourceKey &key);
	bool runInternal();

	void setSize(int width, int height) { QTabDialog::resize(width, height); }
	int width() const { return QTabDialog::width(); }
	int height() const { return QTabDialog::height(); }

private slots:
	void resizeEvent(QResizeEvent *);
	void apply();

signals:
	void resized(const QSize &size);
};

#endif /* __ZLQTOPTIONSDIALOG_H__ */
