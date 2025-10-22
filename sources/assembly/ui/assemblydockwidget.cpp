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
#include "assemblydockwidget.h"

#include "../assemblymanager.h"
#include "../assemblyinfo.h"
#include "../../qetproject.h"
#include "../../qeticons.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QApplication>

/**
	@brief AssemblyDockWidget::AssemblyDockWidget
	Constructor
	@param project : The QET project this widget is associated with
	@param parent : Parent widget
*/
AssemblyDockWidget::AssemblyDockWidget(QETProject *project, QWidget *parent) :
	QDockWidget(parent),
	m_project(project),
	m_assembly_manager(nullptr),
	m_updating_tree(false),
	m_current_filter_text(),
	m_current_filter_type("All")
{
	setWindowTitle(tr("Assembly Manager", "dock widget title"));
	setObjectName("assembly_dock_widget");
	setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	setMinimumWidth(300);

	setupUI();
	setupContextMenu();
	connectSignals();

	if (m_project) {
		setProject(m_project);
	}
}

/**
	@brief AssemblyDockWidget::~AssemblyDockWidget
	Destructor
*/
AssemblyDockWidget::~AssemblyDockWidget()
{
	// Cleanup handled by Qt's parent-child relationship
}

/**
	@brief AssemblyDockWidget::setProject
	Set the project for this dock widget
	@param project : The QET project
*/
void AssemblyDockWidget::setProject(QETProject *project)
{
	m_project = project;
	
	if (m_project) {
		m_assembly_manager = new AssemblyManager(m_project, this);
		connect(m_assembly_manager, &AssemblyManager::assemblyCreated, this, &AssemblyDockWidget::onAssemblyCreated);
		connect(m_assembly_manager, &AssemblyManager::assemblyRemoved, this, &AssemblyDockWidget::onAssemblyRemoved);
		connect(m_assembly_manager, &AssemblyManager::assemblyUpdated, this, &AssemblyDockWidget::onAssemblyUpdated);
		connect(m_assembly_manager, &AssemblyManager::assemblyChildAdded, this, &AssemblyDockWidget::onAssemblyChildAdded);
		connect(m_assembly_manager, &AssemblyManager::assemblyChildRemoved, this, &AssemblyDockWidget::onAssemblyChildRemoved);
		connect(m_assembly_manager, &AssemblyManager::assemblyPropertyChanged, this, &AssemblyDockWidget::onAssemblyPropertyChanged);
		
		refreshAssemblyTree();
	} else {
		if (m_assembly_manager) {
			m_assembly_manager->deleteLater();
			m_assembly_manager = nullptr;
		}
		m_assembly_tree->clear();
	}
}

/**
	@brief AssemblyDockWidget::refreshAssemblyTree
	Refresh the assembly tree display
*/
void AssemblyDockWidget::refreshAssemblyTree()
{
	if (!m_assembly_manager) {
		return;
	}

	m_updating_tree = true;
	m_assembly_tree->clear();
	populateTree();
	applyFilters();
	m_updating_tree = false;
}

/**
	@brief AssemblyDockWidget::selectAssembly
	Select an assembly in the tree
	@param assembly_id : ID of the assembly to select
*/
void AssemblyDockWidget::selectAssembly(const QString &assembly_id)
{
	QTreeWidgetItem *item = findAssemblyItem(assembly_id);
	if (item) {
		m_assembly_tree->setCurrentItem(item);
		m_assembly_tree->scrollToItem(item);
	}
}

/**
	@brief AssemblyDockWidget::clearSelection
	Clear the current selection
*/
void AssemblyDockWidget::clearSelection()
{
	m_assembly_tree->clearSelection();
}

/**
	@brief AssemblyDockWidget::setupUI
	Set up the user interface
*/
void AssemblyDockWidget::setupUI()
{
	m_main_widget = new QWidget(this);
	setWidget(m_main_widget);
	
	m_main_layout = new QVBoxLayout(m_main_widget);
	m_main_layout->setContentsMargins(4, 4, 4, 4);
	m_main_layout->setSpacing(4);
	
	setupToolbar();
	setupTreeWidget();
}

