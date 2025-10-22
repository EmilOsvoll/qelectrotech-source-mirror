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
#include "../../sources/qetproject.h"
#include "../../sources/dataBase/projectdatabase.h"

class TestAssemblyManager : public QObject
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

	void testCreateAssembly()
	{
		QString assembly_id = m_manager->createAssembly("Test Assembly", "Assembly", "Test description");
		QVERIFY(!assembly_id.isEmpty());
		QVERIFY(m_manager->assemblyExists(assembly_id));
		
		AssemblyInfo info = m_manager->getAssemblyInfo(assembly_id);
		QCOMPARE(info.assembly_name, QString("Test Assembly"));
		QCOMPARE(info.assembly_type, QString("Assembly"));
		QCOMPARE(info.description, QString("Test description"));
	}

	void testRemoveAssembly()
	{
		QString assembly_id = m_manager->createAssembly("To Remove", "Component");
		QVERIFY(m_manager->assemblyExists(assembly_id));
		
		bool removed = m_manager->removeAssembly(assembly_id);
		QVERIFY(removed);
		QVERIFY(!m_manager->assemblyExists(assembly_id));
	}

	void testUpdateAssembly()
	{
		QString assembly_id = m_manager->createAssembly("Original Name", "Assembly");
		
		bool updated = m_manager->updateAssembly(assembly_id, "assembly_name", "Updated Name");
		QVERIFY(updated);
		
		AssemblyInfo info = m_manager->getAssemblyInfo(assembly_id);
		QCOMPARE(info.assembly_name, QString("Updated Name"));
	}

	void testAddRemoveChild()
	{
		QString assembly_id = m_manager->createAssembly("Parent Assembly", "Assembly");
		QString element_uuid = "test-element-uuid";
		
		// Add child
		bool added = m_manager->addChildToAssembly(assembly_id, element_uuid);
		QVERIFY(added);
		QVERIFY(m_manager->isElementInAssembly(element_uuid, assembly_id));
		
		QStringList children = m_manager->getAssemblyChildren(assembly_id);
		QCOMPARE(children.count(), 1);
		QCOMPARE(children.first(), element_uuid);
		
		// Remove child
		bool removed = m_manager->removeChildFromAssembly(assembly_id, element_uuid);
		QVERIFY(removed);
		QVERIFY(!m_manager->isElementInAssembly(element_uuid, assembly_id));
		
		children = m_manager->getAssemblyChildren(assembly_id);
		QCOMPARE(children.count(), 0);
	}

	void testAssemblyProperties()
	{
		QString assembly_id = m_manager->createAssembly("Property Test", "Assembly");
		
		// Set property
		bool set = m_manager->setAssemblyProperty(assembly_id, "voltage", "24V", "inherited");
		QVERIFY(set);
		
		QVariant value = m_manager->getAssemblyProperty(assembly_id, "voltage");
		QCOMPARE(value.toString(), QString("24V"));
		
		// Set overridden property
		set = m_manager->setAssemblyProperty(assembly_id, "voltage", "48V", "overridden");
		QVERIFY(set);
		
		value = m_manager->getAssemblyProperty(assembly_id, "voltage");
		QCOMPARE(value.toString(), QString("48V"));
		
		// Get all properties
		QMap<QString, QVariant> properties = m_manager->getAllAssemblyProperties(assembly_id);
		QVERIFY(properties.contains("voltage"));
		QCOMPARE(properties.value("voltage").toString(), QString("48V"));
		
		// Remove property
		bool removed = m_manager->removeAssemblyProperty(assembly_id, "voltage");
		QVERIFY(removed);
		
		value = m_manager->getAssemblyProperty(assembly_id, "voltage");
		QVERIFY(value.isNull());
	}

	void testAssemblyTypes()
	{
		QString assembly_id = m_manager->createAssembly("Assembly Type", "Assembly");
		QVERIFY(!assembly_id.isEmpty());
		
		QString subassembly_id = m_manager->createAssembly("SubAssembly Type", "SubAssembly");
		QVERIFY(!subassembly_id.isEmpty());
		
		QString component_id = m_manager->createAssembly("Component Type", "Component");
		QVERIFY(!component_id.isEmpty());
		
		// Test filtering by type
		QStringList assemblies = m_manager->getAssemblyIdsByType("Assembly");
		QVERIFY(assemblies.contains(assembly_id));
		QVERIFY(!assemblies.contains(subassembly_id));
		QVERIFY(!assemblies.contains(component_id));
		
		QStringList subassemblies = m_manager->getAssemblyIdsByType("SubAssembly");
		QVERIFY(subassemblies.contains(subassembly_id));
		
		QStringList components = m_manager->getAssemblyIdsByType("Component");
		QVERIFY(components.contains(component_id));
	}

	void testAssemblyIdGeneration()
	{
		// Test ID generation
		QString id1 = m_manager->generateAssemblyId("Test Assembly");
		QVERIFY(!id1.isEmpty());
		QVERIFY(m_manager->validateAssemblyId(id1));
		
		// Test duplicate handling
		QString id2 = m_manager->generateAssemblyId("Test Assembly");
		QVERIFY(id1 != id2);
		QVERIFY(id2.contains("Test Assembly"));
		
		// Test invalid ID validation
		QVERIFY(!m_manager->validateAssemblyId(""));
		QVERIFY(!m_manager->validateAssemblyId("Invalid ID!"));
		QVERIFY(!m_manager->validateAssemblyId("Invalid@ID"));
	}

	void testGetAllAssemblies()
	{
		// Clear existing assemblies
		m_manager->clearAllAssemblies();
		
		// Create test assemblies
		QStringList created_ids;
		created_ids << m_manager->createAssembly("Assembly 1", "Assembly");
		created_ids << m_manager->createAssembly("Assembly 2", "SubAssembly");
		created_ids << m_manager->createAssembly("Assembly 3", "Component");
		
		// Get all assembly IDs
		QStringList all_ids = m_manager->getAllAssemblyIds();
		QCOMPARE(all_ids.count(), 3);
		
		for (const QString &id : created_ids) {
			QVERIFY(all_ids.contains(id));
		}
	}

	void testCanRemoveAssembly()
	{
		QString assembly_id = m_manager->createAssembly("Can Remove", "Assembly");
		QVERIFY(m_manager->canRemoveAssembly(assembly_id));
		
		// Add a child element
		m_manager->addChildToAssembly(assembly_id, "test-child");
		QVERIFY(m_manager->canRemoveAssembly(assembly_id)); // Should still be removable
		
		// Remove the assembly
		QVERIFY(m_manager->removeAssembly(assembly_id));
		QVERIFY(!m_manager->canRemoveAssembly(assembly_id)); // Should not be removable anymore
	}

	void testInvalidOperations()
	{
		// Test operations with invalid parameters
		QString empty_id = m_manager->createAssembly("", "Assembly");
		QVERIFY(empty_id.isEmpty());
		
		bool updated = m_manager->updateAssembly("", "assembly_name", "Test");
		QVERIFY(!updated);
		
		bool added = m_manager->addChildToAssembly("", "test-uuid");
		QVERIFY(!added);
		
		bool removed = m_manager->removeAssembly("");
		QVERIFY(!removed);
		
		QVariant value = m_manager->getAssemblyProperty("", "test");
		QVERIFY(value.isNull());
	}

private:
	QETProject *m_project;
	AssemblyManager *m_manager;
};

QTEST_APPLESS_MAIN(TestAssemblyManager)

#include "testassemblymanager.moc"
