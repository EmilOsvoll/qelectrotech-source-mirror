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
#ifndef ASSEMBLYPROPERTYDIALOG_H
#define ASSEMBLYPROPERTYDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableWidget>
#include <QHeaderView>
#include <QTabWidget>

class QETProject;
class AssemblyManager;
class AssemblyInfo;

/**
	@brief The AssemblyPropertyDialog class
	Provides a dialog for editing assembly properties and managing
	child elements and inherited/overridden properties.
*/
class AssemblyPropertyDialog : public QDialog
{
	Q_OBJECT

	public:
		explicit AssemblyPropertyDialog(QETProject *project, const QString &assembly_id = QString(), QWidget *parent = nullptr);
		virtual ~AssemblyPropertyDialog() override;

		// Public methods
		void setAssemblyId(const QString &assembly_id);
		QString getAssemblyId() const;
		void loadAssemblyData();
		void saveAssemblyData();

	private slots:
		void onOkClicked();
		void onCancelClicked();
		void onApplyClicked();
		void onAddPropertyClicked();
		void onRemovePropertyClicked();
		void onAddChildClicked();
		void onRemoveChildClicked();
		void onPropertyTypeChanged(int index);
		void onPropertyTableSelectionChanged();

	private:
		void setupUI();
		void setupBasicPropertiesTab();
		void setupPropertiesTab();
		void setupChildrenTab();
		void setupConnections();
		void updatePropertyButtons();
		void updateChildButtons();
		void loadBasicProperties();
		void loadProperties();
		void loadChildren();
		void saveBasicProperties();
		void saveProperties();
		void saveChildren();
		bool validateInput();

	private:
		QETProject *m_project;
		AssemblyManager *m_assembly_manager;
		QString m_assembly_id;
		bool m_is_new_assembly;
		
		// UI Components
		QTabWidget *m_tab_widget;
		
		// Basic Properties Tab
		QWidget *m_basic_tab;
		QFormLayout *m_basic_layout;
		QLineEdit *m_id_edit;
		QLineEdit *m_name_edit;
		QComboBox *m_type_combo;
		QTextEdit *m_description_edit;
		QLineEdit *m_location_edit;
		QLineEdit *m_responsible_edit;
		QLineEdit *m_revision_edit;
		
		// Properties Tab
		QWidget *m_properties_tab;
		QVBoxLayout *m_properties_layout;
		QGroupBox *m_properties_group;
		QVBoxLayout *m_properties_group_layout;
		QTableWidget *m_properties_table;
		QHBoxLayout *m_property_buttons_layout;
		QPushButton *m_add_property_button;
		QPushButton *m_remove_property_button;
		
		// Children Tab
		QWidget *m_children_tab;
		QVBoxLayout *m_children_layout;
		QGroupBox *m_children_group;
		QVBoxLayout *m_children_group_layout;
		QTableWidget *m_children_table;
		QHBoxLayout *m_child_buttons_layout;
		QPushButton *m_add_child_button;
		QPushButton *m_remove_child_button;
		
		// Dialog buttons
		QHBoxLayout *m_button_layout;
		QPushButton *m_ok_button;
		QPushButton *m_cancel_button;
		QPushButton *m_apply_button;
};

#endif // ASSEMBLYPROPERTYDIALOG_H
