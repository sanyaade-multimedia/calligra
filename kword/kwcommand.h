/* This file is part of the KDE project
   Copyright (C) 2001 David Faure <faure@kde.org>

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

#ifndef KWORD_COMMAND_H
#define KWORD_COMMAND_H

#include <kcommand.h>
#include <koGlobal.h>
#include <koRect.h>
#include <koborder.h>
#include <koparaglayout.h>
#include <koparagcounter.h>
#include "kwvariable.h"
class KWFrameSet;
class KWTableFrameSet;
class KWDocument;
class KoCustomVariable;
class KoLinkVariable;
/**
 * Command created when pasting formatted text
 * [relies on KWord's XML structure, so not moved to kotext]
 */
class KWPasteTextCommand : public KoTextDocCommand
{
public:
    KWPasteTextCommand( KoTextDocument *d, int parag, int idx,
                    const QCString & data );
    ~KWPasteTextCommand() {}
    KoTextCursor *execute( KoTextCursor *c );
    KoTextCursor *unexecute( KoTextCursor *c );
protected:
    int m_parag;
    int m_idx;
    QCString m_data;
    // filled in by execute(), for unexecute()
    int m_lastParag;
    int m_lastIndex;
    KoParagLayout m_oldParagLayout;
};

////////////////////////// Frame commands ////////////////////////////////

// Identifies a frame
struct FrameIndex {
    FrameIndex() {}
    FrameIndex( KWFrame *frame );

    KWFrameSet * m_pFrameSet;
    unsigned int m_iFrameIndex;
};

enum FrameBorderType { FBLeft=0, FBRight=1, FBTop=2, FBBottom=3};

struct FrameBorderTypeStruct {
    FrameBorderType m_EFrameType;
    KoBorder m_OldBorder;
};

/**
 * Command created when changing frame border
 */
class KWFrameBorderCommand : public KNamedCommand
{
public:
    KWFrameBorderCommand( const QString &name, QPtrList<FrameIndex> &_listFrameIndex, QPtrList<FrameBorderTypeStruct> &_frameTypeBorder,const KoBorder & _newBorder ) ;
    ~ KWFrameBorderCommand() {}

    void execute();
    void unexecute();

protected:
    QPtrList<FrameIndex> m_indexFrame;
    QPtrList<FrameBorderTypeStruct> m_oldBorderFrameType;
    KoBorder m_newBorder;
};

/**
 * Command created when changing background color of one or more frames
 */
class KWFrameBackGroundColorCommand : public KNamedCommand
{
public:
    KWFrameBackGroundColorCommand( const QString &name, QPtrList<FrameIndex> &_listFrameIndex, QPtrList<QBrush> &_oldBrush, const QBrush & _newColor ) ;
    ~KWFrameBackGroundColorCommand() {}

    void execute();
    void unexecute();

protected:
    QPtrList<FrameIndex> m_indexFrame;
    QPtrList<QBrush> m_oldBackGroundColor;
    QBrush m_newColor;
};

struct FrameResizeStruct {
    KoRect sizeOfBegin;
    KoRect sizeOfEnd;
};

/**
 * Command created when a frame is resized
 */
class KWFrameResizeCommand : public KNamedCommand
{
public:
    KWFrameResizeCommand( const QString &name, FrameIndex _frameIndex, FrameResizeStruct _frameResize ) ;
    ~KWFrameResizeCommand() {}

    void execute();
    void unexecute();

protected:
    FrameIndex m_indexFrame;
    FrameResizeStruct m_FrameResize;
};

/**
 * Command created when we changed a clipart or picture
 */
class KWFrameChangePictureClipartCommand : public KNamedCommand
{
public:
    KWFrameChangePictureClipartCommand( const QString &name, FrameIndex _frameIndex, const QString & _oldFile, const QString &_newFile, bool _isAPicture) ;
    ~KWFrameChangePictureClipartCommand() {}

    void execute();
    void unexecute();

protected:
    FrameIndex m_indexFrame;
    QString m_oldFile;
    QString m_newFile;
    bool m_isAPicture;
};

/**
 * Command created when one or more frames are moved
 */
class KWFrameMoveCommand : public KNamedCommand
{
public:
    KWFrameMoveCommand( const QString &name,QPtrList<FrameIndex> &_frameIndex,QPtrList<FrameResizeStruct>&_frameMove ) ;
    ~KWFrameMoveCommand() {}

