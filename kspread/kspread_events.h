#ifndef KSPREAD_EVENTS
#define KSPREAD_EVENTS

#include <qevent.h>
#include <qrect.h>
#include <qstring.h>

#include <string.h>

#include <kparts/event.h>
#include <koffice_export.h>

namespace KSpread
{

class KSPREAD_EXPORT SelectionChanged : public KParts::Event
{
public:
    SelectionChanged( const QRect&, const QString& sheet );
    ~SelectionChanged();

    QRect rect() const { return m_rect; }
    QString sheet() const { return m_sheet; }

    static bool test( const QEvent* e ) { return KParts::Event::test( e, s_strSelectionChanged ); }

private:
    static const char *s_strSelectionChanged;
    QRect m_rect;
    QString m_sheet;
};

} // namespace KSpread

#endif
