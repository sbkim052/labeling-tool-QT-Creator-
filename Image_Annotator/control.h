#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include <QTextStream>
#include <QPixmap>
#include <QMainWindow>
#include <QUndoCommand>
#include <QRubberBand>
#include <QFile>

QT_FORWARD_DECLARE_CLASS(QUndoStack)
QT_FORWARD_DECLARE_CLASS(QTextStream)

class Type
{
public:
    explicit Type(QString &className, int &classId, QColor &classColor);
    QString c_name() const;
    QColor c_color() const;
    int c_id() const;

    int typeNum();
    QColor typeToColor(QString &className);
    QString m_c_name;
    int m_c_num;
    QColor m_c_color;

private:

friend class control;
friend class Shape;

};

class Shape
{
public:
    explicit Shape(const QString &type="Car", const QColor &color=Qt::red, const QRect &rect=QRect(), const int selected=0);

    QString type() const;
    QString name() const;
    QRect rect() const;
    QRect resizeHandle(int i) const;
    QColor color() const;

    int selected() const;
    static const QSize minSize;

    QString m_type;
    QColor m_color;
    QRect m_rect;
    QString m_name;

private:

    friend class control;

public:
    int m_seleted; //if shape is selected, m_selected num is 1, otherelse it's 0
};

class control:public QWidget
{
    Q_OBJECT

public:
    control(QWidget *parent=nullptr);
    QList<Type> classList;
    QList<Shape> deleteList;
    QColor colorOfType(QString &className);

    void addClass(const Type &type);

    QString addShape(const Shape &shape);
    QList<Shape> addShapes(QList<Shape> &deletedList);
    void addedNum(int num);
    void deleteShape(const QString &shapeName);
    QList<Shape> deleteShapes(QList<Shape> &deletedList);
    Shape shape(const QString &shapeName) const;
    QString currentShapeName() const;
    QString currentShapeClass() const;

    void setShapeRect(const QString &shapeName, const QRect &rect);
    void setShapeColor(const QString &shapeName, const QColor &color);

    bool load(QTextStream &stream);
    bool load_img(const QString &fileName);
    void save(QTextStream &stream);

    void save_img();
    void save_zoom();

    QString dirName() const;
    void setDirName(const QString &dirName);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString imageName() const;
    void setImageName(const QString &ImageName);

    bool openImage(const QString &fileName);
    void setImage(const QImage &image);
    //void setImage(const QPixmap &image);

    void currentShape(QString &idname);  //ㅊㄱ
    void currentColor(QColor &colorname);  //ㅊㄱ


    QImage image() const { return theImage; }
    QSize sizeHint() const override;
    QList<Shape> m_shapeList;
    QString m_dirName;
    static int shapeindex;

    void scale_rect(int i, double scale);

//    bool drawShape(Shape::Type newshape, QColor newcolor, QString shape_str);  //ㅊㄱ
    bool drawShape(QString newclass, QColor newcolor, QString shape_str);  //ㅊㄱ
    bool drawBrush(QColor newcolor);

    bool send_new=false;  //ㅊㄱ
    bool draw_resizehandle;
    int pre_shape=-1;
    QPoint m_lastpos;  //ㅊㄱ
    QPoint m_pos;  //ㅊㄱ

   // Shape::Type currenttype;  //ㅊㄱ
    QString currenttype;  //ㅊㄱ

    QColor currentcolor;  //ㅊㄱ
    QString currenttype_str;  //ㅊㄱ
    bool draw_done;  //ㅊㄱ
    bool add_done;  //ㅊㄱ

    int addedindex;  //ㅊㄱ
    int deletedindex;  //ㅊㄱ
    QString redo_addshape; //ㅊㄱ
    int flag =0;
    int scale_flag=0;

    QUndoStack *undoStack() const;
    int lastindexOf(const QString &shapeName) const;

    //************추가
    double scaleFactor;

    void selectedShape(const QString &shapeName);
    void setShape();
    int selectedNum();

    int indexOf(const QString &shapeName) const;

    int m_shapeIndex;

    bool removed=false;
    QString deletedName;

    int biggestindexOf() const;
    QString imgBasename() const;

    QString m_fileName;
    QString m_imageName;
    //

    bool draw_pen = false;
    bool draw_spoid;

    QLine line;
    QMatrix line_map;

    int brushSize;
    QColor brushColor;

    int save_flag;
    QRegion line_region;

    void arrangeNum(int num);
    void setCurrentShape(int index);

    //copyrect
    int current_index();
    QString uniqueName(const QString &name) const;

