/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2001       Sven Leiber         <s.leiber@web.de>

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

#ifndef koautoformat_h
#define koautoformat_h

#include <qstring.h>
#include <qcolor.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <qptrvector.h>

class KoDocument;
class KoTextParag;
class KoTextObject;
class KoVariableCollection;
class KoVariableFormatCollection;

namespace Qt3 {
    class QTextCursor;
}
using namespace Qt3;

/******************************************************************/
/* Class: KWAutoFormatEntry					  */
/******************************************************************/
class KoAutoFormatEntry
{
public:
    // The text to find is actually the key in KWAutoFormat's map.
    // What we replace it with is replace().
    KoAutoFormatEntry(const QString& replace = QString::null)
        : m_replace( replace ) {}

    QString replace() const { return m_replace; }

protected:
    QString m_replace;
    // For formatting in the replacement - not implemented yet
    //KWSearchContext m_formatOptions;
};

/******************************************************************/
/* Class: KoAutoFormat						  */
/******************************************************************/
class KoAutoFormat
{
public:
    /**
     * There is a single instance of KoAutoFormat per document
     * (and a temporary one in the auto-format dialog).
     */
    KoAutoFormat( KoDocument *_doc, KoVariableCollection *_varCollection, KoVariableFormatCollection *_varFormatCollection );

    /**
     * Called by edit widget when a character (@p ch) has been inserted
     * into @p parag, at the given @p index.
     */
    void doAutoFormat( QTextCursor* cursor, KoTextParag *parag, int index, QChar ch,KoTextObject *txtObj );

    bool doIgnoreDoubleSpace( KoTextParag *parag, int index,QChar ch );

    /**
     * Helper method, returns the last word before parag,index
     */
    static QString getLastWord( KoTextParag *parag, int index );

    /**
     * Helper method, returns the last word before parag,index
     * different from getLastWord, because we test just space character
     * and not punctualtion character
     */
    static QString getWordAfterSpace( KoTextParag * parag, int index);

    // Config for the typographic quotes. Used by the dialog.
    struct TypographicQuotes
    {
	QChar begin, end;
	bool replace; // aka enabled
    };

    // Configuration (on/off/settings). Called by the dialog.
    void configTypographicDoubleQuotes( TypographicQuotes _tq );
    void configTypographicSimpleQuotes( TypographicQuotes _tq );

    void configUpperCase( bool _uc );
    void configUpperUpper( bool _uu );
    void configAdvancedAutocorrect( bool _aa );
    void configAutoDetectUrl(bool _au);
    void configIgnoreDoubleSpace( bool _ids);
    void configRemoveSpaceBeginEndLine( bool _space);
    void configUseBulletStyle( bool _ubs);

    void configBulletStyle( QChar b );

    void configAutoChangeFormat( bool b);

    void configAutoReplaceNumber( bool b );

    void configAutoNumberStyle( bool b );

    TypographicQuotes getConfigTypographicSimpleQuotes() const
    { return m_typographicSimpleQuotes; }

    TypographicQuotes getConfigTypographicDoubleQuotes() const
    { return m_typographicDoubleQuotes; }

    bool getConfigUpperCase() const
    { return m_convertUpperCase; }
    bool getConfigUpperUpper() const
    { return m_convertUpperUpper; }
    bool getConfigAdvancedAutoCorrect() const
    { return m_advancedAutoCorrect;}
    bool getConfigAutoDetectUrl() const
    { return m_autoDetectUrl;}

    bool getConfigIgnoreDoubleSpace() const
    { return m_ignoreDoubleSpace;}

    bool getConfigRemoveSpaceBeginEndLine() const
    { return m_removeSpaceBeginEndLine;}

    bool getConfigUseBulletSyle() const
    { return m_useBulletStyle;}

    QChar getConfigBulletStyle() const
    { return bulletStyle; }

    bool getConfigAutoChangeFormat() const
    { return m_autoChangeFormat;}

    bool getConfigAutoReplaceNumber() const
    { return m_autoReplaceNumber; }

