/***********************************************************************
*
* Copyright (c) 2012-2018 Barbara Geller
* Copyright (c) 2012-2018 Ansel Sermersheim
* Copyright (c) 2012-2016 Digia Plc and/or its subsidiary(-ies).
* Copyright (c) 2008-2012 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
*
* This file is part of CopperSpice.
*
* CopperSpice is free software. You can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* version 2.1 as published by the Free Software Foundation.
*
* CopperSpice is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* <http://www.gnu.org/licenses/>.
*
***********************************************************************/

#include <qpdfwriter.h>

#ifndef QT_NO_PDF

#include <qpagedpaintdevice_p.h>
#include <qpdf_p.h>
#include <qfile.h>

class QPdfWriterPrivate
{
public:
    QPdfWriterPrivate()
    {
        engine = new QPdfEngine();
        output = 0;
    }

    ~QPdfWriterPrivate()
    {
        delete engine;
        delete output;
    }

    QPdfEngine *engine;
    QFile *output;
};

class QPdfPagedPaintDevicePrivate : public QPagedPaintDevicePrivate
{
public:
    QPdfPagedPaintDevicePrivate(QPdfWriterPrivate *d)
        : QPagedPaintDevicePrivate(), pd(d)
    {}

    virtual ~QPdfPagedPaintDevicePrivate()
    {}

    bool setPageLayout(const QPageLayout &newPageLayout) override
    {
        // Try to set the paint engine page layout
        pd->engine->setPageLayout(newPageLayout);

        // Set QPagedPaintDevice layout to match the current paint engine layout
        m_pageLayout = pd->engine->pageLayout();
        return m_pageLayout.isEquivalentTo(newPageLayout);
    }

    bool setPageSize(const QPageSize &pageSize) override
    {
        // Try to set the paint engine page size
        pd->engine->setPageSize(pageSize);

        // Set QPagedPaintDevice layout to match the current paint engine layout
        m_pageLayout = pd->engine->pageLayout();
        return m_pageLayout.pageSize().isEquivalentTo(pageSize);
    }

    bool setPageOrientation(QPageLayout::Orientation orientation) override
    {
        // Set the print engine value
        pd->engine->setPageOrientation(orientation);
        // Set QPagedPaintDevice layout to match the current paint engine layout
        m_pageLayout = pd->engine->pageLayout();
        return m_pageLayout.orientation() == orientation;
    }

    bool setPageMargins(const QMarginsF &margins) override
    {
        return setPageMargins(margins, pageLayout().units());
    }

    bool setPageMargins(const QMarginsF &margins, QPageLayout::Unit units) override
    {
        // Try to set engine margins
        pd->engine->setPageMargins(margins, units);

        // Set QPagedPaintDevice layout to match the current paint engine layout
        m_pageLayout = pd->engine->pageLayout();
        return m_pageLayout.margins() == margins && m_pageLayout.units() == units;
    }

    QPageLayout pageLayout() const override
    {
        return pd->engine->pageLayout();
    }

    QPdfWriterPrivate *pd;
};

QPdfWriter::QPdfWriter(const QString &filename)
    : QObject(), QPagedPaintDevice(new QPdfPagedPaintDevicePrivate(nullptr)), d_ptr(new QPdfWriterPrivate)
{
   Q_D(QPdfWriter);

   // update pd with our d_ptr
   QPdfPagedPaintDevicePrivate *obj = static_cast<QPdfPagedPaintDevicePrivate *>(d_devicePtr);
   obj->pd = d_ptr.data();

   d->engine->setOutputFilename(filename);

   // Set QPagedPaintDevice layout to match the current paint engine layout
   devicePageLayout() = d->engine->pageLayout();
}

QPdfWriter::QPdfWriter(QIODevice *device)
    : QObject(), QPagedPaintDevice(new QPdfPagedPaintDevicePrivate(nullptr)), d_ptr(new QPdfWriterPrivate)
{
   Q_D(QPdfWriter);

   // update pd with our d_ptr
   QPdfPagedPaintDevicePrivate *obj = static_cast<QPdfPagedPaintDevicePrivate *>(d_devicePtr);
   obj->pd = d_ptr.data();

   d->engine->d_func()->outDevice = device;

   // Set QPagedPaintDevice layout to match the current paint engine layout
   devicePageLayout() = d->engine->pageLayout();
}

QPdfWriter::~QPdfWriter()
{
}

QString QPdfWriter::title() const
{
    Q_D(const QPdfWriter);
    return d->engine->d_func()->title;
}

void QPdfWriter::setTitle(const QString &title)
{
    Q_D(QPdfWriter);
    d->engine->d_func()->title = title;
}


QString QPdfWriter::creator() const
{
    Q_D(const QPdfWriter);
    return d->engine->d_func()->creator;
}

/*!
  Sets the creator of the document to \a creator.
  */
void QPdfWriter::setCreator(const QString &creator)
{
    Q_D(QPdfWriter);
    d->engine->d_func()->creator = creator;
}

/*!
  \reimp
  */
QPaintEngine *QPdfWriter::paintEngine() const
{
    Q_D(const QPdfWriter);

    return d->engine;
}

