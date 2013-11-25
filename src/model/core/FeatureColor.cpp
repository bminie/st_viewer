/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/
#include "FeatureColor.h"

FeatureColor::FeatureColor(int minHits, int maxHits) : ColorScheme(minHits, maxHits)
{

}

QColor FeatureColor::getColor(const DataProxy::FeaturePtr feature) const
{
    qreal alpha = (feature->hits() > m_maxHits || feature->hits() < m_minHits) ? 0.0 : 1.0;
    QColor adjusted = feature->color();
    adjusted.setAlphaF(alpha);
    return adjusted;
}