    bool getConfigAutoNumberStyle() const
    { return m_useAutoNumberStyle; }

    // Add/remove entries, called by the dialog
    void addAutoFormatEntry( const QString &key, const KoAutoFormatEntry &entry ) {
	m_entries.insert( key, entry );
	buildMaxLen();
    }

    void removeAutoFormatEntry( const QString &key ) {
        m_entries.remove( key );
	buildMaxLen();
    }

    // Iterate over the entries. Called by the dialog
    QMap< QString, KoAutoFormatEntry >::Iterator firstAutoFormatEntry()
    { return m_entries.begin(); }

    QMap< QString, KoAutoFormatEntry >::Iterator lastAutoFormatEntry()
    { return m_entries.end(); }

    QMap< QString, KoAutoFormatEntry >::Iterator findFormatEntry(QString find)
    { return m_entries.find(find); }

    // Copy all autoformat entries from another KWAutoFormat. Called by the dialog
    void copyAutoFormatEntries( const KoAutoFormat & other )
    { m_entries = other.m_entries; }

    void copyListException( const QStringList & _list)
	{ upperCaseExceptions=_list;}

    void copyListTwoUpperCaseException( const QStringList &_list)
	{ twoUpperLetterException=_list; }

    QStringList listException() {return upperCaseExceptions;}

    QStringList listTwoUpperLetterException() {return twoUpperLetterException;}

    // Read/save config ( into kwordrc )
    void readConfig();
    void saveConfig();

    static bool isUpper( const QChar &c );
    static bool isLower( const QChar &c );
    static bool isMark( const QChar &c ); // End of sentence
    static bool isSeparator( const QChar &c );

protected:
    bool doAutoCorrect( QTextCursor* textEditCursor, KoTextParag *parag, int index, KoTextObject *txtObj );
    void doUpperCase( QTextCursor* textEditCursor, KoTextParag *parag, int index, const QString & word , KoTextObject *txtObj );
    void doTypographicQuotes( QTextCursor* textEditCursor, KoTextParag *parag, int index, KoTextObject *txtObj, bool doubleQuotes );
    void buildMaxLen();

    void doAutoDetectUrl( QTextCursor *textEditCursor, KoTextParag *parag,int index, const QString & word, KoTextObject *txtObj );
    void doRemoveSpaceBeginEndLine( QTextCursor *textEditCursor, KoTextParag *parag, KoTextObject *txtObj );
    void doAutoChangeFormat( QTextCursor *textEditCursor, KoTextParag *parag,int index, const QString & word, KoTextObject *txtObj );
    void doUseBulletStyle(QTextCursor *textEditCursor, KoTextParag *parag, KoTextObject *txtObj );

    void doAutoReplaceNumber( QTextCursor* textEditCursor, KoTextParag *parag, int index, const QString & word , KoTextObject *txtObj );

    void doUseNumberStyle(QTextCursor * /*textEditCursor*/, KoTextParag *parag, KoTextObject *txtObj );

private:
    void detectStartOfLink(const QString &word);

    KoDocument *m_doc;
    KoVariableCollection *m_varCollection;
    KoVariableFormatCollection *m_varFormatCollection;

    bool m_configRead;
    bool m_convertUpperCase, m_convertUpperUpper,m_advancedAutoCorrect;
    bool m_autoDetectUrl, m_ignoreDoubleSpace, m_removeSpaceBeginEndLine;
    bool m_useBulletStyle, m_autoChangeFormat, m_autoReplaceNumber;
    bool m_useAutoNumberStyle;

    QChar bulletStyle;

    TypographicQuotes m_typographicSimpleQuotes;

    TypographicQuotes m_typographicDoubleQuotes;


    typedef QMap< QString, KoAutoFormatEntry > KoAutoFormatEntryMap;
    KoAutoFormatEntryMap m_entries;
    QStringList upperCaseExceptions;
    QStringList twoUpperLetterException;
    uint m_maxlen;
    uint m_maxFindLength;
    bool m_ignoreUpperCase;
};

#endif
