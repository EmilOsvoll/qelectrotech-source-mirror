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
#include "../../sources/assembly/assemblyinfo.h"
#include "../../sources/properties/elementdata.h"

/**
 * @brief The TestAssemblyInfo class
 * 
 * Unit tests for the AssemblyInfo class
 */
class TestAssemblyInfo : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    // Basic functionality tests
    void testDefaultConstructor();
    void testParameterizedConstructor();
    void testIsValid();
    void testIsEmpty();
    
    // Child element management tests
    void testAddChildElement();
    void testRemoveChildElement();
    void testHasChildElement();
    void testGetChildElementCount();
    
    // Property management tests
    void testSetProperty();
    void testGetProperty();
    void testHasProperty();
    void testRemoveProperty();
    
    // Inherited property tests
    void testInheritedProperties();
    void testOverriddenProperties();
    void testEffectiveProperties();
    
    // XML serialization tests
    void testToXml();
    void testFromXml();
    void testXmlRoundTrip();
    
    // Validation tests
    void testValidationErrors();
    void testStaticMethods();
    
    // Comparison tests
    void testEqualityOperator();
    void testInequalityOperator();
};

void TestAssemblyInfo::initTestCase()
{
    // Initialize test case
}

void TestAssemblyInfo::cleanupTestCase()
{
    // Cleanup test case
}

void TestAssemblyInfo::testDefaultConstructor()
{
    AssemblyInfo info;
    
    QVERIFY(!info.assembly_id.isEmpty());
    QCOMPARE(info.assembly_name, QStringLiteral("New Assembly"));
    QCOMPARE(info.assembly_type, QStringLiteral("Assembly"));
    QVERIFY(info.child_elements.isEmpty());
    QVERIFY(info.properties.isEmpty());
    QVERIFY(info.isValid());
}

