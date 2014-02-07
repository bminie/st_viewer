 #include "CellGLView.h"

#include <QGLPainter>
#include <QGLWidget>
#include <QGLSceneNode>
#include <QGLBuilder>
#include <QGeometryData>
#include <QArray>
#include <QGLCube>
#include <QGLMaterial>
#include <QGLCamera>
#include <QWheelEvent>
#include <QRay3D>
#include <QGLTexture2D>
#include <QtOpenGL>

CellGLView::CellGLView(QWindow *parent) :
    QGLView(parent)
{

}

CellGLView::CellGLView(const QSurfaceFormat &format, QWindow *parent) :
    QGLView(format,parent)
{

}

CellGLView::~CellGLView()
{
    //NOTE View does not own rendering nodes nor texture
}

void CellGLView::initializeGL(QGLPainter *painter)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //maybe better in paintGL
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
    glShadeModel(GL_FLAT);
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    painter->setClearColor(Qt::black);

    camera()->setProjectionType(QGLCamera::Orthographic);
    camera()->setMinViewSize(QSizeF(600.0,600.0));
    camera()->setViewSize(QSizeF(1200.0,1200.0));
    camera()->setAdjustForAspectRatio(true);
    camera()->setFieldOfView(0.0);
    camera()->setNearPlane(-1.0);
    camera()->setFarPlane(1.0);
    camera()->setEyeSeparation(0.0); //2D viewport
    camera()->setUpVector(QVector3D(0.0, 1.0, 0.0));
    camera()->setCenter(QVector3D(0.0, 0.0, 0.0));
    camera()->setEye(QVector3D(0.0, 0.0, 1.0));
}

void CellGLView::paintGL(QGLPainter *painter)
{

    // draw texture
    if ( m_texture != nullptr ) {
        m_texture->draw(painter);
    }

    // draw rendering nodes
    foreach(QGLSceneNode *node, m_nodes) {
        node->draw(painter);
    }

    glFlush(); // forces to send the data to the GPU saving time
}

void CellGLView::rotate()
{
    update();
}

void CellGLView::resizeGL(int width, int height)
{
    //devicePixelRatio() fix the problem with MAC retina
    qreal pixelRatio = devicePixelRatio();
    QGLView::resizeGL(width * pixelRatio, height * pixelRatio);
}

void CellGLView::wheelEvent(QWheelEvent* event)
{
    setZoom(qreal(event->delta()));
    event->ignore();
}

void CellGLView::addRenderingNode(QGLSceneNode *node)
{
    Q_ASSERT(node != nullptr);
    m_nodes.append(node);
    connect(node, SIGNAL(updated()), this, SLOT(update()));
}

void CellGLView::removeRenderingNode(QGLSceneNode *node)
{
    Q_ASSERT(node != nullptr);
    m_nodes.removeOne(node);
    disconnect(node, SIGNAL(updated()), this, SLOT(update()));
}

void CellGLView::setBackGroundTexture(QGLSceneNode *node)
{
    Q_ASSERT(node != nullptr);
    m_texture = node;
}

void CellGLView::setZoom(qreal delta)
{
    camera()->setViewSize( camera()->viewSize() * ( 1 - 0.001 * delta ) );
}

/*
void CellGLView::slotCenterOn(const QPointF& point)
{
    centerOn(point);
}*/

const QImage CellGLView::grabPixmapGL() const
{
    return QImage();
}