/**
	@brief AssemblyDockWidget::setupToolbar
	Set up the toolbar with action buttons
*/
void AssemblyDockWidget::setupToolbar()
{
	m_toolbar_widget = new QWidget();
	m_toolbar_layout = new QHBoxLayout(m_toolbar_widget);
	m_toolbar_layout->setContentsMargins(0, 0, 0, 0);
	m_toolbar_layout->setSpacing(2);
	
	// Create buttons
	m_create_button = new QPushButton(tr("Create", "assembly action"));
	m_create_button->setIcon(QET::Icons::Add);
	m_create_button->setToolTip(tr("Create a new assembly", "tooltip"));
	
	m_edit_button = new QPushButton(tr("Edit", "assembly action"));
	m_edit_button->setIcon(QET::Icons::ElementEdit);
	m_edit_button->setToolTip(tr("Edit selected assembly", "tooltip"));
	m_edit_button->setEnabled(false);
	
	m_delete_button = new QPushButton(tr("Delete", "assembly action"));
	m_delete_button->setIcon(QET::Icons::EditDelete);
	m_delete_button->setToolTip(tr("Delete selected assembly", "tooltip"));
	m_delete_button->setEnabled(false);
	
	m_add_child_button = new QPushButton(tr("Add Child", "assembly action"));
	m_add_child_button->setIcon(QET::Icons::Add);
	m_add_child_button->setToolTip(tr("Add child element to selected assembly", "tooltip"));
	m_add_child_button->setEnabled(false);
	
	m_remove_child_button = new QPushButton(tr("Remove Child", "assembly action"));
	m_remove_child_button->setIcon(QET::Icons::Remove);
	m_remove_child_button->setToolTip(tr("Remove child element from selected assembly", "tooltip"));
	m_remove_child_button->setEnabled(false);
	
	m_refresh_button = new QPushButton(tr("Refresh", "assembly action"));
	m_refresh_button->setIcon(QET::Icons::ViewRefresh);
	m_refresh_button->setToolTip(tr("Refresh assembly tree", "tooltip"));
	
	// Add buttons to layout
	m_toolbar_layout->addWidget(m_create_button);
	m_toolbar_layout->addWidget(m_edit_button);
	m_toolbar_layout->addWidget(m_delete_button);
	m_toolbar_layout->addWidget(m_add_child_button);
	m_toolbar_layout->addWidget(m_remove_child_button);
	m_toolbar_layout->addStretch();
	m_toolbar_layout->addWidget(m_refresh_button);
	
	m_main_layout->addWidget(m_toolbar_widget);
}

/**
	@brief AssemblyDockWidget::setupTreeWidget
	Set up the assembly tree widget
*/
void AssemblyDockWidget::setupTreeWidget()
{
	// Create filter controls
	m_filter_group = new QGroupBox(tr("Filter", "assembly filter group"));
	m_filter_layout = new QHBoxLayout(m_filter_group);
	m_filter_layout->setContentsMargins(4, 4, 4, 4);
	m_filter_layout->setSpacing(4);
	
	m_filter_label = new QLabel(tr("Name:", "filter label"));
	m_filter_edit = new QLineEdit();
	m_filter_edit->setPlaceholderText(tr("Filter by name...", "filter placeholder"));
	
	m_type_label = new QLabel(tr("Type:", "filter label"));
	m_type_combo = new QComboBox();
	m_type_combo->addItem(tr("All", "assembly type filter"));
	m_type_combo->addItem(tr("Assembly", "assembly type"));
	m_type_combo->addItem(tr("SubAssembly", "assembly type"));
	m_type_combo->addItem(tr("Component", "assembly type"));
	
	m_filter_layout->addWidget(m_filter_label);
	m_filter_layout->addWidget(m_filter_edit);
	m_filter_layout->addWidget(m_type_label);
	m_filter_layout->addWidget(m_type_combo);
	
	m_main_layout->addWidget(m_filter_group);
	
	// Create tree widget
	m_assembly_tree = new QTreeWidget();
	m_assembly_tree->setHeaderLabels(QStringList() << tr("Assembly", "tree header") << tr("Type", "tree header") << tr("Children", "tree header"));
	m_assembly_tree->setRootIsDecorated(true);
	m_assembly_tree->setAlternatingRowColors(true);
	m_assembly_tree->setSelectionMode(QAbstractItemView::SingleSelection);
	m_assembly_tree->setContextMenuPolicy(Qt::CustomContextMenu);
	m_assembly_tree->header()->setStretchLastSection(false);
	m_assembly_tree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
	m_assembly_tree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
	m_assembly_tree->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	
	m_main_layout->addWidget(m_assembly_tree);
}

