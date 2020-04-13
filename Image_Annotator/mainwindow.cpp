#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextStream>
#include "control.h"
#include <QMessageBox>

#include <QDebug>
#include <QDir>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QFont>
#include <QPainter>
#include <QShortcut>
#include <QScrollBar>
#include <qmath.h>

#define IDNUMS 3

static QGradient gradient(const QColor &color)
{
    QColor c = color;
    QLinearGradient result(QPointF(0, 0), QPointF(150, 200));

    result.setColorAt(0, c.dark(100));
    result.setColorAt(0.125, c.light(200));
    result.setColorAt(0.25, c.dark(100));
    result.setColorAt(0.375, c.light(200));
    result.setColorAt(0.5, c.dark(100));
    result.setColorAt(0.625, c.light(200));
    result.setColorAt(0.75, c.dark(100));
    result.setColorAt(0.875, c.light(200));
    result.setColorAt(1, c.dark(100));

    return result;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ctr(new control),
    scrollArea(new QScrollArea)
{
    ui->setupUi(this);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(ctr);
    setCentralWidget(scrollArea);

/*Current_file default setting********************************************************************************************************************************/
    ui->Current_file->setReadOnly(true);  // QLineEdit not editable

/*Left_table default setting**********************************************************************************************************************************/
    ui->Left_table->setEditTriggers(QAbstractItemView::NoEditTriggers);  // QTableWidget not editable
    ui->Left_table->setFocusPolicy(Qt::NoFocus);
    ui->Left_table->setSelectionMode(QAbstractItemView::NoSelection);

    ui->Left_table->setHorizontalHeaderItem(0, new QTableWidgetItem("Txt"));  // 8.3 이지훈 수정
    ui->Left_table->setHorizontalHeaderItem(1, new QTableWidgetItem("Image"));  // 8.3 이지훈 수정
    ui->Left_table->setHorizontalHeaderItem(2, new QTableWidgetItem("Label"));  // 8.3 이지훈 수정

    ui->Left_table->setColumnWidth(2, 50);  // 8.3 이지훈 수정
    ui->Left_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);  // 8.3 이지훈 수정
    ui->Left_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);  // 8.3 이지훈 수정
    ui->Left_table->setColumnHidden(3, true);  // hide file's location
    ui->Left_table->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);  // fix row height

/*pallete_table default setting***********************************************************************************************************************************/
    load_class();

/*label_table default setting***********************************************************************************************************************************/
    ui->label_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->label_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->label_table->setFocusPolicy(Qt::NoFocus);

    ui->label_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->label_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

/*set button images***********************************************************/
    QPixmap pixmap1(":/new/prefix1/square.png");
    QIcon ButtonIcon1(pixmap1);
    ui->bt_rec->setIcon(ButtonIcon1);

    QPixmap pixmap2(":/new/prefix1/brush.png");
    QIcon ButtonIcon2(pixmap2);
    ui->Brush->setIcon(ButtonIcon2);

    QPixmap pixmap3(":/new/prefix1/painter.png");
    QIcon ButtonIcon3(pixmap3);
    ui->Painter->setIcon(ButtonIcon3);

    QPixmap pixmap4(":/new/prefix1/eraser.png");
    QIcon ButtonIcon4(pixmap4);
    ui->Eraser->setIcon(ButtonIcon4);

    QPixmap pixmap5(":/new/prefix1/image.png");
    QIcon ButtonIcon5(pixmap5);
    ui->show_image->setIcon(ButtonIcon5);

    QPixmap pixmap6(":/new/prefix1/erase_square.png");
    QIcon ButtonIcon6(pixmap6);
    ui->Delete->setIcon(ButtonIcon6);

    QPixmap pixmap7(":/new/prefix1/save.png");
    QIcon ButtonIcon7(pixmap7);
    ui->Save->setIcon(ButtonIcon7);

    QPixmap pixmap10(":/new/prefix1/txt.png");
    QIcon ButtonIcon10(pixmap10);
    ui->bt_load->setIcon(ButtonIcon10);

    QPixmap pixmap11(":/new/prefix1/txt_folder.png");
    QIcon ButtonIcon11(pixmap11);
    ui->bt_load_folder_txt->setIcon(ButtonIcon11);

    QPixmap pixmap12(":/new/prefix1/img.png");
    QIcon ButtonIcon12(pixmap12);
    ui->bt_load_images->setIcon(ButtonIcon12);

    QPixmap pixmap13(":/new/prefix1/img_folder.png");
    QIcon ButtonIcon13(pixmap13);
    ui->bt_load_folder_bmp->setIcon(ButtonIcon13);

    QPixmap pixmap14(":/new/prefix1/remove.png");
    QIcon ButtonIcon14(pixmap14);
    ui->remove->setIcon(ButtonIcon14);

    QPixmap pixmap15(":/new/prefix1/remove_files.png");
    QIcon ButtonIcon15(pixmap15);
    ui->remove_files->setIcon(ButtonIcon15);

/***************************************************************************************************************************************************************/
    connect(ui->Left_table, SIGNAL(cellClicked(int, int)), this, SLOT(Left_table_clicked(int, int)));
    connect(ui->bt_rec, SIGNAL(triggered()), this, SLOT(addShape()));
    on_palette_cellClicked(0, 0);
    qDebug() << "before undogroup";
    change=0;
    final_scale=1;
    check_index=0;
    conf=0;
    select_flag=0;
    save_clicked=0;

/*set button shortcuts****************************************************************************************************************************************/
    ui->bt_rec->setShortcut(Qt::Key_1);
    ui->Delete->setShortcut(Qt::Key_Delete);
    ui->Brush->setShortcut(Qt::Key_2);
    ui->Painter->setShortcut(Qt::Key_3);
    ui->Eraser->setShortcut(Qt::Key_4);
    ui->show_image->setShortcut(Qt::Key_5);
    ui->Save->setShortcut(QString("Ctrl+S"));

    QShortcut *shortcut_c = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_C), this, SLOT(copyRect()));
    connect(shortcut_c, SIGNAL(triggered()), this, SLOT(copyRect()));
    QShortcut *shortcut_v = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_V), this, SLOT(pasteRect()));
    connect(shortcut_v, SIGNAL(triggered()), this, SLOT(pasteRect()));
    QShortcut *shortcut_esc = new QShortcut(QString("Esc"), this, SLOT(esc()));
    connect(shortcut_esc, SIGNAL(triggered()), this, SLOT(esc()));
    QShortcut *shortcut_all = new QShortcut(QString("Ctrl+A"), this, SLOT(allSelect()));
    connect(shortcut_all, SIGNAL(triggered()), this, SLOT(allSelect()));

/************************************************************/
    m_undoGroup = new QUndoGroup(this);
    QAction *undoAction = m_undoGroup->createUndoAction(this);
    QAction *redoAction = m_undoGroup->createRedoAction(this);

    /************************************************************/
    QPixmap pixmap8(":/new/prefix1/undo.png");
    QIcon ButtonIcon8(pixmap8);

    QPixmap pixmap9(":/new/prefix1/redo.png");
    QIcon ButtonIcon9(pixmap9);

    undoAction->setIcon(pixmap8);
    redoAction->setIcon(pixmap9);

    /************************************************************/

    ui->menuEdit->addAction(undoAction);
    ui->menuEdit->addAction(redoAction);

    ui->bt_undo->setDefaultAction(undoAction);
    ui->bt_redo->setDefaultAction(redoAction);

/************************************************************/

//    ui->bt_rec->setCheckable(true);  // 8.3 이지훈 수정
//    ui->Delete->setCheckable(true);  // 8.3 이지훈 수정
    ui->Brush->setCheckable(true);
    ui->Painter->setCheckable(true);
    ui->Eraser->setCheckable(true);
    ui->show_image->setCheckable(true);

    //9.1성빈
    ui->bt_rec->setCheckable(true);

    all_disable();
//    ui->remove->setDisabled(true);  // 8.3 이지훈 추가

    ui->bt_undo->setShortcut(QString("Ctrl+Z"));
    ui->bt_redo->setShortcut(QString("Ctrl+Y"));

    newDocument();
    updateAction();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)  // 8.3 이지훈 추가
{
    control *doc=currentDocument();
    if(doc==NULL)
        return;

    if(doc->fileName().endsWith("txt")){
        if (change==1) {
            int button
                = QMessageBox::warning(this,
                                tr("Unsaved terminates"),
                                tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                                QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

            if(button==QMessageBox::Cancel)
                event->ignore();
            else if (button == QMessageBox::Yes){
                saveDocument();
                event->accept();
            }
            else if(button == QMessageBox::No)
                event->accept();
        }
    else
        event->accept();
    }
    else{ //줌하고 저장 안했을 때 저장여부 확인.
        //qDebug() << "close event" << doc->save_zoomed;
        if(doc->save_zoomed==0 && change==1){
            int button
                = QMessageBox::warning(this,
                                tr("Unsaved terminates"),
                                tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                                QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

            if(button==QMessageBox::Cancel)
                event->ignore();
            else if (button == QMessageBox::Yes){
                save_clicked=1;
                saveDocument();
                event->accept();
            }
            else if(button == QMessageBox::No)
                event->accept();
        }
        else{}
//            event->accept();
    }
}

void MainWindow::updateAction()
{
    control *doc=currentDocument();
    m_undoGroup->setActiveStack(doc == 0 ? 0 : doc->undoStack());
    QString shapeName=doc == nullptr ? QString() : doc->currentShapeName();

    if(shapeName.isEmpty())
    {
        qDebug() << "update action";
    }
    else {

        Shape shape=doc->shape(shapeName);
    }
}

void MainWindow::change_temp()
{
    change = 1;
    ui->Save->setEnabled(true); // 11.13 이지훈_save표시
}

int MainWindow::send_zoom()
{
   control *doc=currentDocument();
   doc->zoom=zoom;
   return zoom;
}

/*botton slots*************************************/

void MainWindow::ArrowCursor()
{
    scrollArea->setCursor(Qt::ArrowCursor);
}

void MainWindow::SizeFCursor()
{
    scrollArea->setCursor(Qt::SizeFDiagCursor);
}

void MainWindow::SizeBCursor()
{
    scrollArea->setCursor(Qt::SizeBDiagCursor);
}

void MainWindow::ResizeallCursor()
{
    scrollArea->setCursor(Qt::SizeAllCursor);
}

void MainWindow::CrossCursor()
{
    scrollArea->setCursor(Qt::CrossCursor);
}

void MainWindow::CircleCursor()
{
    if(ui->brushsize->value()>=10)
        scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(ui->brushsize->value(), ui->brushsize->value()));
    else
        scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(10, 10));
}

