
#ifndef _FILTER_KWORD_1_3__KWORDFRAMESET_H
#define _FILTER_KWORD_1_3__KWORDFRAMESET_H

#include <qstring.h>
#include <qmap.h>

#include "kwordparagraph.h"

class KWordFrameset
{
public:
    KWordFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordFrameset( void );
public:
    virtual bool addParagraph(const KWordParagraph& para);
public:
    int m_numFrames; ///< Number of frames
    QMap<QString,QString> m_frameData; ///< Data of frames (### TODO: do better!)
protected:
    int m_frameType;
    int m_frameInfo;
    QString m_name; ///< Name of frameset
};

class KWordNormalTextFrameset : public KWordFrameset
{
public:
    KWordNormalTextFrameset( int frameType, int frameInfo, const QString& name );
    virtual ~KWordNormalTextFrameset( void );
public:
    virtual bool addParagraph(const KWordParagraph& para);
public:
    KWordParagraphGroup m_paragraphGroup;
};

#endif // _FILTER_KWORD_1_3__KWORDFRAMESET_H
