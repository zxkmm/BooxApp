/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. 
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QT_NO_QWS_TRANSFORMED

#include "qscreentransformed_qws.h"
#include <qscreendriverfactory_qws.h>
#include <qvector.h>
#include <private/qpainter_p.h>
#include <private/qmemrotate_p.h>
#include <qmatrix.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>

#include <qwindowsystem_qws.h>
#include <qwsdisplay_qws.h>
#include "bs_cmd.h"
QT_BEGIN_NAMESPACE

//#define QT_REGION_DEBUG
#include <QDebug>

class QTransformedScreenPrivate
{
public:
    QTransformedScreenPrivate(QTransformedScreen *parent);

    void configure();

    QTransformedScreen::Transformation transformation;
#ifdef QT_QWS_DEPTH_GENERIC
    bool doGenericColors;
#endif
    QTransformedScreen *q;
};

QTransformedScreenPrivate::QTransformedScreenPrivate(QTransformedScreen *parent)
    : transformation(QTransformedScreen::None),
#ifdef QT_QWS_DEPTH_GENERIC
      doGenericColors(false),
#endif
      q(parent)
{
}

extern "C"
#ifndef QT_BUILD_GUI_LIB
Q_DECL_EXPORT
#endif
void qws_setScreenTransformation(QScreen *that, int t)
{
    QTransformedScreen *tscreen = static_cast<QTransformedScreen*>(that);
    tscreen->setTransformation((QTransformedScreen::Transformation)t);
}

// ---------------------------------------------------------------------------
// Transformed Screen
// ---------------------------------------------------------------------------

/*!
    \internal

    \class QTransformedScreen
    \ingroup qws

    \brief The QTransformedScreen class implements a screen driver for
    a transformed screen.

    Note that this class is only available in \l{Qt for Embedded Linux}.
    Custom screen drivers can be added by subclassing the
    QScreenDriverPlugin class, using the QScreenDriverFactory class to
    dynamically load the driver into the application, but there should
    only be one screen object per application.

    Use the QScreen::isTransformed() function to determine if a screen
    is transformed. The QTransformedScreen class itself provides means
    of rotating the screen with its setTransformation() function; the
    transformation() function returns the currently set rotation in
    terms of the \l Transformation enum (which describes the various
    available rotation settings). Alternatively, QTransformedScreen
    provides an implementation of the QScreen::transformOrientation()
    function, returning the current rotation as an integer value.

    \sa QScreen, QScreenDriverPlugin, {Running Applications}
*/

/*!
    \enum QTransformedScreen::Transformation

    This enum describes the various rotations a transformed screen can
    have.

    \value None No rotation
    \value Rot90 90 degrees rotation
    \value Rot180 180 degrees rotation
    \value Rot270 270 degrees rotation
*/

/*!
    \fn bool QTransformedScreen::isTransformed() const
    \reimp
*/

static int screen_width = 600;    ///< Default value is set to 600 so it can work for all resolutions.

/*!
    Constructs a QTransformedScreen object. The \a displayId argument
    identifies the Qt for Embedded Linux server to connect to.
*/
QTransformedScreen::QTransformedScreen(int displayId)
    : QProxyScreen(displayId, QScreen::TransformedClass)
{
    qDebug("QTransformedScreen is created and change to hif mode!");
    bs_cmd_flag_hif_mode_cmd();
    screen_width = qgetenv("SCREEN_WIDTH").toInt();
    if (screen_width < 0)
    {
        screen_width = 600;
    }

    d_ptr = new QTransformedScreenPrivate(this);
    d_ptr->transformation = None;

#ifdef QT_REGION_DEBUG
    qDebug() << "QTransformedScreen::QTransformedScreen";
#endif
}

void QTransformedScreenPrivate::configure()
{
    // ###: works because setTransformation recalculates unconditionally
    q->setTransformation(transformation);
}

/*!
    Destroys the QTransformedScreen object.
*/
QTransformedScreen::~QTransformedScreen()
{
    delete d_ptr;
}

static int getDisplayId(const QString &spec)
{
    QRegExp regexp(QLatin1String(":(\\d+)\\b"));
    if (regexp.lastIndexIn(spec) != -1) {
        const QString capture = regexp.cap(1);
        return capture.toInt();
    }
    return 0;
}

