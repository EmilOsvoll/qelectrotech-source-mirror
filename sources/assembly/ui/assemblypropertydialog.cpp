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
#include "assemblypropertydialog.h"

#include "../assemblymanager.h"
#include "../assemblyinfo.h"
#include "../../qetproject.h"
#include "../../qeticons.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QHeaderView>
#include <QApplication>

/**
	@brief AssemblyPropertyDialog::AssemblyPropertyDialog
	Constructor
	@param project : The QET project
	@param assembly_id : ID of the assembly to edit (empty for new assembly)
	@param parent : Parent widget
*/
AssemblyPropertyDialog::AssemblyPropertyDialog(QETProject *project, const QString &assembly_id, QWidget *parent) :
	QDialog(parent),
	m_project(project),
	m_assembly_manager(nullptr),
	m_assembly_id(assembly_id),
	m_is_new_assembly(assembly_id.isEmpty())
{
	setWindowTitle(m_is_new_assembly ? tr("Create Assembly", "dialog title") : tr("Edit Assembly", "dialog title"));
	setModal(true);
	setMinimumSize(600, 500);
	
	if (m_project) {
		m_assembly_manager = new AssemblyManager(m_project, this);
	}
	
	setupUI();
	setupConnections();
	
	if (!m_is_new_assembly) {
		loadAssemblyData();
	} else {
		// Set default values for new assembly
		m_type_combo->setCurrentText("Assembly");
	}
}

/**
	@brief AssemblyPropertyDialog::~AssemblyPropertyDialog
	Destructor
*/
AssemblyPropertyDialog::~AssemblyPropertyDialog()
{
	// Cleanup handled by Qt's parent-child relationship
}

/**
	@brief AssemblyPropertyDialog::setAssemblyId
	Set the assembly ID to edit
	@param assembly_id : ID of the assembly
*/
void AssemblyPropertyDialog::setAssemblyId(const QString &assembly_id)
{
	m_assembly_id = assembly_id;
	m_is_new_assembly = assembly_id.isEmpty();
	
	if (!m_is_new_assembly) {
		loadAssemblyData();
	}
}

/**
	@brief AssemblyPropertyDialog::getAssemblyId
	Get the current assembly ID
	@return : Assembly ID
*/
QString AssemblyPropertyDialog::getAssemblyId() const
{
	return m_assembly_id;
}

/**
	@brief AssemblyPropertyDialog::loadAssemblyData
	Load assembly data into the dialog
*/
void AssemblyPropertyDialog::loadAssemblyData()
{
	if (!m_assembly_manager || m_assembly_id.isEmpty()) {
		return;
	}
	
	AssemblyInfo info = m_assembly_manager->getAssemblyInfo(m_assembly_id);
	if (info.assembly_id.isEmpty()) {
		return;
	}
	
	loadBasicProperties();
	loadProperties();
	loadChildren();
}

/**
	@brief AssemblyPropertyDialog::saveAssemblyData
	Save assembly data from the dialog
*/
void AssemblyPropertyDialog::saveAssemblyData()
{
	if (!m_assembly_manager) {
		return;
	}
	
	if (!validateInput()) {
		return;
	}
	
	if (m_is_new_assembly) {
		// Create new assembly
		QString name = m_name_edit->text();
		QString type = m_type_combo->currentText();
		QString description = m_description_edit->toPlainText();
		
		m_assembly_id = m_assembly_manager->createAssembly(name, type, description);
		if (m_assembly_id.isEmpty()) {
			QMessageBox::warning(this, tr("Error", "dialog title"), tr("Failed to create assembly", "error message"));
			return;
		}
		
		m_is_new_assembly = false;
		setWindowTitle(tr("Edit Assembly", "dialog title"));
	}
	
	saveBasicProperties();
	saveProperties();
	saveChildren();
}

