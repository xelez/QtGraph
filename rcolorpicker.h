/***************************************************************************
 *   QtReport                                                              *
 *   Qt Report Builder Soultion                                            *
 *                                                                         * 
 *   Copyright (C) 2010 by Hamed Masafi                                    *
 *   Hamed.Masafi@GMail.COM                                                *
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


#ifndef RCOLORPICKER_H
#define RCOLORPICKER_H

#include <QFrame>
#include <QPushButton>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QColor>
#include <QRectF>
#include <QColorDialog>
#include <QHash>
#include <QApplication>

#define COLORSIZE 20
#define COLS 11
#define SPACE 4
#define MARGINY 19
#define MARGINX 4
class ColorKeyInfo
{
public:

   QColor color;
   QRectF rect;

   ColorKeyInfo( QColor color, QRectF rect )
   {
      this->color = color;
      this->rect = rect;
   }
};

class RColorPickerPopup : public QFrame
{
   Q_OBJECT

   QColor hoverColor;
   QStringList colors;
   QList<ColorKeyInfo> colorInfos;
   QRectF buttonKeyRect;

signals:
   void selected( QColor color );

public:

   RColorPickerPopup( QWidget *parent ) : QFrame( parent, Qt::Popup )
   {
      this->setWindowFlags( Qt::Popup );
      colors = QColor::colorNames();

      int width = COLS;
      int height = (int)(colors.count() / COLS);

      height++;

      width = width * COLORSIZE + SPACE + MARGINX + 4;
      height = height * COLORSIZE + SPACE + MARGINY + 32;

      setFrameShape(QFrame::StyledPanel);
      setFrameShadow(QFrame::Plain);
      resize( width, height  );

      setMouseTracking( true );
      this->setStyleSheet( "QFrame{background-color: rgb(255, 255, 255);border-color: rgb(0, 0, 0);}" );
   }

   void paintButton( QPainter *painter, QRectF rc, QString text, bool hover)
   {
      if( !hover )
      {
         painter->setPen( Qt::black );
         painter->fillRect( rc, Qt::lightGray );
      }
      else
      {
         painter->setPen( Qt::blue );
         painter->fillRect( rc, Qt::yellow );
      }

      painter->drawRect( rc );
      painter->drawText( rc, text, Qt::AlignHCenter | Qt::AlignVCenter );
   }

   void paintEvent ( QPaintEvent * )
   {
      QPainter painter( this );


      painter.drawRect( 0, 0, width() - 1, height() - 1 );
      painter.drawText( QRect( 10, 1, this->width(), 19 ),
                         tr( "Standard" ),
                         Qt::AlignLeft | Qt::AlignVCenter );

      int col, row;

      colorInfos.clear();
      for( int i = 0; i < colors.count(); i++ )
      {
         QRectF rc;
         col = i % COLS;
         row = (int)(i / COLS);

         rc.setLeft( col * COLORSIZE + SPACE + MARGINX );
         rc.setTop( row * COLORSIZE + SPACE + MARGINY );

         rc.setWidth( COLORSIZE - SPACE );
         rc.setHeight( COLORSIZE - SPACE );

         QColor c( colors.at(i) );

         painter.fillRect( rc, QBrush( c ) );
         //colorRects[c] = rc;
         colorInfos.append( ColorKeyInfo(c, rc) );
      }//for

      QPointF mousePos = this->mapFromGlobal( QCursor::pos() );

      buttonKeyRect = QRect( SPACE,
                             (row + 1) * COLORSIZE + SPACE + MARGINY + 5,
                             this->width() - 2 * SPACE,
                             20 );
      paintButton(
            &painter,
            buttonKeyRect,
            tr( "More..." ),
            buttonKeyRect.contains( mousePos ) );

      //hoverColor = 0;

      for( int i = 0; i < colorInfos.count(); i++)
         if( colorInfos[i].rect.contains( mousePos ) )
         {
            QPainter painter( this );

            hoverColor = colorInfos[i].color;
            QRectF rc = colorInfos[i].rect;

            rc.setTop( rc.top() -2 );
            rc.setLeft( rc.left() -2 );
            rc.setBottom( rc.bottom() + 2 );
            rc.setRight( rc.right() + 2 );

            painter.setPen( QPen(Qt::blue, 2) );
            painter.drawRect( rc );
         }


   }

   void mouseMoveEvent ( QMouseEvent * )
   {
      this->repaint();
   }

   void mousePressEvent ( QMouseEvent *event )
   {
      if( !rect().contains( event->pos() ) ) this->close();

      for( int i = 0; i < colorInfos.count(); i++)
         if( colorInfos[i].rect.contains( event->pos() ) )
         {
            hoverColor = colorInfos.at(i).color;
            emit selected( hoverColor );
            this->close();
         }//for

      if( buttonKeyRect.contains( event->posF() ) )
      {
         QColorDialog *dialog = new QColorDialog( this );
         if( dialog->exec() )
         {
            hoverColor = dialog->selectedColor();
            delete dialog;
            emit selected( hoverColor );
            this->close();
         }//if
      }//if

      //qApp->sendEvent( parent(), event );
   }

};

class RColorPicker : public QPushButton
{
    Q_OBJECT
public:
    RColorPicker( QWidget *parent = 0 );
    void paintEvent ( QPaintEvent * event );
    //void click();
    void mouseReleaseEvent ( QMouseEvent * e );

    void setColor( QColor c ){ _selectedColor = c; }
    QColor color() const{ return _selectedColor; }

public slots:
   void on_popup_selected( QColor color );

private:
    RColorPickerPopup *popup;
    QColor _selectedColor;
};

#endif // RCOLORPICKER_H
