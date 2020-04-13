#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QScrollArea>
#include <QUndoGroup>
#include <QCursor>
#include <QResizeEvent>
#include <QWheelEvent>
#include <control.h>
#include <QScrollArea>

class control;
class QScrollArea;


namespace Ui {
class MainWindow;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event) override;  // 8.3 이지훈 추가

public:
    explicit MainWindow(QWidget *parent = nullptr);

    QList<Type> m_typelist; //list for saving classes in conf_class.txt
    QList <Shape> m_copylist;
    void load_bg(QPixmap pix);
    void addDocument(control *doc);
    void removeDocument(control *doc);
    void setCurrentDocument(control *doc);
    void scaleImage(double factor);
    QString conf_class;
    void load_class();
    int classNum;
    QString className;
    QColor classColor;
    control *currentDocument() const;
    int f;
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);
    double oldScale=0;
    double newScale=1;

    QPointF ScrollbarPos;
    QPointF DeltaToPos;
    QPointF Delta;

    void setfileType(QString &t_file);
    int fileType=0; //1:text , 2:img
    void reload();
    int prev_row=0;
    int zoom=0;
    int first_load=0;
    ~MainWindow();


    //8.1 성빈
    void brush_enable();
    void rect_enable();
    void all_disable();

    void bt_set(int type);
    int conf;
    int select_flag;
    int save_clicked;

    int lastScale;


public slots:
    void adjustBg(QString bgname);

    void openFileDocument();
    void openFolderDocument();
    void openFolderDocument_bmp();
    void openDoc_Refac(QStringList fileNames, control *doc, QDir d, QString dirName);
    void saveDocument();
    void closeDocument();
    void newDocument();

    void addShape();
    void removeShape();
    void removeShapes();
    void setShapeColor();

    void addonList();
    void setonList();


    void ArrowCursor();
    void SizeFCursor();
    void SizeBCursor();
    void ResizeallCursor();
    void CrossCursor();
    void CircleCursor();

    /********************/
        void change_temp();
    /********************/
    void label_table_changed(QString shapeName);

    void openImageDocument();

    void copyRect();
    void pasteRect();


    void button_checked(QString buttonName);

    int send_zoom();

    void esc();
    void allSelect();


private slots:
    void on_bt_load_clicked();
    void on_bt_load_folder_txt_clicked();

    void updateAction();

    void on_bt_rec_clicked();
    void Left_table_clicked(int row, int col);
    void on_palette_cellClicked(int row, int column);
    void palette_clicked(int col);


    void on_remove_clicked();

    void on_Save_clicked();

    void on_Brush_clicked();

    void Left_table_label();

    void Left_table_label_default();

    void on_Delete_clicked();

    void on_actionZoom_In_triggered();

    void on_actionZoom_Out_triggered();

    void on_label_table_cellPressed(int row, int column); //ㅊㄱ

    void on_bt_imgload_clicked();

    void on_bt_load_images_clicked();

    void on_Eraser_clicked();

    void on_Painter_clicked();

    void on_bt_check_clicked();

    void on_brushsize_valueChanged(int arg1);

    void on_show_image_clicked();

    void on_opacity_valueChanged(int arg1);

    void on_bt_load_folder_bmp_clicked();

    void on_actionReload_configuration_file_triggered();

    void on_actionLoad_txt_files_triggered();

    void on_actionLoad_txt_folder_triggered();

    void on_actionLoad_image_files_triggered();

    void on_actionLoad_image_folder_triggered();

    void on_actionSave_triggered();

    void on_actionRemove_file_triggered();

    void on_bt_max_clicked();

    void on_actionShortcuts_triggered();

    void on_actionAbout_Program_Creator_triggered();

    void on_remove_files_clicked();

private:
    Ui::MainWindow *ui;
    control *ctr;
    QScrollArea *scrollArea;
    QString conf_name;

    QImage img;
    QPixmap buffer;
    QString fixedWindowTitle(const control *doc) const;

    QWidget *bd;
    QGraphicsProxyWidget *prox;
    int change;
    int id_change_signal;
    QString id_change;
    QString color_change;
    QColor newcolor=Qt::red;
    int prow, pcol;
    int z_row, z_col;
    int shape_count;
    double final_scale;

    bool control_pressed = false;

    int check_index;
    int save_flag;

    QUndoGroup *m_undoGroup;

    QCursor crosscursor;
    double scale_factor;
};

#endif // MAINWINDOW_H
