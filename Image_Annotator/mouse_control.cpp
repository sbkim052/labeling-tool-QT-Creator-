#include "control.h"
#include <QPainter>
#include <QTextStream>
#include <QMouseEvent>
#include <QPixmap>
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <iostream>
#include <string>
#include <QUndoStack>
#include <cmath>



void control::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    int index = indexAt(event->pos());

    /********************************************************/

    if(eraser_clicked==true)
        brushColor = Qt::white;
    else
    {
        if(palette_col<0)
            palette_col=0;
     //   qDebug() << "palet" << palette_col;
        }
    //7.30 성빈 추가
    if(fill_flag==1 && brush_clicked==true){
        brushColor = classList[palette_col].m_c_color;
        if(eraser_clicked==true)
            brushColor = Qt::white;
  //      qDebug() << "palet" << palette_col;
        int x=event->pos().x();
        int y=event->pos().y();
        floodfill(x,y,brushColor.rgb(),theImage.pixel(event->pos().x(),event->pos().y()),0);
        floodfill(x-1,y,brushColor.rgb(),theImage.pixel(event->pos().x(),event->pos().y()),1);
//        fill_flag=0;

        draw_pen = true;
        brush_clicked = true;
    }

    /********************************************************/


    if (index != -1 && !send_new && !draw_pen) {
        setShape(); //clear the selectednum

        selectedShape(m_shapeList[index].name()); //set present shape's selectednum to 1
        setCurrentShape(index);

        qDebug() << "in mousepressevent ";

        const Shape &shape = m_shapeList.at(index);
        m_resizeHandlePressed_tl = shape.resizeHandle(3).contains(event->pos());
        m_resizeHandlePressed_tr = shape.resizeHandle(2).contains(event->pos());
        m_resizeHandlePressed_bl = shape.resizeHandle(1).contains(event->pos());
        m_resizeHandlePressed_br = shape.resizeHandle(0).contains(event->pos());

        draw_resizehandle=true;
        if (m_resizeHandlePressed_br){
            m_mousePressOffset = shape.rect().bottomRight() - event->pos();

            m_fixedpoint=shape.rect().topLeft()+QPoint(4,4);
            m_first_click=1;

            emit cursor_resize_r();
        }
        else if (m_resizeHandlePressed_bl){
            m_mousePressOffset = shape.rect().bottomLeft() - event->pos();
            m_fixedpoint=shape.rect().topRight();
            m_first_click=1;
            emit cursor_resize_l();
        }
        else if (m_resizeHandlePressed_tr){
            m_mousePressOffset = shape.rect().topRight() - event->pos();
            m_first_click=1;
            m_fixedpoint=shape.rect().bottomLeft();
            emit cursor_resize_l();
        }
        else if (m_resizeHandlePressed_tl){
            m_mousePressOffset = shape.rect().topLeft() - event->pos();
            m_first_click=1;
            m_fixedpoint=shape.rect().bottomRight()-QPoint(3,3);
            emit cursor_resize_r();
        }
        else{
            m_mousePressOffset = event->pos() - shape.rect().topLeft();
            emit cursor_resizeall();
        }
    }

    else if(send_new && !draw_pen){
        qDebug() << "draw new rect";
        m_lastpos=event->pos();
        addedindex++;
        rband= new QRubberBand(QRubberBand::Rectangle, this);
        add_done=true;
    }

    else if(index == -1 && !send_new && draw_pen){
        qDebug() << "mouse is pressed with draw_pen";
        m_lastpos=event->pos();


        /******************************************************/
        brushSize = spinbox_changed_value;
//        if(palette_col>=0)
//            brushColor = classList[palette_col].m_c_color;

        emit mouse_pressed();
        /******************************************************/
    }
    m_mousePressIndex = index;
}



