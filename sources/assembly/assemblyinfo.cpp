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
#include "assemblyinfo.h"
#include "../qetxml.h"
#include "../qet.h"

#include <QDomElement>
#include <QDomDocument>
#include <QRegularExpression>
#include <QUuid>
#include <QDate>

/**
 * @brief AssemblyInfo::AssemblyInfo
 * Default constructor
 */
AssemblyInfo::AssemblyInfo()
{
    initializeDefaults();
}

/**
 * @brief AssemblyInfo::AssemblyInfo
 * Constructor with parameters
 * @param id Assembly identifier
 * @param name Assembly name
 * @param type Assembly type
 */
AssemblyInfo::AssemblyInfo(const QString& id, const QString& name, const QString& type)
    : assembly_id(id)
    , assembly_name(name)
    , assembly_type(type)
{
    initializeDefaults();
}

/**
 * @brief AssemblyInfo::initializeDefaults
 * Initialize default values
 */
void AssemblyInfo::initializeDefaults()
{
    if (assembly_id.isEmpty()) {
        assembly_id = generateAssemblyId();
    }
    if (assembly_name.isEmpty()) {
        assembly_name = QStringLiteral("New Assembly");
    }
    if (assembly_type.isEmpty()) {
        assembly_type = QStringLiteral("Assembly");
    }
    if (location.isEmpty()) {
        location = QStringLiteral("");
    }
    if (responsible.isEmpty()) {
        responsible = QStringLiteral("");
    }
    if (revision.isEmpty()) {
        revision = QStringLiteral("1.0");
    }
    if (created_date.isEmpty()) {
        created_date = QDate::currentDate().toString(Qt::ISODate);
    }
    if (modified_date.isEmpty()) {
        modified_date = QDate::currentDate().toString(Qt::ISODate);
    }
}

/**
 * @brief AssemblyInfo::isValid
 * Check if the assembly info is valid
 * @return true if valid, false otherwise
 */
bool AssemblyInfo::isValid() const
{
    return getValidationErrors().isEmpty();
}

/**
 * @brief AssemblyInfo::isEmpty
 * Check if the assembly info is empty
 * @return true if empty, false otherwise
 */
bool AssemblyInfo::isEmpty() const
{
    return assembly_id.isEmpty() && assembly_name.isEmpty() && 
           assembly_type.isEmpty() && child_elements.isEmpty() && 
           properties.isEmpty();
}

/**
 * @brief AssemblyInfo::getValidationErrors
 * Get list of validation errors
 * @return List of error messages
 */
QStringList AssemblyInfo::getValidationErrors() const
{
    QStringList errors;
    
    errors.append(validateAssemblyId(assembly_id));
    errors.append(validateAssemblyName(assembly_name));
    errors.append(validateAssemblyType(assembly_type));
    
    // Remove empty strings
    errors.removeAll(QString());
    
    return errors;
}

/**
 * @brief AssemblyInfo::addChildElement
 * Add a child element to the assembly
 * @param element_ref Reference to the child element
 */
void AssemblyInfo::addChildElement(const QString& element_ref)
{
    if (!element_ref.isEmpty() && !hasChildElement(element_ref)) {
        child_elements.append(element_ref);
    }
}

/**
 * @brief AssemblyInfo::removeChildElement
 * Remove a child element from the assembly
 * @param element_ref Reference to the child element
 */
void AssemblyInfo::removeChildElement(const QString& element_ref)
{
    child_elements.removeAll(element_ref);
}

/**
 * @brief AssemblyInfo::hasChildElement
 * Check if the assembly has a specific child element
 * @param element_ref Reference to the child element
 * @return true if found, false otherwise
 */
bool AssemblyInfo::hasChildElement(const QString& element_ref) const
{
    return child_elements.contains(element_ref);
}

/**
 * @brief AssemblyInfo::getChildElementCount
 * Get the number of child elements
 * @return Number of child elements
 */
int AssemblyInfo::getChildElementCount() const
{
    return child_elements.size();
}

