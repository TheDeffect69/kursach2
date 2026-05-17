#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDomDocument>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openFile();

private:
    void parseAndDrawXml(const QString &fileName);

    struct NodeData {
        QString name;
        int depth;
        int x;
        int y;
        int width;
        int height;
        QList<NodeData*> children;
        NodeData* parent;
    };

    NodeData* buildTree(const QDomElement &element, int depth, NodeData* parent);
    void calculateLayout(NodeData* node, int &currentX);
    void drawTree(NodeData* node);
    void freeTree(NodeData* node);

    QGraphicsScene *scene;
    QGraphicsView *view;

    const int ITEM_WIDTH = 150;
    const int ITEM_HEIGHT = 80;
    const int HORIZONTAL_SPACING = 30;
    const int VERTICAL_SPACING = 60;
};

#endif // MAINWINDOW_H