    void execute();
    void unexecute();
    QPtrList<FrameResizeStruct> & listFrameMoved() { return m_frameMove; }
protected:
    QPtrList<FrameIndex> m_indexFrame;
    QPtrList<FrameResizeStruct> m_frameMove;
};

/**
 * Command created when the properties of a frame are changed
 * (e.g. using frame dialog).
 * In the long run, KWFrameBackGroundColorCommand, KWFrameBorderCommand etc.
 * could be removed and KWFramePropertiesCommand could be used instead.
 * #### This solution is memory eating though, since all settings of the frame
 * are copied. TODO: evaluate using graphite's GenericCommand instead.
 */
class KWFramePropertiesCommand : public KNamedCommand
{
public:
    /** A copy of frameAfter is done internally.
     * But make sure frameBefore is already a copy, its ownership is moved to the command.
     */
    KWFramePropertiesCommand( const QString &name, KWFrame *_frameBefore,  KWFrame *_frameAfter );
    ~KWFramePropertiesCommand();
    void execute();
    void unexecute();
protected:
    FrameIndex m_frameIndex;
    KWFrame *m_frameBefore;
    KWFrame *m_frameAfter;
};


/**
 * Command created when one part is moved or resized
 */
class KWFramePartMoveCommand : public KNamedCommand
{
public:
    KWFramePartMoveCommand( const QString &name,FrameIndex _frameIndex,FrameResizeStruct _frameMove ) ;
    ~KWFramePartMoveCommand() {}

    void execute();
    void unexecute();
    FrameResizeStruct & listFrameMoved() { return m_frameMove; }
    bool frameMoved();
protected:
    FrameIndex m_indexFrame;
    FrameResizeStruct m_frameMove;
};

/**
 * This command changes one property of a frameset.
 */
class KWFrameSetPropertyCommand : public KNamedCommand
{
public:
    enum Property { FSP_NAME, FSP_FLOATING, FSP_KEEPASPECTRATION, FSP_PROTECTSIZE };
    KWFrameSetPropertyCommand( const QString &name, KWFrameSet *frameset,
		               Property prop, const QString & value );
    ~KWFrameSetPropertyCommand() {}

    void execute();
    void unexecute();

protected:
    void setValue( const QString &value );

    KWFrameSet* m_pFrameSet;
    Property m_property;
    QString m_value;
    QString m_oldValue;
};

///////////////////////////////layout command///////////////////////////
struct pageLayout {
    KoPageLayout _pgLayout;
    KoColumns _cl;
    KoKWHeaderFooter _hf;
};

/**
 * Command created when changing the page layout
 */
class KWPageLayoutCommand : public KNamedCommand
{
public:
    KWPageLayoutCommand( const QString &name, KWDocument *_doc, pageLayout &_oldLayout, pageLayout &_newLayout);
    ~KWPageLayoutCommand() {}

    void execute();
    void unexecute();
protected:
    KWDocument *m_pDoc;
    pageLayout m_OldLayout;
    pageLayout m_NewLayout;
};


/**
 * Command created when deleting a frame
 */
class KWDeleteFrameCommand : public KNamedCommand
{
public:
    KWDeleteFrameCommand( const QString &name, KWFrame * frame) ;
    ~KWDeleteFrameCommand();

    void execute();
    void unexecute();
protected:
    FrameIndex m_frameIndex;
    KWFrame *m_copyFrame;
};

/**
 * Command created when creating a frame
 */
class KWCreateFrameCommand : public KWDeleteFrameCommand
{
public:
    KWCreateFrameCommand( const QString &name, KWFrame * frame);
    ~KWCreateFrameCommand() {}

    void execute() { KWDeleteFrameCommand::unexecute(); }
    void unexecute() { KWDeleteFrameCommand::execute(); }
};

/**
 * Command created when ungrouping a table
 */
class KWUngroupTableCommand : public KNamedCommand
{
public:
    KWUngroupTableCommand( const QString &name, KWTableFrameSet * _table) ;
    ~KWUngroupTableCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QPtrList<KWFrameSet> m_ListFrame;
};

/**
 * Command created when deleting a table
 */
class KWDeleteTableCommand : public KNamedCommand
{
public:
    KWDeleteTableCommand( const QString &name, KWTableFrameSet * _table) ;
    ~KWDeleteTableCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
};


/**
 * Command created when creating a table
 */