/**
 * @brief AssemblyInfo::setProperty
 * Set a property value
 * @param key Property key
 * @param value Property value
 */
void AssemblyInfo::setProperty(const QString& key, const QVariant& value)
{
    if (!key.isEmpty()) {
        properties.insert(key, value);
    }
}

/**
 * @brief AssemblyInfo::getProperty
 * Get a property value
 * @param key Property key
 * @return Property value, or empty QVariant if not found
 */
QVariant AssemblyInfo::getProperty(const QString& key) const
{
    return properties.value(key, QVariant());
}

/**
 * @brief AssemblyInfo::hasProperty
 * Check if a property exists
 * @param key Property key
 * @return true if exists, false otherwise
 */
bool AssemblyInfo::hasProperty(const QString& key) const
{
    return properties.contains(key);
}

/**
 * @brief AssemblyInfo::removeProperty
 * Remove a property
 * @param key Property key
 */
void AssemblyInfo::removeProperty(const QString& key)
{
    properties.remove(key);
}

/**
 * @brief AssemblyInfo::setInheritedProperty
 * Set an inherited property value
 * @param key Property key
 * @param value Property value
 */
void AssemblyInfo::setInheritedProperty(const QString& key, const QVariant& value)
{
    if (!key.isEmpty()) {
        inherited_properties.insert(key, value);
    }
}

/**
 * @brief AssemblyInfo::getInheritedProperty
 * Get an inherited property value
 * @param key Property key
 * @return Property value, or empty QVariant if not found
 */
QVariant AssemblyInfo::getInheritedProperty(const QString& key) const
{
    return inherited_properties.value(key, QVariant());
}

/**
 * @brief AssemblyInfo::hasInheritedProperty
 * Check if an inherited property exists
 * @param key Property key
 * @return true if exists, false otherwise
 */
bool AssemblyInfo::hasInheritedProperty(const QString& key) const
{
    return inherited_properties.contains(key);
}

/**
 * @brief AssemblyInfo::setOverriddenProperty
 * Set an overridden property value
 * @param key Property key
 * @param value Property value
 */
void AssemblyInfo::setOverriddenProperty(const QString& key, const QVariant& value)
{
    if (!key.isEmpty()) {
        overridden_properties.insert(key, value);
    }
}

/**
 * @brief AssemblyInfo::getOverriddenProperty
 * Get an overridden property value
 * @param key Property key
 * @return Property value, or empty QVariant if not found
 */
QVariant AssemblyInfo::getOverriddenProperty(const QString& key) const
{
    return overridden_properties.value(key, QVariant());
}

/**
 * @brief AssemblyInfo::hasOverriddenProperty
 * Check if an overridden property exists
 * @param key Property key
 * @return true if exists, false otherwise
 */
bool AssemblyInfo::hasOverriddenProperty(const QString& key) const
{
    return overridden_properties.contains(key);
}

/**
 * @brief AssemblyInfo::getEffectiveProperty
 * Get the effective property value (overridden takes precedence over inherited)
 * @param key Property key
 * @return Effective property value
 */
QVariant AssemblyInfo::getEffectiveProperty(const QString& key) const
{
    // Overridden properties take precedence
    if (hasOverriddenProperty(key)) {
        return getOverriddenProperty(key);
    }
    
    // Then inherited properties
    if (hasInheritedProperty(key)) {
        return getInheritedProperty(key);
    }
    
    // Finally direct properties
    return getProperty(key);
}

/**
 * @brief AssemblyInfo::getAllEffectiveProperties
 * Get all effective properties
 * @return Map of all effective properties
 */