/**
	@brief AssemblyPropertyDialog::setupUI
	Set up the user interface
*/
void AssemblyPropertyDialog::setupUI()
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);
	
	// Create tab widget
	m_tab_widget = new QTabWidget();
	main_layout->addWidget(m_tab_widget);
	
	setupBasicPropertiesTab();
	setupPropertiesTab();
	setupChildrenTab();
	
	// Create dialog buttons
	m_button_layout = new QHBoxLayout();
	m_button_layout->addStretch();
	
	m_ok_button = new QPushButton(tr("OK", "dialog button"));
	m_ok_button->setIcon(QET::Icons::DialogOk);
	
	m_cancel_button = new QPushButton(tr("Cancel", "dialog button"));
	m_cancel_button->setIcon(QET::Icons::Cancel);
	
	m_apply_button = new QPushButton(tr("Apply", "dialog button"));
	m_apply_button->setIcon(QET::Icons::DialogOk);
	m_apply_button->setEnabled(false);
	
	m_button_layout->addWidget(m_ok_button);
	m_button_layout->addWidget(m_cancel_button);
	m_button_layout->addWidget(m_apply_button);
	
	main_layout->addLayout(m_button_layout);
}

/**
	@brief AssemblyPropertyDialog::setupBasicPropertiesTab
	Set up the basic properties tab
*/
void AssemblyPropertyDialog::setupBasicPropertiesTab()
{
	m_basic_tab = new QWidget();
	m_tab_widget->addTab(m_basic_tab, tr("Basic Properties", "tab title"));
	
	m_basic_layout = new QFormLayout(m_basic_tab);
	m_basic_layout->setSpacing(8);
	
	// Assembly ID
	m_id_edit = new QLineEdit();
	m_id_edit->setPlaceholderText(tr("Auto-generated if empty", "placeholder"));
	m_id_edit->setEnabled(m_is_new_assembly);
	m_basic_layout->addRow(tr("Assembly ID:", "form label"), m_id_edit);
	
	// Assembly Name
	m_name_edit = new QLineEdit();
	m_name_edit->setPlaceholderText(tr("Enter assembly name", "placeholder"));
	m_basic_layout->addRow(tr("Name:", "form label"), m_name_edit);
	
	// Assembly Type
	m_type_combo = new QComboBox();
	m_type_combo->addItems(QStringList() << "Assembly" << "SubAssembly" << "Component");
	m_basic_layout->addRow(tr("Type:", "form label"), m_type_combo);
	
	// Description
	m_description_edit = new QTextEdit();
	m_description_edit->setMaximumHeight(100);
	m_description_edit->setPlaceholderText(tr("Enter assembly description", "placeholder"));
	m_basic_layout->addRow(tr("Description:", "form label"), m_description_edit);
	
	// Location
	m_location_edit = new QLineEdit();
	m_location_edit->setPlaceholderText(tr("Enter physical location", "placeholder"));
	m_basic_layout->addRow(tr("Location:", "form label"), m_location_edit);
	
	// Responsible
	m_responsible_edit = new QLineEdit();
	m_responsible_edit->setPlaceholderText(tr("Enter responsible engineer", "placeholder"));
	m_basic_layout->addRow(tr("Responsible:", "form label"), m_responsible_edit);
	
	// Revision
	m_revision_edit = new QLineEdit();
	m_revision_edit->setPlaceholderText(tr("Enter revision number", "placeholder"));
	m_basic_layout->addRow(tr("Revision:", "form label"), m_revision_edit);
}