class KWCreateTableCommand : public KWDeleteTableCommand
{
public:
    KWCreateTableCommand( const QString &name, KWTableFrameSet * _table)
        : KWDeleteTableCommand( name, _table ) {}
    ~KWCreateTableCommand() {}

    void execute() { KWDeleteTableCommand::unexecute(); }
    void unexecute() { KWDeleteTableCommand::execute(); }
};

/**
 * Command created when inserting a column
 */
class KWInsertColumnCommand : public KNamedCommand
{
public:
    /* for the last parameter, _maxRight, you should pass the maximum offset that the table can use at its right (normally m_maxRight - m_pTable->boundingRect().left())*/
    KWInsertColumnCommand( const QString &name, KWTableFrameSet * _table, int _pos, double _maxRight);
    ~KWInsertColumnCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QPtrList<KWFrameSet> m_ListFrameSet;
    unsigned int m_colPos;
    double m_maxRight; // this is the maximum x of the right part of the table (used so that the table does no go off the page)
    double m_oldWidth; // will be 0 after execute() if the width of the table was not changed by the operation
};


/**
 * Command created when inserting a row
 */
class KWInsertRowCommand : public KNamedCommand
{
public:
    KWInsertRowCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWInsertRowCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QPtrList<KWFrameSet> m_ListFrameSet;
    unsigned int m_rowPos;
};

/**
 * Command created when removing a row
 */
class KWRemoveRowCommand : public KNamedCommand
{
public:
    KWRemoveRowCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWRemoveRowCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QPtrList<KWFrameSet> m_ListFrameSet;
    QPtrList<KWFrame> m_copyFrame;
    unsigned int m_rowPos;
};


/**
 * Command created when removing a column
 */
class KWRemoveColumnCommand : public KNamedCommand
{
public:
    KWRemoveColumnCommand( const QString &name, KWTableFrameSet * _table, int _pos);
    ~KWRemoveColumnCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    QPtrList<KWFrameSet> m_ListFrameSet;
    QPtrList<KWFrame> m_copyFrame;
    unsigned int m_colPos;
};

/**
 * Command created when splitting a cell
 */
class KWSplitCellCommand : public KNamedCommand
{
public:
    KWSplitCellCommand( const QString &name, KWTableFrameSet * _table,unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd );
    ~KWSplitCellCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    unsigned int m_colBegin;
    unsigned int m_rowBegin;
    unsigned int m_colEnd;
    unsigned int m_rowEnd;
    QPtrList<KWFrameSet> m_ListFrameSet;
};

/**
 * Command created when joining cells
 */
class KWJoinCellCommand : public KNamedCommand
{
public:
    KWJoinCellCommand( const QString &name, KWTableFrameSet * _table,unsigned int colBegin,unsigned int rowBegin, unsigned int colEnd,unsigned int rowEnd, QPtrList<KWFrameSet> listFrameSet,QPtrList<KWFrame> listCopyFrame);
    ~KWJoinCellCommand() {}

    void execute();
    void unexecute();
protected:
    KWTableFrameSet *m_pTable;
    unsigned int m_colBegin;
    unsigned int m_rowBegin;
    unsigned int m_colEnd;
    unsigned int m_rowEnd;
    QPtrList<KWFrameSet> m_ListFrameSet;
    QPtrList<KWFrame> m_copyFrame;
};

/**
 * Command to starting page setting
 */
class KWChangeStartingPageCommand : public KNamedCommand
{
public:
    KWChangeStartingPageCommand( const QString &name, KWDocument *_doc, int _oldStartingPage, int _newStartingPage);
    ~KWChangeStartingPageCommand(){}

    void execute();
    void unexecute();
protected:
    KWDocument *m_doc;
    int oldStartingPage;
    int newStartingPage;
};

/**
 * Command to display link setting
 */
class KWChangeVariableSettingsCommand : public KNamedCommand
{
public:
    enum VariableProperties { VS_DISPLAYLINK, VS_UNDERLINELINK, VS_DISPLAYCOMMENT};
    KWChangeVariableSettingsCommand( const QString &name, KWDocument *_doc, bool _oldDisplay, bool _newDisplay, VariableProperties _type);
    ~KWChangeVariableSettingsCommand(){}

    void execute();
    void unexecute();
protected:
    void changeValue( bool b );
    KWDocument *m_doc;
    VariableProperties type;
    bool m_bOldValue;
    bool m_bNewValue;
};

