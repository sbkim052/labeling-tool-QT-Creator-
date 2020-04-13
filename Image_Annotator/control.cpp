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
#include <QGraphicsLineItem>
#include <QFileDialog>

#define classNum 100

static const int resizeHandleWidth=6;

const QSize Shape::minSize(30,30);

Type::Type(QString &className, int &classId, QColor &classColor):
    m_c_name(className), m_c_num(classId), m_c_color(classColor)
{

}

QString Type::c_name() const
{
    return m_c_name;
}

int Type::c_id() const
{
    return m_c_num;
}

QColor Type::c_color() const
{
    return m_c_color;
}


Shape::Shape(const QString &type, const QColor &color, const QRect &rect, const int seleted)
    : m_type(type), m_color(color), m_rect(rect), m_seleted(seleted)
{

}

QString Shape::type() const
{
    return m_type;
}

QRect Shape::rect() const
{
    return m_rect;
}

QColor Shape::color() const
{
    return m_color;
}

QString Shape::name() const
{
    return m_name;
}

QRect Shape::resizeHandle(int i) const
{
    if(i==0){
        QPoint br=m_rect.bottomRight();
        return QRect(br-QPoint(resizeHandleWidth, resizeHandleWidth), br);
    }
    else if(i==1){
        QPoint br=m_rect.bottomLeft();
        return QRect(br-QPoint(0, resizeHandleWidth), br+QPoint(resizeHandleWidth, 0));
    }
    else if(i==2){
        QPoint br=m_rect.topRight();
        return QRect(br-QPoint(resizeHandleWidth, 0), br+QPoint(0,resizeHandleWidth));
    }
    else{
        QPoint br=m_rect.topLeft();
        return QRect(br, br+QPoint(resizeHandleWidth, resizeHandleWidth));
    }
}

control::control(QWidget *parent)
    : QWidget(parent), m_currentIndex(-1), m_mousePressIndex(-1), m_resizeHandlePressed_tl(false),m_resizeHandlePressed_tr(false), m_resizeHandlePressed_bl(false), m_resizeHandlePressed_br(false),
      theImage()
{
    scaleFactor=1;
    latestScale=1;
    save_flag=0;
    save_zoomed=0;
    m_undoStack = new QUndoStack(this);
}

QColor control::colorOfType(QString &className)
{
    QColor result;
    Shape shape;
    for(int i=0; i< classList.count(); i++){
        if(classList[i].m_c_name==className){
            result=classList[i].m_c_color;
        }
    }
    return result;
}

bool control::openImage(const QString &fileName)
{
    int num=0;
    QString dir;

    for(int i=0; fileName.length(); i++){
        QChar c = fileName.at(i);

        int v_latin = c.toLatin1();
        if(v_latin<46 || v_latin>47){
            num=i;
            break;
        }
        dir+=fileName.at(i);
    }

    QString path;
    path=control::dirName()+'/'+fileName;
    QImage image;

    if(!image.load(path,"bmp") && !image.load(path,"jpg") && !image.load(path,"png") && !image.load(path,"jpeg")){  // 8.3 이지훈 수정
    //    qDebug() << "fail to load in openimage" << path;
   //     qDebug() << "image format is " << image.format();
        return false;
    }

    setImage(image);
    return true;
}

