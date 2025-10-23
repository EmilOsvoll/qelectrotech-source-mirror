/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it &&/or modify
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
#ifndef ASSEMBLY_INFO_H
#define ASSEMBLY_INFO_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QDomElement>
#include <QDomDocument>

/**
 * @brief The AssemblyInfo class
 * 
 * This class represents information about an assembly (group of components)
 * in QElectroTech. It stores assembly metadata, child elements, && properties
 * that can be inherited by child components.
 */
class AssemblyInfo
{
public:
    /**
     * @brief AssemblyInfo constructor
     */
    AssemblyInfo();
    
    /**
     * @brief AssemblyInfo constructor with parameters
     * @param id Assembly identifier (e.g., "CABINET-01")
     * @param name Assembly name (e.g., "Main Control Cabinet")
     * @param type Assembly type (e.g., "Assembly", "SubAssembly")
     */
    AssemblyInfo(const QString& id, const QString& name, const QString& type);
    
    /**
     * @brief AssemblyInfo destructor
     */
    ~AssemblyInfo() = default;
    
    // Basic assembly properties
    QString assembly_id;
    QString assembly_name;
    QString assembly_type;
    QString parent_assembly;
    QString description;
    QString location;      // Physical location reference
    QString responsible;  // Engineer responsible for design
    QString revision;     // Assembly revision number
    QString created_date; // Creation date
    QString modified_date; // Last modification date
    
    // Child elements && properties
    QStringList child_elements;
    QMap<QString, QVariant> properties;
    QMap<QString, QVariant> inherited_properties;
    QMap<QString, QVariant> overridden_properties;
    
    // Validation && utility methods
    bool isValid() const;
    bool isEmpty() const;
    QStringList getValidationErrors() const;
    
    // Child element management
    void addChildElement(const QString& element_ref);
    void removeChildElement(const QString& element_ref);
    bool hasChildElement(const QString& element_ref) const;
    int getChildElementCount() const;
    
    // Property management
    void setProperty(const QString& key, const QVariant& value);
    QVariant getProperty(const QString& key) const;
    bool hasProperty(const QString& key) const;
    void removeProperty(const QString& key);
    
    // Inherited property management
    void setInheritedProperty(const QString& key, const QVariant& value);
    QVariant getInheritedProperty(const QString& key) const;
    bool hasInheritedProperty(const QString& key) const;
    
    // Overridden property management
    void setOverriddenProperty(const QString& key, const QVariant& value);
    QVariant getOverriddenProperty(const QString& key) const;
    bool hasOverriddenProperty(const QString& key) const;
    
    // Effective property resolution (inherited + overridden)
    QVariant getEffectiveProperty(const QString& key) const;
    QMap<QString, QVariant> getAllEffectiveProperties() const;
    
    // XML serialization
    QDomElement toXml(QDomDocument& document) const;
    bool fromXml(const QDomElement& element);
    
    // Comparison operators
    bool operator==(const AssemblyInfo& other) const;
    bool operator!=(const AssemblyInfo& other) const;
    
    // Static utility methods
    static QString generateAssemblyId(const QString& prefix = "ASSEMBLY");
    static bool isValidAssemblyId(const QString& id);
    static QStringList getValidAssemblyTypes();
    
private:
    void initializeDefaults();
    QStringList validateAssemblyId(const QString& id) const;
    QStringList validateAssemblyName(const QString& name) const;
    QStringList validateAssemblyType(const QString& type) const;
};

#endif // ASSEMBLY_INFO_H
