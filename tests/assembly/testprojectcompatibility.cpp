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
#include "../../sources/properties/elementdata.h"
#include "../../sources/assembly/assemblyinfo.h"

class TestProjectCompatibility : public QObject
{
	Q_OBJECT

private slots:
	void testLegacyProjectFileCompatibility()
	{
		// Test that existing project files without assembly info still work
		QString legacy_xml = R"(
			<definition>
				<informations>
					<label>Test Element</label>
					<description>Test Description</description>
					<comment>Test Comment</comment>
				</informations>
			</definition>
		)";
		
		QDomDocument doc;
		QVERIFY(doc.setContent(legacy_xml));
		QDomElement root = doc.documentElement();
		
		ElementData element_data;
		element_data.fromXml(root);
		
		// Should load without errors
		QVERIFY(!element_data.hasAssemblyInfo());
		QVERIFY(!element_data.isAssembly());
		
		// Should still have basic information
		QCOMPARE(element_data.information("label"), QString("Test Element"));
		QCOMPARE(element_data.information("description"), QString("Test Description"));
		QCOMPARE(element_data.information("comment"), QString("Test Comment"));
	}

	void testNewProjectFileWithAssemblyInfo()
	{
		// Test that new project files with assembly info work correctly
		QString new_xml = R"(
			<definition>
				<informations>
					<label>Assembly Element</label>
					<description>Assembly Description</description>
					<comment>Assembly Comment</comment>
				</informations>
				<assembly_info id="CABINET-01" name="Control Cabinet" type="Assembly" description="Main control cabinet">
					<child_elements>
						<child ref="S1"/>
						<child ref="K1"/>
					</child_elements>
					<inherited_properties>
						<property key="project_name" value="Test Project"/>
						<property key="voltage" value="24V"/>
					</inherited_properties>
					<overridden_properties>
						<property key="location" value="Building A"/>
					</overridden_properties>
				</assembly_info>
			</definition>
		)";
		
		QDomDocument doc;
		QVERIFY(doc.setContent(new_xml));
		QDomElement root = doc.documentElement();
		
		ElementData element_data;
		element_data.fromXml(root);
		
		// Should load with assembly info
		QVERIFY(element_data.hasAssemblyInfo());
		QVERIFY(element_data.isAssembly());
		
		// Should have basic information
		QCOMPARE(element_data.information("label"), QString("Assembly Element"));
		QCOMPARE(element_data.information("description"), QString("Assembly Description"));
		QCOMPARE(element_data.information("comment"), QString("Assembly Comment"));
		
		// Should have assembly info
		AssemblyInfo assembly_info = element_data.getAssemblyInfo();
		QCOMPARE(assembly_info.assembly_id, QString("CABINET-01"));
		QCOMPARE(assembly_info.assembly_name, QString("Control Cabinet"));
		QCOMPARE(assembly_info.assembly_type, QString("Assembly"));
		QCOMPARE(assembly_info.description, QString("Main control cabinet"));
		QCOMPARE(assembly_info.getChildElementCount(), 2);
		QVERIFY(assembly_info.hasChildElement("S1"));
		QVERIFY(assembly_info.hasChildElement("K1"));
		QCOMPARE(assembly_info.getInheritedProperty("project_name").toString(), QString("Test Project"));
		QCOMPARE(assembly_info.getInheritedProperty("voltage").toString(), QString("24V"));
		QCOMPARE(assembly_info.getOverriddenProperty("location").toString(), QString("Building A"));
	}

	void testRoundTripCompatibility()
	{
		// Test that we can save and reload project files correctly
		ElementData original_data;
		
		// Set basic information
		original_data.setInformation("label", "Round Trip Test");
		original_data.setInformation("description", "Testing round trip compatibility");
		
		// Set assembly info
		AssemblyInfo assembly_info;
		assembly_info.assembly_id = "ROUND-TRIP-01";
		assembly_info.assembly_name = "Round Trip Assembly";
		assembly_info.assembly_type = "Component";
		assembly_info.description = "Testing round trip";
		assembly_info.addChildElement("E1");
		assembly_info.addChildElement("E2");
		assembly_info.setInheritedProperty("test_prop", "test_value");
		assembly_info.setOverriddenProperty("overridden_prop", "overridden_value");
		
		original_data.setAssemblyInfo(assembly_info);
		
		// Serialize to XML
		QDomDocument doc;
		QDomElement xml_element = original_data.toXml(doc);
		doc.appendChild(xml_element);
		
		// Deserialize from XML
		ElementData loaded_data;
		loaded_data.fromXml(xml_element);
		
		// Verify basic information
		QCOMPARE(loaded_data.information("label"), QString("Round Trip Test"));
		QCOMPARE(loaded_data.information("description"), QString("Testing round trip compatibility"));
		
		// Verify assembly info
		QVERIFY(loaded_data.hasAssemblyInfo());
		QVERIFY(loaded_data.isAssembly());
		
		AssemblyInfo loaded_assembly_info = loaded_data.getAssemblyInfo();
		QCOMPARE(loaded_assembly_info.assembly_id, QString("ROUND-TRIP-01"));
		QCOMPARE(loaded_assembly_info.assembly_name, QString("Round Trip Assembly"));
		QCOMPARE(loaded_assembly_info.assembly_type, QString("Component"));
		QCOMPARE(loaded_assembly_info.description, QString("Testing round trip"));
		QCOMPARE(loaded_assembly_info.getChildElementCount(), 2);
		QVERIFY(loaded_assembly_info.hasChildElement("E1"));
		QVERIFY(loaded_assembly_info.hasChildElement("E2"));
		QCOMPARE(loaded_assembly_info.getInheritedProperty("test_prop").toString(), QString("test_value"));
		QCOMPARE(loaded_assembly_info.getOverriddenProperty("overridden_prop").toString(), QString("overridden_value"));
	}

	void testPartialAssemblyInfo()
	{
		// Test that partial assembly info works correctly
		QString partial_xml = R"(
			<definition>
				<informations>
					<label>Partial Assembly</label>
				</informations>
				<assembly_info id="PARTIAL-01" name="Partial Assembly" type="Assembly">
					<inherited_properties>
						<property key="voltage" value="24V"/>
					</inherited_properties>
				</assembly_info>
			</definition>
		)";
		
		QDomDocument doc;
		QVERIFY(doc.setContent(partial_xml));
		QDomElement root = doc.documentElement();
		
		ElementData element_data;
		element_data.fromXml(root);
		
		// Should load with partial assembly info
		QVERIFY(element_data.hasAssemblyInfo());
		QVERIFY(element_data.isAssembly());
		
		AssemblyInfo assembly_info = element_data.getAssemblyInfo();
		QCOMPARE(assembly_info.assembly_id, QString("PARTIAL-01"));
		QCOMPARE(assembly_info.assembly_name, QString("Partial Assembly"));
		QCOMPARE(assembly_info.assembly_type, QString("Assembly"));
		QVERIFY(assembly_info.description.isEmpty()); // Not set
		QCOMPARE(assembly_info.getChildElementCount(), 0); // No children
		QCOMPARE(assembly_info.getInheritedProperty("voltage").toString(), QString("24V"));
		QVERIFY(assembly_info.getOverriddenProperty("voltage").isNull()); // No overridden properties
	}

	void testInvalidAssemblyInfo()
	{
		// Test that invalid assembly info is handled gracefully
		QString invalid_xml = R"(
			<definition>
				<informations>
					<label>Invalid Assembly</label>
				</informations>
				<assembly_info>
					<!-- Missing required attributes -->
				</assembly_info>
			</definition>
		)";
		
		QDomDocument doc;
		QVERIFY(doc.setContent(invalid_xml));
		QDomElement root = doc.documentElement();
		
		ElementData element_data;
		element_data.fromXml(root);
		
		// Should handle invalid assembly info gracefully
		QVERIFY(element_data.hasAssemblyInfo()); // Assembly info element exists
		QVERIFY(!element_data.isAssembly()); // But not a valid assembly
		
		AssemblyInfo assembly_info = element_data.getAssemblyInfo();
		QVERIFY(assembly_info.assembly_id.isEmpty()); // Empty due to missing attributes
		QVERIFY(assembly_info.assembly_name.isEmpty());
		QVERIFY(assembly_info.assembly_type.isEmpty());
	}

	void testEmptyAssemblyInfo()
	{
		// Test that empty assembly info is handled correctly
		QString empty_xml = R"(
			<definition>
				<informations>
					<label>Empty Assembly</label>
				</informations>
				<assembly_info id="" name="" type="">
				</assembly_info>
			</definition>
		)";
		
		QDomDocument doc;
		QVERIFY(doc.setContent(empty_xml));
		QDomElement root = doc.documentElement();
		
		ElementData element_data;
		element_data.fromXml(root);
		
		// Should handle empty assembly info
		QVERIFY(element_data.hasAssemblyInfo());
		QVERIFY(!element_data.isAssembly()); // Not a valid assembly due to empty ID
		
		AssemblyInfo assembly_info = element_data.getAssemblyInfo();
		QVERIFY(assembly_info.assembly_id.isEmpty());
		QVERIFY(assembly_info.assembly_name.isEmpty());
		QVERIFY(assembly_info.assembly_type.isEmpty());
	}
};

QTEST_APPLESS_MAIN(TestProjectCompatibility)

#include "testprojectcompatibility.moc"