void control::setImage(const QImage &image)
{
 //   qDebug() << "m_fileName : " << m_fileName;
    if(seg==1 && (m_fileName.endsWith("bmp")||m_fileName.endsWith("BMP"))){  // 이 부분 수정 8.3 이지훈
        QString path;
        path = fileName();

        int pos = path.lastIndexOf(QChar('_'));
        QString temp_origin_path;
        temp_origin_path = path.left(pos);

        QString origin_path;
        origin_path = find_extension(temp_origin_path);

    //    qDebug() << "path : " << path;
    //    qDebug() << "temp_origin_path : " << temp_origin_path;
     //   qDebug() << "origin_path : " << origin_path;

        QImage temp_image;
        QImage temp_image2;

        if(origin_path.endsWith("bmp"))
            temp_image.load(origin_path,"BMP");
        else if(origin_path.endsWith("png"))
            temp_image.load(origin_path,"PNG");
        else if(origin_path.endsWith("jpg"))
            temp_image.load(origin_path,"JPG");
        else if(origin_path.endsWith("jpeg"))
            temp_image.load(origin_path,"JPEG");

        theImage = temp_image.convertToFormat(QImage::Format_ARGB32);
        temp_black = new QImage(path);



        for(int i=1; i<classList.count()+1; i++){ //conf.txt 파일에 저장된 컬러로 변환
                    QRgb rgb=classList[i-1].m_c_color.rgb();
                    temp_black->setColor(i,rgb);
        }
        temp_black->setColor(0,qRgb(255,255,255)); //black background -> white
        *temp_black=temp_black->convertToFormat(QImage::Format_ARGB32);
        org_img = *temp_black;

    }
    else if(m_fileName.endsWith("txt")){
        theImage=image.convertToFormat(QImage::Format_ARGB32);
    }
    else{
        theImage=image.convertToFormat(QImage::Format_ARGB32);
        temp_black = new QImage(image.width(), image.height(), QImage::Format_ARGB32);
        temp_black->fill(Qt::white);

        org_img = *temp_black;
    }

    update();
    updateGeometry();
}

uint control::find_pixel(QRgb pixelValue){
    for(int i=0; i<classList.count(); i++){
        if(pixelValue==classList[i].m_c_color.rgb()){
            return uint(classList[i].m_c_num);
        }
    }
    return 0;
}


QSize control::sizeHint() const
{
    return theImage.size()*scaleFactor;
}


QString control::addShape(const Shape &shape)
{
    QString name;

    if(!m_shapeList.isEmpty())
        setShape();

    if(!removed){
        name=shape.name();
        name = uniqueName(name);
        m_shapeList.append(shape);
        m_shapeList[m_shapeList.count()-1].m_name = name;
        setCurrentShape(m_shapeList.count() - 1);
        update();
        emit added();
    }
    else {
        QString name=shape.name();

        int num=m_shapeList.count();
        if(num==0){
            m_shapeList.append(shape);
            setCurrentShape(m_shapeList.count() - 1);
            m_shapeIndex=0;
            m_shapeList[0].m_seleted=0;
            scale_rect(m_shapeIndex, scaleFactor/latestScale);
            update();
        }
        else{
            if(name.toInt()==num+1){
                m_shapeList.append(shape);
                m_shapeIndex=num;
            }
            else{
                m_shapeIndex=indexOf(name);
                addedNum(m_shapeIndex);
                m_shapeList.insert(m_shapeIndex, shape);
            }
            setCurrentShape(m_shapeList.count() - 1);
            m_shapeList[m_shapeIndex].m_seleted=0;
            update();
        }
        scale_rect(m_shapeIndex, scaleFactor/latestScale);
        emit added();
    }
    removed=false;
    return name;
}

QList<Shape> control::addShapes(QList<Shape> &deletedList)
{
    int index=0;
    for(int i=deletedList.count()-1; i>=0; i--){
        if(m_shapeList.count()!=0){
            Shape shape(deletedList[i].type(), deletedList[i].color(), deletedList[i].rect());
            shape.m_name=deletedList[i].name();
            index=shape.m_name.toInt()-1;
            m_shapeIndex=index;
            addedNum(m_shapeIndex);
            m_shapeList.insert(index, shape);
            setCurrentShape(m_shapeIndex);
        }
        else {
            Shape shape(deletedList[i].type(), deletedList[i].color(), deletedList[i].rect());
            shape.m_name=deletedList[i].name();
            int index=shape.m_name.toInt()-1;
            m_shapeIndex=index;
            m_shapeList.append(shape);
        }
        scale_rect(m_shapeIndex, scaleFactor/latestScale);
        update();
        emit added();
    }
    removed=false;
    return deletedList;
}

void control::deleteShape(const QString &shapeName)
{
    int index = indexOf(shapeName);
    deletedindex=index;
    deletedName=shapeName;

    if (index == -1)
        return;

    update(m_shapeList.at(index).rect());
    m_shapeList.removeAt(index);
    update();

    if(index!=m_shapeList.count())
        arrangeNum(index);
    emit deleted();

    if (index <= m_currentIndex) {
        m_currentIndex = -1;
        if (index == m_shapeList.count())
            --index;
        setCurrentShape(index);
    }
    else{
        setCurrentShape(index-1);
    }
    deletedName=nullptr;
}

