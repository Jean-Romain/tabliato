/****************************************************************************
**
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the documentation of Qt. It was originally
** published as part of Qt Quarterly.
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/

#ifndef PDFVIEWER_H
#define PDFVIEWER_H

#include <QLabel>
#include <QRectF>
#include <poppler-qt5.h>

class PdfViewer : public QLabel
{
    Q_OBJECT

public:
    PdfViewer(QWidget *parent = 0);
    ~PdfViewer();
    void show(int page = -1);
    Poppler::Document *document();
    qreal scale() const;
    void highlight_link_from_lines(QVector<int> lines);
    void highlight_note(int pos, int offset);
    static int get_line(Poppler::Link*);
    static int get_column(Poppler::Link*);

public slots:
    bool setDocument(const QString &filePath);
    void setPage(int page = -1);
    void setScale(qreal scale);
    void wheelEvent(QWheelEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

signals:
    void pageChanged(int);
    void scaleChanged(int);
    void linkClicked(int);

private:
    QPointF to_pdf_relative(QPoint);
    QRect to_img_absolute(QRectF);
    QRectF bbox(Poppler::Link*);
    void clean_pdf();
    void init_links();

    int m_current_page;
    bool m_link_hovered;
    bool m_skip_next_event;
    int  m_previous_pos;
    qreal m_scale_factor;
    QImage m_image;
    Poppler::Document *m_doc;
    QList<Poppler::Link*> m_links;

};

#endif
