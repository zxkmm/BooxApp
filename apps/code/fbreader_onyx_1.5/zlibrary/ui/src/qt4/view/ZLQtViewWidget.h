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

#ifndef __ZLQTVIEWWIDGET_H__
#define __ZLQTVIEWWIDGET_H__

#include <QtGui/QWidget>

#include "../../../../core/src/view/ZLViewWidget.h"
#include <ZLApplication.h>

#include "onyx/ui/ui.h"
#include "onyx/sys/sys.h"
#include "dictionary/dictionary_manager.h"
#include "dictionary/dict_widget.h"
#include "sound/sound.h"
#include "tts/tts_widget.h"



class QGridLayout;
class QScrollBar;
using namespace ui;
using namespace tts;

class ZLQtViewWidget : public QObject, public ZLViewWidget {
    Q_OBJECT

private:
    class Widget : public QWidget {

    public:
        Widget(QWidget *parent, ZLQtViewWidget &holder);

    private:
        void paintEvent(QPaintEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void keyReleaseEvent(QKeyEvent *event);

        int x(const QMouseEvent *event) const;
        int y(const QMouseEvent *event) const;

        void stylusPan(const QPoint &now, const QPoint &old);
        void drawBookmark(QPainter &painter);

    private:
        ZLQtViewWidget &myHolder;
        QPoint last_pos_;
    };

public:
    ZLQtViewWidget(QWidget *parent, ZLApplication *application);
    ~ZLQtViewWidget();
    QWidget *widget();

public Q_SLOTS:
    void popupMenu();
    void onProgressClicked(const int, const int);
    void onSetEncoding(std::string encoding);
    void changeFontFamily(const std::string & family);
    void changeFont(QFont font);
    void rotateScreen();
    void quit();

    void onSearch(BaseSearchContext&);
    bool updateSearchWidget();
    void onSearchClosed();

    void lookup();
    void onDictClosed();

    void nextPage();
    void prevPage();
    void closeDocument();
    void processKeyReleaseEvent(int key);

    void onSdCardChanged(bool);
    void onWakeup();
    void handleMountTreeEvent(bool inserted, const QString &mount_point);
    void onAboutToShutdown();
    void onMusicPlayerStateChanged(int);

private:
    bool isWidgetVisible(QWidget * wnd);
    void hideHelperWidget(QWidget * wnd);

    void startDictLookup();
    void stopDictLookup();

    void showSearchWidget();
    bool updateSearchCriteria();

    bool addBookmark();
    bool removeBookmarks();
    bool clearBookmarks();
    void showAllBookmarks();
    bool hasBookmark();
    void bookmarkModel(QStandardItemModel & model, QModelIndex & selected);
    void processBookmarks(ReadingToolsActions & actions);

private:
    void repaint();
    void trackStylus(bool track);

    void setScrollbarEnabled(ZLView::Direction direction, bool enabled);
    void setScrollbarPlacement(ZLView::Direction direction, bool standard);
    void setScrollbarParameters(ZLView::Direction direction, size_t full, size_t from, size_t to);

    QWidget * addStatusBar();
    void updateProgress(size_t full, size_t from, size_t to);
    void updateActions();

    QFont currentFont();

    void enableTextSelection(bool enable = true);
    bool isTextSelectionEnabled();
    bool adjustDictWidget();
    void fastRefreshWindow(bool);
    void lookupAndUpdate();

    void showGotoPageDialog();

    void loadConf();
    void saveConf();

private:
    QWidget *myFrame;
    Widget *myQWidget;

    StatusBar  *status_bar_;
    EncodingActions encoding_actions_;
    FontFamilyActions font_family_actions_;
    FontActions font_actions_;
    ReadingToolsActions reading_tool_actions_;
    SystemActions system_actions_;

    sys::SysStatus & sys_status_;

    size_t full_;
    size_t from_;
    size_t to_;
    size_t page_step_;

    QRect selected_rect_;
    QString selected_text_;
    bool enable_text_selection_;
    scoped_ptr<Sound> sound_;

    scoped_ptr<DictionaryManager> dicts_;
    scoped_ptr<DictWidget> dict_widget_;

    scoped_ptr<TTS> tts_engine_;
    scoped_ptr<TTSWidget> tts_widget_;

    BaseSearchContext search_context_;
    scoped_ptr<SearchWidget> search_widget_;

    ZLApplication *myApplication;
    bool conf_stored_;
};

#endif /* __ZLQTVIEWWIDGET_H__ */