QList<Shape> control::deleteShapes(QList<Shape> &deletedList)
{
    int index=classNum; //현재 Class 갯수가 100으로 정의되어있음.(최대갯수)

    for(int i=0; i<deletedList.count(); i++){
        int shapeNum=indexOf(deletedList[i].name());
        if(deletedList[i].name().toInt()<index){
            index=deletedList[i].name().toInt();
            deletedName=deletedList[i].name();
        }
        m_shapeList.removeAt(shapeNum);
        arrangeNum(shapeNum);
        emit deleted();
    }

    update();

    if (index <= m_currentIndex) {
        m_currentIndex = -1;
        if (index >= m_shapeList.count()-1)
            index=m_shapeList.count()-1;
        setCurrentShape(index);
    }
    else if(index>=m_shapeList.count()){
        m_currentIndex=-1;
        setCurrentShape(m_shapeList.count()-1);
    }

    return deletedList;
}

void control::addedNum(int num)
{
    QString prefix="00";
    QString prefixs="0";
    QString unique;

    for(int i=num; i<m_shapeList.count(); i++){
        int index=m_shapeList[i].m_name.toInt()+1;
        unique = m_shapeList[i].m_name;
        if (index >= 0 && index+1 <= 10)
            unique = prefix+QString::number(index);
        else if (index+1 > 10)
            unique = prefixs+QString::number(index);
        m_shapeList[i].m_name=unique;
    }
}

QUndoStack *control::undoStack() const
{
    return m_undoStack;
}


Shape control::shape(const QString &shapeName) const
{
    int index=indexOf(shapeName);
    if(index==-1)
        return Shape();
    return m_shapeList.at(index);
}

void control::setShapeRect(const QString &shapeName, const QRect &rect)
{
    int index=indexOf(shapeName);
    m_shapeIndex=index;

    if(index==-1)
        return;

    Shape &shape=m_shapeList[index];

    update(shape.rect());
    //scale_rect(m_shapeIndex, scaleFactor/latestScale);
    update(rect);
    shape.m_rect=rect;
}

void control::setShapeColor(const QString &shapeName, const QColor &color)
{

    int index = indexOf(shapeName);

    if (index == -1)
        return;

    Shape &shape = m_shapeList[index];
    shape.m_color = color;
    update(shape.rect());
}

bool control::load(QTextStream &stream)
{
  //  qDebug() << "in load";

    m_shapeList.clear();
    int cnt=0;
    QString bgname;

    while (!stream.atEnd()) {
        if(cnt==0){
            bgname=stream.readLine();
            if(!openImage(bgname))
                return false;
            setImageName(bgname);
        }

        QString shapeType, shapeName, colorName;
        int left, top, width, height;
        stream >> shapeType >> shapeName /*>> colorName */>> left >> top >> width >> height;

        if(shapeType==nullptr && shapeName==nullptr && left==NULL && width==NULL && height==NULL){
            m_currentIndex = m_shapeList.isEmpty() ? -1 : 0;
            cnt++;
            return true;
        }

        if (stream.status() != QTextStream::Ok)
            return false;

        Shape shape(shapeType);
        QColor color=colorOfType(shapeType);
        if (!color.isValid())
            return false;

        shape.m_type=shapeType;
        shape.m_name = shapeName;
        shape.m_color = color;
        shape.m_rect = QRect(left, top, width, height);
     //   qDebug() << shapeType << shapeName << color;

        m_shapeList.append(shape);
        cnt++;
    }

    m_currentIndex = m_shapeList.isEmpty() ? -1 : 0;
    return true;
}

bool control::load_img(const QString &fileName){
    QImage image;

    if(!image.load(fileName)){
     //   qDebug() << "fail to load";
        return false;}

    setImage(image);
    setImageName(fileName);
    return true;
}