void TestAssemblyInfo::testParameterizedConstructor()
{
    AssemblyInfo info(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    
    QCOMPARE(info.assembly_id, QStringLiteral("CABINET-01"));
    QCOMPARE(info.assembly_name, QStringLiteral("Main Cabinet"));
    QCOMPARE(info.assembly_type, QStringLiteral("Assembly"));
    QVERIFY(info.isValid());
}

void TestAssemblyInfo::testIsValid()
{
    AssemblyInfo info;
    QVERIFY(info.isValid());
    
    // Test with invalid ID
    info.assembly_id = QStringLiteral("");
    QVERIFY(!info.isValid());
    
    // Test with invalid name
    info.assembly_id = QStringLiteral("CABINET-01");
    info.assembly_name = QStringLiteral("");
    QVERIFY(!info.isValid());
    
    // Test with invalid type
    info.assembly_name = QStringLiteral("Main Cabinet");
    info.assembly_type = QStringLiteral("InvalidType");
    QVERIFY(!info.isValid());
}

void TestAssemblyInfo::testIsEmpty()
{
    AssemblyInfo info;
    QVERIFY(!info.isEmpty());
    
    AssemblyInfo empty;
    empty.assembly_id = QStringLiteral("");
    empty.assembly_name = QStringLiteral("");
    empty.assembly_type = QStringLiteral("");
    QVERIFY(empty.isEmpty());
}

void TestAssemblyInfo::testAddChildElement()
{
    AssemblyInfo info;
    
    QCOMPARE(info.getChildElementCount(), 0);
    
    info.addChildElement(QStringLiteral("Q1"));
    QCOMPARE(info.getChildElementCount(), 1);
    QVERIFY(info.hasChildElement(QStringLiteral("Q1")));
    
    info.addChildElement(QStringLiteral("S1"));
    QCOMPARE(info.getChildElementCount(), 2);
    QVERIFY(info.hasChildElement(QStringLiteral("S1")));
    
    // Adding duplicate should not increase count
    info.addChildElement(QStringLiteral("Q1"));
    QCOMPARE(info.getChildElementCount(), 2);
    
    // Adding empty string should not add anything
    info.addChildElement(QStringLiteral(""));
    QCOMPARE(info.getChildElementCount(), 2);
}

void TestAssemblyInfo::testRemoveChildElement()
{
    AssemblyInfo info;
    
    info.addChildElement(QStringLiteral("Q1"));
    info.addChildElement(QStringLiteral("S1"));
    QCOMPARE(info.getChildElementCount(), 2);
    
    info.removeChildElement(QStringLiteral("Q1"));
    QCOMPARE(info.getChildElementCount(), 1);
    QVERIFY(!info.hasChildElement(QStringLiteral("Q1")));
    QVERIFY(info.hasChildElement(QStringLiteral("S1")));
    
    // Removing non-existent element should not change count
    info.removeChildElement(QStringLiteral("K1"));
    QCOMPARE(info.getChildElementCount(), 1);
}

void TestAssemblyInfo::testHasChildElement()
{
    AssemblyInfo info;
    
    QVERIFY(!info.hasChildElement(QStringLiteral("Q1")));
    
    info.addChildElement(QStringLiteral("Q1"));
    QVERIFY(info.hasChildElement(QStringLiteral("Q1")));
    QVERIFY(!info.hasChildElement(QStringLiteral("S1")));
}

void TestAssemblyInfo::testGetChildElementCount()
{
    AssemblyInfo info;
    
    QCOMPARE(info.getChildElementCount(), 0);
    
    info.addChildElement(QStringLiteral("Q1"));
    QCOMPARE(info.getChildElementCount(), 1);
    
    info.addChildElement(QStringLiteral("S1"));
    QCOMPARE(info.getChildElementCount(), 2);
    
    info.removeChildElement(QStringLiteral("Q1"));
    QCOMPARE(info.getChildElementCount(), 1);
}

void TestAssemblyInfo::testSetProperty()
{
    AssemblyInfo info;
    
    QVERIFY(!info.hasProperty(QStringLiteral("manufacturer")));
    
    info.setProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    QVERIFY(info.hasProperty(QStringLiteral("manufacturer")));
    QCOMPARE(info.getProperty(QStringLiteral("manufacturer")).toString(), QStringLiteral("Schneider Electric"));
    
    // Setting empty key should not add property
    info.setProperty(QStringLiteral(""), QStringLiteral("value"));
    QCOMPARE(info.properties.size(), 1);
}

void TestAssemblyInfo::testGetProperty()
{
    AssemblyInfo info;
    
    QVERIFY(info.getProperty(QStringLiteral("nonexistent")).isNull());
    
    info.setProperty(QStringLiteral("voltage"), 400);
    QCOMPARE(info.getProperty(QStringLiteral("voltage")).toInt(), 400);
}

void TestAssemblyInfo::testHasProperty()
{
    AssemblyInfo info;
    
    QVERIFY(!info.hasProperty(QStringLiteral("manufacturer")));
    
    info.setProperty(QStringLiteral("manufacturer"), QStringLiteral("ABB"));
    QVERIFY(info.hasProperty(QStringLiteral("manufacturer")));
}

void TestAssemblyInfo::testRemoveProperty()
{
    AssemblyInfo info;
    
    info.setProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    QVERIFY(info.hasProperty(QStringLiteral("manufacturer")));
    
    info.removeProperty(QStringLiteral("manufacturer"));
    QVERIFY(!info.hasProperty(QStringLiteral("manufacturer")));
}

void TestAssemblyInfo::testInheritedProperties()
{
    AssemblyInfo info;
    
    QVERIFY(!info.hasInheritedProperty(QStringLiteral("location")));
    
    info.setInheritedProperty(QStringLiteral("location"), QStringLiteral("Control Room"));
    QVERIFY(info.hasInheritedProperty(QStringLiteral("location")));
    QCOMPARE(info.getInheritedProperty(QStringLiteral("location")).toString(), QStringLiteral("Control Room"));
}

void TestAssemblyInfo::testOverriddenProperties()
{
    AssemblyInfo info;
    
    QVERIFY(!info.hasOverriddenProperty(QStringLiteral("voltage")));
    
    info.setOverriddenProperty(QStringLiteral("voltage"), 400);
    QVERIFY(info.hasOverriddenProperty(QStringLiteral("voltage")));
    QCOMPARE(info.getOverriddenProperty(QStringLiteral("voltage")).toInt(), 400);
}

void TestAssemblyInfo::testEffectiveProperties()
{
    AssemblyInfo info;
    
    // Set inherited property
    info.setInheritedProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    QCOMPARE(info.getEffectiveProperty(QStringLiteral("manufacturer")).toString(), QStringLiteral("Schneider Electric"));
    
    // Override with overridden property
    info.setOverriddenProperty(QStringLiteral("manufacturer"), QStringLiteral("ABB"));
    QCOMPARE(info.getEffectiveProperty(QStringLiteral("manufacturer")).toString(), QStringLiteral("ABB"));
    
    // Direct property should take highest precedence
    info.setProperty(QStringLiteral("manufacturer"), QStringLiteral("Siemens"));
    QCOMPARE(info.getEffectiveProperty(QStringLiteral("manufacturer")).toString(), QStringLiteral("Siemens"));
    
    // Test getAllEffectiveProperties
    QMap<QString, QVariant> effective = info.getAllEffectiveProperties();
    QVERIFY(effective.contains(QStringLiteral("manufacturer")));
    QCOMPARE(effective.value(QStringLiteral("manufacturer")).toString(), QStringLiteral("Siemens"));
}

void TestAssemblyInfo::testToXml()
{
    AssemblyInfo info(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    info.addChildElement(QStringLiteral("Q1"));
    info.addChildElement(QStringLiteral("S1"));
    info.setProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    info.setInheritedProperty(QStringLiteral("location"), QStringLiteral("Control Room"));
    
    QDomDocument doc;
    QDomElement elem = info.toXml(doc);
    
    QCOMPARE(elem.tagName(), QStringLiteral("assembly_info"));
    QCOMPARE(elem.attribute(QStringLiteral("assembly_id")), QStringLiteral("CABINET-01"));
    QCOMPARE(elem.attribute(QStringLiteral("assembly_name")), QStringLiteral("Main Cabinet"));
    QCOMPARE(elem.attribute(QStringLiteral("assembly_type")), QStringLiteral("Assembly"));
    
    // Check child elements
    QDomElement children_elem = elem.firstChildElement(QStringLiteral("child_elements"));
    QVERIFY(!children_elem.isNull());
    QCOMPARE(children_elem.childNodes().size(), 2);
    
    // Check properties
    QDomElement props_elem = elem.firstChildElement(QStringLiteral("properties"));
    QVERIFY(!props_elem.isNull());
    
    // Check inherited properties
    QDomElement inherited_elem = elem.firstChildElement(QStringLiteral("inherited_properties"));
    QVERIFY(!inherited_elem.isNull());
}

void TestAssemblyInfo::testFromXml()
{
    QDomDocument doc;
    QDomElement elem = doc.createElement(QStringLiteral("assembly_info"));
    elem.setAttribute(QStringLiteral("assembly_id"), QStringLiteral("CABINET-01"));
    elem.setAttribute(QStringLiteral("assembly_name"), QStringLiteral("Main Cabinet"));
    elem.setAttribute(QStringLiteral("assembly_type"), QStringLiteral("Assembly"));
    
    // Add child elements
    QDomElement children_elem = doc.createElement(QStringLiteral("child_elements"));
    QDomElement child1 = doc.createElement(QStringLiteral("child"));
    child1.setAttribute(QStringLiteral("ref"), QStringLiteral("Q1"));
    children_elem.appendChild(child1);
    elem.appendChild(children_elem);
    
    // Add properties
    QDomElement props_elem = doc.createElement(QStringLiteral("properties"));
    QDomElement prop = doc.createElement(QStringLiteral("property"));
    prop.setAttribute(QStringLiteral("key"), QStringLiteral("manufacturer"));
    prop.setAttribute(QStringLiteral("value"), QStringLiteral("Schneider Electric"));
    props_elem.appendChild(prop);
    elem.appendChild(props_elem);
    
    AssemblyInfo info;
    QVERIFY(info.fromXml(elem));
    
    QCOMPARE(info.assembly_id, QStringLiteral("CABINET-01"));
    QCOMPARE(info.assembly_name, QStringLiteral("Main Cabinet"));
    QCOMPARE(info.assembly_type, QStringLiteral("Assembly"));
    QCOMPARE(info.getChildElementCount(), 1);
    QVERIFY(info.hasChildElement(QStringLiteral("Q1")));
    QCOMPARE(info.getProperty(QStringLiteral("manufacturer")).toString(), QStringLiteral("Schneider Electric"));
}

void TestAssemblyInfo::testXmlRoundTrip()
{
    AssemblyInfo original(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    original.addChildElement(QStringLiteral("Q1"));
    original.addChildElement(QStringLiteral("S1"));
    original.setProperty(QStringLiteral("manufacturer"), QStringLiteral("Schneider Electric"));
    original.setInheritedProperty(QStringLiteral("location"), QStringLiteral("Control Room"));
    original.setOverriddenProperty(QStringLiteral("voltage"), 400);
    
    QDomDocument doc;
    QDomElement elem = original.toXml(doc);
    
    AssemblyInfo restored;
    QVERIFY(restored.fromXml(elem));
    
    QCOMPARE(restored.assembly_id, original.assembly_id);
    QCOMPARE(restored.assembly_name, original.assembly_name);
    QCOMPARE(restored.assembly_type, original.assembly_type);
    QCOMPARE(restored.getChildElementCount(), original.getChildElementCount());
    QCOMPARE(restored.getProperty(QStringLiteral("manufacturer")).toString(), original.getProperty(QStringLiteral("manufacturer")).toString());
    QCOMPARE(restored.getInheritedProperty(QStringLiteral("location")).toString(), original.getInheritedProperty(QStringLiteral("location")).toString());
    QCOMPARE(restored.getOverriddenProperty(QStringLiteral("voltage")).toInt(), original.getOverriddenProperty(QStringLiteral("voltage")).toInt());
}

void TestAssemblyInfo::testValidationErrors()
{
    AssemblyInfo info;
    
    // Valid info should have no errors
    QVERIFY(info.getValidationErrors().isEmpty());
    
    // Invalid ID should produce error
    info.assembly_id = QStringLiteral("");
    QStringList errors = info.getValidationErrors();
    QVERIFY(!errors.isEmpty());
    QVERIFY(errors.contains(QStringLiteral("Assembly ID cannot be empty")));
    
    // Invalid name should produce error
    info.assembly_id = QStringLiteral("CABINET-01");
    info.assembly_name = QStringLiteral("");
    errors = info.getValidationErrors();
    QVERIFY(errors.contains(QStringLiteral("Assembly name cannot be empty")));
    
    // Invalid type should produce error
    info.assembly_name = QStringLiteral("Main Cabinet");
    info.assembly_type = QStringLiteral("InvalidType");
    errors = info.getValidationErrors();
    QVERIFY(errors.contains(QStringLiteral("Assembly type is not valid")));
}

void TestAssemblyInfo::testStaticMethods()
{
    // Test generateAssemblyId
    QString id1 = AssemblyInfo::generateAssemblyId(QStringLiteral("CABINET"));
    QString id2 = AssemblyInfo::generateAssemblyId(QStringLiteral("CABINET"));
    QVERIFY(id1.startsWith(QStringLiteral("CABINET-")));
    QVERIFY(id2.startsWith(QStringLiteral("CABINET-")));
    QVERIFY(id1 != id2); // Should be different
    
    // Test isValidAssemblyId
    QVERIFY(AssemblyInfo::isValidAssemblyId(QStringLiteral("CABINET-01")));
    QVERIFY(AssemblyInfo::isValidAssemblyId(QStringLiteral("PANEL-ABC123")));
    QVERIFY(!AssemblyInfo::isValidAssemblyId(QStringLiteral("")));
    QVERIFY(!AssemblyInfo::isValidAssemblyId(QStringLiteral("invalid")));
    QVERIFY(!AssemblyInfo::isValidAssemblyId(QStringLiteral("CABINET")));
    
    // Test getValidAssemblyTypes
    QStringList types = AssemblyInfo::getValidAssemblyTypes();
    QVERIFY(types.contains(QStringLiteral("Assembly")));
    QVERIFY(types.contains(QStringLiteral("SubAssembly")));
    QVERIFY(types.contains(QStringLiteral("Module")));
    QVERIFY(types.contains(QStringLiteral("Panel")));
    QVERIFY(types.contains(QStringLiteral("Cabinet")));
}

void TestAssemblyInfo::testEqualityOperator()
{
    AssemblyInfo info1(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    AssemblyInfo info2(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    AssemblyInfo info3(QStringLiteral("CABINET-02"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    
    QVERIFY(info1 == info2);
    QVERIFY(!(info1 == info3));
}

void TestAssemblyInfo::testInequalityOperator()
{
    AssemblyInfo info1(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    AssemblyInfo info2(QStringLiteral("CABINET-01"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    AssemblyInfo info3(QStringLiteral("CABINET-02"), QStringLiteral("Main Cabinet"), QStringLiteral("Assembly"));
    
    QVERIFY(!(info1 != info2));
    QVERIFY(info1 != info3);
}

QTEST_MAIN(TestAssemblyInfo)
#include "testassemblyinfo.moc"