/*copy & paste**********************************/


void MainWindow::copyRect()
{
    m_copylist.clear();

    control *doc=currentDocument();
    int current=doc->current_index();

    Shape shape(doc->m_shapeList[current].type(), doc->m_shapeList[current].color(), doc->m_shapeList[current].rect(), 0);

    if(m_copylist.isEmpty())
        m_copylist.append(shape);
    else {
        m_copylist.clear();
        m_copylist.append(shape);
    }

}

void MainWindow::pasteRect()
{
    if(m_copylist.empty())
        return;

    control *doc=currentDocument();

    if(m_copylist[0].rect().x()+m_copylist[0].rect().width()>doc->image().width() || m_copylist[0].rect().y()+m_copylist[0].rect().height()>doc->image().height())
        return;

    doc->undoStack()->push(new AddShapeCommand(doc, m_copylist[0]));
    change = 1;
    ui->Save->setEnabled(true); // 11.13 이지훈_save표시

}

/*renew label table**********************************/
void MainWindow::addonList()
{
    control *doc=currentDocument();
    int num=doc->m_shapeList.count()-1;

    if(!doc->removed){
        if(num > ui->label_table->rowCount()-1){
        ui->label_table->insertRow(ui->label_table->rowCount());
        }
        QString rect_name=doc->m_shapeList.at(num).name();
        QString type_name=doc->m_shapeList.at(num).type();
        ui->label_table->setItem(ui->label_table->rowCount()-1, 0, new QTableWidgetItem(type_name));
        ui->label_table->setItem(ui->label_table->rowCount()-1, 1, new QTableWidgetItem(rect_name));
    }
    else
    {
        ui->label_table->insertRow(ui->label_table->rowCount());
        for(int i=doc->m_shapeIndex; i<doc->m_shapeList.count(); i++){
            QString rect_name=doc->m_shapeList.at(i).name();
            QString type_name=doc->m_shapeList.at(i).type();
            ui->label_table->setItem(i, 0, new QTableWidgetItem(type_name));
            ui->label_table->setItem(i, 1, new QTableWidgetItem(rect_name));
        }
    }

    ui->label_table->setSortingEnabled(true);
    ui->label_table->sortItems(1,Qt::AscendingOrder);
}

void MainWindow::setonList()
{
    control *doc=currentDocument();
    QString name=doc->deletedName;
    int num=0;

    for(int i=0; i<doc->m_shapeList.count()+1; i++){
        if(ui->label_table->item(i,1)->text()==name){
            ui->label_table->removeRow(i);
            num=i;
            break;
        }
    }

    if(!doc->m_shapeList.empty()){
        for(int i=0; i<doc->m_shapeList.count(); i++){
            ui->label_table->item(i,1)->setText(doc->m_shapeList[i].name());
        }
    }
    update();
}


/*open files**********************************/
void MainWindow::openFileDocument()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Text(*.txt)"));
    dialog.setViewMode(QFileDialog::Detail);
    QDir CurrDir = QDir::current();
    CurrDir.cdUp();
    auto path = CurrDir.path();
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Directory"), path, tr("Text files(*.txt)"));
    control *doc;
    QDir d;
    QString dirName;

    openDoc_Refac(fileNames, doc, d, dirName);
}

void MainWindow::openImageDocument()
{
    QFileDialog dialog(this);
    dialog.setNameFilter(tr("Image(*.bmp *.png *.jpg *.jpeg)"));  // 8.3 이지훈 수정
    dialog.setViewMode(QFileDialog::Detail);
    QDir CurrDir = QDir::current();
    CurrDir.cdUp();
    auto path = CurrDir.path();
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Open Directory"), path, tr("Image files(*.bmp *.png *.jpg *.jpeg)"));  // 8.3 이지훈 수정
    control *doc;
    QDir d;
    QString dirName;

    openDoc_Refac(fileNames, doc, d, dirName);
}

void MainWindow::openFolderDocument()
{
    QStringList fileNames;
    QString fileName;
    control *doc;
    QDir d;
    QString dirName;
    QDir CurrDir = QDir::current();
    CurrDir.cdUp();
    auto path = CurrDir.path();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir directory(dir);
    QStringList dirs = directory.entryList(QStringList() << "*.txt", QDir::Files);

    for(int i=0; i<dirs.size(); i++)
        fileNames << (fileName = directory.absolutePath() + "/" + dirs.at(i));

    openDoc_Refac(fileNames, doc, d, dirName);
}

void MainWindow::openFolderDocument_bmp()
{
    QStringList fileNames;
    QString fileName;
    control *doc;
    QDir d;
    QString dirName;
    QDir CurrDir = QDir::current();
    CurrDir.cdUp();
    auto path = CurrDir.path();
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    QDir directory(dir);
    QStringList dirs = directory.entryList(QStringList() << "*.bmp" << "*.BMP" << "*.jpg" << "*.JPG *"<< "*.jpeg" << "*.JPEG *" << "*.png" << "*.PNG *", QDir::Files);  // 8.3 이지훈 수정

    for(int i=0; i<dirs.size(); i++)
        fileNames << (fileName = directory.absolutePath() + "/" + dirs.at(i));

    openDoc_Refac(fileNames, doc, d, dirName);
}

