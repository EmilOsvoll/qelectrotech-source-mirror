/*
		Copyright 2006-2025 QElectroTech Team
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
#include <QtTest/QtTest>
#include "../../sources/assembly/assemblymanager.h"
#include "../../sources/assembly/assemblyinfo.h"
#include "../../sources/properties/elementdata.h"
#include "../../sources/qetproject.h"
#include "../../sources/qetgraphicsitem/element.h"

class TestAssemblyIntegration : public QObject
{
	Q_OBJECT

private slots:
	void initTestCase()
	{
		// Initialize test project
		m_project = new QETProject();
		m_manager = new AssemblyManager(m_project);
	}

	void cleanupTestCase()
	{
		delete m_manager;
		delete m_project;
	}

	void testElementDataAssemblyIntegration()
	{
		// Test that ElementData can store assembly information
		ElementData element_data;
		
		// Create assembly info
		AssemblyInfo assembly_info;
		assembly_info.assembly_id = "TEST-CABINET-01";
		assembly_info.assembly_name = "Test Control Cabinet";
		assembly_info.assembly_type = "Assembly";
		assembly_info.description = "Test cabinet for integration testing";
		assembly_info.addChildElement("S1");
		assembly_info.addChildElement("K1");
		assembly_info.setInheritedProperty("project_name", "Test Project");
		assembly_info.setOverriddenProperty("location", "Building A");
		
		// Set assembly info in ElementData
		element_data.setAssemblyInfo(assembly_info);
		
		// Verify it was set correctly
		QVERIFY(element_data.hasAssemblyInfo());
		QVERIFY(element_data.isAssembly());
		
		AssemblyInfo retrieved_info = element_data.getAssemblyInfo();
		QCOMPARE(retrieved_info.assembly_id, QString("TEST-CABINET-01"));
		QCOMPARE(retrieved_info.assembly_name, QString("Test Control Cabinet"));
		QCOMPARE(retrieved_info.assembly_type, QString("Assembly"));
		QCOMPARE(retrieved_info.description, QString("Test cabinet for integration testing"));
		QCOMPARE(retrieved_info.getChildElementCount(), 2);
		QVERIFY(retrieved_info.hasChildElement("S1"));
		QVERIFY(retrieved_info.hasChildElement("K1"));
		QCOMPARE(retrieved_info.getInheritedProperty("project_name").toString(), QString("Test Project"));
		QCOMPARE(retrieved_info.getOverriddenProperty("location").toString(), QString("Building A"));
	}

	void testElementDataXmlSerialization()
	{
		// Test XML serialization of ElementData with assembly info
		ElementData element_data;
		
		// Set up assembly info
		AssemblyInfo assembly_info;
		assembly_info.assembly_id = "XML-TEST-01";
		assembly_info.assembly_name = "XML Test Assembly";
		assembly_info.assembly_type = "Component";
		assembly_info.description = "Testing XML serialization";
		assembly_info.addChildElement("E1");
		assembly_info.setInheritedProperty("voltage", "24V");
		assembly_info.setOverriddenProperty("current", "2A");
		
		element_data.setAssemblyInfo(assembly_info);
		
		// Serialize to XML
		QDomDocument doc;
		QDomElement xml_element = element_data.toXml(doc);
		doc.appendChild(xml_element);
		
		// Deserialize from XML
		ElementData loaded_element_data;
		loaded_element_data.fromXml(xml_element);
		
		// Verify assembly info was preserved
		QVERIFY(loaded_element_data.hasAssemblyInfo());
		QVERIFY(loaded_element_data.isAssembly());
		
		AssemblyInfo loaded_info = loaded_element_data.getAssemblyInfo();
		QCOMPARE(loaded_info.assembly_id, QString("XML-TEST-01"));
		QCOMPARE(loaded_info.assembly_name, QString("XML Test Assembly"));
		QCOMPARE(loaded_info.assembly_type, QString("Component"));
		QCOMPARE(loaded_info.description, QString("Testing XML serialization"));
		QCOMPARE(loaded_info.getChildElementCount(), 1);
		QVERIFY(loaded_info.hasChildElement("E1"));
		QCOMPARE(loaded_info.getInheritedProperty("voltage").toString(), QString("24V"));
		QCOMPARE(loaded_info.getOverriddenProperty("current").toString(), QString("2A"));
	}

	void testAssemblyManagerWithElementData()
	{
		// Test that AssemblyManager can work with ElementData
		QString assembly_id = m_manager->createAssembly("Integration Test", "Assembly", "Testing integration");
		QVERIFY(!assembly_id.isEmpty());
		
		// Create ElementData with assembly info
		ElementData element_data;
		AssemblyInfo assembly_info;
		assembly_info.assembly_id = assembly_id;
		assembly_info.assembly_name = "Integration Test";
		assembly_info.assembly_type = "Assembly";
		assembly_info.description = "Testing integration";
		assembly_info.addChildElement("TEST-ELEMENT-1");
		assembly_info.addChildElement("TEST-ELEMENT-2");
		assembly_info.setInheritedProperty("project", "Integration Test Project");
		
		element_data.setAssemblyInfo(assembly_info);
		
		// Verify ElementData methods work correctly
		QVERIFY(element_data.hasAssemblyInfo());
		QVERIFY(element_data.isAssembly());
		QCOMPARE(element_data.getChildElementCount(), 2);
		QVERIFY(element_data.hasChildElement("TEST-ELEMENT-1"));
		QVERIFY(element_data.hasChildElement("TEST-ELEMENT-2"));
		QCOMPARE(element_data.getInheritedProperty("project").toString(), QString("Integration Test Project"));
		
		// Test effective properties
		QMap<QString, QVariant> effective_props = element_data.getAllEffectiveProperties();
		QVERIFY(effective_props.contains("project"));
		QCOMPARE(effective_props.value("project").toString(), QString("Integration Test Project"));
	}

	void testBackwardCompatibility()
	{
		// Test that existing ElementData without assembly info still works
		ElementData element_data;
		
		// Should not have assembly info by default
		QVERIFY(!element_data.hasAssemblyInfo());
		QVERIFY(!element_data.isAssembly());
		QVERIFY(!element_data.isSubAssembly());
		QCOMPARE(element_data.getChildElementCount(), 0);
		
		// Should handle empty assembly info gracefully
		AssemblyInfo empty_info = element_data.getAssemblyInfo();
		QVERIFY(empty_info.assembly_id.isEmpty());
		QVERIFY(empty_info.assembly_name.isEmpty());
		QVERIFY(empty_info.assembly_type.isEmpty());
		
		// Test XML serialization without assembly info
		QDomDocument doc;
		QDomElement xml_element = element_data.toXml(doc);
		doc.appendChild(xml_element);
		
		// Should not contain assembly_info element
		QDomElement assembly_elem = xml_element.firstChildElement("assembly_info");
		QVERIFY(assembly_elem.isNull());
		
		// Deserialize should work fine
		ElementData loaded_element_data;
		loaded_element_data.fromXml(xml_element);
		QVERIFY(!loaded_element_data.hasAssemblyInfo());
	}

	void testPropertyInheritance()
	{
		// Test property inheritance mechanism
		ElementData parent_data;
		AssemblyInfo parent_info;
		parent_info.assembly_id = "PARENT-01";
		parent_info.assembly_name = "Parent Assembly";
		parent_info.assembly_type = "Assembly";
		parent_info.setInheritedProperty("voltage", "24V");
		parent_info.setInheritedProperty("project", "Test Project");
		parent_data.setAssemblyInfo(parent_info);
		
		ElementData child_data;
		AssemblyInfo child_info;
		child_info.assembly_id = "CHILD-01";
		child_info.assembly_name = "Child Assembly";
		child_info.assembly_type = "SubAssembly";
		child_info.setInheritedProperty("voltage", "24V"); // Inherited from parent
		child_info.setOverriddenProperty("voltage", "48V"); // Overridden
		child_info.setInheritedProperty("project", "Test Project"); // Inherited from parent
		child_data.setAssemblyInfo(child_info);
		
		// Test effective properties
		QCOMPARE(parent_data.getEffectiveProperty("voltage").toString(), QString("24V"));
		QCOMPARE(parent_data.getEffectiveProperty("project").toString(), QString("Test Project"));
		
		QCOMPARE(child_data.getEffectiveProperty("voltage").toString(), QString("48V")); // Overridden
		QCOMPARE(child_data.getEffectiveProperty("project").toString(), QString("Test Project")); // Inherited
		
		// Test property removal
		child_data.removeOverriddenProperty("voltage");
		QCOMPARE(child_data.getEffectiveProperty("voltage").toString(), QString("24V")); // Back to inherited
		
		child_data.removeInheritedProperty("project");
		QVERIFY(child_data.getEffectiveProperty("project").isNull()); // No longer available
	}

	void testAssemblyManagerSignals()
	{
		// Test that AssemblyManager emits correct signals
		QSignalSpy created_spy(m_manager, &AssemblyManager::assemblyCreated);
		QSignalSpy removed_spy(m_manager, &AssemblyManager::assemblyRemoved);
		QSignalSpy updated_spy(m_manager, &AssemblyManager::assemblyUpdated);
		QSignalSpy child_added_spy(m_manager, &AssemblyManager::assemblyChildAdded);
		QSignalSpy child_removed_spy(m_manager, &AssemblyManager::assemblyChildRemoved);
		QSignalSpy property_changed_spy(m_manager, &AssemblyManager::assemblyPropertyChanged);
		
		// Create assembly
		QString assembly_id = m_manager->createAssembly("Signal Test", "Assembly");
		QCOMPARE(created_spy.count(), 1);
		QCOMPARE(created_spy.at(0).at(0).toString(), assembly_id);
		
		// Update assembly
		m_manager->updateAssembly(assembly_id, "assembly_name", "Updated Signal Test");
		QCOMPARE(updated_spy.count(), 1);
		QCOMPARE(updated_spy.at(0).at(0).toString(), assembly_id);
		
		// Add child
		m_manager->addChildToAssembly(assembly_id, "test-child");
		QCOMPARE(child_added_spy.count(), 1);
		QCOMPARE(child_added_spy.at(0).at(0).toString(), assembly_id);
		QCOMPARE(child_added_spy.at(0).at(1).toString(), QString("test-child"));
		
		// Set property
		m_manager->setAssemblyProperty(assembly_id, "test_prop", "test_value");
		QCOMPARE(property_changed_spy.count(), 1);
		QCOMPARE(property_changed_spy.at(0).at(0).toString(), assembly_id);
		QCOMPARE(property_changed_spy.at(0).at(1).toString(), QString("test_prop"));
		
		// Remove child
		m_manager->removeChildFromAssembly(assembly_id, "test-child");
		QCOMPARE(child_removed_spy.count(), 1);
		QCOMPARE(child_removed_spy.at(0).at(0).toString(), assembly_id);
		QCOMPARE(child_removed_spy.at(0).at(1).toString(), QString("test-child"));
		
		// Remove assembly
		m_manager->removeAssembly(assembly_id);
		QCOMPARE(removed_spy.count(), 1);
		QCOMPARE(removed_spy.at(0).at(0).toString(), assembly_id);
	}

private:
	QETProject *m_project;
	AssemblyManager *m_manager;
};

QTEST_APPLESS_MAIN(TestAssemblyIntegration)

#include "testassemblyintegration.moc"