QMap<QString, QVariant> AssemblyInfo::getAllEffectiveProperties() const
{
    QMap<QString, QVariant> effective;
    
    // Start with inherited properties
    for (auto it = inherited_properties.begin(); it != inherited_properties.end(); ++it) {
        effective.insert(it.key(), it.value());
    }
    
    // Override with overridden properties
    for (auto it = overridden_properties.begin(); it != overridden_properties.end(); ++it) {
        effective.insert(it.key(), it.value());
    }
    
    // Add direct properties (highest precedence)
    for (auto it = properties.begin(); it != properties.end(); ++it) {
        effective.insert(it.key(), it.value());
    }
    
    return effective;
}

/**
 * @brief AssemblyInfo::toXml
 * Serialize to XML
 * @param document XML document
 * @return XML element
 */
QDomElement AssemblyInfo::toXml(QDomDocument& document) const
{
    QDomElement assembly_elem = document.createElement(QStringLiteral("assembly_info"));
    
    // Basic properties
    assembly_elem.setAttribute(QStringLiteral("assembly_id"), assembly_id);
    assembly_elem.setAttribute(QStringLiteral("assembly_name"), assembly_name);
    assembly_elem.setAttribute(QStringLiteral("assembly_type"), assembly_type);
    
    if (!parent_assembly.isEmpty()) {
        assembly_elem.setAttribute(QStringLiteral("parent_assembly"), parent_assembly);
    }
    
    if (!description.isEmpty()) {
        assembly_elem.setAttribute(QStringLiteral("description"), description);
    }
    
    if (!location.isEmpty()) {
        assembly_elem.setAttribute(QStringLiteral("location"), location);
    }
    
    if (!responsible.isEmpty()) {
        assembly_elem.setAttribute(QStringLiteral("responsible"), responsible);
    }
    
    if (!revision.isEmpty()) {
        assembly_elem.setAttribute(QStringLiteral("revision"), revision);
    }
    
    if (!created_date.isEmpty()) {
        assembly_elem.setAttribute(QStringLiteral("created_date"), created_date);
    }
    
    if (!modified_date.isEmpty()) {
        assembly_elem.setAttribute(QStringLiteral("modified_date"), modified_date);
    }
    
    // Child elements
    if (!child_elements.isEmpty()) {
        QDomElement children_elem = document.createElement(QStringLiteral("child_elements"));
        for (const QString& child : child_elements) {
            QDomElement child_elem = document.createElement(QStringLiteral("child"));
            child_elem.setAttribute(QStringLiteral("ref"), child);
            children_elem.appendChild(child_elem);
        }
        assembly_elem.appendChild(children_elem);
    }
    
    // Properties
    if (!properties.isEmpty()) {
        QDomElement props_elem = document.createElement(QStringLiteral("properties"));
        for (auto it = properties.begin(); it != properties.end(); ++it) {
            QDomElement prop_elem = document.createElement(QStringLiteral("property"));
            prop_elem.setAttribute(QStringLiteral("key"), it.key());
            prop_elem.setAttribute(QStringLiteral("value"), it.value().toString());
            props_elem.appendChild(prop_elem);
        }
        assembly_elem.appendChild(props_elem);
    }
    
    // Inherited properties
    if (!inherited_properties.isEmpty()) {
        QDomElement inherited_elem = document.createElement(QStringLiteral("inherited_properties"));
        for (auto it = inherited_properties.begin(); it != inherited_properties.end(); ++it) {
            QDomElement prop_elem = document.createElement(QStringLiteral("property"));
            prop_elem.setAttribute(QStringLiteral("key"), it.key());
            prop_elem.setAttribute(QStringLiteral("value"), it.value().toString());
            inherited_elem.appendChild(prop_elem);
        }
        assembly_elem.appendChild(inherited_elem);
    }
    
    // Overridden properties
    if (!overridden_properties.isEmpty()) {
        QDomElement overridden_elem = document.createElement(QStringLiteral("overridden_properties"));
        for (auto it = overridden_properties.begin(); it != overridden_properties.end(); ++it) {
            QDomElement prop_elem = document.createElement(QStringLiteral("property"));
            prop_elem.setAttribute(QStringLiteral("key"), it.key());
            prop_elem.setAttribute(QStringLiteral("value"), it.value().toString());
            overridden_elem.appendChild(prop_elem);
        }
        assembly_elem.appendChild(overridden_elem);
    }
    
    return assembly_elem;
}

