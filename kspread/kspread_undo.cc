/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

// #include <qprinter.h>

#include "kspread_undo.h"
#include "kspread_table.h"
#include "kspread_view.h"
#include "kspread_doc.h"
#include "kspread_map.h"

#include <qcstring.h>
#include <qtextstream.h>

/****************************************************************************
 *
 * KSpreadUndo
 *
 ***************************************************************************/

KSpreadUndo::KSpreadUndo( KSpreadDoc *_doc )
{
    m_pDoc = _doc;

    m_stckUndo.setAutoDelete( FALSE );
    m_stckRedo.setAutoDelete( FALSE );

    m_bLocked = FALSE;
}

KSpreadUndo::~KSpreadUndo()
{
    clear();
}

void KSpreadUndo::appendUndo( KSpreadUndoAction *_action )
{
    if ( m_bLocked )
	return;

    m_stckRedo.setAutoDelete( TRUE );
    m_stckRedo.clear();
    m_stckRedo.setAutoDelete( FALSE );

    m_stckUndo.push( _action );

    if ( m_pDoc )
    {
	m_pDoc->enableUndo( hasUndoActions() );
	m_pDoc->enableRedo( hasRedoActions() );
    }
}

void KSpreadUndo::clear()
{
    if ( m_bLocked )
	return;

    m_stckUndo.setAutoDelete( TRUE );
    m_stckRedo.setAutoDelete( TRUE );

    m_stckUndo.clear();
    m_stckRedo.clear();

    m_stckUndo.setAutoDelete( FALSE );
    m_stckRedo.setAutoDelete( FALSE );
}

void KSpreadUndo::undo()
{
    if ( m_stckUndo.isEmpty() )
	return;
    KSpreadUndoAction *a = m_stckUndo.pop();
    a->undo();
    m_stckRedo.push( a );

    if ( m_pDoc )
    {
	m_pDoc->enableUndo( hasUndoActions() );
	m_pDoc->enableRedo( hasRedoActions() );
    }
}

void KSpreadUndo::redo()
{
    if ( m_stckRedo.isEmpty() )
	return;
    KSpreadUndoAction *a = m_stckRedo.pop();
    a->redo();
    m_stckUndo.push( a );

    if ( m_pDoc )
    {
	m_pDoc->enableUndo( hasUndoActions() );
	m_pDoc->enableRedo( hasRedoActions() );
    }
}

/****************************************************************************
 *
 * KSpreadUndoRemoveColumn
 *
 ***************************************************************************/

KSpreadUndoRemoveColumn::KSpreadUndoRemoveColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column ) :
    KSpreadUndoAction( _doc )
{
    m_tableName = _table->name();
    m_iColumn= _column;
    
    QRect selection;
    selection.setCoords( _column, 0, _column, 0x7fff );
    QDomDocument doc = _table->saveCellRect( selection );
    
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    m_data = buffer.utf8();
}

KSpreadUndoRemoveColumn::~KSpreadUndoRemoveColumn()
{
}

void KSpreadUndoRemoveColumn::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;
    
    doc()->undoBuffer()->lock();

    table->insertColumn( m_iColumn);

    table->paste( m_data, QPoint( m_iColumn, 1 ) );
    table->recalc( true );

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoRemoveColumn::redo()
{
    doc()->undoBuffer()->lock();

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    table->removeColumn( m_iColumn );
    
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertColumn
 *
 ***************************************************************************/

KSpreadUndoInsertColumn::KSpreadUndoInsertColumn( KSpreadDoc *_doc, KSpreadTable *_table, int _column ) :
    KSpreadUndoAction( _doc )
{
    m_tableName = _table->name();
    m_iColumn= _column;
}

KSpreadUndoInsertColumn::~KSpreadUndoInsertColumn()
{
}

void KSpreadUndoInsertColumn::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->removeColumn( m_iColumn );
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoInsertColumn::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->insertColumn( m_iColumn);
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoRemoveRow
 *
 ***************************************************************************/

KSpreadUndoRemoveRow::KSpreadUndoRemoveRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row ) :
    KSpreadUndoAction( _doc )
{
    m_tableName = _table->name();
    m_iRow = _row;
    
    QRect selection;
    selection.setCoords( 0, _row, 0x7fff, _row );
    QDomDocument doc = _table->saveCellRect( selection );
    
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    m_data = buffer.utf8();
}

KSpreadUndoRemoveRow::~KSpreadUndoRemoveRow()
{
}

void KSpreadUndoRemoveRow::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;
    
    doc()->undoBuffer()->lock();

    table->insertRow( m_iRow );

    table->paste( m_data, QPoint( 1, m_iRow ) );
    table->recalc( true );

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoRemoveRow::redo()
{
    doc()->undoBuffer()->lock();

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    table->removeRow( m_iRow );
    
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoInsertRow
 *
 ***************************************************************************/

KSpreadUndoInsertRow::KSpreadUndoInsertRow( KSpreadDoc *_doc, KSpreadTable *_table, int _row ) :
    KSpreadUndoAction( _doc )
{
    m_tableName = _table->name();
    m_iRow = _row;
}

KSpreadUndoInsertRow::~KSpreadUndoInsertRow()
{
}

void KSpreadUndoInsertRow::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->removeRow( m_iRow );
    doc()->undoBuffer()->unlock();
}