/**
	@brief AssemblyPropertyDialog::setupPropertiesTab
	Set up the properties tab
*/
void AssemblyPropertyDialog::setupPropertiesTab()
{
	m_properties_tab = new QWidget();
	m_tab_widget->addTab(m_properties_tab, tr("Properties", "tab title"));
	
	m_properties_layout = new QVBoxLayout(m_properties_tab);
	m_properties_layout->setSpacing(8);
	
	// Properties group
	m_properties_group = new QGroupBox(tr("Assembly Properties", "group title"));
	m_properties_group_layout = new QVBoxLayout(m_properties_group);
	
	// Properties table
	m_properties_table = new QTableWidget();
	m_properties_table->setColumnCount(3);
	m_properties_table->setHorizontalHeaderLabels(QStringList() << tr("Key", "table header") << tr("Value", "table header") << tr("Type", "table header"));
	m_properties_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_properties_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_properties_table->setAlternatingRowColors(true);
	m_properties_table->horizontalHeader()->setStretchLastSection(true);
	m_properties_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_properties_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	m_properties_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	
	m_properties_group_layout->addWidget(m_properties_table);
	
	// Property buttons
	m_property_buttons_layout = new QHBoxLayout();
	m_add_property_button = new QPushButton(tr("Add Property", "button"));
	m_add_property_button->setIcon(QET::Icons::Add);
	m_remove_property_button = new QPushButton(tr("Remove Property", "button"));
	m_remove_property_button->setIcon(QET::Icons::Remove);
	m_remove_property_button->setEnabled(false);
	
	m_property_buttons_layout->addWidget(m_add_property_button);
	m_property_buttons_layout->addWidget(m_remove_property_button);
	m_property_buttons_layout->addStretch();
	
	m_properties_group_layout->addLayout(m_property_buttons_layout);
	m_properties_layout->addWidget(m_properties_group);
}

/**
	@brief AssemblyPropertyDialog::setupChildrenTab
	Set up the children tab
*/
void AssemblyPropertyDialog::setupChildrenTab()
{
	m_children_tab = new QWidget();
	m_tab_widget->addTab(m_children_tab, tr("Child Elements", "tab title"));
	
	m_children_layout = new QVBoxLayout(m_children_tab);
	m_children_layout->setSpacing(8);
	
	// Children group
	m_children_group = new QGroupBox(tr("Child Elements", "group title"));
	m_children_group_layout = new QVBoxLayout(m_children_group);
	
	// Children table
	m_children_table = new QTableWidget();
	m_children_table->setColumnCount(2);
	m_children_table->setHorizontalHeaderLabels(QStringList() << tr("Element UUID", "table header") << tr("Description", "table header"));
	m_children_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	m_children_table->setSelectionMode(QAbstractItemView::SingleSelection);
	m_children_table->setAlternatingRowColors(true);
	m_children_table->horizontalHeader()->setStretchLastSection(true);
	m_children_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	m_children_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
	
	m_children_group_layout->addWidget(m_children_table);
	
	// Child buttons
	m_child_buttons_layout = new QHBoxLayout();
	m_add_child_button = new QPushButton(tr("Add Child", "button"));
	m_add_child_button->setIcon(QET::Icons::Add);
	m_remove_child_button = new QPushButton(tr("Remove Child", "button"));
	m_remove_child_button->setIcon(QET::Icons::Remove);
	m_remove_child_button->setEnabled(false);
	
	m_child_buttons_layout->addWidget(m_add_child_button);
	m_child_buttons_layout->addWidget(m_remove_child_button);
	m_child_buttons_layout->addStretch();
	
	m_children_group_layout->addLayout(m_child_buttons_layout);
	m_children_layout->addWidget(m_children_group);
}

/**
	@brief AssemblyPropertyDialog::setupConnections
	Set up signal-slot connections
*/
void AssemblyPropertyDialog::setupConnections()
{
	connect(m_ok_button, &QPushButton::clicked, this, &AssemblyPropertyDialog::onOkClicked);
	connect(m_cancel_button, &QPushButton::clicked, this, &AssemblyPropertyDialog::onCancelClicked);
	connect(m_apply_button, &QPushButton::clicked, this, &AssemblyPropertyDialog::onApplyClicked);
	
	connect(m_add_property_button, &QPushButton::clicked, this, &AssemblyPropertyDialog::onAddPropertyClicked);
	connect(m_remove_property_button, &QPushButton::clicked, this, &AssemblyPropertyDialog::onRemovePropertyClicked);
	
	connect(m_add_child_button, &QPushButton::clicked, this, &AssemblyPropertyDialog::onAddChildClicked);
	connect(m_remove_child_button, &QPushButton::clicked, this, &AssemblyPropertyDialog::onRemoveChildClicked);
	
	connect(m_properties_table, &QTableWidget::itemSelectionChanged, this, &AssemblyPropertyDialog::onPropertyTableSelectionChanged);
	connect(m_children_table, &QTableWidget::itemSelectionChanged, this, &AssemblyPropertyDialog::updateChildButtons);
}