    /*********************************/
    int spinbox_changed_value;
    int palette_col=0;
    int opacity_spinbox_changed_value;

    bool brush_clicked = false;
    bool eraser_clicked = false;
    bool show_image = false;
    QImage *temp_black;


    //7.30 성빈 추가
    int fill_flag=0;
    void floodfill(int x, int y, QRgb newcolor, QRgb oldcolor, int flag);
    /*********************************/

    int find_seg();

    QString find_extension(QString path);  // 8.3 이지훈 추가

//    bool brush_clicked_save = false;  // @@@@@@@@@@@@@@@@@이지훈 추가
    int seg;  // @@@@@@@@@@@@@@@@@이지훈 추가
    int zoom;

    int resize_release;

    void setdrawShape();
    int m_currentIndex;

    QString file_extension;

    uint find_pixel(QRgb pixelValue);
    int save_zoomed;
    QImage img2;

    double latestScale;

    //8.17성빈
    int rect_flag=0;

    QImage org_img;

signals:
    void currentShapeChanged(const QString &shapeName);
    void added();
    void deleted();
    void redo_added(QString shapename);
    void check_zoom();


    void cursor_arrow();
    void cursor_cross();

    void cursor_resizeall();
    void cursor_resize_r();
    void cursor_resize_l();
    void cursor_circle();

    /********************/
    void mouse_pressed();
    /********************/
    void sig_selected(QString shapeName);

    void button_checked(QString buttonName);


protected:
    virtual void paintEvent(QPaintEvent *event)override;
    virtual void mousePressEvent(QMouseEvent *event)override;
    virtual void mouseReleaseEvent(QMouseEvent *event)override;
    virtual void mouseMoveEvent(QMouseEvent *event)override;

private:

    int indexAt(const QPoint &pos) const;




    int m_mousePressIndex;


    //for resize handler
    bool draw_newrect;  //ㅊㄱ
    QPoint m_mousePressOffset;
    bool m_resizeHandlePressed_tl;
    bool m_resizeHandlePressed_tr;
    bool m_resizeHandlePressed_bl;
    bool m_resizeHandlePressed_br;


    QPixmap pix;

    control *m_doc;
    QString m_shapeName;
    QRect m_oldRect;
    QRect m_newRect;

    QImage theImage;
    QImage img_save;
    QUndoStack *m_undoStack;
    QRubberBand *rband;

    QPoint m_fixedpoint;
    QPoint m_default;
    int m_first_click;
};

class AddShapeCommand: public QUndoCommand
{
public:
    AddShapeCommand(control *doc, const Shape &shape, QUndoCommand *parent=0);
    void undo() override;
    void redo() override;

private:
    control *m_doc;
    Shape m_shape;
    QString m_shapeName;
    double m_scale;
};

class RemoveShapeCommand : public QUndoCommand
{
public:
    RemoveShapeCommand(control *doc, const QString &shapeName, int &shapeIndex, QUndoCommand *parent = 0);
    void undo() override;
    void redo() override;

private:
    control *m_doc;
    Shape m_shape;
    QString m_shapeName;
    int m_shapeIndex;
    double m_scale;
};

class RemoveShapesCommand : public QUndoCommand
{
public:
    RemoveShapesCommand(control *doc, QList<Shape> deleted_shapes, QUndoCommand *parent = 0);
    void undo() override;
    void redo() override;

private:
    control *m_doc;
    Shape m_shape;
    QString m_shapeName;
    int m_shapeIndex;
    QList<Shape> m_deletedList;
    QList<Shape> m_shapeList;
    double m_scale;
};


class SetShapeRectCommand : public QUndoCommand
{
public:
    SetShapeRectCommand(control *doc, const QString &shapeName, const QRect &rect, QUndoCommand *parent=0);

       void undo() override;
       void redo() override;

       bool mergeWith(const QUndoCommand *command) override;
       int id() const override;
private:
    control *m_doc;
    QString m_shapeName;
    QRect m_oldRect;
    QRect m_newRect;
    double m_scale;
};

class SetShapeColorCommand : public QUndoCommand
{
public:
    SetShapeColorCommand(control *doc, const QString &shapeName, const QColor &color,
                            QUndoCommand *parent = 0);

    void undo() override;
    void redo() override;

    bool mergeWith(const QUndoCommand *command) override;
    int id() const override;

private:
    control *m_doc;
    QString m_shapeName;
    QColor m_oldColor;
    QColor m_newColor;
};


#endif // CONTROL_H
