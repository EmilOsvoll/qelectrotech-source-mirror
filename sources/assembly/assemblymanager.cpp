/*
		Copyright 2006-2025 QElectroTech Team
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
#include "assemblymanager.h"

#include "../qetproject.h"
#include "../dataBase/projectdatabase.h"
#include "../qetgraphicsitem/element.h"
#include "assemblyinfo.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QUuid>
#include <QRegExp>

/**
	@brief AssemblyManager::AssemblyManager
	Constructor
	@param project : The QET project this manager is associated with
	@param parent : Parent QObject
*/
AssemblyManager::AssemblyManager(QETProject *project, QObject *parent) :
	QObject(parent),
	m_project(project),
	m_database(nullptr),
	m_cache_dirty(true)
{
	// Get the project's database
	if (m_project) {
		m_database = m_project->dataBase();
	}
}

/**
	@brief AssemblyManager::~AssemblyManager
	Destructor
*/
AssemblyManager::~AssemblyManager()
{
	// Cleanup handled by Qt's parent-child relationship
}

/**
	@brief AssemblyManager::createAssembly
	Create a new assembly
	@param name : Name of the assembly
	@param type : Type of assembly (Assembly, SubAssembly, Component)
	@param description : Description of the assembly
	@return : Generated assembly ID, or empty string if creation failed
*/
QString AssemblyManager::createAssembly(const QString &name, const QString &type, const QString &description)
{
	if (!m_database || name.isEmpty()) {
		return QString();
	}

	// Generate unique assembly ID
	QString assembly_id = generateAssemblyId(name);
	
	// Validate assembly type
	if (!isValidAssemblyType(type)) {
		qDebug() << "Invalid assembly type:" << type;
		return QString();
	}

	// Add assembly to database
	m_database->addAssembly(assembly_id, name, type);
	
	// Set description if provided
	if (!description.isEmpty()) {
		m_database->updateAssembly(assembly_id, "description", description);
	}

	// Invalidate cache
	invalidateCache();

	emit assemblyCreated(assembly_id);
	return assembly_id;
}

/**
	@brief AssemblyManager::removeAssembly
	Remove an assembly
	@param assembly_id : ID of the assembly to remove
	@return : true if removal was successful
*/
bool AssemblyManager::removeAssembly(const QString &assembly_id)
{
	if (!m_database || assembly_id.isEmpty()) {
		return false;
	}

	// Check if assembly can be removed
	if (!canRemoveAssembly(assembly_id)) {
		qDebug() << "Cannot remove assembly:" << assembly_id;
		return false;
	}

	// Remove from database
	m_database->removeAssembly(assembly_id);

	// Invalidate cache
	invalidateCache();

	emit assemblyRemoved(assembly_id);
	return true;
}

/**
	@brief AssemblyManager::updateAssembly
	Update an assembly field
	@param assembly_id : ID of the assembly to update
	@param field : Field name to update
	@param value : New value for the field
	@return : true if update was successful
*/
bool AssemblyManager::updateAssembly(const QString &assembly_id, const QString &field, const QString &value)
{
	if (!m_database || assembly_id.isEmpty() || field.isEmpty()) {
		return false;
	}

	// Validate field name
	QStringList valid_fields = {"assembly_name", "assembly_type", "description", "location", "responsible", "revision"};
	if (!valid_fields.contains(field)) {
		qDebug() << "Invalid field name:" << field;
		return false;
	}

	// Update in database
	m_database->updateAssembly(assembly_id, field, value);

	// Invalidate cache
	invalidateCache();

	emit assemblyUpdated(assembly_id);
	return true;
}