/**
	@brief AssemblyPropertyDialog::onOkClicked
	Handle OK button click
*/
void AssemblyPropertyDialog::onOkClicked()
{
	saveAssemblyData();
	accept();
}

/**
	@brief AssemblyPropertyDialog::onCancelClicked
	Handle Cancel button click
*/
void AssemblyPropertyDialog::onCancelClicked()
{
	reject();
}

/**
	@brief AssemblyPropertyDialog::onApplyClicked
	Handle Apply button click
*/
void AssemblyPropertyDialog::onApplyClicked()
{
	saveAssemblyData();
	m_apply_button->setEnabled(false);
}

/**
	@brief AssemblyPropertyDialog::onAddPropertyClicked
	Handle Add Property button click
*/
void AssemblyPropertyDialog::onAddPropertyClicked()
{
	int row = m_properties_table->rowCount();
	m_properties_table->insertRow(row);
	
	// Key column
	QTableWidgetItem *key_item = new QTableWidgetItem();
	m_properties_table->setItem(row, 0, key_item);
	
	// Value column
	QTableWidgetItem *value_item = new QTableWidgetItem();
	m_properties_table->setItem(row, 1, value_item);
	
	// Type column
	QComboBox *type_combo = new QComboBox();
	type_combo->addItems(QStringList() << "inherited" << "overridden");
	type_combo->setCurrentText("inherited");
	m_properties_table->setCellWidget(row, 2, type_combo);
	
	// Select the new row
	m_properties_table->selectRow(row);
	m_properties_table->editItem(key_item);
	
	m_apply_button->setEnabled(true);
}

/**
	@brief AssemblyPropertyDialog::onRemovePropertyClicked
	Handle Remove Property button click
*/
void AssemblyPropertyDialog::onRemovePropertyClicked()
{
	int row = m_properties_table->currentRow();
	if (row >= 0) {
		m_properties_table->removeRow(row);
		m_apply_button->setEnabled(true);
	}
}

/**
	@brief AssemblyPropertyDialog::onAddChildClicked
	Handle Add Child button click
*/
void AssemblyPropertyDialog::onAddChildClicked()
{
	QString uuid = QInputDialog::getText(this, tr("Add Child Element", "dialog title"), tr("Element UUID:", "dialog label"));
	if (!uuid.isEmpty()) {
		int row = m_children_table->rowCount();
		m_children_table->insertRow(row);
		
		QTableWidgetItem *uuid_item = new QTableWidgetItem(uuid);
		m_children_table->setItem(row, 0, uuid_item);
		
		QTableWidgetItem *desc_item = new QTableWidgetItem(tr("Element", "default description"));
		m_children_table->setItem(row, 1, desc_item);
		
		m_apply_button->setEnabled(true);
	}
}

/**
	@brief AssemblyPropertyDialog::onRemoveChildClicked
	Handle Remove Child button click
*/
void AssemblyPropertyDialog::onRemoveChildClicked()
{
	int row = m_children_table->currentRow();
	if (row >= 0) {
		m_children_table->removeRow(row);
		m_apply_button->setEnabled(true);
	}
}

/**
	@brief AssemblyPropertyDialog::onPropertyTypeChanged
	Handle property type change
	@param index : New type index
*/
void AssemblyPropertyDialog::onPropertyTypeChanged(int index)
{
	Q_UNUSED(index)
	m_apply_button->setEnabled(true);
}

