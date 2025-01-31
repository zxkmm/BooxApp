#include "onyx/screen/screen_proxy.h"
#include "url_dialog.h"

namespace explorer
{

namespace view
{

UrlDialog::UrlDialog(QWidget *parent)
    : OnyxDialog(parent)
    , hbox_(&content_widget_)
    , text_edit_("", this)
    , confirm_button_(QApplication::tr("Confirm"), this)
    , clear_button_(QApplication::tr("Clear"), this)
    , keyboard_(0)
    , update_state_(UPDATE_NONE)
{
    createLayout();

    // Widget attributes.
    setModal(true);
    setFocusPolicy(Qt::NoFocus);

    text_edit_.installEventFilter(this);
    confirm_button_.installEventFilter(this);
    clear_button_.installEventFilter(this);
    keyboard_.installEventFilter(this);
}

UrlDialog::~UrlDialog(void)
{
}

void UrlDialog::createLayout()
{
    title_icon_label_.setPixmap(QPixmap(":/images/dictionary_search.png"));

    content_widget_.setFixedHeight(WIDGET_HEIGHT + 4 * SPACING);

    // hbox to layout line edit and buttons.
    hbox_.setContentsMargins(SPACING, SPACING, SPACING, SPACING);
    hbox_.setSpacing(SPACING * 4);

    // Line edit.
    text_edit_.setFixedHeight(WIDGET_HEIGHT);
    text_edit_.setText("");
    hbox_.addWidget(&text_edit_, 400);

    // Buttons.
    confirm_button_.setFixedHeight(WIDGET_HEIGHT);
    clear_button_.setFixedHeight(WIDGET_HEIGHT);
    confirm_button_.setFocusPolicy(Qt::NoFocus);
    clear_button_.setFocusPolicy(Qt::NoFocus);

    hbox_.addWidget(&confirm_button_);
    hbox_.addWidget(&clear_button_);

    // keyboard.
    keyboard_.attachReceiver(this);
    vbox_.addWidget(&keyboard_);

    // Setup connections.
    connect(&confirm_button_, SIGNAL(clicked()), this, SLOT(onConfirmClicked()), Qt::QueuedConnection);
    connect(&clear_button_, SIGNAL(clicked()), &text_edit_, SLOT(clear()));
    connect(&text_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
}

/// Update all widget according to the specified parameter.
void UrlDialog::updateChildrenWidgets(bool searching)
{
    confirm_button_.setEnabled(!searching);
    clear_button_.setEnabled(!searching);
    text_edit_.setEnabled(!searching);
    keyboard_.setVisible(!searching);
}

void UrlDialog::onConfirmClicked()
{
    shadows_.show(false);
    accept();
}

void UrlDialog::onCloseClicked()
{
    // Make sure caller stop searching.
    shadows_.show(false);
    done(QDialog::Rejected);
}

void UrlDialog::onTextChanged(const QString& text)
{
    confirm_button_.setEnabled(!text.isEmpty());
    clear_button_.setEnabled(!text.isEmpty());

}

/// This function is called by parent widget to display the search widget.
QString UrlDialog::popup(const QString & path)
{
    onyx::screen::instance().enableUpdate(false);

    shadows_.show(true);
    if (isHidden())
    {
        show();
    }
    resize(parentWidget()->width() - 2 * shadows_.PIXELS, height());

    // TODO, need to figure out the height of status bar. Or we can ask parent widget
    // to place the position.
    move(shadows_.PIXELS,
         parentWidget()->height() - height() - shadows_.PIXELS - 20);

    if (text_edit_.text().isEmpty())
    {
        text_edit_.setText(path);
    }
    onTextChanged(text_edit_.text());
    updateTitle(QApplication::tr("Url"));
    onyx::screen::instance().enableUpdate(true);
    if (exec() != QDialog::Accepted)
    {
        return QString();
    }
    return text_edit_.text();
}

void UrlDialog::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void UrlDialog::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void UrlDialog::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
}

void UrlDialog::keyReleaseEvent(QKeyEvent *ke)
{
    ke->accept();
    int key = ke->key();
    if (key == Qt::Key_Escape || key == ui::Device_Menu_Key)
    {
        onCloseClicked();
        return;
    }
}

/// The keyPressEvent could be sent from virtual keyboard.
void UrlDialog::keyPressEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Enter)
    {
        return;
    }
    else if (ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_CapsLock)
    {
        return;
    }

