/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002   Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIPROJECTPARTPROXY_H
#define KEXIPROJECTPARTPROXY_H

#include <qobject.h>
#include <qguardedptr.h>

#include "kexiprojecthandler.h"
#include "kexiproject.h"
#include "kexihandlerpopupmenu.h"
#include "kexiview.h"

class KexiProjectHandlerProxy : public QObject
{
	Q_OBJECT

	public:
		KexiProjectHandlerProxy(KexiProjectHandler *part, KexiView *view);
		virtual ~KexiProjectHandlerProxy() {};

		virtual KexiPartPopupMenu	*groupContext() = 0;
		virtual KexiPartPopupMenu	*itemContext(const QString &identifier) = 0;

		KexiView *kexiView();
		KexiProjectHandler *part();

		virtual void executeItem(const QString& identifier)=0;
	protected:
		QGuardedPtr<KexiView> m_view;
		KexiProjectHandler *m_part;
		
};

#endif