/*!
    \since 5.3

    Sets the PDF \a resolution in DPI.

    This setting affects the coordinate system as returned by, for
    example QPainter::viewport().

    \sa resolution()
*/

void QPdfWriter::setResolution(int resolution)
{
    Q_D(const QPdfWriter);
    if (resolution > 0)
        d->engine->setResolution(resolution);
}

/*!
    \since 5.3

    Returns the resolution of the PDF in DPI.

    \sa setResolution()
*/

int QPdfWriter::resolution() const
{
    Q_D(const QPdfWriter);
    return d->engine->resolution();
}

// Defined in QPagedPaintDevice but non-virtual, add QPdfWriter specific doc here
#ifdef Q_QDOC
/*!
    \fn bool QPdfWriter::setPageLayout(const QPageLayout &newPageLayout)
    \since 5.3

    Sets the PDF page layout to \a newPageLayout.

    You should call this before calling QPainter::begin(), or immediately
    before calling newPage() to apply the new page layout to a new page.
    You should not call any painting methods between a call to setPageLayout()
    and newPage() as the wrong paint metrics may be used.

    Returns true if the page layout was successfully set to \a newPageLayout.

    \sa pageLayout()
*/

/*!
    \fn bool QPdfWriter::setPageSize(const QPageSize &pageSize)
    \since 5.3

    Sets the PDF page size to \a pageSize.

    To get the current QPageSize use pageLayout().pageSize().

    You should call this before calling QPainter::begin(), or immediately
    before calling newPage() to apply the new page size to a new page.
    You should not call any painting methods between a call to setPageSize()
    and newPage() as the wrong paint metrics may be used.

    Returns true if the page size was successfully set to \a pageSize.

    \sa pageLayout()
*/

/*!
    \fn bool QPdfWriter::setPageOrientation(QPageLayout::Orientation orientation)
    \since 5.3

    Sets the PDF page \a orientation.

    The page orientation is used to define the orientation of the
    page size when obtaining the page rect.

    You should call this before calling QPainter::begin(), or immediately
    before calling newPage() to apply the new orientation to a new page.
    You should not call any painting methods between a call to setPageOrientation()
    and newPage() as the wrong paint metrics may be used.

    To get the current QPageLayout::Orientation use pageLayout().pageOrientation().

    Returns true if the page orientation was successfully set to \a orientation.

    \sa pageLayout()
*/

/*!
    \fn bool QPdfWriter::setPageMargins(const QMarginsF &margins)
    \since 5.3

    Set the PDF page \a margins in the current page layout units.

    You should call this before calling QPainter::begin(), or immediately
    before calling newPage() to apply the new margins to a new page.
    You should not call any painting methods between a call to setPageMargins()
    and newPage() as the wrong paint metrics may be used.

    To get the current page margins use pageLayout().pageMargins().

    Returns true if the page margins were successfully set to \a margins.

    \sa pageLayout()
*/

/*!
    \fn bool QPdfWriter::setPageMargins(const QMarginsF &margins, QPageLayout::Unit units)
    \since 5.3

    Set the PDF page \a margins defined in the given \a units.

    You should call this before calling QPainter::begin(), or immediately
    before calling newPage() to apply the new margins to a new page.
    You should not call any painting methods between a call to setPageMargins()
    and newPage() as the wrong paint metrics may be used.

    To get the current page margins use pageLayout().pageMargins().

    Returns true if the page margins were successfully set to \a margins.

    \sa pageLayout()
*/

/*!
    \fn QPageLayout QPdfWriter::pageLayout() const
    \since 5.3

    Returns the current page layout.  Use this method to access the current
    QPageSize, QPageLayout::Orientation, QMarginsF, fullRect() and paintRect().

    Note that you cannot use the setters on the returned object, you must either
    call the individual QPdfWriter methods or use setPageLayout().

    \sa setPageLayout(), setPageSize(), setPageOrientation(), setPageMargins()
*/
#endif

/*!
    \reimp

    \obsolete Use setPageSize(QPageSize(id)) instead

    \sa setPageSize()
*/

void QPdfWriter::setPageSize(PageSize size)
{
    setPageSize(QPageSize(QPageSize::PageSizeId(size)));
}

/*!
    \reimp

    \obsolete Use setPageSize(QPageSize(size, QPageSize::Millimeter)) instead

    \sa setPageSize()
*/

void QPdfWriter::setPageSizeMM(const QSizeF &size)
{
    setPageSize(QPageSize(size, QPageSize::Millimeter));
}

/*!
    \internal

    Returns the metric for the given \a id.
*/
int QPdfWriter::metric(PaintDeviceMetric id) const
{
    Q_D(const QPdfWriter);
    return d->engine->metric(id);
}

/*!
  \reimp
*/
bool QPdfWriter::newPage()
{
    Q_D(QPdfWriter);

    return d->engine->newPage();
}


/*!
    \reimp

    \obsolete Use setPageMargins(QMarginsF(l, t, r, b), QPageLayout::Millimeter) instead

    \sa setPageMargins()
  */
void QPdfWriter::setMargins(const Margins &m)
{
    setPageMargins(QMarginsF(m.left, m.top, m.right, m.bottom), QPageLayout::Millimeter);
}

QT_END_NAMESPACE

#endif // QT_NO_PDF