void MainWindow::openDoc_Refac(QStringList fileNames, control *doc, QDir d, QString dirName){
    bool exist = false;
    bool color;
    QString existed_list;
    QString parse_error_list;
    QString color_list;
    z_col=0;
    QString txtfile="txt";

    if(fileNames.isEmpty())
        return;
    else {
        for(int i=0; i<fileNames.count(); i++)
        {
            QFile file(fileNames.at(i));
            QFileInfo fi(file);
            QString getname=fi.suffix();
            setfileType(getname);
            if(!file.open(QIODevice::ReadOnly)){
                QMessageBox::warning(this,
                                    tr("file error"),
                                     tr("failed to open\n%1").arg(fileNames.at(i)));
                return;
            }

            if(fileNames.at(i).endsWith("(c).jpg")||fileNames.at(i).endsWith("(c).JPG")||
               fileNames.at(i).endsWith("(c).jpeg")||fileNames.at(i).endsWith("(c).JPEG")||
               fileNames.at(i).endsWith("(c).bmp")||fileNames.at(i).endsWith("(c).BMP")||
               fileNames.at(i).endsWith("(c).png")||fileNames.at(i).endsWith("(c).PNG"))
                color = true;
            else
                color = false;

            if(ui->Left_table->rowCount()==0){ //현재 로드된 파일 없음.
                QTextStream stream(&file);
                doc=new control();
                d = QFileInfo(fileNames.at(i)).absoluteDir();
                dirName=d.absolutePath();
                doc->setDirName(dirName);
                doc->classList=m_typelist;

                if(first_load==0){
                    doc->palette_col = 0;
                    zoom = 1;
                   // qDebug() << "$$$$$$$$$$$$$$4" << doc->palette_col;
                }


                doc->setFileName(fileNames.at(i));  // @@@@@@@@@@@@@@@@@@@@@@@@@ 이지훈 밑에서 이동
                doc->seg = doc->find_seg();  // @@@@@@@@@@@@@@@@@@@@@@@@@ 이지훈 추가
             //   qDebug() << "asdfsadfsadfsadfasd" << doc->seg;  // @@@@@@@@@@@@@@@@@@@@@@@@@ 이지훈 추가

                if(fileType==1){ //txt file
                     //doc추가 시 doc의 classList에 현재 mainwindow의 m_typelist copy
                    if( !doc->load(stream)){
          //              qDebug() << "goto load" << getname;
//                        QMessageBox::warning(this, tr("parse error"), tr("failed to parse1\n%1").arg(fileNames.at(i)));
                        delete doc;

                        parse_error_list += fileNames.at(i) + "\n";

                        continue;
                    }

                    //8.1 성빈
                    rect_enable();

                }
                else if(fileType==2){ //bmp file
          //          qDebug() << "goto load_img" << fileNames.at(i);
                    if(!doc->load_img(fileNames.at(i))){
//                        QMessageBox::warning(this, tr("parse error"), tr("failed to parse2\n%1").arg(fileNames.at(i)));
                        delete doc;

                        parse_error_list += fileNames.at(i) + "\n";

                        continue;
                    }

                    //8.1 성빈
                    brush_enable();
                }

                if(color==true){
                    delete doc;
                    color_list += fileNames.at(i) + "\n";
                    continue;
                }

                ui->Left_table->insertRow(ui->Left_table->rowCount());
                ui->Current_file->setText(fileNames.at(i));
                QString short_filename = fi.fileName();
                //QFileInfo fi(fileNames.at(i));

                if(short_filename.endsWith("txt"))
                    fileType = 1;
                else
                    fileType = 2;

                if(fileType==1){

                    //8.1 성빈
                    rect_enable();

          //          qDebug() << short_filename << doc->imageName() << fileNames.at(i);
                    ui->Left_table->setItem(ui->Left_table->rowCount()-1, 0, new QTableWidgetItem(short_filename));
                    ui->Left_table->setItem(ui->Left_table->rowCount()-1, 1, new QTableWidgetItem(doc->imageName()));
                    ui->Left_table->setItem(ui->Left_table->rowCount()-1, 3, new QTableWidgetItem(fileNames.at(i)));
                }
                else{

                    //8.1 성빈
                    brush_enable();
         //           qDebug() << fi.baseName()+".txt" << fi.baseName()+".bmp" << fileNames.at(i);
                    ui->Left_table->setItem(ui->Left_table->rowCount()-1, 0, new QTableWidgetItem());
                    ui->Left_table->setItem(ui->Left_table->rowCount()-1, 1, new QTableWidgetItem(short_filename));
                    ui->Left_table->setItem(ui->Left_table->rowCount()-1, 3, new QTableWidgetItem(fileNames.at(i)));
                }

                addDocument(doc);

                Left_table_label_default();  // 파일 load할 때 label 유무 표시

                for(int i=0; i<ui->Left_table->rowCount()-1; i++){
                    ui->Left_table->item(i, 0)->setBackgroundColor(Qt::white);
                    ui->Left_table->item(i, 0)->setForeground(Qt::black);
                    ui->Left_table->item(i, 1)->setBackgroundColor(Qt::white);
                    ui->Left_table->item(i, 1)->setForeground(Qt::black);
                    ui->Left_table->item(i, 2)->setBackgroundColor(Qt::white);
                    ui->Left_table->item(i, 2)->setForeground(Qt::black);
                }

                ui->Left_table->item(ui->Left_table->rowCount()-1, 0)->setBackgroundColor(Qt::gray);
                ui->Left_table->item(ui->Left_table->rowCount()-1, 0)->setForeground(Qt::white);
                ui->Left_table->item(ui->Left_table->rowCount()-1, 1)->setBackgroundColor(Qt::gray);
                ui->Left_table->item(ui->Left_table->rowCount()-1, 1)->setForeground(Qt::white);
                ui->Left_table->item(ui->Left_table->rowCount()-1, 2)->setBackgroundColor(Qt::gray);
                ui->Left_table->item(ui->Left_table->rowCount()-1, 2)->setForeground(Qt::white);
            }
            else{
                exist = false;
                for(int j=0; j<ui->Left_table->rowCount(); j++){
                    if(fileNames.at(i)==ui->Left_table->item(j, 3)->text()){
                        existed_list += fileNames.at(i) + "\n";
                        exist=true;
                    }
                }

//                if(color==true){
//                    delete doc;
//                    color_list += fileNames.at(i) + "\n";
//                    continue;
//                }


                    if(exist==false){
                        QTextStream stream(&file);
                        doc=new control();

                        doc->classList=m_typelist; //doc추가 시 doc의 classList에 현재 mainwindow의 m_typelist copy
                        d = QFileInfo(fileNames.at(i)).absoluteDir();
                        dirName=d.absolutePath();
                        doc->setDirName(dirName);


                        doc->setFileName(fileNames.at(i));  // @@@@@@@@@@@@@@@@@@@@@@@@@ 이지훈 밑에서 이동
                        doc->seg = doc->find_seg();  // @@@@@@@@@@@@@@@@@@@@@@@@@ 이지훈 추가
                        qDebug() << "asdfsadfsadfsadfasd" << doc->seg;  // @@@@@@@@@@@@@@@@@@@@@@@@@ 이지훈 추가



                        if(getname==txtfile){
                            if(!doc->load(stream)){
         //                       qDebug() << "goto load" << getname;
//                                QMessageBox::warning(this, tr("parse error"), tr("failed to parse3\n%1").arg(fileNames.at(i)));
                                delete doc;

                                parse_error_list += fileNames.at(i) + "\n";

                                continue;
                            }
                        }
                        else{
         //                   qDebug() << "goto load_img" << fileNames.at(i);
                            if(!doc->load_img(fileNames.at(i))){
//                                QMessageBox::warning(this, tr("parse error"), tr("failed to parse4\n%1").arg(fileNames.at(i)));
                                delete doc;

                                parse_error_list += fileNames.at(i) + "\n";

                                continue;
                            }
                        }

                        if(color==true){
                            delete doc;
                            color_list += fileNames.at(i) + "\n";
                            continue;
                        }

                        ui->Left_table->insertRow(ui->Left_table->rowCount());
                        ui->Current_file->setText(fileNames.at(i));
                        QFileInfo fi(fileNames.at(i));
                        QString short_filename = fi.fileName();

                        if(short_filename.endsWith("txt"))
                            fileType = 1;
                        else
                            fileType = 2;

                        if(fileType==1){
         //                   qDebug() << short_filename << doc->imageName() << fileNames.at(i);
                            ui->Left_table->setItem(ui->Left_table->rowCount()-1, 0, new QTableWidgetItem(short_filename));
                            ui->Left_table->setItem(ui->Left_table->rowCount()-1, 1, new QTableWidgetItem(doc->imageName()));
                            ui->Left_table->setItem(ui->Left_table->rowCount()-1, 3, new QTableWidgetItem(fileNames.at(i)));
                            rect_enable();
                        }
                        else{
         //                   qDebug() << fi.baseName()+".txt" << fi.baseName()+".bmp" << fileNames.at(i);
                            ui->Left_table->setItem(ui->Left_table->rowCount()-1, 0, new QTableWidgetItem());
                            ui->Left_table->setItem(ui->Left_table->rowCount()-1, 1, new QTableWidgetItem(short_filename));
                            ui->Left_table->setItem(ui->Left_table->rowCount()-1, 3, new QTableWidgetItem(fileNames.at(i)));
                            brush_enable();
                        }
                        addDocument(doc);

                        Left_table_label_default();  // 파일 load할 때 label 유무 표시

                        for(int i=0; i<ui->Left_table->rowCount()-1; i++){

                            ui->Left_table->item(i, 0)->setBackgroundColor(Qt::white);
                            ui->Left_table->item(i, 0)->setForeground(Qt::black);
                            ui->Left_table->item(i, 1)->setBackgroundColor(Qt::white);
                            ui->Left_table->item(i, 1)->setForeground(Qt::black);
                            ui->Left_table->item(i, 2)->setBackgroundColor(Qt::white);
                            ui->Left_table->item(i, 2)->setForeground(Qt::black);
                        }

                        ui->Left_table->item(ui->Left_table->rowCount()-1, 0)->setBackgroundColor(Qt::gray);
                        ui->Left_table->item(ui->Left_table->rowCount()-1, 0)->setForeground(Qt::white);
                        ui->Left_table->item(ui->Left_table->rowCount()-1, 1)->setBackgroundColor(Qt::gray);
                        ui->Left_table->item(ui->Left_table->rowCount()-1, 1)->setForeground(Qt::white);
                        ui->Left_table->item(ui->Left_table->rowCount()-1, 2)->setBackgroundColor(Qt::gray);
                        ui->Left_table->item(ui->Left_table->rowCount()-1, 2)->setForeground(Qt::white);
                    }
            }
        }

        if(existed_list!=NULL && parse_error_list==NULL){
            qDebug() << "1111111111111111";
            QMessageBox::warning(this, tr("file error"), tr("Already existed.\n\n%1").arg(existed_list));}
        else if(parse_error_list!=NULL && existed_list==NULL){
            qDebug() << "2222222222222222";
            QMessageBox::warning(this, tr("parse error"), tr("Failed to parse.\n\n%1").arg(parse_error_list));}
        else if(existed_list!=NULL && parse_error_list!=NULL){
            qDebug() << "3333333333333333";
            QMessageBox::warning(this, tr("file&parse error"), tr("Already existed.\n\n%1\nFailed to parse.\n\n%2").arg(existed_list).arg(parse_error_list));}
    }
    z_row=ui->Left_table->rowCount()-1;

    prev_row = ui->Left_table->rowCount()-1;

    change = 0;
    ui->Save->setDisabled(true); // 11.13 이지훈_save표시

    if(ui->Left_table->rowCount()!=0)  // 8.3 이지훈 추가
        ui->remove->setEnabled(true);  // 8.3 이지훈 추가

}

void MainWindow::load_class(){ //set the palette(get the class in conf_class.txt)
    if(conf==1){
        int num=ui->palette->columnCount();
        for(int i=0; i<num; i++){
            ui->palette->removeColumn(ui->palette->columnCount()-1);
        }
        m_typelist.clear();

    }

    ui->palette->setEditTriggers(QAbstractItemView::NoEditTriggers);  // QTableWidget not editable
    ui->palette->setRowCount(1);

    ui->palette->setVerticalHeaderItem(0, new QTableWidgetItem("Class"));
    ui->palette->verticalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);

    ui->palette->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->palette->setFocusPolicy(Qt::NoFocus);

    ui->palette->setSelectionMode(QAbstractItemView::NoSelection);

    QDir CurrDir = QDir::current();
    CurrDir.cdUp();
    auto path = CurrDir.path();
    conf_name = QFileDialog::getOpenFileName(this, tr("Open Directory"), path, tr("Text files(*.txt)"));

    QFile file(conf_name);
    QTextStream stream(&file);
    int cnt=0;

    Shape shape;

    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,
                            tr("Configuration file error"),
                             tr("failed to open\n%1").arg(conf_name));
        return;
    }

    while (!stream.atEnd()) {
        QString classname;
        int id, r, g, b;

        stream >> classname >> id >> r >> g >> b;
/* conf_class에 class추가 시 classname id r g b 순서 로 추가할 것
 * 예시) Car 001 255 0 0
 * (255,0,0)는 QColor로 받음 */

        QColor classColor(r,g,b);
        Type type(classname, id, classColor);

        m_typelist.append(type); //mainwindow내 m_typelist에 type class추가

        ui->palette->insertColumn(cnt);

//        ui->palette->horizontalHeader()->setSectionResizeMode(cnt, QHeaderView::Stretch);
        ui->palette->setItem(0, cnt, new QTableWidgetItem);
        ui->palette->item(0, cnt)->setBackground(classColor);
        ui->palette->item(0, cnt)->setTextAlignment(Qt::AlignCenter);
        ui->palette->item(0, cnt)->setText(classname);

        cnt++;
    }
    classNum=cnt;
    //f=1;

    QTableWidgetItem *currentItem = ui->palette->item(0, 0);
  //  Q_ASSERT(currentItem);
    QFont font;
    font.setStyleHint (QFont::Monospace);
    font.setPointSize (18);
    font.setFixedPitch (true);
    font.setBold(true);

    QBrush brush(gradient(QColor(255, 0, 0)));
    ui->palette->item(0, 0)->setData(Qt::BackgroundRole, QBrush(brush));
    ui->palette->item(0, 0)->setFont (font);

    if(conf==1){
        //on_palette_cellClicked(0, 0);
        //all_disable();
        newDocument();
        updateAction();
        qDebug() << "conf1 done";
    }
    conf=0;
}

