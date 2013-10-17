/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms. 
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GENEPLOTTERGL_H
#define GENEPLOTTERGL_H

#include <QDebug>
#include "utils/DebugHelper.h"

#include <QGraphicsItem>

#include "qgl.h"
#include "QGLShaderProgram"

#include "GLCommon.h"
#include "data/GLElementData.h"
#include "data/GLElementShapeFactory.h"
#include "math/GLQuadTree.h"

#include "SelectionEvent.h"

#include "utils/Utils.h"

#include "controller/data/DataProxy.h"

// Specialized graphical object class used to visualize gene data in the
// cell view. Contains all necessary data to configure and display genes
// as well as the grid on which they exist.
class GenePlotterGL: public QGraphicsObject
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    
public:

    enum VisualMode
    {
        NormalMode,
        DynamicRangeMode,
        DynamicRangeModeGenes,
        HeatMapMode
    };
    
    explicit GenePlotterGL(QGraphicsItem* parent = 0);
    virtual ~GenePlotterGL();
   
    void clear(); // clear rendering data
    void reset(); // reset member variables

    void initGL();  // init opengl data
    void loadShader(const uint id, const QString &vertexPath, const QString &fragmentPath);

    inline virtual QRectF boundingRect() const { return m_border; }
    virtual QPainterPath shape() const;
    virtual bool contains(const QPointF& point) const;

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    virtual bool isObscuredBy(const QGraphicsItem* item) const;
    virtual QPainterPath opaqueArea() const;

    enum { Type = QGraphicsItem::UserType + 2 };
    inline virtual int type() const { return Type; }

    // member access
    inline Globals::Shape geneShape() const { return m_geneShape; }

    // update gene data
    void updateGeneData();
    // update chip size
    void updateChipSize();
    // load dataset trandformation matrix
    void updateTransformation();

    const QList<QString> selectedFeatureList() const;

    void selectAll(const DataProxy::GeneList &geneList);
    void selectAll(const DataProxy::FeatureList &featureList);

public slots:
    
    void setHitCount(int min, int max, int sum);

    // update selection
    void setSelectionArea(const SelectionEvent *event);

    // update gene render list
    void updateGeneColor(QScopedPointer<Gene> gene);
    void updateGeneSelection(QScopedPointer<Gene> gene);

    void setGeneVisible(bool geneVisible);
    void setGeneShape(int geneShape);
    void setGeneLimit(int geneLimit);
    void setGeneIntensity(int geneIntensity);
    void setGeneSize(int geneSize);

    void setGeneVisualMode(const VisualMode mode);
    
    void setGridVisible(bool gridVisible);
    void setGridColor(const QColor& gridColor);
    void setGridBorderColor(const QColor& gridBorderColor);
    void setGridLineSpace(qreal gridLineSpace);
    void setGridLineSize(qreal gridLineSize);

private:
    // chip grid stuff
    void setBoundingRect(const QRectF& rect);
    QRectF m_rect;      // the internal gene (x,y) area
    QRectF m_border;    // area including the chip border (not total)

    // grid data
    GL::GLElementData m_gridData;
    qreal m_gridLineSpace;
    qreal m_gridLineSize;
    bool m_gridVisible;
    QColor m_gridColor;
    QColor m_gridBorderColor;

    void rebuildGridData();
    void generateGridData();

    // gene plot data
    struct LookupData
    {
        enum IndexType { DataIndex = 0, IndexIndex };
        inline LookupData() { }
        inline LookupData(GL::GLindex vertexDataIndex, GL::GLindex indexDataIndex) : 
            vertexDataIndex(vertexDataIndex), 
            indexDataIndex(indexDataIndex), featureCount(0), refCount(0) { }
        GL::GLindex vertexDataIndex;
        GL::GLindex indexDataIndex;
        quint8 featureCount, refCount;
    };
    
    typedef QList<LookupData> GeneInfoList;
    // lookup maps
    typedef QHash<QString, GeneInfoList::size_type> GeneInfoByIdMap;
    typedef QPair<LookupData::IndexType, GeneInfoList::size_type> GeneInfoByIdxKey;
    typedef QHash<GeneInfoByIdxKey, GeneInfoList::size_type> GeneInfoByIdxMap;
    typedef QHash<GeneInfoList::size_type, QString> GeneInfoReverseMap;
    // lookup quadtree
    typedef GL::GLQuadTree<GeneInfoList::size_type,8> GeneInfoQuadTree;
    // selection set
    typedef QSet<GeneInfoList::size_type> GeneInfoSelectedSet;

    typedef QMap<uint, QGLShaderProgram *> ShaderProgramList;

    // gene visual data
    GL::GLElementData  m_geneData;
    // gene lookup data
    GeneInfoList m_geneInfo;
    GeneInfoByIdMap m_geneInfoById;
    GeneInfoByIdxMap m_geneInfoByIdx;
    GeneInfoReverseMap m_geneInfoReverse;
    GeneInfoQuadTree m_geneInfoQuadTree;
    // gene selection data
    GeneInfoSelectedSet m_geneInfoSelection;
    // shader
    ShaderProgramList m_geneProgramList;
    QGLShaderProgram *m_geneProgram;

    bool m_geneVisible;
    Globals::Shape m_geneShape;
    qreal m_geneIntensity;
    qreal m_geneSize;
    int m_geneLimit;

    bool m_geneNameVisible;
    QFont m_geneNameFont;
    bool m_geneNumberVisible;

    void generateGeneData();
    void updateGeneVisualData();
    void updateGeneSizeData();
    void rebuildGeneData();

    // color scheme
    // Strategy pattern to allow features to be rendered with different colors.
    class ColorScheme
    {
        public:
	        inline ColorScheme() {};
	        virtual inline ~ColorScheme() {};
            virtual QColor getColor(DataProxy::FeaturePtr feature) const = 0;
    };
    // render feature according to feature color
    class FeatureColor : public ColorScheme
    {
        public:
            FeatureColor();
            virtual QColor getColor(DataProxy::FeaturePtr feature) const;
    };
    // render feature with feature color but set alpha to normalized hit count 
    class DynamicRangeColor : public ColorScheme
    {
        public:
            DynamicRangeColor(int minHits, int maxHits);
            virtual QColor getColor(DataProxy::FeaturePtr feature) const;
        private:
            int m_minHits, m_maxHits;
    };
    // render feature with color from normalized hit count mapped to HSV color space
    class HeatMapColor : public ColorScheme
    {
        public:
            HeatMapColor(int minHits, int maxHits);
            virtual QColor getColor(DataProxy::FeaturePtr feature) const;
        private:
            int m_minHits, m_maxHits;
    };

    ColorScheme *m_colorScheme;
    VisualMode m_visualMode;
    int m_hitCountMin, m_hitCountMax, m_hitCountSum;
};

#endif // GENEPLOTTERGL_H