class KWChangeCustomVariableValue : public KNamedCommand
{
 public:
    KWChangeCustomVariableValue( const QString &name, KWDocument *_doc,const QString & _oldValue, const QString & _newValue, KoCustomVariable *var);
    ~KWChangeCustomVariableValue();
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    QString newValue;
    QString oldValue;
    KoCustomVariable *m_var;
};

class KWChangeLinkVariable : public KNamedCommand
{
 public:
    KWChangeLinkVariable( const QString &name, KWDocument *_doc,const QString & _oldHref, const QString & _newHref, const QString & _oldLink,const QString &_newLink, KoLinkVariable *var);
    ~KWChangeLinkVariable(){};
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    QString oldHref;
    QString newHref;
    QString oldLink;
    QString newLink;
    KoLinkVariable *m_var;
};

class KWHideShowHeader : public KNamedCommand
{
 public:
    KWHideShowHeader( const QString &name, KWDocument *_doc, bool _newValue);
    ~KWHideShowHeader(){};
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    bool newValue;
};

class KWHideShowFooter : public KNamedCommand
{
 public:
    KWHideShowFooter( const QString &name, KWDocument *_doc, bool _newValue);
    ~KWHideShowFooter(){};
    void execute();
    void unexecute();
 protected:
    KWDocument *m_doc;
    bool newValue;
};


class KWProtectContentCommand : public KNamedCommand
{
public:
    KWProtectContentCommand( const QString &name, KWTextFrameSet*frameset,bool protect );
    ~KWProtectContentCommand() {}

    void execute();
    void unexecute();

protected:
    KWTextFrameSet* m_pFrameSet;
    bool m_bProtect;
};

struct FrameMarginsStruct {
    FrameMarginsStruct() {}
    FrameMarginsStruct( KWFrame *frame );
    FrameMarginsStruct( double _left, double top, double right, double bottom );
    double topMargin;
    double bottomMargin;
    double leftMargin;
    double rightMargin;
};

class KWFrameChangeFrameMarginCommand :public KNamedCommand
{
public:
    KWFrameChangeFrameMarginCommand( const QString &name, FrameIndex _frameIndex, FrameMarginsStruct _frameMarginsBegin, FrameMarginsStruct _frameMarginsEnd );
    ~KWFrameChangeFrameMarginCommand() {}
    void execute();
    void unexecute();
protected:
    FrameIndex m_indexFrame;
    FrameMarginsStruct m_frameMarginsBegin;
    FrameMarginsStruct m_frameMarginsEnd;
};


class KWChangeFootEndNoteSettingsCommand : public KNamedCommand
{
public:
    KWChangeFootEndNoteSettingsCommand( const QString &name, KoParagCounter _oldCounter, KoParagCounter _newCounter ,bool _footNote ,KWDocument *_doc );
    ~KWChangeFootEndNoteSettingsCommand() {}

    void execute();
    void unexecute();

protected:
    void changeCounter( KoParagCounter counter);
    KoParagCounter m_oldCounter;
    KoParagCounter m_newCounter;
    bool m_footNote;
    KWDocument *m_doc;
};

class KWChangeTabStopValueCommand : public KNamedCommand
{
public:
    KWChangeTabStopValueCommand( const QString &name,  double _oldValue, double _newValue, KWDocument *_doc);

    ~KWChangeTabStopValueCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    KWDocument *m_doc;
    double m_oldValue;
    double m_newValue;
};


struct FootNoteParameter {
    FootNoteParameter() {}
    FootNoteParameter( KWFootNoteVariable *_var );
    FootNoteParameter( NoteType _noteType, KWFootNoteVariable::Numbering _numberingType, const QString &_manualString);
    NoteType noteType;
    KWFootNoteVariable::Numbering numberingType;
    QString manualString;
};

class KWChangeFootNoteParametersCommand : public KNamedCommand
{
public:
    KWChangeFootNoteParametersCommand( const QString &name, KWFootNoteVariable * _var, FootNoteParameter _oldParameter, FootNoteParameter _newParameter, KWDocument *_doc);

    ~KWChangeFootNoteParametersCommand() {}

    virtual void execute();
    virtual void unexecute();
protected:
    void changeVariableParameter( FootNoteParameter _param );
    KWDocument *m_doc;
    KWFootNoteVariable *m_var;
    FootNoteParameter m_oldParameter;
    FootNoteParameter m_newParameter;
};

#endif