static QTransformedScreen::Transformation filterTransformation(QString &spec)
{
    QRegExp regexp(QLatin1String("\\bRot(\\d+):?\\b"), Qt::CaseInsensitive);
    if (regexp.indexIn(spec) == -1)
        return QTransformedScreen::None;

    const int degrees = regexp.cap(1).toInt();
    spec.remove(regexp.pos(0), regexp.matchedLength());

    return static_cast<QTransformedScreen::Transformation>(degrees / 90);
}

/*!
    \reimp
*/
bool QTransformedScreen::connect(const QString &displaySpec)
{
    QString dspec = displaySpec.trimmed();
    if (dspec.startsWith(QLatin1String("Transformed:"), Qt::CaseInsensitive))
        dspec = dspec.mid(QString(QLatin1String("Transformed:")).size());
    else if (!dspec.compare(QLatin1String("Transformed"), Qt::CaseInsensitive))
        dspec = QString();

    const QString displayIdSpec = QString(QLatin1String(" :%1")).arg(displayId);
    if (dspec.endsWith(displayIdSpec))
        dspec = dspec.left(dspec.size() - displayIdSpec.size());

    d_ptr->transformation = filterTransformation(dspec);

    QString driver = dspec;
    int colon = driver.indexOf(QLatin1Char(':'));
    if (colon >= 0)
        driver.truncate(colon);

    if (!QScreenDriverFactory::keys().contains(driver, Qt::CaseInsensitive))
        if (!dspec.isEmpty())
            dspec.prepend(QLatin1String(":"));

    const int id = getDisplayId(dspec);
    QScreen *s = qt_get_screen(id, dspec.toLatin1().constData());

    setScreen(s);

#ifdef QT_QWS_DEPTH_GENERIC
    d_ptr->doGenericColors = dspec.contains(QLatin1String("genericcolors"));
#endif

    d_ptr->configure();

    // XXX
    qt_screen = this;

    return true;
}

/*!
    Returns the currently set rotation.

    \sa setTransformation(), QScreen::transformOrientation()
*/
QTransformedScreen::Transformation QTransformedScreen::transformation() const
{
    return d_ptr->transformation;
}

/*!
    \reimp
*/
int QTransformedScreen::transformOrientation() const
{
    return (int)d_ptr->transformation;
}

/*!
    \reimp
*/
void QTransformedScreen::exposeRegion(QRegion region, int changing)
{
    QRect rc = region.boundingRect();

    // bs_cmd_run_sys();
    QTime t;
    t.start();
    if (!data || d_ptr->transformation == None) {

        QProxyScreen::exposeRegion(region, changing);

        /*
        QImage snap(QScreen::data, 600, 800, QImage::Format_Indexed8);
        snap.setNumColors(256);
        for(int i = 0; i < 256; ++i) snap.setColor(i, qRgb(i,i,i));
        snap.save("600x800.png", "png");    
        */

        // update
        bs_cmd_ld_img_area_data(3, rc.left(), rc.top(), rc.width(), rc.height(), QScreen::data, screen_width);
        qDebug("Time copy data to controller %d", t.elapsed());
        return;
    }
    QScreen::exposeRegion(region, changing);

    if (d_ptr->transformation == Rot90)
    {
        bs_cmd_ld_img_area_data(3, rc.top(), bs_hsize - rc.left() - rc.width(),
                                rc.height(), rc.width(), QScreen::data, screen_width);
        qDebug("Time copy data to controller %d", t.elapsed());
    }
    else if(d_ptr->transformation == Rot270)
    {
        bs_cmd_ld_img_area_data(3, bs_vsize - rc.top() - rc.height(), rc.left(),
                                rc.height(), rc.width(), QScreen::data, screen_width);
        qDebug("Time copy data to controller %d", t.elapsed());
    }
}

/*!
    Rotates this screen object according to the specified \a transformation.

    \sa transformation()
*/
void QTransformedScreen::setTransformation(Transformation transformation)
{
    d_ptr->transformation = transformation;
    QSize size = mapFromDevice(QSize(dw, dh));
    w = size.width();
    h = size.height();

    const QScreen *s = screen();
    size = mapFromDevice(QSize(s->physicalWidth(), s->physicalHeight()));
    physWidth = size.width();
    physHeight = size.height();

#ifdef QT_REGION_DEBUG
    qDebug() << "QTransformedScreen::setTransformation" << transformation
             << "size" << w << h << "dev size" << dw << dh;
#endif

}