/**
	@brief AssemblyPropertyDialog::onPropertyTableSelectionChanged
	Handle property table selection change
*/
void AssemblyPropertyDialog::onPropertyTableSelectionChanged()
{
	updatePropertyButtons();
}

/**
	@brief AssemblyPropertyDialog::updatePropertyButtons
	Update property button states
*/
void AssemblyPropertyDialog::updatePropertyButtons()
{
	bool has_selection = (m_properties_table->currentRow() >= 0);
	m_remove_property_button->setEnabled(has_selection);
}

/**
	@brief AssemblyPropertyDialog::updateChildButtons
	Update child button states
*/
void AssemblyPropertyDialog::updateChildButtons()
{
	bool has_selection = (m_children_table->currentRow() >= 0);
	m_remove_child_button->setEnabled(has_selection);
}

/**
	@brief AssemblyPropertyDialog::loadBasicProperties
	Load basic properties from assembly
*/
void AssemblyPropertyDialog::loadBasicProperties()
{
	if (!m_assembly_manager || m_assembly_id.isEmpty()) {
		return;
	}
	
	AssemblyInfo info = m_assembly_manager->getAssemblyInfo(m_assembly_id);
	
	m_id_edit->setText(info.assembly_id);
	m_name_edit->setText(info.assembly_name);
	m_type_combo->setCurrentText(info.assembly_type);
	m_description_edit->setPlainText(info.description);
	m_location_edit->setText(info.location);
	m_responsible_edit->setText(info.responsible);
	m_revision_edit->setText(info.revision);
}

/**
	@brief AssemblyPropertyDialog::loadProperties
	Load properties from assembly
*/
void AssemblyPropertyDialog::loadProperties()
{
	if (!m_assembly_manager || m_assembly_id.isEmpty()) {
		return;
	}
	
	AssemblyInfo info = m_assembly_manager->getAssemblyInfo(m_assembly_id);
	
	m_properties_table->setRowCount(0);
	
	// Load inherited properties
	for (auto it = info.inherited_properties.constBegin(); it != info.inherited_properties.constEnd(); ++it) {
		int row = m_properties_table->rowCount();
		m_properties_table->insertRow(row);
		
		m_properties_table->setItem(row, 0, new QTableWidgetItem(it.key()));
		m_properties_table->setItem(row, 1, new QTableWidgetItem(it.value().toString()));
		
		QComboBox *type_combo = new QComboBox();
		type_combo->addItems(QStringList() << "inherited" << "overridden");
		type_combo->setCurrentText("inherited");
		m_properties_table->setCellWidget(row, 2, type_combo);
	}
	
	// Load overridden properties
	for (auto it = info.overridden_properties.constBegin(); it != info.overridden_properties.constEnd(); ++it) {
		int row = m_properties_table->rowCount();
		m_properties_table->insertRow(row);
		
		m_properties_table->setItem(row, 0, new QTableWidgetItem(it.key()));
		m_properties_table->setItem(row, 1, new QTableWidgetItem(it.value().toString()));
		
		QComboBox *type_combo = new QComboBox();
		type_combo->addItems(QStringList() << "inherited" << "overridden");
		type_combo->setCurrentText("overridden");
		m_properties_table->setCellWidget(row, 2, type_combo);
	}
}

/**
	@brief AssemblyPropertyDialog::loadChildren
	Load children from assembly
*/
void AssemblyPropertyDialog::loadChildren()
{
	if (!m_assembly_manager || m_assembly_id.isEmpty()) {
		return;
	}
	
	QStringList children = m_assembly_manager->getAssemblyChildren(m_assembly_id);
	
	m_children_table->setRowCount(0);
	
	for (const QString &child_uuid : children) {
		int row = m_children_table->rowCount();
		m_children_table->insertRow(row);
		
		m_children_table->setItem(row, 0, new QTableWidgetItem(child_uuid));
		m_children_table->setItem(row, 1, new QTableWidgetItem(tr("Element", "default description")));
	}
}

