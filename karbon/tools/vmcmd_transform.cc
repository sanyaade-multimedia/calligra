/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <klocale.h>

#include "vmcmd_transform.h"
#include "vpath.h"

VMCmdTransform::VMCmdTransform( KarbonPart* part, QPtrList<VObject> objects, const QWMatrix &mat )
	: VCommand( part, i18n("Transform Object") ), m_objects( objects ), m_mat( mat )
{
}

VMCmdTransform::VMCmdTransform( KarbonPart* part, QPtrList<VObject> objects, const QString &name )
	: VCommand( part, name ), m_objects( objects )
{
}
void
VMCmdTransform::execute()
{
	QPtrListIterator<VObject> itr( m_objects );
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m_mat );
}

void
VMCmdTransform::unexecute()
{
	// inverting the matrix should undo the affine transformation
	QPtrListIterator<VObject> itr( m_objects );
	for ( ; itr.current() ; ++itr )
		itr.current()->transform( m_mat.invert() );
}

VMCmdTranslate::VMCmdTranslate( KarbonPart* part, QPtrList<VObject> objects, double d1, double d2 )
	: VMCmdTransform( part, objects, i18n("Translate Object") )
{
	m_mat.translate( d1, d2 );
}

VMCmdScale::VMCmdScale( KarbonPart* part, QPtrList<VObject> objects, const QPoint &p, double s1, double s2 )
	: VMCmdTransform( part, objects, i18n("Scale Object") )
{
	m_mat.translate( p.x(), p.y() );
	m_mat.scale( s1, s2 );
	m_mat.translate( -p.x(), -p.y() );
}

