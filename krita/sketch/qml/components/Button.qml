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

Item {
    id: base;

    signal clicked();

    property bool checkable: false;
    property bool checked: false;

    property alias image: icon.source;
    property color color: "white";
    property alias text: label.text;
    property alias textColor: label.color;
    property alias textSize: label.font.pixelSize;
    property bool shadow: true;

    property bool highlight: true;
    property color highlightColor: color;

    width: Constants.GridWidth;
    height: Constants.GridHeight;

    DropShadow {
        id: shadow;
        anchors.fill: parent;
        anchors.margins: Constants.DefaultMargin;
        size: base.shadow ? Constants.DefaultMargin : 0;

        Rectangle {
            id: fill;
            anchors.fill: parent;
            anchors.margins: -1;

            color: base.state == "pressed" && base.highlight ? base.highlightColor : base.color;
            Behavior on color { ColorAnimation { duration: 50; } }

            Image {
                id: icon;
                anchors.fill: parent;
                anchors.margins: Constants.DefaultMargin;
                fillMode: Image.PreserveAspectFit;
                smooth: true;
            }

            Label {
                id: label;
                anchors.centerIn: parent;
            }
        }
    }

    MouseArea {
        id: mouse;
        anchors.fill: parent;
        onClicked: { base.clicked(); if( base.checkable ) base.checked = !base.checked; }
    }

    states: State {
        name: "pressed";
        when: mouse.pressed || base.checked;

        PropertyChanges { target: shadow; size: Constants.DefaultMargin * 0.333; }
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { properties: "size"; duration: 50; }
        }
    }
}