/**
	@brief AssemblyManager::getAssemblyInfo
	Get assembly information
	@param assembly_id : ID of the assembly
	@return : AssemblyInfo object, or empty AssemblyInfo if not found
*/
AssemblyInfo AssemblyManager::getAssemblyInfo(const QString &assembly_id) const
{
	if (!m_database || assembly_id.isEmpty()) {
		return AssemblyInfo();
	}

	// Check cache first
	if (!m_cache_dirty && m_assembly_cache.contains(assembly_id)) {
		return m_assembly_cache.value(assembly_id);
	}

	// Query database
	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT * FROM assembly WHERE assembly_id = :assembly_id");
	query.bindValue(":assembly_id", assembly_id);

	if (!query.exec()) {
		qDebug() << "Error querying assembly:" << query.lastError();
		return AssemblyInfo();
	}

	if (!query.next()) {
		return AssemblyInfo(); // Assembly not found
	}

	// Create AssemblyInfo from query result
	AssemblyInfo info;
	info.assembly_id = query.value("assembly_id").toString();
	info.assembly_name = query.value("assembly_name").toString();
	info.assembly_type = query.value("assembly_type").toString();
	info.description = query.value("description").toString();
	info.location = query.value("location").toString();
	info.responsible = query.value("responsible").toString();
	info.revision = query.value("revision").toString();
	info.created_date = query.value("created_date").toString();
	info.modified_date = query.value("modified_date").toString();

	// Load child elements
	QSqlQuery children_query = m_database->newQuery();
	children_query.prepare("SELECT child_element_uuid FROM assembly_children WHERE assembly_id = :assembly_id");
	children_query.bindValue(":assembly_id", assembly_id);
	
	if (children_query.exec()) {
		while (children_query.next()) {
			info.addChildElement(children_query.value("child_element_uuid").toString());
		}
	}

	// Load properties
	QSqlQuery props_query = m_database->newQuery();
	props_query.prepare("SELECT property_key, property_value, property_type FROM assembly_properties WHERE assembly_id = :assembly_id");
	props_query.bindValue(":assembly_id", assembly_id);
	
	if (props_query.exec()) {
		while (props_query.next()) {
			QString key = props_query.value("property_key").toString();
			QString value = props_query.value("property_value").toString();
			QString type = props_query.value("property_type").toString();
			
			if (type == "inherited") {
				info.setInheritedProperty(key, value);
			} else if (type == "overridden") {
				info.setOverriddenProperty(key, value);
			}
		}
	}

	return info;
}

/**
	@brief AssemblyManager::getAllAssemblyIds
	Get all assembly IDs in the project
	@return : List of assembly IDs
*/
QStringList AssemblyManager::getAllAssemblyIds() const
{
	if (!m_database) {
		return QStringList();
	}

	QStringList ids;
	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT assembly_id FROM assembly ORDER BY assembly_name");
	
	if (query.exec()) {
		while (query.next()) {
			ids.append(query.value("assembly_id").toString());
		}
	}

	return ids;
}

/**
	@brief AssemblyManager::getAssemblyIdsByType
	Get assembly IDs filtered by type
	@param type : Assembly type to filter by
	@return : List of assembly IDs of the specified type
*/
QStringList AssemblyManager::getAssemblyIdsByType(const QString &type) const
{
	if (!m_database || type.isEmpty()) {
		return QStringList();
	}

	QStringList ids;
	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT assembly_id FROM assembly WHERE assembly_type = :type ORDER BY assembly_name");
	query.bindValue(":type", type);
	
	if (query.exec()) {
		while (query.next()) {
			ids.append(query.value("assembly_id").toString());
		}
	}

	return ids;
}

/**
	@brief AssemblyManager::assemblyExists
	Check if an assembly exists
	@param assembly_id : ID of the assembly to check
	@return : true if assembly exists
*/
bool AssemblyManager::assemblyExists(const QString &assembly_id) const
{
	if (!m_database || assembly_id.isEmpty()) {
		return false;
	}

	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT 1 FROM assembly WHERE assembly_id = :assembly_id");
	query.bindValue(":assembly_id", assembly_id);
	
	return query.exec() && query.next();
}

/**
	@brief AssemblyManager::addChildToAssembly
	Add a child element to an assembly
	@param assembly_id : ID of the assembly
	@param element_uuid : UUID of the element to add
	@return : true if addition was successful
*/
bool AssemblyManager::addChildToAssembly(const QString &assembly_id, const QString &element_uuid)
{
	if (!m_database || assembly_id.isEmpty() || element_uuid.isEmpty()) {
		return false;
	}

	// Check if assembly exists
	if (!assemblyExists(assembly_id)) {
		qDebug() << "Assembly does not exist:" << assembly_id;
		return false;
	}

	// Add child to database
	m_database->addAssemblyChild(assembly_id, element_uuid);

	// Invalidate cache
	invalidateCache();

	emit assemblyChildAdded(assembly_id, element_uuid);
	return true;
}

/**
	@brief AssemblyManager::removeChildFromAssembly
	Remove a child element from an assembly
	@param assembly_id : ID of the assembly
	@param element_uuid : UUID of the element to remove
	@return : true if removal was successful
*/
bool AssemblyManager::removeChildFromAssembly(const QString &assembly_id, const QString &element_uuid)
{
	if (!m_database || assembly_id.isEmpty() || element_uuid.isEmpty()) {
		return false;
	}

	// Remove child from database
	m_database->removeAssemblyChild(assembly_id, element_uuid);

	// Invalidate cache
	invalidateCache();

	emit assemblyChildRemoved(assembly_id, element_uuid);
	return true;
}

