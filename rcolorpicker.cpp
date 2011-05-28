/***************************************************************************
 *   QtReport                                                              *
 *   Qt Report Builder Soultion                                            *
 *                                                                         * 
 *   Copyright (C) 2010 by Hamed Masafi                                    *
 *   Hamed.Masafi@GMail.COM                                                *
 *   Copyright (C) 2011 by Alexander Ankudinov                             *
 *   xelez0@gmail.com                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include <QPainter>
#include <QStyleOptionButton>

#include "rcolorpicker.h"

RColorPicker::RColorPicker(QWidget *parent) :
    QPushButton( parent )
{
   popup = new RColorPickerPopup( 0 );
   popup->setObjectName( "popup" );
   popup->setFrameShape(QFrame::StyledPanel);
   popup->setFrameShadow(QFrame::Plain);

   connect( popup, SIGNAL(selected(QColor)),
            this,  SLOT(on_popup_selected(QColor)) );

   setMinimumWidth( 100 );
   _selectedColor = Qt::black;

   QMetaObject::connectSlotsByName( this );
}


void RColorPicker::paintEvent ( QPaintEvent * )
{
   QStyleOptionButton option;
   option.initFrom( this );

   option.features = QStyleOptionButton::HasMenu;

   QPixmap icon( 16, 16 );
   icon.fill(_selectedColor);

   option.text = _selectedColor.name();
   option.iconSize = QSize(16,16);
   option.icon = QIcon(icon);

   QPainter painter ( this );


   style()->drawControl( QStyle::CE_PushButton, &option, &painter, this );
}


void RColorPicker::mouseReleaseEvent ( QMouseEvent * )
{
   popup->move(  this->mapToGlobal( QPoint(0, this->height()) ) );
   popup->setWindowFlags( Qt::Popup );
   popup->show();
}


void RColorPicker::on_popup_selected( QColor color )
{
   _selectedColor = color;
   repaint();
}
