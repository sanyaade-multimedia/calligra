/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

import QtQuick 1.1
import ":/components"

Item {
    id: base;
    property bool collapsed: true;

    DropShadow {
        id: panel;

        width: parent.width;
        height: parent.height;
        Rectangle {
            anchors.fill: parent;
            color: "white";
            clip: true;

            DropShadow {
                id: header;
                height: Constants.GridHeight;
                width: parent.width;
                z: 2;

                Rectangle {
                    anchors.fill: parent;

                    color: Constants.Theme.TertiaryColor;

                    Label {
                        anchors {
                            left: parent.left;
                            leftMargin: Constants.DefaultMargin;
                            verticalCenter: parent.verticalCenter;
                        }
                        text: "Create New";
                        font.pixelSize: Constants.LargeFontSize;
                        color: "white";
                    }
                }
            }

            NewImageList {
                anchors.top: header.bottom;
                width: parent.width;
                height: Constants.GridHeight * 6;
            }
        }
    }

    states: State {
        name: "collapsed";
        when: base.collapsed;
        PropertyChanges { target: panel; x: -base.width; opacity: 0; }
    }

    transitions: Transition {
        NumberAnimation { properties: "x,opacity"; }
    }
}