static inline QRect correctNormalized(const QRect &r) {
    const int x1 = qMin(r.left(), r.right());
    const int x2 = qMax(r.left(), r.right());
    const int y1 = qMin(r.top(), r.bottom());
    const int y2 = qMax(r.top(), r.bottom());

    return QRect( QPoint(x1,y1), QPoint(x2,y2) );
}

template <class DST, class SRC>
static inline void blit90(QScreen *screen, const QImage &image,
                          const QRect &rect, const QPoint &topLeft)
{
    const SRC *src = (const SRC*)(image.scanLine(rect.top())) + rect.left();
    DST *dest = (DST*)(screen->base() + topLeft.y() * screen->linestep())
                + topLeft.x();
    qt_memrotate90(src, rect.width(), rect.height(), image.bytesPerLine(),
                   dest, screen->linestep());
}

template <class DST, class SRC>
static inline void blit180(QScreen *screen, const QImage &image,
                           const QRect &rect, const QPoint &topLeft)
{
    const SRC *src = (const SRC*)(image.scanLine(rect.top())) + rect.left();
    DST *dest = (DST*)(screen->base() + topLeft.y() * screen->linestep())
                + topLeft.x();
    qt_memrotate180(src, rect.width(), rect.height(), image.bytesPerLine(),
                    dest, screen->linestep());
}

template <class DST, class SRC>
static inline void blit270(QScreen *screen, const QImage &image,
                           const QRect &rect, const QPoint &topLeft)
{
    const SRC *src = (const SRC *)(image.scanLine(rect.top())) + rect.left();
    DST *dest = (DST*)(screen->base() + topLeft.y() * screen->linestep())
                + topLeft.x();
    qt_memrotate270(src, rect.width(), rect.height(), image.bytesPerLine(),
                    dest, screen->linestep());
}

typedef void (*BlitFunc)(QScreen *, const QImage &, const QRect &, const QPoint &);

#define SET_BLIT_FUNC(dst, src, rotation, func) \
do {                                            \
    switch (rotation) {                         \
    case Rot90:                                 \
        func = blit90<dst, src>;                \
        break;                                  \
    case Rot180:                                \
        func = blit180<dst, src>;               \
        break;                                  \
    case Rot270:                                \
        func = blit270<dst, src>;               \
        break;                                  \
    default:                                    \
        break;                                  \
    }                                           \
} while (0)

/*!
    \reimp
*/
void QTransformedScreen::blit(const QImage &image, const QPoint &topLeft,
                              const QRegion &region)
{
    const Transformation trans = d_ptr->transformation;
    if (trans == None) {
        QProxyScreen::blit(image, topLeft, region);
        return;
    }

    const QVector<QRect> rects = region.rects();
    const QRect bound = QRect(0, 0, QScreen::w, QScreen::h)
                        & QRect(topLeft, image.size());

    BlitFunc func = 0;
#ifdef QT_QWS_DEPTH_GENERIC
    if (d_ptr->doGenericColors && depth() == 16) {
        if (image.depth() == 16)
            SET_BLIT_FUNC(qrgb_generic16, quint16, trans, func);
        else
            SET_BLIT_FUNC(qrgb_generic16, quint32, trans, func);
    } else
#endif
    switch (depth()) {
#ifdef QT_QWS_DEPTH_32
    case 32:
#ifdef QT_QWS_DEPTH_16
        if (image.depth() == 16)
            SET_BLIT_FUNC(quint32, quint16, trans, func);
        else
#endif
            SET_BLIT_FUNC(quint32, quint32, trans, func);
        break;
#endif
#if defined(QT_QWS_DEPTH_24) || defined(QT_QWS_DEPTH18)
    case 24:
    case 18:
        SET_BLIT_FUNC(quint24, quint24, trans, func);
        break;
#endif
#if defined(QT_QWS_DEPTH_16) || defined(QT_QWS_DEPTH_15) || defined(QT_QWS_DEPTH_12)
    case 16:
    case 15:
    case 12:
        if (image.depth() == 16)
            SET_BLIT_FUNC(quint16, quint16, trans, func);
        else
            SET_BLIT_FUNC(quint16, quint32, trans, func);
        break;
#endif
#ifdef QT_QWS_DEPTH_8
    case 8:
        if (image.depth() == 16)
        {
            SET_BLIT_FUNC(quint8, quint16, trans, func);
        }
        else
        {
            printf("image NOT 16 %d screen 8\n\n", image.depth());
            SET_BLIT_FUNC(quint8, quint32, trans, func);
        }
        break;
#endif
    default:
        return;
    }
    if (!func)
        return;

    // printf("QTransformedScreen::blit update func\n\n");
    QWSDisplay::grab();
    for (int i = 0; i < rects.size(); ++i) {
        const QRect r = rects.at(i) & bound;

        QPoint dst;
        switch (trans) {
        case Rot90:
            dst = mapToDevice(r.topRight(), QSize(w, h));
            break;
        case Rot180:
            dst = mapToDevice(r.bottomRight(), QSize(w, h));
            break;
        case Rot270:
            dst = mapToDevice(r.bottomLeft(), QSize(w, h));
            break;
        default:
            break;
        }
        func(this, image, r.translated(-topLeft), dst);
    }
    QWSDisplay::ungrab();
}