void control::mouseReleaseEvent(QMouseEvent *event)
{
    draw_resizehandle=true;
    event->accept();
    int index = indexAt(event->pos());

    m_mousePressIndex = -1;
    if(send_new)  //ㅊㄱ
    {
        //8.17성빈
        if(rect_flag==2){
            send_new=false;
            emit cursor_arrow();
            rect_flag=0;
        }
        else {
            rect_flag=2;
        }

        draw_done=true;


        /********************************************************************************************************/
                if(flag==2){
                    flag = 0;
                    rband->hide();

                    if(event->pos().x()<1){
                        if(m_pos.y()<=m_lastpos.y()){
                            Shape newshape(currenttype, currentcolor, QRect(0, m_pos.y(), m_lastpos.x()+4, m_lastpos.y()-m_pos.y()+4));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                        else{
                            Shape newshape(currenttype, currentcolor, QRect(0, m_lastpos.y(), m_lastpos.x()+4, m_pos.y()-m_lastpos.y()+4));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                    }
                    else if(event->pos().x()>image().width()*scaleFactor-1){
                        if(m_pos.y()<=m_lastpos.y()){
                            Shape newshape(currenttype, currentcolor, QRect(m_lastpos.x(), m_pos.y(), m_pos.x()-m_lastpos.x()+2, m_lastpos.y()-m_pos.y()+2));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                        else{
                            Shape newshape(currenttype, currentcolor, QRect(m_lastpos.x(), m_lastpos.y(), m_pos.x()-m_lastpos.x()+2, m_pos.y()-m_lastpos.y()+2));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                    }
                    else if(event->pos().y()<1){
                        if(m_pos.x()<=m_lastpos.x()){
                            Shape newshape(currenttype, currentcolor, QRect(m_pos.x(), 0, m_lastpos.x()-m_pos.x()+2, m_lastpos.y()-m_pos.y()+3));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                        else{
                            Shape newshape(currenttype, currentcolor, QRect(m_lastpos.x(), 0, m_pos.x()-m_lastpos.x()+2, m_lastpos.y()-m_pos.y()+3));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                    }
                    else if(event->pos().y()>image().height()*scaleFactor-1){
                        if(m_pos.x()<=m_lastpos.x()){
                            Shape newshape(currenttype, currentcolor, QRect(m_pos.x(), m_lastpos.y(), m_lastpos.x()-m_pos.x()+2, m_pos.y()-m_lastpos.y()+3));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                        else{
                            Shape newshape(currenttype, currentcolor, QRect(m_lastpos.x(), m_lastpos.y(), m_pos.x()-m_lastpos.x()+2, m_pos.y()-m_lastpos.y()+3));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                    }

                    qDebug() << "111111111111111111111";
                }
        /********************************************************************************************************/

            if(flag==1){
            flag=0;
            rband->hide();
            m_pos=event->pos();
            if(m_pos.x()<m_lastpos.x() && m_pos.y()<m_lastpos.y()){
                            Shape newshape(currenttype, currentcolor, QRect( m_pos, m_lastpos));
                            m_undoStack->push(new AddShapeCommand(this, newshape));}
                        else if(m_pos.x()<m_lastpos.x() && m_pos.y()>m_lastpos.y()){
                            int m_width=m_lastpos.x()-m_pos.x();
                            int m_height=m_pos.y()-m_lastpos.y();
                            Shape newshape(currenttype, currentcolor, QRect( m_pos.x(), m_lastpos.y(), m_width, m_height));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                        else if(m_pos.x()>m_lastpos.x() && m_pos.y()<m_lastpos.y()){
                            int m_width=m_pos.x()-m_lastpos.x();
                            int m_height=m_lastpos.y()-m_pos.y();
                            Shape newshape(currenttype, currentcolor, QRect( m_lastpos.x(), m_pos.y(), m_width, m_height));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }
                        else{
                            Shape newshape(currenttype, currentcolor, QRect( m_lastpos, m_pos));
                            m_undoStack->push(new AddShapeCommand(this, newshape));
                        }

        }

        emit button_checked("bt_rec_clicked");

        //8.17성빈


    }  //ㅊㄱ

    else if(!send_new && draw_pen){
        //m_pos=event->pos();
//        line=QLine(m_lastpos, event->pos());
//        line_map.map(line);
    //    qDebug() << line_map;
//        draw_pen=false;
//        eraser_clicked = false;  // @@@@@@@@@@@@@@@@@이지훈 추가

    }

    if(resize_release==1){
        emit cursor_arrow();
        resize_release = 0;
    }

    org_img=temp_black->scaled(theImage.width()*scaleFactor,theImage.height()*scaleFactor);
}


void control::mouseMoveEvent(QMouseEvent *event)
{
    event->accept();

    QPoint img_topleft;
    QPoint img_botright;
    QSize temp;
    QSize sizeboundary;

    img_topleft.setX(0);
    img_topleft.setY(0);
    img_botright.setX(image().width()*scaleFactor);
    img_botright.setY(image().height()*scaleFactor);

    if (m_mousePressIndex == -1 && !send_new && !draw_pen)
        return;
    QRect rect;
    if(send_new && !draw_pen){
        /************************************/
        if(event->pos().x()<2){
            flag=2;
            return;
        }
        if(event->pos().x()>image().width()*scaleFactor-2){
            flag=2;
            return;
        }
        if(event->pos().y()<2){
            flag=2;
            return;
        }
        if(event->pos().y()>image().height()*scaleFactor-2){
            flag=2;
            return;
        }
        /************************************/
        if(add_done){
            flag=1;
            m_pos=event->pos();
            rband->show();
        }

        rband->setGeometry(QRect(m_lastpos, event->pos()).normalized());

        if(flag==0){
            m_pos=event->pos();
            const Shape &shape=m_shapeList.last();
            rect = QRect(shape.rect().topLeft(), m_pos);
            add_done=false;
            setShapeRect(shape.name(), rect);  //this line makes rectangle move
            m_undoStack->push(new SetShapeRectCommand(this, shape.name(), shape.rect()));
        }

        /********************/
        emit mouse_pressed();
        /********************/
        //8.17성빈
        rect_flag=1;
    }
    else if(!draw_pen){
        const Shape &shape = m_shapeList.at(m_mousePressIndex);

        draw_resizehandle=false;

       if (m_resizeHandlePressed_br) {
            if(m_first_click==1){
                m_first_click=0;
                rect=QRect(shape.rect().topLeft(),event->pos());
            }
            else if(m_fixedpoint.x()> (event->pos().x())
                    && m_fixedpoint.y()-1>event->pos().y()){
                QPoint tl(event->pos().x()-1,event->pos().y()-1);
                rect=QRect(tl,m_fixedpoint+QPoint(2,2));
      //          qDebug()<<"우측하단 클릭 - 좌측상단으로 이동중";

//                temp.setWidth(m_fixedpoint.x()+3);
//                temp.setHeight(m_fixedpoint.y()+3);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.x()<0 || rect.y()<0)  return;
                /*************************/
            }
            else if(m_fixedpoint.x()>=event->pos().x()){
                QPoint tl(event->pos().x()-1,m_fixedpoint.y()-4);
                QPoint br(m_fixedpoint.x()+2,event->pos().y());
                rect =QRect(tl,br);
      //          qDebug()<<"우측하단 클릭 - 좌측하단으로 이동중";

//                temp.setWidth(m_fixedpoint.x()+3);
//                temp.setHeight(img_botright.y()-rect.y()+1);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.x()<0 || rect.bottomRight().y()>img_botright.y())  return;
                /*************************/
            }
            else if(m_fixedpoint.y()-1>=event->pos().y()){
                QPoint tl(m_fixedpoint.x()-4,event->pos().y()-1);
                QPoint br(event->pos().x(),m_fixedpoint.y()+1);
                rect =QRect(tl,br);
      //          qDebug()<<"우측하단 클릭 - 우측상단으로 이동중";

//                temp.setWidth(img_botright.x()-rect.x()+1);
//                temp.setHeight(m_fixedpoint.y()+3);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.y()<0 || rect.bottomRight().x()>img_botright.x())  return;
                /*************************/
            }

            else{
                rect = QRect(m_fixedpoint-QPoint(4,4), event->pos());
      //          qDebug()<<"우측하단 클릭 - 우측하단으로 이동중";

//                temp.setWidth(img_botright.x()-rect.x()+1);
//                temp.setHeight(img_botright.y()-rect.y()+1);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.bottomRight().x()>img_botright.x() || rect.bottomRight().y()>img_botright.y())  return;
                /*************************/
            }

            emit cursor_cross();
        }
        else if (m_resizeHandlePressed_bl) {
            if(m_first_click==1){
                m_first_click=0;
                QPoint tl(event->pos().x()-1,shape.rect().topLeft().y());
                QPoint br(m_fixedpoint.x(),event->pos().y());
                rect=QRect(tl,br);
            }
            else if(m_fixedpoint.x()< (event->pos().x()+1)
                    && m_fixedpoint.y()>event->pos().y()-3){
                QPoint tl(m_fixedpoint.x()-5,event->pos().y()-1);
                QPoint br(event->pos().x(),m_fixedpoint.y()+3);
                rect=QRect(tl,br);
      //          qDebug()<<"좌측하단 클릭 - 우측상단 이동중";

//                temp.setWidth(img_botright.x()-m_fixedpoint.x()+5);
//                temp.setHeight(m_fixedpoint.y()+3);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.y()<0 || rect.bottomRight().x()>img_botright.x())  return;
                /*************************/
            }
            else if(m_fixedpoint.x()<event->pos().x()+1){

                QPoint br(event->pos().x(),event->pos().y());
                rect =QRect(m_fixedpoint-QPoint(5,0),br);
      //          qDebug()<<"좌측하단 클릭 - 우측하단으로 이동중";

//                temp.setWidth(img_botright.x()-m_fixedpoint.x()+5);
//                temp.setHeight(img_botright.y()-rect.y()+1);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.bottomRight().x()>img_botright.x() || rect.bottomRight().y()>img_botright.y())  return;
                /*************************/
            }
            else if(m_fixedpoint.y()>event->pos().y()-3){
                QPoint tl(event->pos().x()-1,event->pos().y()-1);
                rect =QRect(tl,m_fixedpoint+QPoint(0,4));
      //          qDebug()<<"좌측하단 클릭 - 좌측상단으로 이동중";

//                temp.setWidth(m_fixedpoint.x()+3);
//                temp.setHeight(m_fixedpoint.y()+3);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.x()<0 || rect.y()<0)  return;
                /*************************/
            }
            else{
                QPoint tl(event->pos().x()-1, m_fixedpoint.y());
                QPoint br(m_fixedpoint.x(), event->pos().y());
                rect = QRect(tl, br);
      //          qDebug()<<"좌측하단 클릭 - 좌측하단으로 이동중";

//                temp.setWidth(m_fixedpoint.x()+3);
//                temp.setHeight(img_botright.y()-rect.y()+1);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.x()<0 || rect.bottomRight().y()>img_botright.y())  return;
                /*************************/
            }

            emit cursor_cross();
        }
        else if (m_resizeHandlePressed_tr) {
           if(m_first_click==1){
               m_first_click=0;
               QPoint tl(shape.rect().topLeft().x(),event->pos().y()-1);
               QPoint br(event->pos().x(),m_fixedpoint.y());
               rect=QRect(tl,br);
           }
           else if(m_fixedpoint.x()> (event->pos().x()-3)
                   && m_fixedpoint.y()<event->pos().y()+2){
               QPoint tl(event->pos().x()-1,m_fixedpoint.y()-4);
               QPoint br(m_fixedpoint.x()+5,event->pos().y());
               rect=QRect(tl,br);
      //         qDebug()<<"우측상단 클릭 - 좌측하단으로 이동중";

//               temp.setWidth(m_fixedpoint.x()+3);
//               temp.setHeight(img_botright.y()-m_fixedpoint.y()+5);
//               sizeboundary = rect.size().boundedTo(temp);
//               rect.setSize(sizeboundary);

               /*************************/
               if(rect.x()<0 || rect.bottomRight().y()>img_botright.y())  return;
               /*************************/
           }
           else if(m_fixedpoint.x()>event->pos().x()-3){
               QPoint tl(event->pos().x()-1,event->pos().y()-1);
               rect =QRect(tl,m_fixedpoint+QPoint(5,0));
      //         qDebug()<<"우측상단 클릭 - 좌측상단 이동중";

//               temp.setWidth(m_fixedpoint.x()+3);
//               temp.setHeight(m_fixedpoint.y()+3);
//               sizeboundary = rect.size().boundedTo(temp);
//               rect.setSize(sizeboundary);

               /*************************/
               if(rect.x()<0 || rect.y()<0)  return;
               /*************************/
           }
           else if(m_fixedpoint.y()<event->pos().y()+2){
               QPoint br(event->pos().x(),event->pos().y());
               rect =QRect(m_fixedpoint-QPoint(0,4),br);
          //     qDebug()<<"우측상단 클릭 - 우측하단으로 이동중";

//               temp.setWidth(img_botright.x()-m_fixedpoint.x()+1);
//               temp.setHeight(img_botright.y()-m_fixedpoint.y()+5);
//               sizeboundary = rect.size().boundedTo(temp);
//               rect.setSize(sizeboundary);

               /*************************/
               if(rect.bottomRight().x()>img_botright.x() || rect.bottomRight().y()>img_botright.y())  return;
               /*************************/
           }
           else{
               QPoint tl(m_fixedpoint.x(),event->pos().y()-1);
               QPoint br(event->pos().x(),m_fixedpoint.y());
               rect = QRect(tl, br);
          //     qDebug()<<"우측상단 클릭 - 우측상단으로 이동중";

//               temp.setWidth(img_botright.x()-m_fixedpoint.x()+1);
//               temp.setHeight(m_fixedpoint.y()+3);
//               sizeboundary = rect.size().boundedTo(temp);
//               rect.setSize(sizeboundary);

               /*************************/
               if(rect.y()<0 || rect.bottomRight().x()>img_botright.x())  return;
               /*************************/
           }
           emit cursor_cross();
        }
        else if (m_resizeHandlePressed_tl) {
            if(m_first_click==1){
                m_first_click=0;
                rect=QRect(event->pos()-QPoint(1,1),shape.rect().bottomRight());
            }
            else if(m_fixedpoint.x()< (event->pos().x() )
                    && m_fixedpoint.y()<event->pos().y() ){
                QPoint br(event->pos().x(),event->pos().y());
                rect=QRect(m_fixedpoint-QPoint(2,2),br);
          //      qDebug()<<"좌측상단 클릭 - 우측하단으로 이동중";

//                temp.setWidth(img_botright.x()-m_fixedpoint.x()+3);//
//                temp.setHeight(img_botright.y()-m_fixedpoint.y()+3);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.bottomRight().x()>img_botright.x() || rect.bottomRight().y()>img_botright.y())  return;
                /*************************/
            }
            else if(m_fixedpoint.x()<event->pos().x()){
                QPoint tl(m_fixedpoint.x()-2,event->pos().y()-1);
                QPoint br(event->pos().x(),m_fixedpoint.y()+3);
                rect =QRect(tl,br);
          //      qDebug()<<"좌측상단 클릭 - 우측상단 이동중";

//                temp.setWidth(img_botright.x()-m_fixedpoint.x()+3);
//                temp.setHeight(m_fixedpoint.y()+3);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.y()<0 || rect.bottomRight().x()>img_botright.x())  return;
                /*************************/
            }
            else if(m_fixedpoint.y()<event->pos().y()){
                QPoint tl(event->pos().x()-1,m_fixedpoint.y()-2);
                QPoint br(m_fixedpoint.x()+3,event->pos().y());
                rect =QRect(tl,br);
          //      qDebug()<<"좌측상단 클릭 - 좌측하단 이동중";

//                temp.setWidth(m_fixedpoint.x()+3);
//                temp.setHeight(img_botright.y()-m_fixedpoint.y()+3);
//                sizeboundary = rect.size().boundedTo(temp);
//                rect.setSize(sizeboundary);

                /*************************/
                if(rect.x()<0 || rect.bottomRight().y()>img_botright.y())  return;
                /*************************/
            }
            else{
                 rect = QRect(event->pos()-QPoint(1,1),m_fixedpoint+QPoint(3,3));
          //       qDebug()<<"좌측상단 클릭 - 좌측상단으로 이동중";

//                 temp.setWidth(m_fixedpoint.x()+3);//
//                 temp.setHeight(m_fixedpoint.y()+3);//
//                 sizeboundary = rect.size().boundedTo(temp);
//                 rect.setSize(sizeboundary);

                 /*************************/
                 if(rect.x()<0 || rect.y()<0)  return;
                 /*************************/
            }
            emit cursor_cross();
        }
        else {
            rect = shape.rect();
            rect.moveTopLeft(event->pos() - m_mousePressOffset);

            if(rect.x()<img_topleft.x())    return;
            if(rect.y()<img_topleft.y())    return;
            if(rect.bottomRight().x()>img_botright.x()) return;
            if(rect.bottomRight().y()>img_botright.y()) return;
        }

        setShapeRect(shape.name(), rect);  //this line makes rectangle move
        m_undoStack->push(new SetShapeRectCommand(this, shape.name(), rect));

        /********************/
        emit mouse_pressed();
        /********************/

        resize_release = 1;
    }
    else if(draw_pen && fill_flag==1)
        return;
    else if(draw_pen){
//        QPainter painter(&theImage);
//        if(!brushColor.isValid())
//            brushColor=Qt::red;
//        painter.setPen(QPen(brushColor, brushSize, Qt::SolidLine,
//        Qt::RoundCap, Qt::RoundJoin));
//        painter.drawLine(m_lastpos, event->pos());
////        line=QLine(m_lastpos, event->pos());
////        line_map.map(line);
//        m_lastpos = event->pos();
//        update();


        /**************************************************/
        QPainter painter(temp_black);


        if(eraser_clicked==true)
            brushColor = Qt::white;
        else
            brushColor = classList[palette_col].m_c_color;

        painter.setPen(QPen(brushColor, brushSize, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

        painter.drawLine(m_lastpos, event->pos());

        m_lastpos = event->pos();
        update();
        /**************************************************/

        emit cursor_circle();
    }
}

void control::scale_rect(int i, double scale)
{
    const Shape &shape = m_shapeList.at(i);
    QRect rect;

    rect=QRect(round(shape.m_rect.x()*scale),round(shape.m_rect.y()*scale),round(shape.m_rect.width()*scale),round(shape.m_rect.height()*scale));
    Shape newshape(shape.type(), shape.color(), rect);
    setShapeRect(shape.name(), rect);
}


void control::floodfill(int x, int y, QRgb newcolor, QRgb oldcolor, int flag){
    if(flag==0){
        oldcolor=temp_black->pixel(QPoint(x,y));
        if(newcolor==oldcolor)
            return;
        if(newcolor!=oldcolor&&x>=0&&y>=0&&x<temp_black->width()&&y<temp_black->height()){
            temp_black->setPixel(QPoint(x,y),newcolor);
            floodfill(x+1,y,newcolor,oldcolor,0);
            floodfill(x,y-1,newcolor,oldcolor,0);
            floodfill(x,y+1,newcolor,oldcolor,0);
            this->update();
        }
    }
    else if(flag==1){
        oldcolor=temp_black->pixel(QPoint(x,y));
        if(newcolor==oldcolor)
            return;
        if(newcolor!=oldcolor&&x>=0&&y>=0&&x<temp_black->width()&&y<temp_black->height()){
            temp_black->setPixel(QPoint(x,y),newcolor);
            floodfill(x-1,y,newcolor,oldcolor,1);
            floodfill(x,y-1,newcolor,oldcolor,1);
            floodfill(x,y+1,newcolor,oldcolor,1);
            this->update();
        }
    }
}