    // Disable the parent widget to update screen.
    onyx::screen::instance().enableUpdate(false);

    if (text_edit_.hasFocus() ||
        (ke->key() != Qt::Key_Down &&
         ke->key() != Qt::Key_Up &&
         ke->key() != Qt::Key_Left &&
         ke->key() != Qt::Key_Right))
    {
        QKeyEvent * key_event = new QKeyEvent(ke->type(), ke->key(), ke->modifiers(), ke->text());
        QApplication::postEvent(&text_edit_, key_event);
    }

    while (QApplication::hasPendingEvents())
    {
        QApplication::processEvents();
    }

    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW, true, onyx::screen::ScreenCommand::WAIT_ALL);
}

void UrlDialog::updateTitle(const QString &message)
{
    if (message.isEmpty())
    {
        OnyxDialog::updateTitle(QApplication::tr("Url"));
    }
    else
    {
        OnyxDialog::updateTitle(message);
    }
}

bool UrlDialog::event(QEvent * event)
{
    bool ret = OnyxDialog::event(event);
    if (event->type() == QEvent::UpdateRequest)
    {
        if (update_state_ == UPDATE_NONE)
        {
            QTimer::singleShot(0, this, SLOT(forceUpdate()));
        }

        if (onyx::screen::instance().isUpdateEnabled())
        {
            onyx::screen::instance().sync(&shadows_.hor_shadow());
            onyx::screen::instance().sync(&shadows_.ver_shadow());
            if (UPDATE_INITIAL == update_state_)
            {
                onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GC);
                update_state_ = UPDATE_NORMAL;
            }
            else if (update_state_ == UPDATE_NORMAL)
            {
                onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
            }
        }
    }
    return ret;
}

bool UrlDialog::eventFilter(QObject *obj, QEvent *event)
{
     QWidget * wnd = 0;
     if (event->type() == QEvent::KeyRelease)
     {
         QKeyEvent *key_event = static_cast<QKeyEvent *>(event);
         if (obj == &text_edit_)
         {
             if (key_event->key() == Qt::Key_Down)
             {
                 return true;
             }
             else if (key_event->key() == Qt::Key_Right && confirm_button_.isEnabled())
             {
                 confirm_button_.setFocus();
                 return true;
             }
         }
         else if (obj == &confirm_button_)
         {
             if (key_event->key() == Qt::Key_Down)
             {
                 return true;
             }
             else if (key_event->key() == Qt::Key_Right && clear_button_.isEnabled())
             {
                 clear_button_.setFocus();
                 return true;
             }
             else if (key_event->key() == Qt::Key_Left)
             {
                 text_edit_.setFocus();
                 return true;
             }
         }
         else if (obj == &clear_button_)
         {
             if (key_event->key() == Qt::Key_Down)
             {
                 return true;
             }
             else if (key_event->key() == Qt::Key_Left && confirm_button_.isEnabled())
             {
                 confirm_button_.setFocus();
                 return true;
             }
         }
         else if (obj == &keyboard_)
         {
             if (key_event->key() == Qt::Key_Down ||
                 key_event->key() == Qt::Key_Up ||
                 key_event->key() == Qt::Key_Left ||
                 key_event->key() == Qt::Key_Right)
             {
                 wnd = ui::moveFocus(this, key_event->key());
                 if (wnd)
                 {
                     wnd->setFocus();
                 }
                 return true;
             }
        }

     }
     // standard event processing
     return QObject::eventFilter(obj, event);
}

void UrlDialog::moveEvent(QMoveEvent *e)
{
    OnyxDialog::moveEvent(e);
}

void UrlDialog::resizeEvent(QResizeEvent *e)
{
    OnyxDialog::resizeEvent(e);
}

void UrlDialog::forceUpdate()
{
    update_state_ = UPDATE_INITIAL;
    update();
}

void UrlDialog::onTimeout()
{
    // Some simple animation. Using the fast update if possible.
}

}   // namespace view

}   // namespace explorer