/*!
    \reimp
*/
void QTransformedScreen::solidFill(const QColor &color, const QRegion &region)
{
    const QRegion tr = mapToDevice(region, QSize(w,h));

    Q_ASSERT(tr.boundingRect() == mapToDevice(region.boundingRect(), QSize(w,h)));

#ifdef QT_REGION_DEBUG
    qDebug() << "QTransformedScreen::solidFill region" << region << "transformed" << tr;
#endif
    QProxyScreen::solidFill(color, tr);
}

/*!
    \reimp
*/
QSize QTransformedScreen::mapToDevice(const QSize &s) const
{
    switch (d_ptr->transformation) {
    case None:
    case Rot180:
        break;
    case Rot90:
    case Rot270:
        return QSize(s.height(), s.width());
        break;
    }
    return s;
}

/*!
    \reimp
*/
QSize QTransformedScreen::mapFromDevice(const QSize &s) const
{
    switch (d_ptr->transformation) {
    case None:
    case Rot180:
        break;
    case Rot90:
    case Rot270:
        return QSize(s.height(), s.width());
        break;
    }
    return s;
}

/*!
    \reimp
*/
QPoint QTransformedScreen::mapToDevice(const QPoint &p, const QSize &s) const
{
    QPoint rp(p);

    switch (d_ptr->transformation) {
    case None:
        break;
    case Rot90:
        rp.setX(p.y());
        rp.setY(s.width() - p.x() - 1);
        break;
    case Rot180:
        rp.setX(s.width() - p.x() - 1);
        rp.setY(s.height() - p.y() - 1);
        break;
    case Rot270:
        rp.setX(s.height() - p.y() - 1);
        rp.setY(p.x());
        break;
    }

    return rp;
}

/*!
    \reimp
*/
QPoint QTransformedScreen::mapFromDevice(const QPoint &p, const QSize &s) const
{
    QPoint rp(p);

    switch (d_ptr->transformation) {
    case None:
        break;
    case Rot90:
        rp.setX(s.height() - p.y() - 1);
        rp.setY(p.x());
        break;
    case Rot180:
        rp.setX(s.width() - p.x() - 1);
        rp.setY(s.height() - p.y() - 1);
        break;
    case Rot270:
        rp.setX(p.y());
        rp.setY(s.width() - p.x() - 1);
        break;
    }

    return rp;
}

/*!
    \reimp
*/
QRect QTransformedScreen::mapToDevice(const QRect &r, const QSize &s) const
{
    if (r.isNull())
        return QRect();

    QRect tr;
    switch (d_ptr->transformation) {
    case None:
        tr = r;
        break;
    case Rot90:
        tr.setCoords(r.y(), s.width() - r.x() - 1,
                     r.bottom(), s.width() - r.right() - 1);
        break;
    case Rot180:
        tr.setCoords(s.width() - r.x() - 1, s.height() - r.y() - 1,
                     s.width() - r.right() - 1, s.height() - r.bottom() - 1);
        break;
    case Rot270:
        tr.setCoords(s.height() - r.y() - 1, r.x(),
                     s.height() - r.bottom() - 1, r.right());
        break;
    }

    return correctNormalized(tr);
}