void control::save(QTextStream &stream)
{
    stream << control::imgBasename();
    stream << "\r\n";
    for (int i = 0; i < m_shapeList.count(); ++i) {
        const Shape &shape = m_shapeList.at(i);
        QRect r = shape.rect();
        stream << shape.m_type << QLatin1Char(' ')
               << shape.name() << QLatin1Char(' ')
               << r.left() << QLatin1Char(' ')
               << r.top() << QLatin1Char(' ')
               << r.width() << QLatin1Char(' ')
               << r.height();
        if (i != m_shapeList.count() - 1)
            stream << "\r\n";
    }
}

QString control::dirName() const
{
    return m_dirName;
}

void control::setDirName(const QString &dirName)
{
    m_dirName = dirName;
}

QString control::fileName() const
{
    return m_fileName;
}

QString control::imgBasename() const{
    QFile file(m_fileName);
    QFileInfo fi(file);
    QString getsuffix=fi.suffix();
    QString getBasename/*=fi.baseName()+".bmp"*/;

 //   qDebug() << "m_filename@@@@@@@@2" << m_fileName;
 //   qDebug() << "getsuffix##########" << getsuffix;
 //   qDebug() << "getbasename%%%%%%%%" << getBasename;
 //   qDebug() << "temp_origin%%%%%%%%" << m_fileName.left(m_fileName.lastIndexOf(QChar('.')));

    QString temp_origin_path = m_fileName.left(m_fileName.lastIndexOf(QChar('.')));

    QFileInfo check_file_bmp1(temp_origin_path+".bmp");
    QFileInfo check_file_bmp2(temp_origin_path+".BMP");

    if((check_file_bmp1.isFile() && check_file_bmp1.exists()) || (check_file_bmp2.isFile() && check_file_bmp2.exists()))
        getBasename = fi.baseName()+".bmp";

    QFileInfo check_file_png1(temp_origin_path+".png");
    QFileInfo check_file_png2(temp_origin_path+".PNG");

    if((check_file_png1.isFile() && check_file_png1.exists()) || (check_file_png2.isFile() && check_file_png2.exists()))
        getBasename = fi.baseName()+".png";

    QFileInfo check_file_jpg1(temp_origin_path+".jpg");
    QFileInfo check_file_jpg2(temp_origin_path+".JPG");

    if((check_file_jpg1.isFile() && check_file_jpg1.exists()) || (check_file_jpg2.isFile() && check_file_jpg2.exists()))
        getBasename = fi.baseName()+".jpg";

    QFileInfo check_file_jpeg1(temp_origin_path+".png");
    QFileInfo check_file_jpeg2(temp_origin_path+".PNG");

    if((check_file_jpeg1.isFile() && check_file_jpeg1.exists()) || (check_file_jpeg2.isFile() && check_file_jpeg2.exists()))
        getBasename = fi.baseName()+".png";

 //   qDebug() << "get_basename&&&&&&&&&&&&&&&&" << getBasename;

    if(getsuffix==".txt")
        return m_imageName;
    else
        return getBasename;

//    QFile file(m_fileName);
//    QFileInfo fi(file);
//    QString getsuffix=fi.suffix();
//    QString getBasename=fi.baseName()+".bmp";

//    qDebug() << "111111111111111" << m_imageName;
//    qDebug() << "222222222222222" << getBasename;


//    if(getsuffix==".txt")
//        return m_imageName;
//    else
//        return getBasename;
}

int control::find_seg(){
    int result=0;
    QFile file(m_fileName);
    QFileInfo fi(file);
    QString getBasename=fi.baseName();
    QString seg="";
    int length=int(getBasename.length());
    for(int i=length-1; i>=0; i--){
        QString tmp=getBasename.at(i);
        seg.insert(0,tmp);
        if(seg=="_seg")
        {
            result=1;
            return result; //when _seg file is found, return 1
        }
    }
    return result; //otherwise, retun 0
}

void control::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

QString control::imageName() const
{
    return m_imageName;
}

void control::setImageName(const QString &imageName)
{
    m_imageName=imageName;
}

int control::indexAt(const QPoint &pos) const
{
    for (int i = m_shapeList.count() - 1; i >= 0; --i) {
        if (m_shapeList.at(i).rect().contains(pos))
            return i;
    }
    return -1;
}