/**
	@brief AssemblyManager::getAssemblyChildren
	Get all child elements of an assembly
	@param assembly_id : ID of the assembly
	@return : List of child element UUIDs
*/
QStringList AssemblyManager::getAssemblyChildren(const QString &assembly_id) const
{
	if (!m_database || assembly_id.isEmpty()) {
		return QStringList();
	}

	QStringList children;
	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT child_element_uuid FROM assembly_children WHERE assembly_id = :assembly_id");
	query.bindValue(":assembly_id", assembly_id);
	
	if (query.exec()) {
		while (query.next()) {
			children.append(query.value("child_element_uuid").toString());
		}
	}

	return children;
}

/**
	@brief AssemblyManager::isElementInAssembly
	Check if an element is part of an assembly
	@param element_uuid : UUID of the element
	@param assembly_id : ID of the assembly
	@return : true if element is in the assembly
*/
bool AssemblyManager::isElementInAssembly(const QString &element_uuid, const QString &assembly_id) const
{
	if (!m_database || element_uuid.isEmpty() || assembly_id.isEmpty()) {
		return false;
	}

	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT 1 FROM assembly_children WHERE assembly_id = :assembly_id AND child_element_uuid = :element_uuid");
	query.bindValue(":assembly_id", assembly_id);
	query.bindValue(":element_uuid", element_uuid);
	
	return query.exec() && query.next();
}

/**
	@brief AssemblyManager::setAssemblyProperty
	Set a property for an assembly
	@param assembly_id : ID of the assembly
	@param key : Property key
	@param value : Property value
	@param type : Property type (inherited or overridden)
	@return : true if property was set successfully
*/
bool AssemblyManager::setAssemblyProperty(const QString &assembly_id, const QString &key, const QVariant &value, const QString &type)
{
	if (!m_database || assembly_id.isEmpty() || key.isEmpty()) {
		return false;
	}

	// Validate property type
	if (type != "inherited" && type != "overridden") {
		qDebug() << "Invalid property type:" << type;
		return false;
	}

	// Set property in database
	m_database->setAssemblyProperty(assembly_id, key, value.toString());

	// Invalidate cache
	invalidateCache();

	emit assemblyPropertyChanged(assembly_id, key);
	return true;
}

/**
	@brief AssemblyManager::removeAssemblyProperty
	Remove a property from an assembly
	@param assembly_id : ID of the assembly
	@param key : Property key to remove
	@return : true if property was removed successfully
*/
bool AssemblyManager::removeAssemblyProperty(const QString &assembly_id, const QString &key)
{
	if (!m_database || assembly_id.isEmpty() || key.isEmpty()) {
		return false;
	}

	// Remove property from database
	m_database->removeAssemblyProperty(assembly_id, key);

	// Invalidate cache
	invalidateCache();

	emit assemblyPropertyChanged(assembly_id, key);
	return true;
}

/**
	@brief AssemblyManager::getAssemblyProperty
	Get a property value for an assembly
	@param assembly_id : ID of the assembly
	@param key : Property key
	@return : Property value, or empty QVariant if not found
*/
QVariant AssemblyManager::getAssemblyProperty(const QString &assembly_id, const QString &key) const
{
	if (!m_database || assembly_id.isEmpty() || key.isEmpty()) {
		return QVariant();
	}

	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT property_value FROM assembly_properties WHERE assembly_id = :assembly_id AND property_key = :key");
	query.bindValue(":assembly_id", assembly_id);
	query.bindValue(":key", key);
	
	if (query.exec() && query.next()) {
		return query.value("property_value");
	}

	return QVariant();
}

/**
	@brief AssemblyManager::getAllAssemblyProperties
	Get all properties for an assembly
	@param assembly_id : ID of the assembly
	@return : Map of property key-value pairs
*/
QMap<QString, QVariant> AssemblyManager::getAllAssemblyProperties(const QString &assembly_id) const
{
	if (!m_database || assembly_id.isEmpty()) {
		return QMap<QString, QVariant>();
	}

	QMap<QString, QVariant> properties;
	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT property_key, property_value FROM assembly_properties WHERE assembly_id = :assembly_id");
	query.bindValue(":assembly_id", assembly_id);
	
	if (query.exec()) {
		while (query.next()) {
			properties.insert(query.value("property_key").toString(), query.value("property_value"));
		}
	}

	return properties;
}