void KSpreadUndoInsertRow::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();
    table->insertRow( m_iRow );
    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoSetText
 *
 ***************************************************************************/

KSpreadUndoSetText::KSpreadUndoSetText( KSpreadDoc *_doc, KSpreadTable *_table, const QString& _text, int _column, int _row ) :
    KSpreadUndoAction( _doc )
{
    m_strText = _text;
    m_iColumn= _column;
    m_iRow = _row;
    m_tableName = _table->name();
}

KSpreadUndoSetText::~KSpreadUndoSetText()
{
}

void KSpreadUndoSetText::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    KSpreadCell *cell = table->nonDefaultCell( m_iColumn, m_iRow );
    m_strRedoText = cell->text();
    if ( m_strText.isNull() )
	cell->setCellText( "" );
    else
	cell->setCellText( m_strText );

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoSetText::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    KSpreadCell *cell = table->nonDefaultCell( m_iColumn, m_iRow );
    m_strText = cell->text();
    if ( m_strRedoText.isNull() )
	cell->setCellText( "" );
    else
	cell->setCellText( m_strRedoText );

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoSetTableName
 *
 ***************************************************************************/

KSpreadUndoSetTableName::KSpreadUndoSetTableName( KSpreadDoc *doc, KSpreadTable *table, const QString& name ) :
    KSpreadUndoAction( doc )
{
    m_name = name;
    m_tableName = table->name();
}

KSpreadUndoSetTableName::~KSpreadUndoSetTableName()
{
}

void KSpreadUndoSetTableName::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    m_redoName = table->tableName();

    table->setTableName( m_name );

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoSetTableName::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    table->setTableName( m_redoName );

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoCellLayout
 *
 ***************************************************************************/

KSpreadUndoCellLayout::KSpreadUndoCellLayout( KSpreadDoc *_doc, KSpreadTable *_table, QRect &_selection ) :
    KSpreadUndoAction( _doc )
{
  m_rctRect = _selection;
  m_tableName = _table->name();
  m_lstLayouts.setAutoDelete( TRUE );

  copyLayout( m_lstLayouts, _table );
}

void KSpreadUndoCellLayout::copyLayout( QList<KSpreadLayout> &list, KSpreadTable* table )
{
    list.clear();

    for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
	for ( int x = m_rctRect.left(); x <= m_rctRect.right(); x++ )
	{
	    KSpreadLayout *l = new KSpreadLayout( table );
	    l->copy( *(table->cellAt( x, y )) );
	    list.append( l );
	}
}

KSpreadUndoCellLayout::~KSpreadUndoCellLayout()
{
}

void KSpreadUndoCellLayout::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    copyLayout( m_lstRedoLayouts, table );

    KSpreadLayout *l;
    l = m_lstLayouts.first();

    for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
	for ( int x = m_rctRect.left(); x <= m_rctRect.right(); x++ )
	{
	    KSpreadCell *cell = table->nonDefaultCell( x, y );
	    cell->copy( *l );
	    cell->setLayoutDirtyFlag();
	    cell->setDisplayDirtyFlag();
	    table->updateCell( cell, x, y );	
	    l = m_lstLayouts.next();
	}

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoCellLayout::redo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    doc()->undoBuffer()->lock();

    KSpreadLayout *l;
    l = m_lstRedoLayouts.first();

    for ( int y = m_rctRect.top(); y <= m_rctRect.bottom(); y++ )
	for ( int x = m_rctRect.left(); x <= m_rctRect.right(); x++ )
	{
	    KSpreadCell *cell = table->nonDefaultCell( x, y );
	    cell->copy( *l );
	    cell->setLayoutDirtyFlag();
	    cell->setDisplayDirtyFlag();
	    table->updateCell( cell, x, y );
	    l = m_lstRedoLayouts.next();
	}

    doc()->undoBuffer()->unlock();
}

/****************************************************************************
 *
 * KSpreadUndoDelete
 *
 ***************************************************************************/

KSpreadUndoDelete::KSpreadUndoDelete( KSpreadDoc *_doc, KSpreadTable* table, QRect & _selection)
    : KSpreadUndoAction( _doc )
{
    m_tableName = table->name();
    m_selection = _selection;
    
    QDomDocument doc = table->saveCellRect( _selection );
    
    // Save to buffer
    QString buffer;
    QTextStream str( &buffer, IO_WriteOnly );
    str << doc;

    m_data = buffer.utf8();
}

KSpreadUndoDelete::~KSpreadUndoDelete()
{
}

void KSpreadUndoDelete::undo()
{
    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;
    
    doc()->undoBuffer()->lock();

    table->paste( m_data, m_selection.topLeft() );
    table->recalc( true );

    doc()->undoBuffer()->unlock();
}

void KSpreadUndoDelete::redo()
{
    doc()->undoBuffer()->lock();

    KSpreadTable* table = doc()->map()->findTable( m_tableName );
    if ( !table )
	return;

    table->deleteCells( m_selection );
    
    doc()->undoBuffer()->unlock();
}