void MainWindow::adjustBg(QString bgname){

}

void MainWindow::addDocument(control *doc)
{
    doc->classList=m_typelist; //doc추가 시 doc의 classList에 현재 mainwindow의 m_typelist copy

    for(int i=ui->label_table->rowCount()-1; i>=0; i--)
    {
        ui->label_table->removeRow(i);
    }
    m_undoGroup->addStack(doc->undoStack());

    connect(doc, SIGNAL(currentShapeChanged(QString)), this, SLOT(updateAction()));

    connect(doc, SIGNAL(deleted()), this, SLOT(setonList()));

    connect(doc, SIGNAL(cursor_cross()),this,SLOT(CrossCursor()));

    connect(doc, SIGNAL(cursor_arrow()), this, SLOT(ArrowCursor()));

    connect(doc, SIGNAL(cursor_resize_r()), this, SLOT(SizeFCursor()));

    connect(doc, SIGNAL(cursor_resize_l()), this, SLOT(SizeBCursor()));

    connect(doc, SIGNAL(cursor_circle()), this, SLOT(CircleCursor()));

    connect(doc, SIGNAL(mouse_pressed()), this, SLOT(change_temp()));

    connect(doc, SIGNAL(added()), this, SLOT(addonList()));

    connect(doc, SIGNAL(sig_selected(QString)), this, SLOT(label_table_changed(QString)));

    connect(doc, SIGNAL(button_checked(QString)), this, SLOT(button_checked(QString)));

    connect(doc, SIGNAL(check_zoom()), this, SLOT(send_zoom()));

    setCurrentDocument(doc);

    int num=doc->m_shapeList.count();
    for(int i=0; i<num; ++i){
        ui->label_table->insertRow(ui->label_table->rowCount());
        QString typeName=doc->m_shapeList.at(i).type();
        QString idName=doc->m_shapeList.at(i).name();
        ui->label_table->setItem(ui->label_table->rowCount()-1, 0, new QTableWidgetItem(typeName));
        ui->label_table->setItem(ui->label_table->rowCount()-1, 1, new QTableWidgetItem(idName));
    }

    //

    scrollArea->setCursor(Qt::ArrowCursor);

    doc->currentcolor=newcolor;

    on_show_image_clicked();
    on_show_image_clicked();
}

QString MainWindow::fixedWindowTitle(const control *doc) const
{
//    QString title = doc->fileName();

//    if (title.isEmpty())
//        title = tr("Unnamed");
//    else
//        title = QFileInfo(title).fileName();

//    QString result;

//    for (int i = 0; ; ++i) {
//        result = title;
//        if (i > 0)
//            result += QString::number(i);

//        bool unique = true;
//        for (int j = 0; j < ui->tabwidget->count(); ++j) {
//            const QWidget *widget = ui->tabwidget->widget(j);
//            if (widget == doc)
//                continue;
//            if (result == ui->tabwidget->tabText(j)) {
//                unique = false;
//                break;
//            }
//        }

//        if (unique)
//            break;
//    }

//    return result;
}

void MainWindow::setCurrentDocument(control *doc)
{
    scrollArea->setWidget(doc);
}

control *MainWindow::currentDocument() const
{
    return qobject_cast<control*>(scrollArea->widget());
}

void MainWindow::saveDocument()
{
    control *doc = currentDocument();
    QString fileName;
    if (doc == nullptr)
        return;
    shape_count=doc->m_shapeList.count();
    for (int i = 0; i < shape_count; ++i){
        doc->scale_rect(i,1/final_scale);
    }
    for (;;) {
        if(change!=0){ //수정한 내용 있을 때
            fileName = doc->fileName();
            setfileType((fileName));
            QFile file(fileName);
            QFileInfo fi(file);
            QString getname=fi.baseName();
            QString dir=fi.absolutePath();
            QString imgName;
            imgName=getname+".bmp";
            doc->imageName()=imgName;
            if(fileName.endsWith("txt")){
                    fileName=dir+'/'+getname+".txt";

                qDebug() << "save txt and bmp" <<dir <<  imgName << "&" << fileName;
                QFile file_r(fileName);
                if (fileName.isEmpty()){
                    fileName = QFileDialog::getSaveFileName(this);
                }
                if (fileName.isEmpty())
                   break;
                if (!file_r.open(QIODevice::WriteOnly)) {
                    QMessageBox::warning(this,
                                        tr("File error"),
                                        tr("Failed to open\n%1").arg(fileName));
                    doc->setFileName(QString());
                } else {
                    QTextStream stream(&file_r);

//                    doc->save_img();
                    doc->save(stream);
             //       doc->setFileName(fileName);
                    break;
                }
                final_scale=1;
            }
            else{
               // qDebug() << "bmp" <<imgName;
                if(save_clicked==0)
                   doc->save_img();
                else{
                    qDebug() << "bmp save_zoom save";
                    doc->save_zoom();}
                save_clicked=0;
                final_scale=1;
             //   doc->setFileName(doc->fileName());
                break;
            }
            change = 0;
            ui->Save->setDisabled(true); // 11.13 이지훈_save표시
        }
        else{ //수정한 내용 없을 때

           // change=1;   //확인할 것 왜 1인가
            fileName = doc->fileName();
            setfileType(fileName);
            if (fileName.isEmpty())
                fileName = QFileDialog::getSaveFileName(this);
            if (fileName.isEmpty())
                break;

            QFile file(fileName);
            if(fileName.endsWith("txt")){
                if (!file.open(QIODevice::WriteOnly)) {
                    QMessageBox::warning(this,
                                        tr("File error"),
                                        tr("Failed to open\n%1").arg(fileName));
                    doc->setFileName(QString());

                } else {
                    QTextStream stream(&file);
                    doc->save(stream);
                   // doc->setFileName(fileName);
                    break;
                }
            }
            else{

                if(save_clicked==0)
                   doc->save_img();
                else{
                    qDebug() << "bmp save_zoom save";
                    doc->save_zoom();}
                save_clicked=0;
                doc->save_img();
           //     doc->setFileName(doc->fileName());
                break;
            }

        }
    }
    if(fileName!=doc->fileName()){
        qDebug() << "different name" << fileName << doc->fileName();
        doc->load_img(doc->fileName());
        doc->setFileName(doc->fileName());
        QFileInfo fi(doc->fileName());
        QString short_filename = fi.fileName();
        ui->Left_table->setItem(prev_row, 0, new QTableWidgetItem());
        ui->Left_table->setItem(prev_row, 1, new QTableWidgetItem(short_filename));
        ui->Left_table->setItem(prev_row, 3, new QTableWidgetItem(doc->fileName()));
        ui->Current_file->setText(doc->fileName());

//        Left_table_clicked(prev_row, 0);
    }
    else {
        doc->setFileName(fileName);

    }
    setfileType(fileName);
    for (int i = 0; i < shape_count; ++i){
        doc->scale_rect(i,1*final_scale);
    }

}

void MainWindow::closeDocument()
{
    control *doc = currentDocument();
    if (doc == NULL)
        return;
    if (change==1) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            on_Save_clicked();
//            saveDocument();
        }
        else {

        }
    }
    //removeDocument(doc);
    delete doc;
    change = 0;
    ui->Save->setDisabled(true); // 11.13 이지훈_save표시

}

void MainWindow::removeDocument(control *doc)
{

}

void MainWindow::setfileType(QString &t_file){

    if(t_file=="txt" || t_file=="TXT") // file type is text file.
        fileType=1;
    else if(t_file=="bmp" || t_file=="BMP" || t_file=="jpg" || t_file=="JPG" || t_file=="jpeg" || t_file=="JPEG" || t_file=="png" || t_file=="PNG")// filetype is img(bmp)  // 8.3 이지훈 수정
        fileType=2;
}

void MainWindow::on_bt_load_clicked()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            //8.15성빈
            //saveDocument();
            on_Save_clicked();
        }
        else {

        }
    }

    openFileDocument();

}

void MainWindow::on_bt_load_folder_txt_clicked()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            //8.15성빈
            //saveDocument();
            on_Save_clicked();
        }
        else {

        }
    }

    openFolderDocument();
}



void MainWindow::on_bt_load_folder_bmp_clicked()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            //8.15성빈
            //saveDocument();
            on_Save_clicked();
        }
        else {

        }
    }

    openFolderDocument_bmp();
}


void MainWindow::on_bt_imgload_clicked()
{

}

void MainWindow::on_bt_rec_clicked()
{
    ui->bt_rec->setChecked(true);


    addShape();


    /**************************************************************************/
    control *doc = currentDocument();
    doc->brush_clicked = false;
    doc->eraser_clicked = false;
    /**************************************************************************/

}