int control::indexOf(const QString &shapeName) const
{
    for (int i = 0; i < m_shapeList.count(); ++i) {
        if (m_shapeList.at(i).name() == shapeName)
            return i;
    }

    return -1;
}

int control::lastindexOf(const QString &shapeName) const
{
    for (int i = m_shapeList.count(); i==0; --i) {
        if (m_shapeList.at(i).type() == shapeName)
            return i+1;
    }
    return -1;
}

bool control::drawShape(QString newclass, QColor newcolor, QString shape_str){  //ㅊㄱ
    send_new=true;
    currenttype=newclass;
    currentcolor=newcolor;
    currenttype_str=shape_str;

    return true;
}

bool control::drawBrush(QColor newcolor){
    brushColor=newcolor;
    return true;
}

void control::setdrawShape()
{
    send_new=false;
    draw_pen=false;
    emit cursor_arrow();
}

static QGradient gradient(const QColor &color, const QRect &rect)
{
    QColor c = color;
    c.setAlpha(0);
    QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
    gradient.setColorAt(0, c.light(0));
    return gradient;
}

void control::paintEvent(QPaintEvent *event)
{
    QRegion paintRegion = event->region();
    QPainter painter(this);



    this->update();

    img2=theImage.scaled(theImage.width()*scaleFactor,theImage.height()*scaleFactor);


    if(brush_clicked==false){
        if(seg==1){
            *temp_black=temp_black->scaled(theImage.width()*scaleFactor,theImage.height()*scaleFactor);
            painter.drawImage(QPoint(0, 0), *temp_black);
//            qDebug() << "111111111111111";
        }
        else if(show_image==true){
            *temp_black=temp_black->scaled(theImage.width()*scaleFactor,theImage.height()*scaleFactor);
            painter.drawImage(QPoint(0, 0), *temp_black);
            painter.setOpacity(opacity_spinbox_changed_value*0.01);
            painter.drawImage(QPoint(0, 0), img2);
//            qDebug() << "55555555555555";
        }
        else{
            painter.drawImage(QPoint(0, 0), img2);
//            qDebug() << "222222222222222";
        }
    }
    else{
            *temp_black=temp_black->scaled(theImage.width()*scaleFactor,theImage.height()*scaleFactor);
        painter.drawImage(QPoint(0, 0), *temp_black);
        if(show_image==true){
            painter.setOpacity(opacity_spinbox_changed_value*0.01);
            painter.drawImage(QPoint(0, 0), img2);
//            qDebug() << "333333333333333";
        }
        else{
           // *temp_black=temp_black->scaled(theImage.width()*scaleFactor,theImage.height()*scaleFactor);
            painter.drawImage(QPoint(0, 0), *temp_black);
//            qDebug() << "444444444444444";
        }
    }

    QPalette pal = palette();

    for (int i = 0; i < m_shapeList.count(); ++i) {
        const Shape &shape = m_shapeList.at(i);

        if (!paintRegion.contains(shape.rect()))
            continue;

        QPen pen = shape.color();
        pen.setWidth(i == m_currentIndex ? 2 : 1);
        painter.setPen(pen);
        painter.setBrush(gradient(shape.color(), shape.rect()));

        QRect rect = shape.rect();
        rect.adjust(3, 3, -resizeHandleWidth/2, -resizeHandleWidth/2);

        for(int i=0; i<classList.count(); i++){
            if(shape.type()==classList[i].c_name()){
                painter.setBrush(gradient(shape.color(), shape.rect()));
                painter.drawRect(rect);
            }
        }

        // paint the resize handle
        if(draw_resizehandle){
            if(m_shapeList[i].m_seleted==1){
                painter.setPen(pal.text().color());
                painter.setBrush(Qt::white);
                for(int j=0;j<4;j++){
                    painter.drawRect(shape.resizeHandle(j).adjusted(0, 0, -1, -1));
                }
            }
         }
        // paint the shape name
        painter.setPen(shape.color());
        painter.drawText(rect, Qt::AlignCenter, shape.name());
    }
    scale_flag=0;
}

int control::biggestindexOf() const
{
    int biggestNum=0;

    for (int i=0; i<m_shapeList.count(); i++) {
        if (biggestNum<m_shapeList.at(i).name().toInt())
            biggestNum=m_shapeList.at(i).name().toInt();
    }
    if(biggestNum>=0) return biggestNum;
    else return -1;
}

