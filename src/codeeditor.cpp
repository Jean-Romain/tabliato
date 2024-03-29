﻿/****************************************************************************
 **
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
 **     the names of its contributors may be used to endorse or promote
 **     products derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include <QtGui>
#include <QToolTip>
#include "codeeditor.h"


 CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
 {
     lineNumberArea = new LineNumberArea(this);

     connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
     connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
     connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

     //setMouseTracking(true);
     updateLineNumberAreaWidth(0);
     highlightCurrentLine();
 }

 bool CodeEditor::event(QEvent *event)
 {
     if (event->type() == QEvent::ToolTip)
     {
         QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);

         QPoint pos = helpEvent->pos();
         pos.setX(pos.x() - viewportMargins().left());
         pos.setY(pos.y() - viewportMargins().top());

         QTextCursor cursor = cursorForPosition(pos);
         int start,end;

         while (!cursor.atEnd())
         {
             cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
             if (cursor.selectedText() == ' ' || cursor.selectedText() == QChar(8233)) //8233 unicode for new paragraph
             {
                 cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
                 break;
             }

             cursor.clearSelection();
         }

         end = cursor.position();

         while (!cursor.atStart())
         {
             cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
             if (cursor.selectedText() == ' ' || cursor.selectedText() == QChar(8233))
             {
                 break;
             }
             cursor.clearSelection();
         }

         start = cursor.position();

         for(int i= start ; i < end ; i++)
         {
             cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
         }

         QString s = cursor.selectedText();

         if (!s.isEmpty())
         {
            QString tooltip = keyword_tooltip(s);

            if (!tooltip.isEmpty())
                QToolTip::showText(helpEvent->globalPos(), tooltip);
            else
                QToolTip::hideText();
         }
         else
         {
             QToolTip::hideText();
         }

         return true;
     }

     return QPlainTextEdit::event(event);
 }

 void CodeEditor::wheelEvent ( QWheelEvent * event )
 {
     if(event->modifiers() & Qt::ControlModifier)
     {
         QFont font = this->font();
         int size = font.pointSize();

         if(event->delta() > 0)
             size++;
         else
             size--;

         font.setPointSize(size);
         setFont(font);
     }
     else
         QPlainTextEdit::wheelEvent(event);
 }

 void CodeEditor::resizeEvent(QResizeEvent *e)
 {
     QPlainTextEdit::resizeEvent(e);

     QRect cr = contentsRect();
     lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
 }


 int CodeEditor::lineNumberAreaWidth()
 {
     int digits = 1;
     int max = qMax(1, blockCount());
     while (max >= 10) {
         max /= 10;
         ++digits;
     }

     int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

     return space;
 }

 void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
 {
     setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
 }

 void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
 {
     if (dy)
         lineNumberArea->scroll(0, dy);
     else
         lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

     if (rect.contains(viewport()->rect()))
         updateLineNumberAreaWidth(0);
 }

 void CodeEditor::highlightCurrentLine()
 {
     QList<QTextEdit::ExtraSelection> extraSelections;

     if (!isReadOnly()) {
         QTextEdit::ExtraSelection selection;

         QColor lineColor = QColor(Qt::yellow).lighter(180);

         selection.format.setBackground(lineColor);
         selection.format.setProperty(QTextFormat::FullWidthSelection, true);
         selection.cursor = textCursor();
         selection.cursor.clearSelection();
         extraSelections.append(selection);
     }

     setExtraSelections(extraSelections);
 }

 void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
 {
     QPainter painter(lineNumberArea);
     painter.fillRect(event->rect(),  QColor(Qt::lightGray).light(115));


     QTextBlock block = firstVisibleBlock();
     int blockNumber = block.blockNumber();
     int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
     int bottom = top + (int) blockBoundingRect(block).height();

     while (block.isValid() && top <= event->rect().bottom())
     {
         if (block.isVisible() && bottom >= event->rect().top())
         {
             QString number = QString::number(blockNumber + 1);
             painter.setPen(Qt::black);
             painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
         }

         block = block.next();
         top = bottom;
         bottom = top + (int) blockBoundingRect(block).height();
         ++blockNumber;
     }
 }


QString CodeEditor::keyword_tooltip(QString str)
{
    QStringList keywords;
    QStringList tooltips;

    keywords << "\\repeat"
             << "volta"
             << "\\break"
             << "\\tuplet"
             << "\\alternative"
             << "\\time"
             << "\\motif"
             << "\\partial"
             << "\\finger"
             << "\\mbox";
    tooltips << "\\repeat : répétiton ou reprise. Doit être suivit du mot clé volta"
             << "volta : suit le mot clé repeat et indique un reprise"
             << "\\break : instruction pour forcer le passage à la ligne suivante"
             << "\\tuplet : suivit de 3/2 pour écrire un triolet"
             << "\\alternative: une répétition avec fins alternatives. Vient après un bloc \\repeat volta n"
             << "\\time : le chiffre de mesure indique le mètre d’une pièce"
             << "\\motif : suivit d'un motif rythmique tel que [B:4 a:4 a:4] indique comment produire les informations d'accompagnement main gauche"
             << "\\partial : mesure partielle d'anacrouse. Doit être de la durée de la mesure partielle"
             << "\\finger : indication de doigté"
             << "\\mbox : suivit d'une chaine de caractères. Crée un boite au dessus de la porté pour ajouter une annotation.";

    int idx = keywords.indexOf(str);

    if (idx == -1)
        return ""; //str;
    else
        return tooltips[idx];
}