void MainWindow::Left_table_clicked(int row, int col){
    QTableWidgetItem *itab = ui->Left_table->item(row, 3);
    QString fileName = itab->text();

    //8.15성빈
    if(z_row!=row){
        z_col=col;
        z_row=row;
    }

    closeDocument();
    //on_Save_clicked();

    ui->Current_file->setText(fileName);

    /**************************************************************************/

    int temp_filetype;

    if(ui->Current_file->text().endsWith("txt")){
        temp_filetype = 1;
        fileType=1;
        ui->bt_rec->setChecked(false);
        ui->Delete->setChecked(false);
    }
    else{
        temp_filetype = 2;
        fileType=2;
        ui->Brush->setChecked(false);
        ui->Painter->setChecked(false);
        ui->Eraser->setChecked(false);
        ui->show_image->setChecked(false);
    }

//    bt_set(temp_filetype);

    /**************************************************************************/


    for(int i=0; i<ui->Left_table->rowCount(); i++){
        if(i==row){
            ui->Left_table->item(i, 0)->setBackgroundColor(Qt::gray);
            ui->Left_table->item(i, 0)->setForeground(Qt::white);
            ui->Left_table->item(i, 1)->setBackgroundColor(Qt::gray);
            ui->Left_table->item(i, 1)->setForeground(Qt::white);
            ui->Left_table->item(i, 2)->setBackgroundColor(Qt::gray);
            ui->Left_table->item(i, 2)->setForeground(Qt::white);
        }
        else{
            ui->Left_table->item(i, 0)->setBackgroundColor(Qt::white);
            ui->Left_table->item(i, 0)->setForeground(Qt::black);
            ui->Left_table->item(i, 1)->setBackgroundColor(Qt::white);
            ui->Left_table->item(i, 1)->setForeground(Qt::black);
            ui->Left_table->item(i, 2)->setBackgroundColor(Qt::white);
            ui->Left_table->item(i, 2)->setForeground(Qt::black);
        }
    }

    if(z_row!=row){  // 다른 row가 선택되어잇을 때
        final_scale=1;
        z_col=col;
        z_row=row;

        //on_Save_clicked();
        //closeDocument();

        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);

        control *doc=new control();
        QDir d = QFileInfo(fileName).absoluteDir();
        QString dirName=d.absolutePath();

        doc->setDirName(dirName);
        doc->classList=m_typelist;
        /**************************************************************************/

        QFileInfo fi(file);
        QString getBasename = fi.baseName();

       // qDebug() << "z_row clicked";
     //   qDebug() << "basename" << getBasename;

        if(getBasename.endsWith("_seg")){
            doc->seg = 1;
            doc->m_fileName = fileName;
        }
        else
            doc->seg = 0;

   //     qDebug() << "result" << doc->seg;

        /**************************************************************************/

        /**************************************************************************/

        if(temp_filetype==1){
            doc->draw_pen=false;

            if(!doc->load(stream)){
    //            qDebug() << "stream_filename" << doc->fileName();
                QMessageBox::warning(this, tr("parse error"), tr("failed to parse5\n%1").arg(fileName));
                delete doc;
                return;
            }
            //8.1 성빈
            rect_enable();
        }
        else{
            doc->load_img(fileName);
            //8.1 성빈
            brush_enable();
        }
        /**************************************************************************/


        doc->setFileName(fileName);
        addDocument(doc);
        doc->palette_col = ui->palette->currentColumn();
    }
    else{  // 동일한 row 선택되어잇는데 선택
       final_scale=1;
        //on_Save_clicked();
        // closeDocument();

        QFile file(fileName);
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);

        control *doc=new control();
        QDir d = QFileInfo(fileName).absoluteDir();
        QString dirName=d.absolutePath();

        doc->setDirName(dirName);
        doc->classList=m_typelist;
        /**************************************************************************/

        QFileInfo fi(file);
        QString getBasename = fi.baseName();

      //  qDebug() << "z_row clicked";
     //   qDebug() << "basename" << getBasename;

        if(getBasename.endsWith("_seg")){
            doc->seg = 1;
            doc->m_fileName = fileName;
        }
        else
            doc->seg = 0;

     //   qDebug() << "result" << doc->seg;

        /**************************************************************************/

        /**************************************************************************/

        if(temp_filetype==1){

            doc->draw_pen=false;
            if(!doc->load(stream)){
                QMessageBox::warning(this, tr("parse error"), tr("failed to parse6\n%1").arg(fileName));
                delete doc;
                return;
            }
            //8.1 성빈
            rect_enable();

        }
        else{
            doc->load_img(fileName);

            //8.1 성빈
            brush_enable();
        }
        /**************************************************************************/

        doc->setFileName(fileName);
        addDocument(doc);
        doc->palette_col = ui->palette->currentColumn();
    }
    //updateAction();
   // qDebug() << "\r\n";
    prev_row=row;

}

void MainWindow::newDocument()
{
    addDocument(new control());


}

void MainWindow::on_palette_cellClicked(int row, int column)
{
    control *doc=currentDocument();

    //8.17성빈
    //doc->send_new=false;
    scrollArea->setCursor(Qt::ArrowCursor);

  //  qDebug() << "in palette clicked";
    id_change_signal=1;

    //9.1성빈
    if(doc->rect_flag==2){
        scrollArea->setCursor(Qt::CrossCursor);
        qDebug() << "1111 : " << doc->rect_flag;
    }
    if(prow==row&&pcol==column)
        return;
    //9.1 성빈

    prow=row;
    pcol=column;


    /**********************************************************/

    doc->palette_col = ui->palette->currentColumn();
//    doc->eraser_clicked = false;
    newcolor=ui->palette->item(row,column)->backgroundColor();

    //8.17성빈
    doc->currentcolor=newcolor;
    if(doc->rect_flag==2){
        scrollArea->setCursor(Qt::CrossCursor);
        qDebug() << "2222" << doc->rect_flag;
    }

    //9.1성빈
    className=m_typelist[column].m_c_name;
    doc->currenttype=className;
    //9.1성빈

    if(doc->brush_clicked==true && doc->fill_flag==false && doc->draw_pen==true){
        if(ui->brushsize->value()>=10)
            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(ui->brushsize->value(), ui->brushsize->value()));
        else
            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(10, 10));
    }
    else if(doc->brush_clicked==true && doc->fill_flag==true && doc->draw_pen==true)
        scrollArea->setCursor(QPixmap(":/new/prefix1/painter_cursor.png"));

    /**********************************************************/


    id_change=ui->palette->item(row,column)->text();
    color_change=ui->palette->item(row,column)->backgroundColor().name();
  //  qDebug() << "debugging in palette clicked" << id_change << color_change << doc->classList.count();

    if(!doc->classList.count())
        return;
  //  qDebug() << "in palette clicked22222222222";
    classNum=doc->classList.count();
    newcolor=doc->classList[column].m_c_color;
    doc->drawBrush(newcolor);

    if (doc == 0)
        return;

    QTableWidgetItem *currentItem = ui->palette->currentItem();
    //Q_ASSERT(currentItem);


    for(int i=0; i<classNum; i++){
 //        qDebug() <<i << "th debuging in on plaeete : " << doc->classList[i].c_name();
        if(i==column){
            QFont font;
            font.setStyleHint (QFont::Monospace);
            font.setPointSize (18);
            font.setFixedPitch (true);
            font.setBold(true);
 //           qDebug() << "before grad";
            QBrush brush(gradient(doc->classList[i].m_c_color));
            ui->palette->item(row, i)->setData(Qt::BackgroundRole, QBrush(brush));
            ui->palette->item(row, i)->setFont (font);
        }
        else{
            QFont font;
            font.setStyleHint (QFont::Monospace);
            font.setPointSize (9);
            font.setFixedPitch (false);
            font.setBold(false);
 //           qDebug() << doc->classList[i].m_c_color;
            ui->palette->item(row, i)->setBackgroundColor(doc->classList[i].m_c_color);
            ui->palette->item(row, i)->setFont (font);
        }
    }

 //   qDebug() << "before sendor";
    QString shapeName = doc->currentShapeName();
    if (shapeName.isEmpty())
        return;

    QColor color;
    if(sender()==ui->palette)
    {
        color=doc->classList[ui->palette->currentColumn()].m_c_color;
    }
    else
        return;

    if (color == doc->shape(shapeName).color())
        return;

}


void MainWindow::setShapeColor()
{

}

void MainWindow::addShape()
{
    qDebug()<<"im in addshape";

    control *doc=currentDocument();
    int lastnum;
    lastnum=doc->addedindex;
    zoom=1;


    check_index=doc->addedindex;

    if(doc==nullptr){
        return;}

    if(sender()==ui->bt_rec)
    {
        scrollArea->setCursor(Qt::CrossCursor);
        className=m_typelist[pcol].m_c_name;
    }

    QColor color;

    if(newcolor.isValid()) color=newcolor;
    else color=Qt::red; //default color는 red로 함.

    doc->drawShape(className, color, id_change);
  //  qDebug() << doc->send_new;

    change = 1;
    ui->Save->setEnabled(true); // 11.13 이지훈_save표시

}

void MainWindow::palette_clicked(int col)
{

}

void MainWindow::removeShape()
{
    control *doc = currentDocument();
    if (doc ==  nullptr)
        return;

    doc->selectedNum();

    for(int i=doc->m_shapeList.count()-1; i>=0; --i){
        if(doc->m_shapeList[i].m_seleted==1){
            QString shapeName = doc->m_shapeList[i].name();
            int shapeIndex=doc->indexOf(shapeName);
            doc->undoStack()->push(new RemoveShapeCommand(doc, shapeName, shapeIndex));
 //           qDebug() << i << "th is in removeshape : " << shapeName;

        }
    }
 //   qDebug() << doc->m_shapeList.count() << "done in removeshapep";
    update();
    select_flag=0;
}