QString control::uniqueName(const QString &name) const
{
    QString unique;
    QString prefix="00";
    QString prefixs="0";
    int i=0;
    i=m_shapeList.count();
    int lastName=biggestindexOf()+1;

    unique = name;
    if (lastName >= 0 && lastName+1 <= 10)
        unique = prefix+QString::number(lastName);
    else if (lastName+1 > 10)
        unique = prefixs+QString::number(lastName);
    return unique;
}

void control::arrangeNum(int num)
{
    QString prefix="00";
    QString prefixs="0";
    QString unique;

    for(int i=num; i<m_shapeList.count(); i++){
        int index=m_shapeList[i].m_name.toInt()-1;
        unique = m_shapeList[i].m_name;
        if (index >= 0 && index+1 <= 10)
            unique = prefix+QString::number(index);
        else if (index+1 > 10)
            unique = prefixs+QString::number(index);
        m_shapeList[i].m_name=unique;
    }
}

void control::setCurrentShape(int index)
{
    QString currentName;

    if (m_currentIndex != -1 && m_shapeList.count()>0){
        update(m_shapeList.at(m_currentIndex).rect());
    }

    m_currentIndex = index;

    if (m_currentIndex != -1 && m_shapeList.count()>0) {
        const Shape &current = m_shapeList.at(m_currentIndex);
        update(current.rect());
        currentName = current.name();
    }

    if(currentName.isEmpty()) return;

    emit currentShapeChanged(currentName);
    emit sig_selected(currentName);
}

QString control::currentShapeName() const
{
    if (m_currentIndex == -1)
        return QString();
    return m_shapeList.at(m_currentIndex).name();
}

QString control::currentShapeClass() const
{
    if (m_currentIndex == -1)
        return QString();
    return m_shapeList.at(m_currentIndex).m_type;
}


void control::selectedShape(const QString &shapeName){
    for(int i=0; i<m_shapeList.count(); i++){
        if(m_shapeList.at(i).name()==shapeName){
            if(m_shapeList[i].m_seleted==1) m_shapeList[i].m_seleted=0;
            else m_shapeList[i].m_seleted=1;
        }
    }
    draw_resizehandle=true;
    update();
}

void control::setShape(){
    for(int i=0; i<m_shapeList.count(); i++){
        m_shapeList[i].m_seleted=0;
    }
}

int control::selectedNum(){
    int returnNum=0;

    for(int i=0; i<m_shapeList.count(); i++){
        if(m_shapeList[i].m_seleted==1)
            returnNum++;
    }
    return returnNum;
}


void control::save_img(){
    QFile file(fileName());
    QFileInfo fi(file);
    QString getname=fi.baseName();
    QString dir=fi.absolutePath();
    QString imgName;
    imgName=getname+".bmp";
    QString fileName;

    if(save_flag==1){
        if(brush_clicked==false)
            save_flag=0;
        if(find_seg()==1)
            fileName=dir+'/'+getname+".bmp";
        else
            fileName=dir+'/'+getname+"_seg"+".bmp";
    }
    QString filePath=fileName;

    if(!filePath.isEmpty()){
        if(brush_clicked==true){
            img_save = org_img;
        }
        else
            img_save = theImage;

        QPainter painter(&img_save);

        QImage img2=img_save.scaled(theImage.width()*scaleFactor,theImage.height()*scaleFactor);
        painter.drawImage(QPoint(0, 0), img2);
        QPalette pal = palette();
    }
    scale_flag=0;
    if (filePath == "")
        return;

    img_save.scaled(theImage.width(),theImage.height());
    img_save.save(filePath);
    m_fileName=filePath;
    save_zoomed=1;
}