/**
	@brief AssemblyDockWidget::setupContextMenu
	Set up the context menu
*/
void AssemblyDockWidget::setupContextMenu()
{
	m_context_menu = new QMenu(this);
	
	m_create_action = m_context_menu->addAction(QET::Icons::Add, tr("Create Assembly", "context menu"));
	m_edit_action = m_context_menu->addAction(QET::Icons::ElementEdit, tr("Edit Assembly", "context menu"));
	m_delete_action = m_context_menu->addAction(QET::Icons::EditDelete, tr("Delete Assembly", "context menu"));
	m_context_menu->addSeparator();
	m_add_child_action = m_context_menu->addAction(QET::Icons::Add, tr("Add Child Element", "context menu"));
	m_remove_child_action = m_context_menu->addAction(QET::Icons::Remove, tr("Remove Child Element", "context menu"));
	m_context_menu->addSeparator();
	m_refresh_action = m_context_menu->addAction(QET::Icons::ViewRefresh, tr("Refresh", "context menu"));
}

/**
	@brief AssemblyDockWidget::connectSignals
	Connect all signals and slots
*/
void AssemblyDockWidget::connectSignals()
{
	// Toolbar buttons
	connect(m_create_button, &QPushButton::clicked, this, &AssemblyDockWidget::createAssembly);
	connect(m_edit_button, &QPushButton::clicked, this, &AssemblyDockWidget::editSelectedAssembly);
	connect(m_delete_button, &QPushButton::clicked, this, &AssemblyDockWidget::deleteSelectedAssembly);
	connect(m_add_child_button, &QPushButton::clicked, this, &AssemblyDockWidget::addChildToSelectedAssembly);
	connect(m_remove_child_button, &QPushButton::clicked, this, &AssemblyDockWidget::removeChildFromSelectedAssembly);
	connect(m_refresh_button, &QPushButton::clicked, this, &AssemblyDockWidget::refreshTree);
	
	// Tree widget
	connect(m_assembly_tree, &QTreeWidget::itemSelectionChanged, this, &AssemblyDockWidget::onTreeItemSelectionChanged);
	connect(m_assembly_tree, &QTreeWidget::itemDoubleClicked, this, &AssemblyDockWidget::onTreeItemDoubleClicked);
	connect(m_assembly_tree, &QTreeWidget::customContextMenuRequested, this, &AssemblyDockWidget::onContextMenuRequested);
	
	// Filter controls
	connect(m_filter_edit, &QLineEdit::textChanged, this, &AssemblyDockWidget::onFilterTextChanged);
	connect(m_type_combo, QOverload<const QString &>::of(&QComboBox::currentTextChanged), this, &AssemblyDockWidget::onFilterTypeChanged);
	
	// Context menu actions
	connect(m_create_action, &QAction::triggered, this, &AssemblyDockWidget::createAssembly);
	connect(m_edit_action, &QAction::triggered, this, &AssemblyDockWidget::editSelectedAssembly);
	connect(m_delete_action, &QAction::triggered, this, &AssemblyDockWidget::deleteSelectedAssembly);
	connect(m_add_child_action, &QAction::triggered, this, &AssemblyDockWidget::addChildToSelectedAssembly);
	connect(m_remove_child_action, &QAction::triggered, this, &AssemblyDockWidget::removeChildFromSelectedAssembly);
	connect(m_refresh_action, &QAction::triggered, this, &AssemblyDockWidget::refreshTree);
}

