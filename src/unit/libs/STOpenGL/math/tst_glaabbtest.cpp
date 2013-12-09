/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#include <QDebug>

#include <QtTest/QTest>

#include <math/GLAABB.h>
Q_DECLARE_METATYPE(GL::GLpoint)
Q_DECLARE_METATYPE(GL::GLaabb)
#include <GLQt.h>

#include "tst_glaabbtest.h"

namespace unit
{

GLAABBTest::GLAABBTest(QObject *parent) : QObject(parent) { }

void GLAABBTest::initTestCase()
{
    QVERIFY2(true, "Empty");
}
void GLAABBTest::cleanupTestCase()
{
    QVERIFY2(true, "Empty");
}

void GLAABBTest::testConstructFromPoints()
{
    QFETCH(GL::GLpoint, p0);
    QFETCH(GL::GLpoint, p1);
    QFETCH(GL::GLaabb, b);
    QFETCH(bool, expected);

    QCOMPARE(fuzzyEqual(GL::GLaabb::fromPoints(p0, p1), b), expected);
}
void GLAABBTest::testConstructFromPoints_data()
{
    QTest::addColumn<GL::GLpoint>("p0");
    QTest::addColumn<GL::GLpoint>("p1");
    QTest::addColumn<GL::GLaabb>("b");
    QTest::addColumn<bool>("expected");

    QTest::newRow("simple0") << GL::GLpoint(0.0f, 0.0f) << GL::GLpoint(1.0f, 1.0f) << GL::GLaabb(0.0f, 0.0f, 1.0f, 1.0f) << true;
    QTest::newRow("simple1") << GL::GLpoint(-1.0f, -1.0f) << GL::GLpoint(1.0f, 1.0f) << GL::GLaabb(-1.0f, -1.0f, 2.0f, 2.0f) << true;
    QTest::newRow("reverse0") << GL::GLpoint(1.0f, 1.0f) << GL::GLpoint(0.0f, 0.0f) << GL::GLaabb(0.0f, 0.0f, 1.0f, 1.0f) << true;
    QTest::newRow("reverse1") << GL::GLpoint(1.0f, 1.0f) << GL::GLpoint(-1.0f, -1.0f) << GL::GLaabb(-1.0f, -1.0f, 2.0f, 2.0f) << true;
    QTest::newRow("corner0") << GL::GLpoint(1.0f, 0.0f) << GL::GLpoint(0.0f, 1.0f) << GL::GLaabb(0.0f, 0.0f, 1.0f, 1.0f) << true;
    QTest::newRow("corner1") << GL::GLpoint(1.0f, -1.0f) << GL::GLpoint(-1.0f, 1.0f) << GL::GLaabb(-1.0f, -1.0f, 2.0f, 2.0f) << true;
}

void GLAABBTest::testContainsByPoint()
{
    QFETCH(GL::GLpoint, p);
    QFETCH(GL::GLaabb, b);
    QFETCH(bool, expected);

    QCOMPARE(b.contains(p), expected);
}
void GLAABBTest::testContainsByPoint_data()
{
    QTest::addColumn<GL::GLpoint>("p");
    QTest::addColumn<GL::GLaabb>("b");
    QTest::addColumn<bool>("expected");

    GL::GLaabb aabb[] = {
        GL::GLaabb(0.0f, 0.0f, 1.0f, 1.0f),
        GL::GLaabb(1.0f, 0.0f, 1.0f, 1.0f),
    };

    QTest::newRow("simple0") << GL::GLpoint(0.5f, 0.5f) << aabb[0] << true;
    QTest::newRow("simple1") << GL::GLpoint(0.5f, 0.5f) << aabb[1] << false;
    QTest::newRow("edge0") << GL::GLpoint(1.0f, 0.0f) << aabb[0] << true;
    QTest::newRow("edge1") << GL::GLpoint(1.0f, 1.0f) << aabb[1] << true;
    QTest::newRow("close0") << GL::GLpoint(1.5f, 1.1f) << aabb[1] << false;
    QTest::newRow("close1") << GL::GLpoint(1.1f, 0.5f) << aabb[0] << false;
}

void GLAABBTest::testContainsByAABB()
{
    QFETCH(GL::GLaabb, b0);
    QFETCH(GL::GLaabb, b1);
    QFETCH(bool, expected);

    QCOMPARE(b0.contains(b1), expected);
}
void GLAABBTest::testContainsByAABB_data()
{
    QTest::addColumn<GL::GLaabb>("b0");
    QTest::addColumn<GL::GLaabb>("b1");
    QTest::addColumn<bool>("expected");

    GL::GLaabb aabb[] = {
        GL::GLaabb(0.0f, 0.0f, 2.0f, 2.0f),
        GL::GLaabb(0.5f, 0.5f, 1.0f, 1.0f),
        GL::GLaabb(1.0f, 1.0f, 1.0f, 1.0f),
    };

    QTest::newRow("simple0") << aabb[0] << aabb[1] << true;
    QTest::newRow("simple0_rev") << aabb[1] << aabb[0] << false;
    QTest::newRow("simple1") << aabb[0] << aabb[2] << true;
    QTest::newRow("simple1_rev") << aabb[2] << aabb[0] << false;
    QTest::newRow("simple2") << aabb[1] << aabb[2] << false;
    QTest::newRow("simple2_rev") << aabb[2] << aabb[1] << false;
    QTest::newRow("self0") << aabb[0] << aabb[0] << true;
}

void GLAABBTest::testIntersects()
{
    QFETCH(GL::GLaabb, b0);
    QFETCH(GL::GLaabb, b1);
    QFETCH(bool, expected);

    // intersect op is commutative
    QCOMPARE(b0.intersects(b1), expected);
    QCOMPARE(b1.intersects(b0), expected);
}
void GLAABBTest::testIntersects_data()
{
    QTest::addColumn<GL::GLaabb>("b0");
    QTest::addColumn<GL::GLaabb>("b1");
    QTest::addColumn<bool>("expected");

    GL::GLaabb aabb[] = {
        GL::GLaabb(0.0f, 0.0f, 1.0f, 1.0f),
        GL::GLaabb(1.0f, 0.0f, 1.0f, 1.0f),
        GL::GLaabb(1.0f, 1.0f, 1.0f, 1.0f),
        GL::GLaabb(0.0f, 1.0f, 1.0f, 1.0f),

        GL::GLaabb(0.5f, 0.5f, 1.0f, 1.0f)
    };

    QTest::newRow("simple0") << aabb[0] << aabb[1] << false;
    QTest::newRow("simple1") << aabb[0] << aabb[2] << false;
    QTest::newRow("simple2") << aabb[0] << aabb[3] << false;
    QTest::newRow("simple3") << aabb[0] << aabb[4] << true;
    QTest::newRow("simple4") << aabb[2] << aabb[4] << true;
}

void GLAABBTest::testCut()
{
    QFETCH(GL::GLaabb, b0);
    QFETCH(GL::GLaabb, b1);
    QFETCH(GL::GLaabb, b2);
    QFETCH(bool, expected);

    // cut op is commutative
    QCOMPARE(fuzzyEqual(b0.cut(b1), b2), expected);
    QCOMPARE(fuzzyEqual(b1.cut(b0), b2), expected);
}
void GLAABBTest::testCut_data()
{
    QTest::addColumn<GL::GLaabb>("b0");
    QTest::addColumn<GL::GLaabb>("b1");
    QTest::addColumn<GL::GLaabb>("b2");
    QTest::addColumn<bool>("expected");

    GL::GLaabb aabb[] = {
        GL::GLaabb(0.0f, 0.0f, 1.0f, 1.0f),
        GL::GLaabb(1.0f, 1.0f, 1.0f, 1.0f),
        GL::GLaabb(0.5f, 0.5f, 1.0f, 1.0f),

        GL::GLaabb(0.5f, 0.5f, 0.5f, 0.5f),
        GL::GLaabb(1.0f, 1.0f, 0.5f, 0.5f),
        GL::GLaabb(0.0f, 0.0f, 0.0f, 0.0f)
    };

    QTest::newRow("simple0") << aabb[0] << aabb[2] << aabb[3] << true;
    QTest::newRow("simple1") << aabb[1] << aabb[2] << aabb[4] << true;
    QTest::newRow("simple2") << aabb[0] << aabb[1] << aabb[5] << true;
}

void GLAABBTest::testJoin()
{
    QFETCH(GL::GLaabb, b0);
    QFETCH(GL::GLaabb, b1);
    QFETCH(GL::GLaabb, b2);
    QFETCH(bool, expected);

    // cut op is commutative
    QCOMPARE(fuzzyEqual(b0.join(b1), b2), expected);
    QCOMPARE(fuzzyEqual(b1.join(b0), b2), expected);
}
void GLAABBTest::testJoin_data()
{
    QTest::addColumn<GL::GLaabb>("b0");
    QTest::addColumn<GL::GLaabb>("b1");
    QTest::addColumn<GL::GLaabb>("b2");
    QTest::addColumn<bool>("expected");

    GL::GLaabb aabb[] = {
        GL::GLaabb(0.0f, 0.0f, 1.0f, 1.0f),
        GL::GLaabb(1.0f, 1.0f, 1.0f, 1.0f),
        GL::GLaabb(0.5f, 0.5f, 1.0f, 1.0f),

        GL::GLaabb(0.0f, 0.0f, 1.5f, 1.5f),
        GL::GLaabb(0.5f, 0.5f, 1.5f, 1.5f),
        GL::GLaabb(0.0f, 0.0f, 2.0f, 2.0f)
    };

    QTest::newRow("simple0") << aabb[0] << aabb[2] << aabb[3] << true;
    QTest::newRow("simple1") << aabb[1] << aabb[2] << aabb[4] << true;
    QTest::newRow("simple2") << aabb[0] << aabb[1] << aabb[5] << true;
}

} // namespace unit //