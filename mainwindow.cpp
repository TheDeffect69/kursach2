#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("UML Deployment Diagram Viewer");
    resize(800, 600);

    scene = new QGraphicsScene(this);
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    setCentralWidget(view);

    QMenu *fileMenu = menuBar()->addMenu("&File");
    QAction *openAction = new QAction("&Open XML...", this);
    connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAction);

    QAction *exitAction = new QAction("E&xit", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);
}

MainWindow::~MainWindow()
{
}

void MainWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open XML File", "", "XML Files (*.xml);;All Files (*)");
    if (!fileName.isEmpty()) {
        parseAndDrawXml(fileName);
    }
}

void MainWindow::parseAndDrawXml(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file:\n" + file.errorString());
        return;
    }

    QDomDocument doc;
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!doc.setContent(&file, &errorStr, &errorLine, &errorColumn)) {
        QMessageBox::warning(this, "Parse Error",
                             QString("Parse error at line %1, column %2:\n%3")
                             .arg(errorLine).arg(errorColumn).arg(errorStr));
        file.close();
        return;
    }
    file.close();

    scene->clear();

    QDomElement rootElement = doc.documentElement();
    if (rootElement.isNull()) {
        QMessageBox::information(this, "Info", "The XML file is empty.");
        return;
    }

    if (rootElement.tagName() == "xml") {
        rootElement = rootElement.firstChildElement();
    }

    if (rootElement.isNull()) {
        return;
    }

    NodeData* root = buildTree(rootElement, 0, nullptr);
    if (root) {
        int currentX = 0;
        calculateLayout(root, currentX);
        drawTree(root);
        freeTree(root);

        // Center the scene
        view->setSceneRect(scene->itemsBoundingRect().marginsAdded(QMarginsF(50, 50, 50, 50)));
    }
}

MainWindow::NodeData* MainWindow::buildTree(const QDomElement &element, int depth, NodeData* parent)
{
    if (element.tagName() != "item") {
        return nullptr;
    }

    NodeData* node = new NodeData();
    node->name = element.attribute("name");
    node->depth = depth;
    node->parent = parent;
    node->width = ITEM_WIDTH;
    node->height = ITEM_HEIGHT;

    QDomNode n = element.firstChild();
    while (!n.isNull()) {
        QDomElement e = n.toElement();
        if (!e.isNull() && e.tagName() == "item") {
            NodeData* child = buildTree(e, depth + 1, node);
            if (child) {
                node->children.append(child);
            }
        }
        n = n.nextSibling();
    }

    return node;
}

void MainWindow::calculateLayout(NodeData* node, int &currentX)
{
    if (node->children.isEmpty()) {
        node->x = currentX;
        node->y = node->depth * (ITEM_HEIGHT + VERTICAL_SPACING);
        currentX += ITEM_WIDTH + HORIZONTAL_SPACING;
    } else {
        int startX = currentX;
        for (NodeData* child : node->children) {
            calculateLayout(child, currentX);
        }

        // Center parent above children
        int firstChildX = node->children.first()->x;
        int lastChildX = node->children.last()->x;
        node->x = (firstChildX + lastChildX) / 2;
        node->y = node->depth * (ITEM_HEIGHT + VERTICAL_SPACING);
    }
}

void MainWindow::drawTree(NodeData* node)
{
    // Draw connections to children first so they are behind the rectangles
    for (NodeData* child : node->children) {
        int parentBottomX = node->x + node->width / 2;
        int parentBottomY = node->y + node->height;
        int childTopX = child->x + child->width / 2;
        int childTopY = child->y;

        // Draw orthogonal lines
        int midY = parentBottomY + VERTICAL_SPACING / 2;

        QPen linePen(Qt::black, 2);

        // Line down from parent
        scene->addLine(parentBottomX, parentBottomY, parentBottomX, midY, linePen);

        // Horizontal line connecting them
        scene->addLine(parentBottomX, midY, childTopX, midY, linePen);

        // Line down to child
        scene->addLine(childTopX, midY, childTopX, childTopY, linePen);

        drawTree(child);
    }

    // Draw the node itself
    QGraphicsRectItem* rectItem = scene->addRect(node->x, node->y, node->width, node->height, QPen(Qt::black, 2), QBrush(Qt::white));

    // Special border for root node
    if (node->depth == 0) {
        QGraphicsRectItem* innerRect = scene->addRect(node->x + 5, node->y + 5, node->width - 10, node->height - 10, QPen(Qt::black, 1));
    }

    QGraphicsTextItem* textItem = scene->addText(node->name);

    // Center text
    QRectF textBounds = textItem->boundingRect();
    textItem->setPos(node->x + (node->width - textBounds.width()) / 2,
                     node->y + (node->height - textBounds.height()) / 2);
}

void MainWindow::freeTree(NodeData* node)
{
    for (NodeData* child : node->children) {
        freeTree(child);
    }
    delete node;
}
