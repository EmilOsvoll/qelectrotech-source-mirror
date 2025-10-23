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
#ifndef ASSEMBLYDOCKWIDGET_H
#define ASSEMBLYDOCKWIDGET_H

#include <QDockWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QSplitter>

class QETProject;
class AssemblyManager;
class AssemblyInfo;

/**
	@brief The AssemblyDockWidget class
	Provides a dock widget for managing assemblies in a QET project.
	This widget displays a tree view of assemblies && their hierarchical
	relationships, along with controls for creating, editing, && managing assemblies.
*/
class AssemblyDockWidget : public QDockWidget
{
	Q_OBJECT

	public:
		explicit AssemblyDockWidget(QETProject *project, QWidget *parent = nullptr);
		virtual ~AssemblyDockWidget() override;

		// Public methods
		void setProject(QETProject *project);
		void refreshAssemblyTree();
		void selectAssembly(const QString &assembly_id);
		void clearSelection();

	signals:
		void assemblySelected(const QString &assembly_id);
		void assemblyDoubleClicked(const QString &assembly_id);
		void requestCreateAssembly();
		void requestEditAssembly(const QString &assembly_id);
		void requestDeleteAssembly(const QString &assembly_id);
		void requestAddChildToAssembly(const QString &assembly_id);
		void requestRemoveChildFromAssembly(const QString &assembly_id, const QString &child_uuid);

	private slots:
		void onAssemblyCreated(const QString &assembly_id);
		void onAssemblyRemoved(const QString &assembly_id);
		void onAssemblyUpdated(const QString &assembly_id);
		void onAssemblyChildAdded(const QString &assembly_id, const QString &element_uuid);
		void onAssemblyChildRemoved(const QString &assembly_id, const QString &element_uuid);
		void onAssemblyPropertyChanged(const QString &assembly_id, const QString &key);

		void onTreeItemSelectionChanged();
		void onTreeItemDoubleClicked(QTreeWidgetItem *item, int column);
		void onContextMenuRequested(const QPoint &pos);
		void onFilterTextChanged(const QString &text);
		void onFilterTypeChanged(const QString &type);

		void createAssembly();
		void editSelectedAssembly();
		void deleteSelectedAssembly();
		void addChildToSelectedAssembly();
		void removeChildFromSelectedAssembly();
		void refreshTree();

	private:
		void setupUI();
		void setupTreeWidget();
		void setupToolbar();
		void setupContextMenu();
		void connectSignals();
		
		QTreeWidgetItem *createAssemblyItem(const QString &assembly_id);
		QTreeWidgetItem *findAssemblyItem(const QString &assembly_id);
		void updateAssemblyItem(QTreeWidgetItem *item, const QString &assembly_id);
		void removeAssemblyItem(const QString &assembly_id);
		void addChildToAssemblyItem(QTreeWidgetItem *parent_item, const QString &child_uuid);
		void removeChildFromAssemblyItem(QTreeWidgetItem *parent_item, const QString &child_uuid);
		
		QString getSelectedAssemblyId() const;
		QTreeWidgetItem *getSelectedAssemblyItem() const;
		void populateTree();
		void applyFilters();

	private:
		QETProject *m_project;
		AssemblyManager *m_assembly_manager;
		
		// UI Components
		QWidget *m_main_widget;
		QVBoxLayout *m_main_layout;
		QSplitter *m_splitter;
		
		// Toolbar
		QWidget *m_toolbar_widget;
		QHBoxLayout *m_toolbar_layout;
		QPushButton *m_create_button;
		QPushButton *m_edit_button;
		QPushButton *m_delete_button;
		QPushButton *m_add_child_button;
		QPushButton *m_remove_child_button;
		QPushButton *m_refresh_button;
		
		// Filter controls
		QGroupBox *m_filter_group;
		QHBoxLayout *m_filter_layout;
		QLabel *m_filter_label;
		QLineEdit *m_filter_edit;
		QLabel *m_type_label;
		QComboBox *m_type_combo;
		
		// Tree widget
		QTreeWidget *m_assembly_tree;
		
		// Context menu
		QMenu *m_context_menu;
		QAction *m_create_action;
		QAction *m_edit_action;
		QAction *m_delete_action;
		QAction *m_add_child_action;
		QAction *m_remove_child_action;
		QAction *m_refresh_action;
		
		// State
		bool m_updating_tree;
		QString m_current_filter_text;
		QString m_current_filter_type;
};

#endif // ASSEMBLYDOCKWIDGET_H