/**
	@brief AssemblyDockWidget::onAssemblyCreated
	Handle assembly creation signal
	@param assembly_id : ID of the created assembly
*/
void AssemblyDockWidget::onAssemblyCreated(const QString &assembly_id)
{
	if (m_updating_tree) {
		return;
	}
	
	QTreeWidgetItem *item = createAssemblyItem(assembly_id);
	if (item) {
		m_assembly_tree->addTopLevelItem(item);
		applyFilters();
	}
}

/**
	@brief AssemblyDockWidget::onAssemblyRemoved
	Handle assembly removal signal
	@param assembly_id : ID of the removed assembly
*/
void AssemblyDockWidget::onAssemblyRemoved(const QString &assembly_id)
{
	if (m_updating_tree) {
		return;
	}
	
	removeAssemblyItem(assembly_id);
}

/**
	@brief AssemblyDockWidget::onAssemblyUpdated
	Handle assembly update signal
	@param assembly_id : ID of the updated assembly
*/
void AssemblyDockWidget::onAssemblyUpdated(const QString &assembly_id)
{
	if (m_updating_tree) {
		return;
	}
	
	QTreeWidgetItem *item = findAssemblyItem(assembly_id);
	if (item) {
		updateAssemblyItem(item, assembly_id);
	}
}

/**
	@brief AssemblyDockWidget::onAssemblyChildAdded
	Handle assembly child addition signal
	@param assembly_id : ID of the assembly
	@param element_uuid : UUID of the added child element
*/
void AssemblyDockWidget::onAssemblyChildAdded(const QString &assembly_id, const QString &element_uuid)
{
	if (m_updating_tree) {
		return;
	}
	
	QTreeWidgetItem *parent_item = findAssemblyItem(assembly_id);
	if (parent_item) {
		addChildToAssemblyItem(parent_item, element_uuid);
	}
}

/**
	@brief AssemblyDockWidget::onAssemblyChildRemoved
	Handle assembly child removal signal
	@param assembly_id : ID of the assembly
	@param element_uuid : UUID of the removed child element
*/
void AssemblyDockWidget::onAssemblyChildRemoved(const QString &assembly_id, const QString &element_uuid)
{
	if (m_updating_tree) {
		return;
	}
	
	QTreeWidgetItem *parent_item = findAssemblyItem(assembly_id);
	if (parent_item) {
		removeChildFromAssemblyItem(parent_item, element_uuid);
	}
}

/**
	@brief AssemblyDockWidget::onAssemblyPropertyChanged
	Handle assembly property change signal
	@param assembly_id : ID of the assembly
	@param key : Property key that changed
*/
void AssemblyDockWidget::onAssemblyPropertyChanged(const QString &assembly_id, const QString &key)
{
	if (m_updating_tree) {
		return;
	}
	
	QTreeWidgetItem *item = findAssemblyItem(assembly_id);
	if (item) {
		updateAssemblyItem(item, assembly_id);
	}
}

/**
	@brief AssemblyDockWidget::onTreeItemSelectionChanged
	Handle tree item selection change
*/
void AssemblyDockWidget::onTreeItemSelectionChanged()
{
	QTreeWidgetItem *item = m_assembly_tree->currentItem();
	bool has_selection = (item != nullptr);
	
	m_edit_button->setEnabled(has_selection);
	m_delete_button->setEnabled(has_selection);
	m_add_child_button->setEnabled(has_selection);
	m_remove_child_button->setEnabled(has_selection);
	
	m_edit_action->setEnabled(has_selection);
	m_delete_action->setEnabled(has_selection);
	m_add_child_action->setEnabled(has_selection);
	m_remove_child_action->setEnabled(has_selection);
	
	if (has_selection) {
		QString assembly_id = item->data(0, Qt::UserRole).toString();
		emit assemblySelected(assembly_id);
	}
}