/**
 * @brief AssemblyInfo::fromXml
 * Deserialize from XML
 * @param element XML element
 * @return true if successful, false otherwise
 */
bool AssemblyInfo::fromXml(const QDomElement& element)
{
    if (element.tagName() != QStringLiteral("assembly_info")) {
        return false;
    }
    
    // Basic properties
    assembly_id = element.attribute(QStringLiteral("assembly_id"));
    assembly_name = element.attribute(QStringLiteral("assembly_name"));
    assembly_type = element.attribute(QStringLiteral("assembly_type"));
    parent_assembly = element.attribute(QStringLiteral("parent_assembly"));
    description = element.attribute(QStringLiteral("description"));
    location = element.attribute(QStringLiteral("location"));
    responsible = element.attribute(QStringLiteral("responsible"));
    revision = element.attribute(QStringLiteral("revision"));
    created_date = element.attribute(QStringLiteral("created_date"));
    modified_date = element.attribute(QStringLiteral("modified_date"));
    
    // Child elements
    child_elements.clear();
    QDomElement children_elem = element.firstChildElement(QStringLiteral("child_elements"));
    if (!children_elem.isNull()) {
        QDomNodeList children = children_elem.elementsByTagName(QStringLiteral("child"));
        for (int i = 0; i < children.size(); ++i) {
            QDomElement child_elem = children.at(i).toElement();
            if (!child_elem.isNull()) {
                QString ref = child_elem.attribute(QStringLiteral("ref"));
                if (!ref.isEmpty()) {
                    child_elements.append(ref);
                }
            }
        }
    }
    
    // Properties
    properties.clear();
    QDomElement props_elem = element.firstChildElement(QStringLiteral("properties"));
    if (!props_elem.isNull()) {
        QDomNodeList props = props_elem.elementsByTagName(QStringLiteral("property"));
        for (int i = 0; i < props.size(); ++i) {
            QDomElement prop_elem = props.at(i).toElement();
            if (!prop_elem.isNull()) {
                QString key = prop_elem.attribute(QStringLiteral("key"));
                QString value = prop_elem.attribute(QStringLiteral("value"));
                if (!key.isEmpty()) {
                    properties.insert(key, value);
                }
            }
        }
    }
    
    // Inherited properties
    inherited_properties.clear();
    QDomElement inherited_elem = element.firstChildElement(QStringLiteral("inherited_properties"));
    if (!inherited_elem.isNull()) {
        QDomNodeList props = inherited_elem.elementsByTagName(QStringLiteral("property"));
        for (int i = 0; i < props.size(); ++i) {
            QDomElement prop_elem = props.at(i).toElement();
            if (!prop_elem.isNull()) {
                QString key = prop_elem.attribute(QStringLiteral("key"));
                QString value = prop_elem.attribute(QStringLiteral("value"));
                if (!key.isEmpty()) {
                    inherited_properties.insert(key, value);
                }
            }
        }
    }
    
    // Overridden properties
    overridden_properties.clear();
    QDomElement overridden_elem = element.firstChildElement(QStringLiteral("overridden_properties"));
    if (!overridden_elem.isNull()) {
        QDomNodeList props = overridden_elem.elementsByTagName(QStringLiteral("property"));
        for (int i = 0; i < props.size(); ++i) {
            QDomElement prop_elem = props.at(i).toElement();
            if (!prop_elem.isNull()) {
                QString key = prop_elem.attribute(QStringLiteral("key"));
                QString value = prop_elem.attribute(QStringLiteral("value"));
                if (!key.isEmpty()) {
                    overridden_properties.insert(key, value);
                }
            }
        }
    }
    
    return true;
}

/**
 * @brief AssemblyInfo::operator==
 * Equality operator
 * @param other Other AssemblyInfo to compare
 * @return true if equal, false otherwise
 */
