/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QtTest>
#include <QDomDocument>
#include <QDomElement>
#include "../../sources/properties/elementdata.h"
#include "../../sources/assembly/assemblyinfo.h"

/**
 * @brief The TestElementDataAssembly class
 * 
 * Unit tests for the ElementData class assembly extensions
 */
class TestElementDataAssembly : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Assembly info tests
    void testSetAssemblyInfo();
    void testGetAssemblyInfo();
    void testIsAssembly();
    void testIsSubAssembly();
    void testHasAssemblyInfo();
    void testClearAssemblyInfo();
    
    // Child element management tests
    void testAddChildElement();
    void testRemoveChildElement();
    void testHasChildElement();
    void testGetChildElements();
    void testGetChildElementCount();
    
    // Property inheritance tests
    void testInheritedProperties();
    void testOverriddenProperties();
    void testEffectiveProperties();
    
    // XML serialization tests
    void testXmlSerialization();
    void testXmlDeserialization();
    void testBackwardCompatibility();
};

void TestElementDataAssembly::initTestCase()
{
    // Initialize test case
}

void TestElementDataAssembly::cleanupTestCase()
{
    // Cleanup test case
}

void TestElementDataAssembly::testSetAssemblyInfo()
{
    ElementData data;
    
    QVERIFY(!data.hasAssemblyInfo());
    
    data.setAssemblyInfo(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    
    QVERIFY(data.hasAssemblyInfo());
    QCOMPARE(data.getAssemblyInfo().assembly_id, QStringLiteral("CABINET-01"));
    QCOMPARE(data.getAssemblyInfo().assembly_name, QStringLiteral("Main Cabinet"));
    QCOMPARE(data.getAssemblyInfo().assembly_type, QStringLiteral("Assembly"));
}

void TestElementDataAssembly::testGetAssemblyInfo()
{
    ElementData data;
    
    // Default assembly info should be empty
    AssemblyInfo info = data.getAssemblyInfo();
    QVERIFY(info.assembly_id.isEmpty());
    
    data.setAssemblyInfo(QStringLiteral("PANEL-01"), QStringLiteral("Control Panel"), QStringLiteral("Panel"));
    
    info = data.getAssemblyInfo();
    QCOMPARE(info.assembly_id, QStringLiteral("PANEL-01"));
    QCOMPARE(info.assembly_name, QStringLiteral("Control Panel"));
    QCOMPARE(info.assembly_type, QStringLiteral("Panel"));
}

void TestElementDataAssembly::testIsAssembly()
{
    ElementData data;
    
    QVERIFY(!data.isAssembly());
    
    data.setAssemblyInfo(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    QVERIFY(data.isAssembly());
    
    data.setAssemblyInfo(QStringLiteral("PANEL-01"), QStringLiteral("Control Panel"), QStringLiteral("Panel"));
    QVERIFY(!data.isAssembly());
}

void TestElementDataAssembly::testIsSubAssembly()
{
    ElementData data;
    
    QVERIFY(!data.isSubAssembly());
    
    data.setAssemblyInfo(QStringLiteral("SUBMODULE-01"), QStringLiteral("Power Module"), QStringLiteral("SubAssembly"));
    QVERIFY(data.isSubAssembly());
    
    data.setAssemblyInfo(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    QVERIFY(!data.isSubAssembly());
}

void TestElementDataAssembly::testHasAssemblyInfo()
{
    ElementData data;
    
    QVERIFY(!data.hasAssemblyInfo());
    
    data.setAssemblyInfo(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    QVERIFY(data.hasAssemblyInfo());
    
    data.clearAssemblyInfo();
    QVERIFY(!data.hasAssemblyInfo());
}

void TestElementDataAssembly::testClearAssemblyInfo()
{
    ElementData data;
    
    data.setAssemblyInfo(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    data.addChildElement(QStringLiteral("Q1"));
    data.setInheritedProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    
    QVERIFY(data.hasAssemblyInfo());
    QCOMPARE(data.getChildElementCount(), 1);
    QVERIFY(data.hasInheritedProperty(QStringLiteral("manufacturer")));
    
    data.clearAssemblyInfo();
    
    QVERIFY(!data.hasAssemblyInfo());
    QCOMPARE(data.getChildElementCount(), 0);
    QVERIFY(!data.hasInheritedProperty(QStringLiteral("manufacturer")));
}

void TestElementDataAssembly::testAddChildElement()
{
    ElementData data;
    
    QCOMPARE(data.getChildElementCount(), 0);
    
    data.addChildElement(QStringLiteral("Q1"));
    QCOMPARE(data.getChildElementCount(), 1);
    QVERIFY(data.hasChildElement(QStringLiteral("Q1")));
    
    data.addChildElement(QStringLiteral("S1"));
    QCOMPARE(data.getChildElementCount(), 2);
    QVERIFY(data.hasChildElement(QStringLiteral("S1")));
}

void TestElementDataAssembly::testRemoveChildElement()
{
    ElementData data;
    
    data.addChildElement(QStringLiteral("Q1"));
    data.addChildElement(QStringLiteral("S1"));
    QCOMPARE(data.getChildElementCount(), 2);
    
    data.removeChildElement(QStringLiteral("Q1"));
    QCOMPARE(data.getChildElementCount(), 1);
    QVERIFY(!data.hasChildElement(QStringLiteral("Q1")));
    QVERIFY(data.hasChildElement(QStringLiteral("S1")));
}

void TestElementDataAssembly::testHasChildElement()
{
    ElementData data;
    
    QVERIFY(!data.hasChildElement(QStringLiteral("Q1")));
    
    data.addChildElement(QStringLiteral("Q1"));
    QVERIFY(data.hasChildElement(QStringLiteral("Q1")));
    QVERIFY(!data.hasChildElement(QStringLiteral("S1")));
}

void TestElementDataAssembly::testGetChildElements()
{
    ElementData data;
    
    QStringList children = data.getChildElements();
    QVERIFY(children.isEmpty());
    
    data.addChildElement(QStringLiteral("Q1"));
    data.addChildElement(QStringLiteral("S1"));
    
    children = data.getChildElements();
    QCOMPARE(children.size(), 2);
    QVERIFY(children.contains(QStringLiteral("Q1")));
    QVERIFY(children.contains(QStringLiteral("S1")));
}

void TestElementDataAssembly::testGetChildElementCount()
{
    ElementData data;
    
    QCOMPARE(data.getChildElementCount(), 0);
    
    data.addChildElement(QStringLiteral("Q1"));
    QCOMPARE(data.getChildElementCount(), 1);
    
    data.addChildElement(QStringLiteral("S1"));
    QCOMPARE(data.getChildElementCount(), 2);
    
    data.removeChildElement(QStringLiteral("Q1"));
    QCOMPARE(data.getChildElementCount(), 1);
}

void TestElementDataAssembly::testInheritedProperties()
{
    ElementData data;
    
    QVERIFY(!data.hasInheritedProperty(QStringLiteral("manufacturer")));
    
    data.setInheritedProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    QVERIFY(data.hasInheritedProperty(QStringLiteral("manufacturer")));
    QCOMPARE(data.getInheritedProperty(QStringLiteral("manufacturer")).toString(), QStringLiteral("Schneider Electric"));
    
    data.removeInheritedProperty(QStringLiteral("manufacturer"));
    QVERIFY(!data.hasInheritedProperty(QStringLiteral("manufacturer")));
}

void TestElementDataAssembly::testOverriddenProperties()
{
    ElementData data;
    
    QVERIFY(!data.hasOverriddenProperty(QStringLiteral("voltage")));
    
    data.setOverriddenProperty(QStringLiteral("voltage"), 400);
    QVERIFY(data.hasOverriddenProperty(QStringLiteral("voltage")));
    QCOMPARE(data.getOverriddenProperty(QStringLiteral("voltage")).toInt(), 400);
    
    data.removeOverriddenProperty(QStringLiteral("voltage"));
    QVERIFY(!data.hasOverriddenProperty(QStringLiteral("voltage")));
}

void TestElementDataAssembly::testEffectiveProperties()
{
    ElementData data;
    
    // Set inherited property
    data.setInheritedProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    QCOMPARE(data.getEffectiveProperty(QStringLiteral("manufacturer")).toString(), QStringLiteral("Schneider Electric"));
    
    // Override with overridden property
    data.setOverriddenProperty(QStringLiteral("manufacturer"), QStringLiteral("ABB"));
    QCOMPARE(data.getEffectiveProperty(QStringLiteral("manufacturer")).toString(), QStringLiteral("ABB"));
    
    // Test getAllEffectiveProperties
    QMap<QString, QVariant> effective = data.getAllEffectiveProperties();
    QVERIFY(effective.contains(QStringLiteral("manufacturer")));
    QCOMPARE(effective.value(QStringLiteral("manufacturer")).toString(), QStringLiteral("ABB"));
}

void TestElementDataAssembly::testXmlSerialization()
{
    ElementData data;
    data.setAssemblyInfo(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    data.addChildElement(QStringLiteral("Q1"));
    data.setInheritedProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    
    QDomDocument doc;
    QDomElement elem = data.toXml(doc);
    
    QCOMPARE(elem.tagName(), QStringLiteral("element_data"));
    
    // Check if assembly info is present
    QDomElement assembly_elem = elem.firstChildElement(QStringLiteral("assembly_info"));
    QVERIFY(!assembly_elem.isNull());
    QCOMPARE(assembly_elem.attribute(QStringLiteral("assembly_id")), QStringLiteral("CABINET-01"));
}

void TestElementDataAssembly::testXmlDeserialization()
{
    QDomDocument doc;
    QDomElement elem = doc.createElement(QStringLiteral("definition"));
    elem.setAttribute(QStringLiteral("type"), QStringLiteral("element"));
    
    // Add assembly info
    QDomElement assembly_elem = doc.createElement(QStringLiteral("assembly_info"));
    assembly_elem.setAttribute(QStringLiteral("assembly_id"), QStringLiteral("CABINET-01"));
    assembly_elem.setAttribute(QStringLiteral("assembly_name"), QStringLiteral("Main Cabinet"));
    assembly_elem.setAttribute(QStringLiteral("assembly_type"), QStringLiteral("Assembly"));
    elem.appendChild(assembly_elem);
    
    ElementData data;
    QVERIFY(data.fromXml(elem));
    
    QVERIFY(data.hasAssemblyInfo());
    QCOMPARE(data.getAssemblyInfo().assembly_id, QStringLiteral("CABINET-01"));
    QCOMPARE(data.getAssemblyInfo().assembly_name, QStringLiteral("Main Cabinet"));
    QCOMPARE(data.getAssemblyInfo().assembly_type, QStringLiteral("Assembly"));
}

void TestElementDataAssembly::testBackwardCompatibility()
{
    // Test that existing XML without assembly info still works
    QDomDocument doc;
    QDomElement elem = doc.createElement(QStringLiteral("definition"));
    elem.setAttribute(QStringLiteral("type"), QStringLiteral("element"));
    
    ElementData data;
    QVERIFY(data.fromXml(elem));
    
    // Should not have assembly info
    QVERIFY(!data.hasAssemblyInfo());
    QCOMPARE(data.getChildElementCount(), 0);
    QVERIFY(data.getChildElements().isEmpty());
}

QTEST_MAIN(TestElementDataAssembly)
#include "testelementdataassembly.moc"