/**
	@brief AssemblyDockWidget::onTreeItemDoubleClicked
	Handle tree item double click
	@param item : The double-clicked item
	@param column : Column that was double-clicked
*/
void AssemblyDockWidget::onTreeItemDoubleClicked(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(column)
	
	if (item) {
		QString assembly_id = item->data(0, Qt::UserRole).toString();
		emit assemblyDoubleClicked(assembly_id);
	}
}

/**
	@brief AssemblyDockWidget::onContextMenuRequested
	Handle context menu request
	@param pos : Position where context menu was requested
*/
void AssemblyDockWidget::onContextMenuRequested(const QPoint &pos)
{
	QTreeWidgetItem *item = m_assembly_tree->itemAt(pos);
	
	// Enable/disable context menu actions based on selection
	bool has_selection = (item != nullptr);
	m_edit_action->setEnabled(has_selection);
	m_delete_action->setEnabled(has_selection);
	m_add_child_action->setEnabled(has_selection);
	m_remove_child_action->setEnabled(has_selection);
	
	m_context_menu->exec(m_assembly_tree->mapToGlobal(pos));
}

/**
	@brief AssemblyDockWidget::onFilterTextChanged
	Handle filter text change
	@param text : New filter text
*/
void AssemblyDockWidget::onFilterTextChanged(const QString &text)
{
	m_current_filter_text = text;
	applyFilters();
}

/**
	@brief AssemblyDockWidget::onFilterTypeChanged
	Handle filter type change
	@param type : New filter type
*/
void AssemblyDockWidget::onFilterTypeChanged(const QString &type)
{
	m_current_filter_type = type;
	applyFilters();
}

/**
	@brief AssemblyDockWidget::createAssembly
	Create a new assembly
*/
void AssemblyDockWidget::createAssembly()
{
	if (!m_assembly_manager) {
		return;
	}
	
	// Simple input dialog for now - could be replaced with a proper dialog
	QString name = QInputDialog::getText(this, tr("Create Assembly", "dialog title"), tr("Assembly Name:", "dialog label"));
	if (name.isEmpty()) {
		return;
	}
	
	QString type = QInputDialog::getItem(this, tr("Create Assembly", "dialog title"), tr("Assembly Type:", "dialog label"), 
										QStringList() << "Assembly" << "SubAssembly" << "Component", 0);
	if (type.isEmpty()) {
		return;
	}
	
	QString description = QInputDialog::getText(this, tr("Create Assembly", "dialog title"), tr("Description (optional):", "dialog label"));
	
	QString assembly_id = m_assembly_manager->createAssembly(name, type, description);
	if (!assembly_id.isEmpty()) {
		selectAssembly(assembly_id);
	}
}

/**
	@brief AssemblyDockWidget::editSelectedAssembly
	Edit the selected assembly
*/
void AssemblyDockWidget::editSelectedAssembly()
{
	QString assembly_id = getSelectedAssemblyId();
	if (assembly_id.isEmpty()) {
		return;
	}
	
	emit requestEditAssembly(assembly_id);
}