bool AssemblyInfo::operator==(const AssemblyInfo& other) const
{
    return assembly_id == other.assembly_id &&
           assembly_name == other.assembly_name &&
           assembly_type == other.assembly_type &&
           parent_assembly == other.parent_assembly &&
           description == other.description &&
           child_elements == other.child_elements &&
           properties == other.properties &&
           inherited_properties == other.inherited_properties &&
           overridden_properties == other.overridden_properties;
}

/**
 * @brief AssemblyInfo::operator!=
 * Inequality operator
 * @param other Other AssemblyInfo to compare
 * @return true if not equal, false otherwise
 */
bool AssemblyInfo::operator!=(const AssemblyInfo& other) const
{
    return !(*this == other);
}

/**
 * @brief AssemblyInfo::generateAssemblyId
 * Generate a unique assembly ID
 * @param prefix Prefix for the ID
 * @return Generated assembly ID
 */
QString AssemblyInfo::generateAssemblyId(const QString& prefix)
{
    QString base_id = prefix.isEmpty() ? QStringLiteral("ASSEMBLY") : prefix;
    QString uuid = QUuid::createUuid().toString().mid(1, 8); // Remove braces && take first 8 chars
    return QStringLiteral("%1-%2").arg(base_id, uuid);
}

/**
 * @brief AssemblyInfo::isValidAssemblyId
 * Check if an assembly ID is valid
 * @param id Assembly ID to check
 * @return true if valid, false otherwise
 */
bool AssemblyInfo::isValidAssemblyId(const QString& id)
{
    if (id.isEmpty()) {
        return false;
    }
    
    // Assembly ID should match pattern: PREFIX-NUMBER or PREFIX-UUID
    QRegularExpression regex(QStringLiteral("^[A-Z][A-Z0-9]*-[A-Z0-9]+$"));
    return regex.match(id).hasMatch();
}

/**
 * @brief AssemblyInfo::getValidAssemblyTypes
 * Get list of valid assembly types
 * @return List of valid assembly types
 */
QStringList AssemblyInfo::getValidAssemblyTypes()
{
    return QStringList() << QStringLiteral("Assembly") 
                         << QStringLiteral("SubAssembly") 
                         << QStringLiteral("Module") 
                         << QStringLiteral("Panel") 
                         << QStringLiteral("Cabinet");
}

/**
 * @brief AssemblyInfo::validateAssemblyId
 * Validate assembly ID
 * @param id Assembly ID to validate
 * @return List of validation errors
 */
QStringList AssemblyInfo::validateAssemblyId(const QString& id) const
{
    QStringList errors;
    
    if (id.isEmpty()) {
        errors.append(QStringLiteral("Assembly ID cannot be empty"));
    } else if (!isValidAssemblyId(id)) {
        errors.append(QStringLiteral("Assembly ID format is invalid"));
    }
    
    return errors;
}

/**
 * @brief AssemblyInfo::validateAssemblyName
 * Validate assembly name
 * @param name Assembly name to validate
 * @return List of validation errors
 */
QStringList AssemblyInfo::validateAssemblyName(const QString& name) const
{
    QStringList errors;
    
    if (name.isEmpty()) {
        errors.append(QStringLiteral("Assembly name cannot be empty"));
    } else if (name.length() > 100) {
        errors.append(QStringLiteral("Assembly name is too long (max 100 characters)"));
    }
    
    return errors;
}

/**
 * @brief AssemblyInfo::validateAssemblyType
 * Validate assembly type
 * @param type Assembly type to validate
 * @return List of validation errors
 */
QStringList AssemblyInfo::validateAssemblyType(const QString& type) const
{
    QStringList errors;
    
    if (type.isEmpty()) {
        errors.append(QStringLiteral("Assembly type cannot be empty"));
    } else if (!getValidAssemblyTypes().contains(type)) {
        errors.append(QStringLiteral("Assembly type is not valid"));
    }
    
    return errors;
}