/*!
    \reimp
*/
QRect QTransformedScreen::mapFromDevice(const QRect &r, const QSize &s) const
{
    if (r.isNull())
        return QRect();

    QRect tr;
    switch (d_ptr->transformation) {
    case None:
        tr = r;
        break;
    case Rot90:
        tr.setCoords(s.height() - r.y() - 1, r.x(),
                     s.height() - r.bottom() - 1, r.right());
        break;
    case Rot180:
        tr.setCoords(s.width() - r.x() - 1, s.height() - r.y() - 1,
                     s.width() - r.right() - 1, s.height() - r.bottom() - 1);
        break;
    case Rot270:
        tr.setCoords(r.y(), s.width() - r.x() - 1,
                     r.bottom(), s.width() - r.right() - 1);
        break;
    }

    return correctNormalized(tr);
}

/*!
    \reimp
*/
QRegion QTransformedScreen::mapToDevice(const QRegion &rgn, const QSize &s) const
{
    if (d_ptr->transformation == None)
        return QProxyScreen::mapToDevice(rgn, s);

#ifdef QT_REGION_DEBUG
    qDebug() << "mapToDevice size" << s << "rgn:  " << rgn;
#endif
    QRect tr;
    QRegion trgn;
    QVector<QRect> a = rgn.rects();
    const QRect *r = a.data();

    int w = s.width();
    int h = s.height();
    int size = a.size();

    switch (d_ptr->transformation) {
    case None:
        break;
    case Rot90:
        for (int i = 0; i < size; i++, r++) {
            tr.setCoords(r->y(), w - r->x() - 1,
                         r->bottom(), w - r->right() - 1);
            trgn |= correctNormalized(tr);
        }
        break;
    case Rot180:
        for (int i = 0; i < size; i++, r++) {
            tr.setCoords(w - r->x() - 1, h - r->y() - 1,
                         w - r->right() - 1, h - r->bottom() - 1);
            trgn |= correctNormalized(tr);
        }
        break;
    case Rot270:
        for (int i = 0; i < size; i++, r++) {
            tr.setCoords(h - r->y() - 1, r->x(),
                         h - r->bottom() - 1, r->right());
            trgn |= correctNormalized(tr);
        }
        break;
    }
#ifdef QT_REGION_DEBUG
    qDebug() << "mapToDevice trgn:  " << trgn;
#endif
    return trgn;
}

/*!
    \reimp
*/
QRegion QTransformedScreen::mapFromDevice(const QRegion &rgn, const QSize &s) const
{
    if (d_ptr->transformation == None)
        return QProxyScreen::mapFromDevice(rgn, s);
#ifdef QT_REGION_DEBUG
    qDebug() << "fromDevice: realRegion count:  " << rgn.rects().size() << " isEmpty? " << rgn.isEmpty() << "  bounds:" << rgn.boundingRect();
#endif
    QRect tr;
    QRegion trgn;
    QVector<QRect> a = rgn.rects();
    const QRect *r = a.data();

    int w = s.width();
    int h = s.height();
    int size = a.size();

    switch (d_ptr->transformation) {
    case None:
        break;
    case Rot90:
        for (int i = 0; i < size; i++, r++) {
            tr.setCoords(h - r->y() - 1, r->x(),
                         h - r->bottom() - 1, r->right());
            trgn |= correctNormalized(tr);
        }
        break;
    case Rot180:
        for (int i = 0; i < size; i++, r++) {
            tr.setCoords(w - r->x() - 1, h - r->y() - 1,
                         w - r->right() - 1, h - r->bottom() - 1);
            trgn |= correctNormalized(tr);
        }
        break;
    case Rot270:
        for (int i = 0; i < size; i++, r++) {
            tr.setCoords(r->y(), w - r->x() - 1,
                         r->bottom(), w - r->right() - 1);
            trgn |= correctNormalized(tr);
        }
        break;
    }
#ifdef QT_REGION_DEBUG
    qDebug() << "fromDevice: transRegion count: " << trgn.rects().size() << " isEmpty? " << trgn.isEmpty() << "  bounds:" << trgn.boundingRect();
#endif
    return trgn;
}

/*!
    \reimp
*/
void QTransformedScreen::setDirty(const QRect& rect)
{
    const QRect r = mapToDevice(rect, QSize(width(), height()));
    QProxyScreen::setDirty(r);
}

/*!
    \reimp
*/
QRegion QTransformedScreen::region() const
{
    QRegion deviceRegion = QProxyScreen::region();
    return mapFromDevice(deviceRegion, QSize(deviceWidth(), deviceHeight()));
}

QT_END_NAMESPACE

#endif // QT_NO_QWS_TRANSFORMED
