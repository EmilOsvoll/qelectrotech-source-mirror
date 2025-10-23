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
#ifndef ASSEMBLYMANAGER_H
#define ASSEMBLYMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVariant>
#include <QDomElement>
#include <QDomDocument>

class QETProject;
class Element;
class projectDataBase;
class AssemblyInfo;

/**
	@brief The AssemblyManager class
	Manages assemblies && their relationships within a QET project.
	This class provides high-level functionality for creating, managing,
	&& querying assemblies && their hierarchical relationships.
*/
class AssemblyManager : public QObject
{
	Q_OBJECT

	public:
		explicit AssemblyManager(QETProject *project, QObject *parent = nullptr);
		virtual ~AssemblyManager() override;

		// Core assembly management
		QString createAssembly(const QString &name, const QString &type = "Assembly", const QString &description = QString());
		bool removeAssembly(const QString &assembly_id);
		bool updateAssembly(const QString &assembly_id, const QString &field, const QString &value);
		
		// Assembly information access
		AssemblyInfo getAssemblyInfo(const QString &assembly_id) const;
		QStringList getAllAssemblyIds() const;
		QStringList getAssemblyIdsByType(const QString &type) const;
		bool assemblyExists(const QString &assembly_id) const;

		// Child element management
		bool addChildToAssembly(const QString &assembly_id, const QString &element_uuid);
		bool removeChildFromAssembly(const QString &assembly_id, const QString &element_uuid);
		QStringList getAssemblyChildren(const QString &assembly_id) const;
		bool isElementInAssembly(const QString &element_uuid, const QString &assembly_id) const;

		// Property management
		bool setAssemblyProperty(const QString &assembly_id, const QString &key, const QVariant &value, const QString &type = "inherited");
		bool removeAssemblyProperty(const QString &assembly_id, const QString &key);
		QVariant getAssemblyProperty(const QString &assembly_id, const QString &key) const;
		QMap<QString, QVariant> getAllAssemblyProperties(const QString &assembly_id) const;

		// Hierarchical operations
		bool setParentAssembly(const QString &child_assembly_id, const QString &parent_assembly_id);
		bool removeParentAssembly(const QString &child_assembly_id);
		QString getParentAssembly(const QString &child_assembly_id) const;
		QStringList getChildAssemblies(const QString &parent_assembly_id) const;

		// Element operations
		bool assignElementToAssembly(Element *element, const QString &assembly_id);
		bool removeElementFromAssembly(Element *element, const QString &assembly_id);
		QStringList getElementAssemblies(const QString &element_uuid) const;

		// Validation && utilities
		bool validateAssemblyId(const QString &assembly_id) const;
		QString generateAssemblyId(const QString &base_name) const;
		bool canRemoveAssembly(const QString &assembly_id) const;

		// Project integration
		void syncWithProject();
		void clearAllAssemblies();

	signals:
		void assemblyCreated(const QString &assembly_id);
		void assemblyRemoved(const QString &assembly_id);
		void assemblyUpdated(const QString &assembly_id);
		void assemblyChildAdded(const QString &assembly_id, const QString &element_uuid);
		void assemblyChildRemoved(const QString &assembly_id, const QString &element_uuid);
		void assemblyPropertyChanged(const QString &assembly_id, const QString &key);

	private:
		QETProject *m_project;
		projectDataBase *m_database;
		
		// Cache for performance
		mutable QMap<QString, AssemblyInfo> m_assembly_cache;
		mutable bool m_cache_dirty;

		// Helper methods
		void invalidateCache();
		void updateCache();
		bool isValidAssemblyType(const QString &type) const;
		QString sanitizeAssemblyId(const QString &id) const;
};

#endif // ASSEMBLYMANAGER_H