/**
	@brief AssemblyManager::validateAssemblyId
	Validate an assembly ID format
	@param assembly_id : Assembly ID to validate
	@return : true if assembly ID is valid
*/
bool AssemblyManager::validateAssemblyId(const QString &assembly_id) const
{
	if (assembly_id.isEmpty()) {
		return false;
	}

	// Assembly ID should be alphanumeric with hyphens && underscores allowed
	QRegExp regex("^[A-Za-z0-9_-]+$");
	return regex.exactMatch(assembly_id);
}

/**
	@brief AssemblyManager::generateAssemblyId
	Generate a unique assembly ID based on a base name
	@param base_name : Base name for the assembly
	@return : Generated assembly ID
*/
QString AssemblyManager::generateAssemblyId(const QString &base_name) const
{
	if (base_name.isEmpty()) {
		return QString();
	}

	// Sanitize the base name
	QString sanitized = sanitizeAssemblyId(base_name);
	
	// Check if ID already exists && add suffix if needed
	QString assembly_id = sanitized;
	int suffix = 1;
	
	while (assemblyExists(assembly_id)) {
		assembly_id = QString("%1-%2").arg(sanitized).arg(suffix);
		suffix++;
	}
	
	return assembly_id;
}

/**
	@brief AssemblyManager::canRemoveAssembly
	Check if an assembly can be safely removed
	@param assembly_id : ID of the assembly to check
	@return : true if assembly can be removed
*/
bool AssemblyManager::canRemoveAssembly(const QString &assembly_id) const
{
	if (!m_database || assembly_id.isEmpty()) {
		return false;
	}

	// Check if assembly exists
	if (!assemblyExists(assembly_id)) {
		return false;
	}

	// Check if assembly has child assemblies (prevent orphaned children)
	QSqlQuery query = m_database->newQuery();
	query.prepare("SELECT 1 FROM assembly WHERE parent_assembly_id = :assembly_id");
	query.bindValue(":assembly_id", assembly_id);
	
	if (query.exec() && query.next()) {
		return false; // Has child assemblies
	}

	return true;
}

/**
	@brief AssemblyManager::invalidateCache
	Mark the assembly cache as dirty
*/
void AssemblyManager::invalidateCache()
{
	m_cache_dirty = true;
	m_assembly_cache.clear();
}

/**
	@brief AssemblyManager::updateCache
	Update the assembly cache with current data
*/
void AssemblyManager::updateCache()
{
	if (!m_cache_dirty) {
		return;
	}

	m_assembly_cache.clear();
	QStringList assembly_ids = getAllAssemblyIds();
	
	for (const QString &assembly_id : assembly_ids) {
		m_assembly_cache.insert(assembly_id, getAssemblyInfo(assembly_id));
	}
	
	m_cache_dirty = false;
}

/**
	@brief AssemblyManager::isValidAssemblyType
	Check if an assembly type is valid
	@param type : Assembly type to validate
	@return : true if assembly type is valid
*/
bool AssemblyManager::isValidAssemblyType(const QString &type) const
{
	QStringList valid_types = {"Assembly", "SubAssembly", "Component"};
	return valid_types.contains(type);
}

/**
	@brief AssemblyManager::sanitizeAssemblyId
	Sanitize an assembly ID to make it valid
	@param id : Assembly ID to sanitize
	@return : Sanitized assembly ID
*/
QString AssemblyManager::sanitizeAssemblyId(const QString &id) const
{
	QString sanitized = id;
	
	// Remove invalid characters && replace with underscores
	sanitized = sanitized.replace(QRegExp("[^A-Za-z0-9_-]"), "_");
	
	// Ensure it starts with a letter or number
	if (!QRegExp("^[A-Za-z0-9]").exactMatch(sanitized)) {
		sanitized.prepend("ASSEMBLY_");
	}
	
	// Convert to uppercase for consistency
	return sanitized.toUpper();
}

/**
	@brief AssemblyManager::syncWithProject
	Synchronize assembly data with the project
*/
void AssemblyManager::syncWithProject()
{
	if (!m_database) {
		return;
	}

	// Update database with current project state
	m_database->updateDB();
	
	// Invalidate cache to force refresh
	invalidateCache();
}

/**
	@brief AssemblyManager::clearAllAssemblies
	Clear all assemblies from the project
*/
void AssemblyManager::clearAllAssemblies()
{
	if (!m_database) {
		return;
	}

	QStringList assembly_ids = getAllAssemblyIds();
	
	for (const QString &assembly_id : assembly_ids) {
		removeAssembly(assembly_id);
	}
	
	invalidateCache();
}