void control::save_zoom(){
    QFile file(fileName());
    QFileInfo fi(file);
    QString getname=fi.baseName();
    QString dir=fi.absolutePath();
    QString imgName;
    imgName=getname+".bmp";
    QString fileName;
    //8.15성빈
    QString fileName_c;


    if(save_flag==1){
        if(brush_clicked==false)
            save_flag=0;
        if(find_seg()==1){
            fileName=dir+'/'+getname+".bmp";
            //8.15성빈
            fileName_c=dir+'/'+getname+"(c)"+".bmp";
        }
        else{
            fileName=dir+'/'+getname+"_seg"+".bmp";
            //8.15성빈
            fileName_c=dir+'/'+getname+"_seg(c)"+".bmp";
        }
    }
    QString filePath=fileName;
    //8.15성빈
    QString filePath_c=fileName_c;


    QImage convertedImg;
    if(!filePath.isEmpty()){
        if(brush_clicked==true){
            img_save = org_img;
            qDebug() << "brush clicked";

            //8.15성빈
            QImage img_save_c=img_save;
            img_save_c=img_save_c.scaled(theImage.width(),theImage.height());
            img_save_c.save(filePath_c);
        }
        else
            img_save = theImage;

        QPainter painter(&img_save);
        QImage img2;
        if(scaleFactor>=1)
            img2=img_save.scaled(theImage.width(),theImage.height());
        else{
            img2=img_save.scaled(theImage.width()*scaleFactor,theImage.height()*scaleFactor);
        }painter.drawImage(QPoint(0, 0), img2);
        QPalette pal = palette();
    }
    scale_flag=0;
    if (filePath == "")
        return;


    //img_save.scaled(theImage.width(),theImage.height());

    if(scaleFactor>=1){

        convertedImg=QImage(theImage.width(),theImage.height(),QImage::Format_Indexed8);

        qDebug() <<  classList.count() << theImage.width() << convertedImg.width();
        for(int i=0; i<classList.count()+1; i++){
            convertedImg.setColor(i,qRgb(i,i,i));
        }

        for(int i=0;i<theImage.width();i++){
            for(int j=0;j<theImage.height();j++){
                uint helo=find_pixel(img_save.pixel(i,j));
                convertedImg.setPixel(QPoint(i,j),uint(helo));
            }
        }
    }
    else { //when scaleFactor is less than 1 (it means zoom out)
        convertedImg=QImage(theImage.width()*scaleFactor,theImage.height()*scaleFactor, QImage::Format_Indexed8);
        for(int i=0; i<classList.count()+1; i++){
            convertedImg.setColor(i,qRgb(i,i,i));
        }

        for(int i=0;i<theImage.width()*scaleFactor;i++){
            for(int j=0;j<theImage.height()*scaleFactor;j++){
                uint helo=find_pixel(img_save.pixel(i,j));
                //if(helo) return;
                //if(scaleFactor1)
                convertedImg.setPixel(QPoint(i,j),uint(helo));
            }
        }

        convertedImg=convertedImg.scaled(theImage.width(),theImage.height());
    }

    //bt_save=0;
    convertedImg.save(filePath);
    //img_save.save(filePath);
    m_fileName=filePath;
    save_zoomed=0;
}

int control::current_index()
{
    return m_currentIndex;
}

QString control::find_extension(QString path)  // 함수 생성(헤더에도 추가할 것) 8.3 이지훈
{
    QFileInfo check_file_bmp1(path+".bmp");
    QFileInfo check_file_bmp2(path+".BMP");

    if((check_file_bmp1.isFile() && check_file_bmp1.exists()) || (check_file_bmp2.isFile() && check_file_bmp2.exists()))
        return path+".bmp";

    QFileInfo check_file_png1(path+".png");
    QFileInfo check_file_png2(path+".PNG");

    if((check_file_png1.isFile() && check_file_png1.exists()) || (check_file_png2.isFile() && check_file_png2.exists()))
        return path+".png";

    QFileInfo check_file_jpg1(path+".jpg");
    QFileInfo check_file_jpg2(path+".JPG");

    if((check_file_jpg1.isFile() && check_file_jpg1.exists()) || (check_file_jpg2.isFile() && check_file_jpg2.exists()))
        return path+".jpg";

    QFileInfo check_file_jpeg1(path+".png");
    QFileInfo check_file_jpeg2(path+".PNG");

    if((check_file_jpeg1.isFile() && check_file_jpeg1.exists()) || (check_file_jpeg2.isFile() && check_file_jpeg2.exists()))
        return path+".png";
}