void MainWindow::removeShapes()
{
    control *doc = currentDocument();
    if (doc ==  nullptr)
        return;
    change = 1;
    ui->Save->setEnabled(true); // 11.13 이지훈_save표시

    doc->deleteList.clear();
    doc->selectedNum();
    QList<Shape> list;

    for(int i=doc->m_shapeList.count()-1; i>=0; --i){
        if(doc->m_shapeList[i].m_seleted==1){
//            QString shapeName = doc->m_shapeList[i].name();
//            int shapeIndex=doc->indexOf(shapeName);
//            doc->undoStack()->push(new RemoveShapeCommand(doc, shapeName, shapeIndex));
 //           qDebug() << i << "th is in removeshape : " << doc->m_shapeList[i].name();
            Shape shape(doc->m_shapeList[i].type(), doc->m_shapeList[i].color(),
                        doc->m_shapeList[i].rect());
            shape.m_name=doc->m_shapeList[i].name();
            list.append(shape);
        }
    }
     doc->undoStack()->push(new RemoveShapesCommand(doc, list));

   // qDebug() << doc->m_shapeList.count() << "done in removeshapep";
    update();
    select_flag=0;
}

void MainWindow::on_remove_clicked()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            saveDocument();
        }
        else {

        }
    }

    if(ui->Left_table->rowCount()==0){
        return;
    }

    int c_doc_row;
    control *doc = currentDocument();

//   qDebug() << "currentDocument fileName"<<currentDocument()->fileName();

    for(int i=0; i<ui->Left_table->rowCount(); i++){
        if(currentDocument()->fileName()==ui->Left_table->item(i, 3)->text())
            c_doc_row = i;
    }
    delete doc;

    ui->Left_table->removeRow(c_doc_row);


    if(ui->Left_table->rowCount()==0){
        int num=ui->label_table->rowCount();
        for(int i=0; i<num; i++){
            ui->label_table->removeRow(ui->label_table->rowCount()-1);

        }
        //on_palette_cellClicked(0,0);
       // update();
        //return;
    }

    all_disable();


        if(c_doc_row==ui->Left_table->rowCount()){
            if(c_doc_row==0){
                ui->Current_file->setText("");
                return;
//            else if(c_doc_row==1)
            }
            else
                Left_table_clicked(c_doc_row-1, 0);
        }
        else
            Left_table_clicked(c_doc_row, 0);

        change = 0;
        ui->Save->setDisabled(true); // 11.13 이지훈_save표시

}

void MainWindow::on_Save_clicked()
{
    control *doc = currentDocument();
    qDebug() << "^^^^^^^^^" << doc->show_image;

    bool temp_show_image = doc->show_image;

    if(change==0)
        return;

    save_clicked=1;

    saveDocument();
    change = 0;

    Left_table_label();  // rect가 생성될 때마다 확인하여 label 유무 표시

    //8.15성빈
    if(fileType!=1&&prev_row==z_row)
        Left_table_clicked(prev_row, 0);
    if(fileType!=1){
        on_Brush_clicked();
    }

    control *save_doc = currentDocument();

    save_doc->show_image = temp_show_image;
    ui->show_image->setChecked(temp_show_image);

    ui->Save->setDisabled(true); // 11.13 이지훈_save표시

}

void MainWindow::on_Delete_clicked()
{
    control *doc=currentDocument();
    int selectedNum=doc->selectedNum();

    if(selectedNum==1)
        removeShape();
    else
        removeShapes();

    change=1;
    ui->Save->setEnabled(true); // 11.13 이지훈_save표시

}

void MainWindow::Left_table_label_default()
{
    if(ui->label_table->rowCount()>0){
        ui->Left_table->setItem(ui->Left_table->rowCount()-1, 2, new QTableWidgetItem("O"));
        ui->Left_table->item(ui->Left_table->rowCount()-1, 2)->setTextAlignment(Qt::AlignCenter);

    }
    else{
        ui->Left_table->setItem(ui->Left_table->rowCount()-1, 2, new QTableWidgetItem("X"));
        ui->Left_table->item(ui->Left_table->rowCount()-1, 2)->setTextAlignment(Qt::AlignCenter);
    }
}

void MainWindow::Left_table_label()
{
    int c_doc_row=0;
    control *doc = currentDocument();

    for(int i=0; i<ui->Left_table->rowCount(); i++){
        if(fileType==1){
            if(currentDocument()->fileName()==ui->Left_table->item(i, 3)->text())
                c_doc_row = i;
        }
        else {
            if(doc->imageName()==ui->Left_table->item(i, 3)->text())
                c_doc_row = i;
        }
//        qDebug() << currentDocument()->fileName() << ui->Left_table->item(i, 3)->text() <<c_doc_row;
    }




    if(ui->label_table->rowCount()>0){
        ui->Left_table->setItem(c_doc_row, 2, new QTableWidgetItem("O"));
        ui->Left_table->item(c_doc_row, 2)->setTextAlignment(Qt::AlignCenter);
        ui->Left_table->item(c_doc_row, 2)->setBackgroundColor(Qt::gray);  // 8.3 이지훈 추가
        ui->Left_table->item(c_doc_row, 2)->setForeground(Qt::white);  // 8.3 이지훈 추가

    }
    else{
        ui->Left_table->setItem(c_doc_row, 2, new QTableWidgetItem("X"));
        ui->Left_table->item(c_doc_row, 2)->setTextAlignment(Qt::AlignCenter);
        ui->Left_table->item(c_doc_row, 2)->setBackgroundColor(Qt::gray);  // 8.3 이지훈 추가
        ui->Left_table->item(c_doc_row, 2)->setForeground(Qt::white);  // 8.3 이지훈 추가1
    }
}

//**********************추가한 함수

void MainWindow::on_actionZoom_In_triggered()
{
    scaleImage(1.25);
}

void MainWindow::on_actionZoom_Out_triggered()
{
    scaleImage(0.8);
}

void MainWindow::scaleImage(double factor)
{
    zoom=1;
    //change=1;

    control *doc=currentDocument();

    scale_factor=doc->scaleFactor;
    final_scale=scale_factor*factor;

    doc->palette_col = ui->palette->currentColumn();
    doc->scaleFactor=scale_factor*factor;

    *(doc->temp_black)=doc->org_img;

    qDebug() << "in scaleimage " << doc->scaleFactor;

    if(fileType==1)
    {
        shape_count=doc->m_shapeList.count();
        for (int i = 0; i < shape_count; ++i){
            doc->scale_rect(i,factor);
        }
    }
//    this->update();
    doc->resize(doc->size()*factor);
    doc->scale_flag=1;
    doc->seg = doc->find_seg();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers().testFlag(Qt::ControlModifier)){
        scrollArea->verticalScrollBar()->setEnabled(false);
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Control){
            scrollArea->verticalScrollBar()->setEnabled(true);
    }
}


void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers().testFlag(Qt::ControlModifier)){
        control *doc=currentDocument();

        oldScale = doc->scaleFactor; // Get old scale factor
         // Set new scale, use QWheelEvent...


        if(event->delta()>0&&final_scale<2.5){
            newScale=1.25;
            //newScale=oldScale*newScale;
            ScrollbarPos = QPointF(scrollArea->horizontalScrollBar()->value(), scrollArea->verticalScrollBar()->value());
            DeltaToPos = event->posF() / oldScale - doc->pos() / oldScale;
            Delta = DeltaToPos * newScale - DeltaToPos * oldScale;
            scaleImage(1.25);
            qDebug() << "!!!!!!!!!!in here" <<ScrollbarPos <<  DeltaToPos << Delta <<  doc->pos() / oldScale;
        }
        else if(event->delta()<0&&final_scale>0.5)
        {
            newScale=0.8;
            //newScale=oldScale*newScale;
            ScrollbarPos = QPointF(scrollArea->horizontalScrollBar()->value(), scrollArea->verticalScrollBar()->value());
            DeltaToPos = event->posF() / oldScale - doc->pos() / oldScale;
            Delta = DeltaToPos * newScale - DeltaToPos * oldScale;
            scaleImage(0.8);
            qDebug() << "@@@@@@@@@@@in here" <<ScrollbarPos <<  DeltaToPos << Delta <<  doc->pos() / oldScale;
        }

        if(ScrollbarPos.x() + Delta.x()<0 && ScrollbarPos.y() + Delta.y()>0){
            scrollArea->horizontalScrollBar()->setValue(ScrollbarPos.x() - Delta.x());
            scrollArea->verticalScrollBar()->setValue(ScrollbarPos.y() + Delta.y());
        }
        else if(ScrollbarPos.x() + Delta.x()<0 && ScrollbarPos.y() + Delta.y()<0){
            scrollArea->horizontalScrollBar()->setValue(ScrollbarPos.x() - Delta.x());
            scrollArea->verticalScrollBar()->setValue(ScrollbarPos.y() - Delta.y());
        }
        else if(ScrollbarPos.x() + Delta.x()>0 && ScrollbarPos.y() + Delta.y()<0){
            scrollArea->horizontalScrollBar()->setValue(ScrollbarPos.x() + Delta.x());
            scrollArea->verticalScrollBar()->setValue(ScrollbarPos.y() - Delta.y());
        }
        else if(ScrollbarPos.x() + Delta.x()>0 && ScrollbarPos.y() + Delta.y()>0){
            scrollArea->horizontalScrollBar()->setValue(ScrollbarPos.x() + Delta.x());
            scrollArea->verticalScrollBar()->setValue(ScrollbarPos.y() + Delta.y());
        }
        qDebug() << "in here" << ScrollbarPos.x() + Delta.x() << ScrollbarPos.y() + Delta.y();
        //lastScale=newScale;
    }

    else {
        scrollArea->verticalScrollBar()->setEnabled(true);
    }
}

