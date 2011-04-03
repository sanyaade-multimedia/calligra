/* This file is part of the KDE project
   Copyright (C) 2011 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrViewModeOutline.h"

#include "KoPAView.h"
#include "KoPACanvas.h"
#include "KoPAPageBase.h"
#include "KoPADocument.h"
#include "KPrPage.h"
#include "KoTextShapeData.h"

#include "KPrOutlineEditor.h"

#include <KDebug>
#include <QtGui/qtextformat.h>
#include <../libs/kotext/KoTextShapeData.h>

KPrViewModeOutline::KPrViewModeOutline(KoPAView *view, KoPACanvas *canvas)
    : KoPAViewMode( view, canvas )
    , m_outlineEditor(new KPrOutlineEditor(this, view->parentWidget()))
{
    m_outlineEditor->hide();
    connect(m_outlineEditor, SIGNAL(cursorPositionChanged()), SLOT(slotCursorPositionChanged()));
}

void KPrViewModeOutline::paint(KoPACanvasBase* canvas, QPainter& painter, const QRectF &paintRect)
{
    Q_UNUSED(canvas);
    Q_UNUSED(painter);
    Q_UNUSED(paintRect);
}

void KPrViewModeOutline::tabletEvent(QTabletEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeOutline::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModeOutline::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModeOutline::activate(KoPAViewMode *previousViewMode)
{
    Q_UNUSED(previousViewMode);
    
    populate();
    
    activateSynchronize();
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->hide();
    }
    m_outlineEditor->show();
    updateActivePage( m_view->activePage() );
}


void KPrViewModeOutline::deactivate()
{
    deactivateSynchronize();
    m_outlineEditor->hide();
     // Active the view as a basic but active one
    m_view->setActionEnabled(KoPAView::AllActions, true);
    m_view->doUpdateActivePage(m_view->activePage());
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    if (view) {
        view->show();
    }
}

void KPrViewModeOutline::populate()
{
    deactivateSynchronize();
    
    m_outlineEditor->clear();
    QTextCursor currentCursor = m_outlineEditor->textCursor();
    // For each slides
    foreach (KoPAPageBase * pageBase, m_view->kopaDocument()->pages()) {
        if (KPrPage * page = dynamic_cast<KPrPage *>(pageBase)) {
            int pageNumber = m_view->kopaDocument()->pages().indexOf(pageBase);
            bool firstBlock = true;
            // Copy relevant content of the "view" block of the page in the "outline" block
            // Do three iterations in order to kept the correct order i.e. Title, Subtitle, Outline
            foreach (OutlinePair pair, page->placeholders().outlineData()) {
                if (pair.first == Title) {
                    createBlock(currentCursor, pageNumber, pair, firstBlock);
                    firstBlock = false;
                }
            }
            foreach (OutlinePair pair, page->placeholders().outlineData()) {
                if (pair.first == Subtitle) {
                    createBlock(currentCursor, pageNumber, pair, firstBlock);
                    firstBlock = false;
                }
            }
            foreach (OutlinePair pair, page->placeholders().outlineData()) {
                if (pair.first == Outline) {
                    createBlock(currentCursor, pageNumber, pair, firstBlock);
                    firstBlock = false;
                }
            }
        }
    }
    // Delete the first empty line
    currentCursor.setPosition(0);
    currentCursor.deleteChar();

    m_outlineEditor->setTextCursor(currentCursor);

    activateSynchronize();
}

void KPrViewModeOutline::createBlock(QTextCursor cursor, int pageNumber, OutlinePair pair, bool firstBlock)
{
    QTextBlockFormat blockFormat;
    QTextCharFormat charFormat;
    blockFormat.setBackground((pageNumber%2)?QBrush(Qt::gray):QBrush(Qt::white));

    if (firstBlock) {
        blockFormat.setTopMargin(5);
        firstBlock = false;
    }
    if(pair.first == Title){
        charFormat.setFontPointSize(charFormat.fontPointSize()*1.4);
        charFormat.setFontWeight(QFont::Bold);
    }
    cursor.insertBlock(blockFormat, charFormat);
    int start = cursor.blockNumber();
    cursor.insertText(pair.second->document()->toPlainText());
    for(; start <= cursor.blockNumber(); start++){
        m_outlineEditor->document()->findBlockByNumber(start).setUserData(new SlideUserBlockData(pageNumber, pair));
    }

}

void KPrViewModeOutline::synchronize(int position, int charsRemoved, int charsAdded)
{
    // We take the good cursor and move it to the good position, recording the position
    QTextCursor cursor = m_outlineEditor->textCursor();
    cursor.setPosition(position);
    QTextBlock firstShapeBlock;
    // Trying to find which "record" block we are the nearer
    SlideUserBlockData *userData;
    while (!(userData = dynamic_cast<SlideUserBlockData*>( cursor.block().userData() )))
    {
        if (cursor.blockNumber() > 0) {
            cursor.movePosition(QTextCursor::PreviousBlock);
        }
    }

    // If we have found a good block, we then add and remove things to it
    if (userData = dynamic_cast<SlideUserBlockData*>( cursor.block().userData() )) {
        // Look for the first shape block
        for(int i=0; i < m_outlineEditor->document()->blockCount();i++) {
            firstShapeBlock = m_outlineEditor->document()->findBlockByNumber(i);
            SlideUserBlockData *u = dynamic_cast<SlideUserBlockData*>(firstShapeBlock.userData());
            if(u && u->pageNumber() == userData->pageNumber() && u->outlinePair() == userData->outlinePair()){
                break;
            }
        }
        QTextCursor viewCursor =  QTextCursor(userData->outlinePair().second->document());
        
        // Remove stuff to be removed
        if (charsRemoved > 0) {
            // Take position in a shape context
            viewCursor.setPosition(position - firstShapeBlock.position());
            viewCursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsRemoved);
            viewCursor.deleteChar();
        }

        // Add the stuff to be added
        if (charsAdded > 0) {
            cursor.setPosition(position);
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, charsAdded);
            // Take position in a shape context
            viewCursor.setPosition(position - firstShapeBlock.position());
            viewCursor.insertText(cursor.selectedText());
        }
    } else {
        kDebug(33001) << "No user data anymore in the outline::synchronize";
    }
    populate();

    // Put the cursor at the good position
    cursor.setPosition(position + charsAdded - charsRemoved);
    m_outlineEditor->setTextCursor(cursor);
}

void KPrViewModeOutline::slotCursorPositionChanged()
{
    QTextCursor cursor = m_outlineEditor->textCursor();
    SlideUserBlockData *userData = dynamic_cast<SlideUserBlockData*>( cursor.block().userData() );
    if(userData){
        KoPAPageBase * page = m_view->kopaDocument()->pageByIndex(userData->pageNumber(), false);
        if ( m_view->activePage() != page ) {
            m_view->setActivePage(page);
        }
    }
}

void KPrViewModeOutline::updateActivePage( KoPAPageBase * page )
{
    if ( m_view->activePage() != page ) {
        m_view->setActivePage( page );
    }
    KoPAView *view = dynamic_cast<KoPAView *>(m_view);
    setCursorTo(view->kopaDocument()->pageIndex(page));
    m_outlineEditor->setFocus(Qt::ActiveWindowFocusReason);
}

void KPrViewModeOutline::setCursorTo(int slide)
{
    int position = 0;
    for(int i=0; i < m_outlineEditor->document()->blockCount(); i++){
        QTextBlock block = m_outlineEditor->document()->findBlockByNumber(i);
        SlideUserBlockData *userData = dynamic_cast<SlideUserBlockData*>( block.userData() );
        if (userData && userData->pageNumber() == slide) {
            position = block.position();
            break;
        }
    }
    QTextCursor cursor = m_outlineEditor->textCursor();
    cursor.setPosition(position);
    m_outlineEditor->setTextCursor(cursor);
}

void KPrViewModeOutline::activateSynchronize()
{
    if (!(m_synchronizeActivated)) {
        // Enable synchronization
        connect(m_outlineEditor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(synchronize(int, int, int)));
    }
    m_synchronizeActivated = true;
}

void KPrViewModeOutline::deactivateSynchronize()
{
    // Disable synchronization
    disconnect(m_outlineEditor->document(), SIGNAL(contentsChange(int,int,int)), this, SLOT(synchronize(int, int, int)));
    m_synchronizeActivated = false;
}