/**
	@brief AssemblyPropertyDialog::saveBasicProperties
	Save basic properties to assembly
*/
void AssemblyPropertyDialog::saveBasicProperties()
{
	if (!m_assembly_manager || m_assembly_id.isEmpty()) {
		return;
	}
	
	// Update basic properties
	m_assembly_manager->updateAssembly(m_assembly_id, "assembly_name", m_name_edit->text());
	m_assembly_manager->updateAssembly(m_assembly_id, "assembly_type", m_type_combo->currentText());
	m_assembly_manager->updateAssembly(m_assembly_id, "description", m_description_edit->toPlainText());
	m_assembly_manager->updateAssembly(m_assembly_id, "location", m_location_edit->text());
	m_assembly_manager->updateAssembly(m_assembly_id, "responsible", m_responsible_edit->text());
	m_assembly_manager->updateAssembly(m_assembly_id, "revision", m_revision_edit->text());
}

/**
	@brief AssemblyPropertyDialog::saveProperties
	Save properties to assembly
*/
void AssemblyPropertyDialog::saveProperties()
{
	if (!m_assembly_manager || m_assembly_id.isEmpty()) {
		return;
	}
	
	// Clear existing properties
	AssemblyInfo info = m_assembly_manager->getAssemblyInfo(m_assembly_id);
	for (auto it = info.inherited_properties.constBegin(); it != info.inherited_properties.constEnd(); ++it) {
		m_assembly_manager->removeAssemblyProperty(m_assembly_id, it.key());
	}
	for (auto it = info.overridden_properties.constBegin(); it != info.overridden_properties.constEnd(); ++it) {
		m_assembly_manager->removeAssemblyProperty(m_assembly_id, it.key());
	}
	
	// Save new properties
	for (int row = 0; row < m_properties_table->rowCount(); ++row) {
		QString key = m_properties_table->item(row, 0)->text();
		QString value = m_properties_table->item(row, 1)->text();
		
		if (!key.isEmpty()) {
			QComboBox *type_combo = qobject_cast<QComboBox*>(m_properties_table->cellWidget(row, 2));
			QString type = type_combo ? type_combo->currentText() : "inherited";
			m_assembly_manager->setAssemblyProperty(m_assembly_id, key, value, type);
		}
	}
}

/**
	@brief AssemblyPropertyDialog::saveChildren
	Save children to assembly
*/
void AssemblyPropertyDialog::saveChildren()
{
	if (!m_assembly_manager || m_assembly_id.isEmpty()) {
		return;
	}
	
	// Get current children
	QStringList current_children = m_assembly_manager->getAssemblyChildren(m_assembly_id);
	
	// Get new children from table
	QStringList new_children;
	for (int row = 0; row < m_children_table->rowCount(); ++row) {
		QString uuid = m_children_table->item(row, 0)->text();
		if (!uuid.isEmpty()) {
			new_children.append(uuid);
		}
	}
	
	// Remove children that are no longer in the list
	for (const QString &child_uuid : current_children) {
		if (!new_children.contains(child_uuid)) {
			m_assembly_manager->removeChildFromAssembly(m_assembly_id, child_uuid);
		}
	}
	
	// Add new children
	for (const QString &child_uuid : new_children) {
		if (!current_children.contains(child_uuid)) {
			m_assembly_manager->addChildToAssembly(m_assembly_id, child_uuid);
		}
	}
}

/**
	@brief AssemblyPropertyDialog::validateInput
	Validate user input
	@return : true if input is valid
*/
bool AssemblyPropertyDialog::validateInput()
{
	if (m_name_edit->text().isEmpty()) {
		QMessageBox::warning(this, tr("Validation Error", "dialog title"), tr("Assembly name cannot be empty", "error message"));
		m_name_edit->setFocus();
		return false;
	}
	
	if (m_type_combo->currentText().isEmpty()) {
		QMessageBox::warning(this, tr("Validation Error", "dialog title"), tr("Assembly type must be selected", "error message"));
		m_type_combo->setFocus();
		return false;
	}
	
	return true;
}