void MainWindow::label_table_changed(QString shapeName)
{
    control *doc=currentDocument();

   // qDebug() << "changed " << shapeName << "&" << doc->m_shapeIndex;
    for(int i=0; i<ui->label_table->rowCount(); i++){
        if(ui->label_table->item(i,1)->text()==shapeName){
            if(doc->m_shapeList[doc->indexOf(shapeName)].m_seleted==1 && !ui->label_table->item(i,1)->isSelected()){
                ui->label_table->item(i,0)->setSelected(true);
                ui->label_table->item(i,1)->setSelected(true);
                //update();
            }
        }
        else{
            ui->label_table->item(i,0)->setSelected(false);
            ui->label_table->item(i,1)->setSelected(false);
        }
            //qDebug() << ui->label_table->rowCount() << "label_table_changed " << i << "th" << "&" << doc->m_shapeList.at(i).name();
    }
    update();
//    qDebug() << "inlabelchanged done" << endl;
}

void MainWindow::on_label_table_cellPressed(int row, int column)
{
    control *doc=currentDocument();
    QString name=ui->label_table->item(row, 1)->text();
   // qDebug() << name << "is pressed";
    doc->selectedShape(name);


}

void MainWindow::on_bt_load_images_clicked()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            //8.15성빈
            //saveDocument();
            on_Save_clicked();
        }
        else {

        }
    }

    openImageDocument();
}

void MainWindow::on_Brush_clicked()
{
    zoom=0;
//    ui->Brush->setChecked(true);
//    ui->Painter->setChecked(false);

    control *doc=currentDocument();

    qDebug() << "$$$$$$$$$$$$$$$$$$$$$$$$$";
    qDebug() << "brush_clicked" << doc->brush_clicked;
    qDebug() << "fill_flag" << doc->fill_flag;
    qDebug() << "eraser_clicked" << doc->eraser_clicked;
    qDebug() << "show_image" << doc->show_image;
    qDebug() << "draw_pen" << doc->draw_pen;
    qDebug() << "$$$$$$$$$$$$$$$$$$$$$$$$";

    if(doc->brush_clicked==true && doc->fill_flag==0){
        doc->brush_clicked = false;
        doc->draw_pen = false;
        doc->show_image = false;
        doc->eraser_clicked = false;
        doc->fill_flag = 0;

        ui->Brush->setChecked(false);
        ui->show_image->setChecked(false);
        ui->Painter->setChecked(false);
        ui->Eraser->setChecked(false);

        scrollArea->setCursor(Qt::ArrowCursor);
    }
    else if(doc->draw_pen==false){
        doc->brush_clicked = true;
        doc->fill_flag = 0;
        doc->show_image = true;
        doc->draw_pen = true;

        ui->Brush->setChecked(true);
        ui->show_image->setChecked(true);
    }
    else {
        doc->brush_clicked = true;
        doc->draw_pen=true;
        doc->fill_flag = 0;

        ui->Brush->setChecked(true);
        ui->Painter->setChecked(false);

        if(newcolor.isValid()) doc->drawBrush(newcolor);
        else doc->drawBrush(Qt::red);
    }

    doc->save_flag=1;

    if(doc->brush_clicked==true){
        doc->spinbox_changed_value = ui->brushsize->value();

        if(ui->brushsize->value()>=10)
            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(ui->brushsize->value(), ui->brushsize->value()));
        else
            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(10, 10));
    }
    else
        scrollArea->setCursor(Qt::ArrowCursor);

    qDebug() << "##############################";
    qDebug() << "brush_clicked" << doc->brush_clicked;
    qDebug() << "fill_flag" << doc->fill_flag;
    qDebug() << "eraser_clicked" << doc->eraser_clicked;
    qDebug() << "show_image" << doc->show_image;
    qDebug() << "draw_pen" << doc->draw_pen;
    qDebug() << "##############################";

}

void MainWindow::on_Eraser_clicked()
{
    control *doc = currentDocument();

//    if(doc->brush_clicked==true)  // 8.3 이지훈 수정
//        ui->Eraser->setChecked(true);  // 8.3 이지훈 수정
//    else  // 8.3 이지훈 수정
//        ui->Eraser->setChecked(false);  // 8.3 이지훈 수정

    if(doc->eraser_clicked==true){
        doc->eraser_clicked = false;
        ui->Eraser->setChecked(false);
    }
    else if(doc->draw_pen==false){
        doc->draw_pen = true;
        doc->brush_clicked = true;
        doc->show_image = true;
        doc->eraser_clicked = true;

        ui->Brush->setChecked(true);
        ui->Eraser->setChecked(true);
        ui->show_image->setChecked(true);
    }
    else {
        doc->eraser_clicked = true;
        ui->Eraser->setChecked(true);

        if(doc->fill_flag==0)
            ui->Brush->setChecked(true);
        else
            ui->Painter->setChecked(true);
    }

    if(doc->fill_flag==0){
        doc->draw_pen = true;

        if(ui->brushsize->value()>=10)
            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(ui->brushsize->value(), ui->brushsize->value()));
        else
            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(10, 10));
    }
    else
        scrollArea->setCursor(QPixmap(":/new/prefix1/painter_cursor.png"));

    qDebug() << "##############################";
    qDebug() << "brush_clicked" << doc->brush_clicked;
    qDebug() << "fill_flag" << doc->fill_flag;
    qDebug() << "eraser_clicked" << doc->eraser_clicked;
    qDebug() << "show_image" << doc->show_image;
    qDebug() << "draw_pen" << doc->draw_pen;
    qDebug() << "##############################";

}

void MainWindow::on_Painter_clicked()
{
    control *doc=currentDocument();

    if(doc->fill_flag==1){
        doc->brush_clicked = false;
        doc->draw_pen = false;
        doc->show_image = false;
        doc->eraser_clicked = false;
        doc->fill_flag = 0;

        ui->Brush->setChecked(false);
        ui->show_image->setChecked(false);
        ui->Painter->setChecked(false);
        ui->Eraser->setChecked(false);

        scrollArea->setCursor(Qt::ArrowCursor);
    }
    else if(doc->draw_pen==false){
        doc->fill_flag = 1;
        doc->draw_pen = true;
        doc->brush_clicked = true;
        doc->show_image = true;

        ui->Painter->setChecked(true);
        ui->show_image->setChecked(true);
    }
    else {
        doc->fill_flag = 1;
        ui->Painter->setChecked(true);
        ui->Brush->setChecked(false);
    }

//    if(doc->brush_clicked==true && doc->draw_pen==true){
//        if(ui->brushsize->value()>=10)
//            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(ui->brushsize->value(), ui->brushsize->value()));
//        else
//            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(10, 10));
//    }
//    else

    if(doc->fill_flag==1)
        scrollArea->setCursor(QPixmap(":/new/prefix1/painter_cursor.png"));
    else
        scrollArea->setCursor(Qt::ArrowCursor);

    qDebug() << "##############################";
    qDebug() << "brush_clicked" << doc->brush_clicked;
    qDebug() << "fill_flag" << doc->fill_flag;
    qDebug() << "eraser_clicked" << doc->eraser_clicked;
    qDebug() << "show_image" << doc->show_image;
    qDebug() << "draw_pen" << doc->draw_pen;
    qDebug() << "##############################";

}

void MainWindow::on_brushsize_valueChanged(int arg1)
{
    control *doc = currentDocument();

    doc->spinbox_changed_value = arg1;

    if(doc->brush_clicked==true){
        if(ui->brushsize->value()>=10)
            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(ui->brushsize->value(), ui->brushsize->value()));
        else
            scrollArea->setCursor(QPixmap(":/new/prefix1/draw_cursor.png").scaled(10, 10));
    }
  //  qDebug() << "spinbox_change" << doc->spinbox_changed_value;
}

void MainWindow::on_show_image_clicked()
{
    control *doc = currentDocument();

    doc->opacity_spinbox_changed_value = ui->opacity->value();

    if(doc->show_image==false){
        doc->show_image = true;
        ui->show_image->setChecked(true);
    }
    else{
        doc->show_image = false;
        ui->show_image->setChecked(false);
    }

    qDebug() << "show_image" << doc->show_image;

    update();
}

void MainWindow::on_opacity_valueChanged(int arg1)
{
    control *doc = currentDocument();

    doc->opacity_spinbox_changed_value = arg1;
}


void MainWindow::on_bt_check_clicked()
{
    control *doc=currentDocument();
//    for(int i=0; i<doc->m_shapeList.count(); i++){
//        qDebug() <<i<< "th check => " << doc->m_shapeList[i].type() << doc->m_shapeList[i].name();
//    }
 //   qDebug() <<  doc->find_seg();
//    qDebug() << "\r\n";
}


//8.1 성빈
void MainWindow::brush_enable()
{
    ui->bt_rec->setDisabled(true);
    ui->Delete->setDisabled(true);
    ui->Brush->setEnabled(true);
    ui->Painter->setEnabled(true);
    ui->show_image->setEnabled(true);
    ui->Eraser->setEnabled(true);
    ui->Save->setEnabled(true);
    ui->brushsize->setEnabled(true);
    ui->opacity->setEnabled(true);
}
void MainWindow::rect_enable()
{
    ui->bt_rec->setEnabled(true);
    ui->Delete->setEnabled(true);
    ui->Brush->setDisabled(true);
    ui->Painter->setDisabled(true);
    ui->show_image->setDisabled(true);
    ui->Eraser->setDisabled(true);
    ui->Save->setEnabled(true);
    ui->brushsize->setDisabled(true);
    ui->opacity->setDisabled(true);
}
void MainWindow::all_disable()
{
    ui->bt_rec->setDisabled(true);
    ui->Delete->setDisabled(true);
    ui->Brush->setDisabled(true);
    ui->Painter->setDisabled(true);
    ui->show_image->setDisabled(true);
    ui->Eraser->setDisabled(true);
    ui->Save->setDisabled(true);
    ui->brushsize->setDisabled(true);
    ui->opacity->setDisabled(true);
}