/**
	@brief AssemblyDockWidget::deleteSelectedAssembly
	Delete the selected assembly
*/
void AssemblyDockWidget::deleteSelectedAssembly()
{
	QString assembly_id = getSelectedAssemblyId();
	if (assembly_id.isEmpty()) {
		return;
	}
	
	AssemblyInfo info = m_assembly_manager->getAssemblyInfo(assembly_id);
	
	int ret = QMessageBox::question(this, tr("Delete Assembly", "dialog title"),
									tr("Are you sure you want to delete the assembly '%1'?", "confirmation message").arg(info.assembly_name),
									QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	
	if (ret == QMessageBox::Yes) {
		m_assembly_manager->removeAssembly(assembly_id);
	}
}

/**
	@brief AssemblyDockWidget::addChildToSelectedAssembly
	Add a child element to the selected assembly
*/
void AssemblyDockWidget::addChildToSelectedAssembly()
{
	QString assembly_id = getSelectedAssemblyId();
	if (assembly_id.isEmpty()) {
		return;
	}
	
	emit requestAddChildToAssembly(assembly_id);
}

/**
	@brief AssemblyDockWidget::removeChildFromSelectedAssembly
	Remove a child element from the selected assembly
*/
void AssemblyDockWidget::removeChildFromSelectedAssembly()
{
	QString assembly_id = getSelectedAssemblyId();
	if (assembly_id.isEmpty()) {
		return;
	}
	
	emit requestRemoveChildFromAssembly(assembly_id, QString()); // Empty UUID means show selection dialog
}

/**
	@brief AssemblyDockWidget::refreshTree
	Refresh the assembly tree
*/
void AssemblyDockWidget::refreshTree()
{
	refreshAssemblyTree();
}

/**
	@brief AssemblyDockWidget::createAssemblyItem
	Create a tree widget item for an assembly
	@param assembly_id : ID of the assembly
	@return : Created tree widget item
*/
QTreeWidgetItem *AssemblyDockWidget::createAssemblyItem(const QString &assembly_id)
{
	if (!m_assembly_manager) {
		return nullptr;
	}
	
	AssemblyInfo info = m_assembly_manager->getAssemblyInfo(assembly_id);
	if (info.assembly_id.isEmpty()) {
		return nullptr;
	}
	
	QTreeWidgetItem *item = new QTreeWidgetItem();
	item->setData(0, Qt::UserRole, assembly_id);
	item->setText(0, info.assembly_name);
	item->setText(1, info.assembly_type);
	item->setText(2, QString::number(info.getChildElementCount()));
	
	// Add child elements
	QStringList children = m_assembly_manager->getAssemblyChildren(assembly_id);
	for (const QString &child_uuid : children) {
		addChildToAssemblyItem(item, child_uuid);
	}
	
	return item;
}

/**
	@brief AssemblyDockWidget::findAssemblyItem
	Find a tree widget item by assembly ID
	@param assembly_id : ID of the assembly to find
	@return : Found tree widget item, or nullptr if not found
*/
QTreeWidgetItem *AssemblyDockWidget::findAssemblyItem(const QString &assembly_id)
{
	for (int i = 0; i < m_assembly_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *item = m_assembly_tree->topLevelItem(i);
		if (item->data(0, Qt::UserRole).toString() == assembly_id) {
			return item;
		}
	}
	return nullptr;
}

/**
	@brief AssemblyDockWidget::updateAssemblyItem
	Update a tree widget item with current assembly data
	@param item : Tree widget item to update
	@param assembly_id : ID of the assembly
*/
void AssemblyDockWidget::updateAssemblyItem(QTreeWidgetItem *item, const QString &assembly_id)
{
	if (!m_assembly_manager || !item) {
		return;
	}
	
	AssemblyInfo info = m_assembly_manager->getAssemblyInfo(assembly_id);
	item->setText(0, info.assembly_name);
	item->setText(1, info.assembly_type);
	item->setText(2, QString::number(info.getChildElementCount()));
}

/**
	@brief AssemblyDockWidget::removeAssemblyItem
	Remove a tree widget item for an assembly
	@param assembly_id : ID of the assembly to remove
*/
void AssemblyDockWidget::removeAssemblyItem(const QString &assembly_id)
{
	QTreeWidgetItem *item = findAssemblyItem(assembly_id);
	if (item) {
		delete item;
	}
}

/**
	@brief AssemblyDockWidget::addChildToAssemblyItem
	Add a child element to an assembly tree item
	@param parent_item : Parent assembly tree item
	@param child_uuid : UUID of the child element
*/
void AssemblyDockWidget::addChildToAssemblyItem(QTreeWidgetItem *parent_item, const QString &child_uuid)
{
	if (!parent_item) {
		return;
	}
	
	QTreeWidgetItem *child_item = new QTreeWidgetItem(parent_item);
	child_item->setText(0, child_uuid);
	child_item->setText(1, tr("Element", "child type"));
	child_item->setData(0, Qt::UserRole, child_uuid);
	
	// Update parent's child count
	QString assembly_id = parent_item->data(0, Qt::UserRole).toString();
	if (m_assembly_manager) {
		AssemblyInfo info = m_assembly_manager->getAssemblyInfo(assembly_id);
		parent_item->setText(2, QString::number(info.getChildElementCount()));
	}
}

/**
	@brief AssemblyDockWidget::removeChildFromAssemblyItem
	Remove a child element from an assembly tree item
	@param parent_item : Parent assembly tree item
	@param child_uuid : UUID of the child element to remove
*/
void AssemblyDockWidget::removeChildFromAssemblyItem(QTreeWidgetItem *parent_item, const QString &child_uuid)
{
	if (!parent_item) {
		return;
	}
	
	for (int i = 0; i < parent_item->childCount(); ++i) {
		QTreeWidgetItem *child_item = parent_item->child(i);
		if (child_item->data(0, Qt::UserRole).toString() == child_uuid) {
			delete child_item;
			break;
		}
	}
	
	// Update parent's child count
	QString assembly_id = parent_item->data(0, Qt::UserRole).toString();
	if (m_assembly_manager) {
		AssemblyInfo info = m_assembly_manager->getAssemblyInfo(assembly_id);
		parent_item->setText(2, QString::number(info.getChildElementCount()));
	}
}

/**
	@brief AssemblyDockWidget::getSelectedAssemblyId
	Get the ID of the currently selected assembly
	@return : Assembly ID, or empty string if none selected
*/
QString AssemblyDockWidget::getSelectedAssemblyId() const
{
	QTreeWidgetItem *item = getSelectedAssemblyItem();
	if (item) {
		return item->data(0, Qt::UserRole).toString();
	}
	return QString();
}

/**
	@brief AssemblyDockWidget::getSelectedAssemblyItem
	Get the currently selected assembly tree item
	@return : Selected tree item, or nullptr if none selected
*/
QTreeWidgetItem *AssemblyDockWidget::getSelectedAssemblyItem() const
{
	return m_assembly_tree->currentItem();
}

/**
	@brief AssemblyDockWidget::populateTree
	Populate the assembly tree with all assemblies
*/
void AssemblyDockWidget::populateTree()
{
	if (!m_assembly_manager) {
		return;
	}
	
	QStringList assembly_ids = m_assembly_manager->getAllAssemblyIds();
	for (const QString &assembly_id : assembly_ids) {
		QTreeWidgetItem *item = createAssemblyItem(assembly_id);
		if (item) {
			m_assembly_tree->addTopLevelItem(item);
		}
	}
}

/**
	@brief AssemblyDockWidget::applyFilters
	Apply current filters to the tree
*/
void AssemblyDockWidget::applyFilters()
{
	if (m_updating_tree) {
		return;
	}
	
	for (int i = 0; i < m_assembly_tree->topLevelItemCount(); ++i) {
		QTreeWidgetItem *item = m_assembly_tree->topLevelItem(i);
		bool visible = true;
		
		// Apply name filter
		if (!m_current_filter_text.isEmpty()) {
			visible = item->text(0).contains(m_current_filter_text, Qt::CaseInsensitive);
		}
		
		// Apply type filter
		if (visible && m_current_filter_type != "All") {
			visible = (item->text(1) == m_current_filter_type);
		}
		
		item->setHidden(!visible);
	}
}