void MainWindow::bt_set(int type)
{
//    control *doc=currentDocument();

//    if(type==1) //txt
//    {
//        ui->bt_rec->setEnabled(true);
//        ui->Delete->setEnabled(true);

//        ui->Brush->setEnabled(false);
//        ui->Painter->setEnabled(false);
//        ui->Eraser->setEnabled(false);
//        ui->show_image->setEnabled(false);
//    }
//    else {
//        ui->bt_rec->setEnabled(false);
//        ui->Delete->setEnabled(false);

//        ui->Brush->setEnabled(true);
//        ui->Painter->setEnabled(true);
//        ui->Eraser->setEnabled(true);
//        ui->show_image->setEnabled(true);
//    }
}

void MainWindow::button_checked(QString buttonName)
{
    //9.1성빈
    control *doc=currentDocument();

    if(buttonName=="bt_rec_clicked"&&ui->bt_rec->isChecked()==false)
        ui->bt_rec->setChecked(false);
    else if(buttonName=="bt_rec_clicked"&&doc->send_new==false)
        ui->bt_rec->setChecked(false);
    //9.1성빈
    else if(buttonName=="eraser_clicked"){
        ui->Brush->setChecked(false);
        ui->Painter->setChecked(false);
    }
    else if(buttonName=="painter_clicked"){
        ui->Brush->setChecked(false);
        ui->Eraser->setChecked(false);
    }
}

void MainWindow::esc()
{
    control *doc=currentDocument();

    //9.1성빈
    doc->send_new=false;
    //9.1성빈
    doc->rect_flag = false;

    if(doc->draw_pen==true){
        doc->brush_clicked = false;
        doc->draw_pen = false;
        doc->show_image = false;
        doc->eraser_clicked = false;
        doc->fill_flag = 0;

        ui->Brush->setChecked(false);
        ui->show_image->setChecked(false);
        ui->Painter->setChecked(false);
        ui->Eraser->setChecked(false);

        scrollArea->setCursor(Qt::ArrowCursor);
    }
//    ui->Brush->setChecked(false);
//    ui->Brush->released();
    ui->bt_rec->setChecked(false);
    ui->bt_rec->released();
    doc->setdrawShape();

    qDebug() << "##############################";
    qDebug() << "brush_clicked" << doc->brush_clicked;
    qDebug() << "fill_flag" << doc->fill_flag;
    qDebug() << "eraser_clicked" << doc->eraser_clicked;
    qDebug() << "show_image" << doc->show_image;
    qDebug() << "draw_pen" << doc->draw_pen;
    qDebug() << "##############################";
}

void MainWindow::allSelect()
{
    control *doc=currentDocument();
    if(select_flag==0 || doc->selectedNum()<doc->m_shapeList.count()-1){
        doc->setShape();
        doc->setCurrentShape(doc->m_shapeList.count()-1);
        for(int i=0; i<doc->m_shapeList.count(); i++){
            QString name=ui->label_table->item(i, 1)->text();
            if(doc->m_shapeList[i].m_seleted!=1){
                doc->selectedShape(name);
                ui->label_table->item(i,0)->setSelected(true);
                ui->label_table->item(i,1)->setSelected(true);
            }
        }
    }

    select_flag=1;


   // qDebug() << name << "is pressed";

}

void MainWindow::on_actionReload_configuration_file_triggered()
{
    conf=1;
    load_class();
}

void MainWindow::on_actionLoad_txt_files_triggered()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            saveDocument();
        }
        else {

        }
    }

    openFileDocument();
}

void MainWindow::on_actionLoad_txt_folder_triggered()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            saveDocument();
        }
        else {

        }
    }

    openFolderDocument();
}

void MainWindow::on_actionLoad_image_files_triggered()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            saveDocument();
        }
        else {

        }
    }

    openImageDocument();
}

void MainWindow::on_actionLoad_image_folder_triggered()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            saveDocument();
        }
        else {

        }
    }

    openFolderDocument_bmp();
}

void MainWindow::on_actionSave_triggered()
{
    if(change==0)
        return;

    save_clicked=1;

    saveDocument();
    change = 0;

    ui->Save->setDisabled(true); // 11.13 이지훈_save표시

    Left_table_label();  // rect가 생성될 때마다 확인하여 label 유무 표시

    if(fileType!=1)
        Left_table_clicked(prev_row, 0);
}

void MainWindow::on_actionRemove_file_triggered()
{
    if (change==1 && ui->Left_table->rowCount()!=0) {
        int button
            = QMessageBox::warning(this,
                            tr("Unsaved changes"),
                            tr("Would you like to save this document?\n%1").arg(currentDocument()->fileName()),
                            QMessageBox::Yes, QMessageBox::No);
        if (button == QMessageBox::Yes){
            saveDocument();
        }
        else {

        }
    }
    if(ui->Left_table->rowCount()==1)  // 8.3 이지훈 추가
        ui->remove->setDisabled(true);  // 8.3 이지훈 추가

    if(ui->Left_table->rowCount()==0){
        return;
    }

    int c_doc_row;
    control *doc = currentDocument();

//   qDebug() << "currentDocument fileName"<<currentDocument()->fileName();

    for(int i=0; i<ui->Left_table->rowCount(); i++){
        if(currentDocument()->fileName()==ui->Left_table->item(i, 3)->text())
            c_doc_row = i;
    }
    delete doc;

    ui->Left_table->removeRow(c_doc_row);


    if(ui->Left_table->rowCount()==0){
        int num=ui->label_table->rowCount();
        for(int i=0; i<num; i++){
            ui->label_table->removeRow(ui->label_table->rowCount()-1);

        }
        //on_palette_cellClicked(0,0);
       // update();
        //return;
    }

    all_disable();


        if(c_doc_row==ui->Left_table->rowCount()){
            if(c_doc_row==0){
                ui->Current_file->setText("");
                return;
//            else if(c_doc_row==1)
            }
            else
                Left_table_clicked(c_doc_row-1, 0);
        }
        else
            Left_table_clicked(c_doc_row, 0);

        change=0;
        ui->Save->setDisabled(true); // 11.13 이지훈_save표시

}

void MainWindow::on_bt_max_clicked()
{
    control *doc=currentDocument();
    //get scrollArea size
    double x=scrollArea->size().width()-40;
    double y=scrollArea->size().height();
    double s=pow(x,2)+pow(y,2);
    s=sqrt(s);

    //get doc size
    double x_doc=doc->size().width();
    double y_doc=doc->size().height();
    double s_doc=pow(x_doc,2)+pow(y_doc,2);
    s_doc=sqrt(s_doc);

    if(x_doc>x || y_doc>y){
        doc->scaleFactor=s_doc/s;
        qDebug() << "in 111111111111 bt max clicked" << doc->scaleFactor;
    }
    //scalefactor is s/s_doc
    else{
        doc->scaleFactor=s/s_doc;
        qDebug() << "in 222222222222222 bt max clicked" << doc->scaleFactor;
    }
    doc->update();

    if(fileType==1)
    {
        shape_count=doc->m_shapeList.count();

        for (int i = 0; i < shape_count; ++i){
            doc->scale_rect(i,doc->scaleFactor);
        }
        this->update();
    }
    doc->resize(doc->size()*(doc->scaleFactor));
    doc->adjustSize();
}

void MainWindow::on_actionShortcuts_triggered()  // 8.14 이지훈 'Shortcuts, Program Information 작동 안함' 수정 -> trigger 함수 생성
{
    QMessageBox::information(this,
                        tr("Shortcuts"),
                         tr("Ctrl + S : Save\n"
                            "Ctrl + C : Copy (only Detection)\n"
                            "Ctrl + V : Paste (only Detection)\n"
                            "Ctrl + A : Select All (only Detection)\n"
                            "Ctrl + Z : Undo\n"
                            "Ctrl + Y : Redo\n\n"

                            "Delete : Delete (only Detection)\n"
                            "Esc : Cancel\n\n"

                            "Number 1 : New Box (only Detection)\n"
                            "Number 2 : Brush (only Segmentation)\n"
                            "Number 3 : Painter (only Segmentation)\n"
                            "Number 4 : Eraser (only Segmentation)\n"
                            "Number 5 : Show Image (only Segmentation)"
                            ));
}

void MainWindow::on_actionAbout_Program_Creator_triggered()
{
    QPixmap pixmap(":/new/prefix1/Logo.png");

    QMessageBox information;
    information.setWindowTitle("About Program Creator");
    information.setText(" Image Annotator 1.1");
    information.setInformativeText(" Based on Qt 5.12.0(MinGW 2015, 64bit)\n\n"
                                   " Build on Aug 15 2019\n\n"
                                   " Created By Kang Yi, Yeil Kim, Sung Bin Kim, Ji Hoon Lee\n\n"
                                   " The program is designed to CREATE and MODIFY\n  learning data for machine learning.");
    information.setIconPixmap(pixmap);
    information.setStandardButtons(QMessageBox::Close);

    information.exec();
}

void MainWindow::on_remove_files_clicked()
{
    int total_row = ui->Left_table->rowCount();

    qDebug() << "^^^^^^^^^^6" << total_row;

    for(int i=total_row-1; i>=0; i--){
        Left_table_clicked(i,1);
        control *doc = currentDocument();
        ui->Left_table->removeRow(i);
        delete doc;
    }

    if(ui->Left_table->rowCount()==0){
        int num = ui->label_table->rowCount();
        for(int i=0; i<num; i++)
            ui->label_table->removeRow(ui->label_table->rowCount()-1);
    }
    ui->Current_file->setText("");
    all_disable();
    change = 0;
    ui->Save->setDisabled(true); // 11.13 이지훈_save표시
}
