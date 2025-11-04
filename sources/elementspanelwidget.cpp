/*
	Copyright 2006-2025 The QElectroTech Team
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
#include "elementspanelwidget.h"

#include "diagram.h"
#include "editor/ui/qetelementeditor.h"
#include "elementscategoryeditor.h"
#include "qetapp.h"
#include "qeticons.h"
#include "qetproject.h"
#include "titleblock/templatedeleter.h"
#include "undocommand/changetitleblockcommand.h"
#include "titleblockproperties.h"
#include <QLineEdit>
#include <QTimer>

/*
	When the ENABLE_PANEL_WIDGET_DND_CHECKS flag is set, the panel
	performs checks during drag'n drop of items and categories.
	For example, it checks that a target category is writable
	before authorizing the drop of an element.
	Removing this flag allows you to test the behavior of management functions
	items (copy, move, etc.).

	Lorsque le flag ENABLE_PANEL_WIDGET_DND_CHECKS est defini, le panel
	effectue des verifications lors des drag'n drop d'elements et categories.
	Par exemple, il verifie qu'une categorie cible est accessible en ecriture
	avant d'y autoriser le drop d'un element.
	Supprimer ce flag permet de tester le comportement des fonctions de gestion
	des items (copy, move, etc.).
*/
#define ENABLE_PANEL_WIDGET_DND_CHECKS

/**
	Constructeur
	@param parent Le QWidget parent de ce widget
*/
ElementsPanelWidget::ElementsPanelWidget(QWidget *parent) : QWidget(parent) {
	// initalise le panel d'elements
	elements_panel = new ElementsPanel(this);

	// initialise les actions
	open_directory           = new QAction(QET::Icons::FolderOpen,             tr("Open the underlying directory"),     this);
	copy_path                = new QAction(QET::Icons::IC_CopyFile,            tr("Copy full path to clipboard"),                    this);
	prj_activate             = new QAction(QET::Icons::ProjectFile,            tr("Activate this project"),             this);
	prj_close                = new QAction(QET::Icons::DocumentClose,          tr("Close this project"),                    this);
	prj_edit_prop            = new QAction(QET::Icons::DialogInformation,      tr("Project properties"),          this);
	prj_prop_diagram         = new QAction(QET::Icons::DialogInformation,      tr("Folio properties"),       this);
	prj_add_diagram          = new QAction(QET::Icons::DiagramAdd,             tr("Add a folio"),                this);
	prj_add_title_page       = new QAction(QET::Icons::DiagramAdd,             tr("Add a title page folio"),       this);
	prj_del_diagram          = new QAction(QET::Icons::DiagramDelete,          tr("Delete this folio"),              this);
	prj_rename_diagram       = new QAction(QET::Icons::EditRename,             tr("Rename"),                      this);
	prj_rename_project       = new QAction(QET::Icons::EditRename,             tr("Rename"),                      this);
	prj_move_diagram_up      = new QAction(QET::Icons::GoUp,                   tr("Move this folio up"),               this);
	prj_move_diagram_down    = new QAction(QET::Icons::GoDown,                 tr("Move this folio down"),               this);
	prj_move_diagram_upx10   = new QAction(QET::Icons::GoUpDouble,             tr("Move this folio up x10"),           this);
	prj_move_diagram_upx100  = new QAction(QET::Icons::GoUpDouble,             tr("Move this folio up x100"),           this);
	prj_move_diagram_top     = new QAction(QET::Icons::GoTop,                  tr("Move this folio to the start"),               this);
	prj_move_diagram_downx10 = new QAction(QET::Icons::GoDownDouble,           tr("Move this folio down x10"),           this);
	prj_move_diagram_downx100 = new QAction(QET::Icons::GoDownDouble,           tr("Move this folio down x100"),           this);
	tbt_add               = new QAction(QET::Icons::TitleBlock,                tr("New template"),                   this);
	tbt_edit              = new QAction(QET::Icons::TitleBlock,                tr("Edit this template"),              this);
	tbt_remove            = new QAction(QET::Icons::TitleBlock,                tr("Delete this template"),              this);


	prj_del_diagram -> setShortcut(QKeySequence(Qt::Key_Delete));
	prj_rename_diagram -> setShortcut(QKeySequence(Qt::Key_F2));
	prj_rename_project -> setShortcut(QKeySequence(Qt::Key_F2));
	prj_move_diagram_up -> setShortcut(QKeySequence(Qt::Key_F3));
	prj_move_diagram_down -> setShortcut(QKeySequence(Qt::Key_F4));
	prj_move_diagram_top -> setShortcut(QKeySequence(Qt::Key_F5));
	prj_move_diagram_downx10 -> setShortcut(QKeySequence(Qt::Key_F6));
	prj_move_diagram_downx100 -> setShortcut(QKeySequence(Qt::Key_F7));
	prj_move_diagram_upx10 -> setShortcut(QKeySequence(Qt::Key_F8));
	prj_move_diagram_upx100 -> setShortcut(QKeySequence(Qt::Key_F9));



	// initialise le champ de texte pour filtrer avec une disposition horizontale
	filter_textfield = new QLineEdit(this);
	filter_textfield -> setClearButtonEnabled(true);
	filter_textfield -> setPlaceholderText(tr("Filter"));


	context_menu = new QMenu(this);

	connect(open_directory,        SIGNAL(triggered()), this,           SLOT(openDirectoryForSelectedItem()));
	connect(copy_path,             SIGNAL(triggered()), this,           SLOT(copyPathForSelectedItem()));
	connect(prj_activate,          SIGNAL(triggered()), this,           SLOT(activateProject()));
	connect(prj_close,             SIGNAL(triggered()), this,           SLOT(closeProject()));
	connect(prj_edit_prop,         SIGNAL(triggered()), this,           SLOT(editProjectProperties()));
	connect(prj_prop_diagram,      SIGNAL(triggered()), this,           SLOT(editDiagramProperties()));
	connect(prj_add_diagram,       SIGNAL(triggered()), this,           SLOT(newDiagram()));
	connect(prj_add_title_page,   SIGNAL(triggered()), this,           SLOT(newTitlePageFolio()));
	connect(prj_del_diagram,       SIGNAL(triggered()), this,           SLOT(deleteDiagram()));
	connect(prj_rename_diagram,    SIGNAL(triggered()), this,           SLOT(renameDiagram()));
	connect(prj_rename_project,   SIGNAL(triggered()), this,           SLOT(renameProject()));
	connect(prj_move_diagram_up,   SIGNAL(triggered()), this,           SLOT(moveDiagramUp()));
	connect(prj_move_diagram_down, SIGNAL(triggered()), this,           SLOT(moveDiagramDown()));
	connect(prj_move_diagram_top,  SIGNAL(triggered()), this,           SLOT(moveDiagramUpTop()));
	connect(prj_move_diagram_upx10,   SIGNAL(triggered()), this,        SLOT(moveDiagramUpx10()));
	connect(prj_move_diagram_upx100,  SIGNAL(triggered()), this,        SLOT(moveDiagramUpx100()));
	connect(prj_move_diagram_downx10, SIGNAL(triggered()), this,        SLOT(moveDiagramDownx10()));
	connect(prj_move_diagram_downx100,SIGNAL(triggered()), this,        SLOT(moveDiagramDownx100()));
	connect(tbt_add,               SIGNAL(triggered()), this,           SLOT(addTitleBlockTemplate()));
	connect(tbt_edit,              SIGNAL(triggered()), this,           SLOT(editTitleBlockTemplate()));
	connect(tbt_remove,            SIGNAL(triggered()), this,           SLOT(removeTitleBlockTemplate()));

	connect(filter_textfield,      SIGNAL(textChanged(const QString &)), this,             SLOT(filterEdited(const QString &)));

	connect(elements_panel,        SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this, SLOT(updateButtons()));
	connect(elements_panel,        SIGNAL(customContextMenuRequested(const QPoint &)),               this, SLOT(handleContextMenu(const QPoint &)));
	connect(elements_panel,        SIGNAL(itemChanged(QTreeWidgetItem *, int)),                     this, SLOT(itemChanged(QTreeWidgetItem *, int)));
	connect(
		elements_panel,
		SIGNAL(requestForTitleBlockTemplate(const TitleBlockTemplateLocation &)),
		QETApp::instance(),
		SLOT(openTitleBlockTemplate(const TitleBlockTemplateLocation &))
	);
	
	// Set custom delegate for diagram items to allow editing only the title part
	elements_panel->setItemDelegate(new DiagramTitleDelegate(this));

	// disposition verticale
	QVBoxLayout *vlayout = new QVBoxLayout(this);
	vlayout -> setContentsMargins(0,0,0,0);
	vlayout -> setSpacing(0);
	vlayout -> addWidget(filter_textfield);
	vlayout -> addWidget(elements_panel);
	vlayout -> setStretchFactor(elements_panel, 75000);
	setLayout(vlayout);
	
	m_updating_item_ = false;
}

/**
	Destructeur
*/
ElementsPanelWidget::~ElementsPanelWidget()
{
}

/**
	Require the desktop environment to open the directory containing the file
	represented by the selected item, if any.
*/
void ElementsPanelWidget::openDirectoryForSelectedItem()
{
	if (QTreeWidgetItem *qtwi = elements_panel -> currentItem()) {
		QString dir_path = elements_panel -> dirPathForItem(qtwi);
		if (!dir_path.isEmpty()) {
			QDesktopServices::openUrl(QUrl::fromLocalFile(dir_path));
		}
	}
}

/**
	Copy the full path to the file represented by the selected item to the
	clipboard.
*/
void ElementsPanelWidget::copyPathForSelectedItem()
{
	if (QTreeWidgetItem *qtwi = elements_panel -> currentItem()) {
		QString file_path = elements_panel -> filePathForItem(qtwi);
		file_path = QDir::toNativeSeparators(file_path);
		if (!file_path.isEmpty()) {
			QApplication::clipboard() -> setText(file_path);
		}
	}
}

/**
	Recharge le panel d'elements
*/
void ElementsPanelWidget::reloadAndFilter()
{
	// recharge tous les elements
	elements_panel -> reload();
	// reapplique le filtre
	if (!filter_textfield -> text().isEmpty()) {
		elements_panel -> filter(filter_textfield -> text());
	}
}

/**
	* Emit the requestForProject signal with the selected project
*/
void ElementsPanelWidget::activateProject()
{
	if (QETProject *selected_project = elements_panel -> selectedProject()) {
		emit(requestForProject(selected_project));
	}
}

/**
	Emet le signal requestForProjectClosing avec le projet selectionne
*/
void ElementsPanelWidget::closeProject()
{
	if (QETProject *selected_project = elements_panel -> selectedProject()) {
		emit(requestForProjectClosing(selected_project));
	}
}

/**
	Emet le signal requestForProjectPropertiesEdition avec le projet selectionne
*/
void ElementsPanelWidget::editProjectProperties()
{
	if (QETProject *selected_project = elements_panel -> selectedProject()) {
		emit(requestForProjectPropertiesEdition(selected_project));
	}
}

/**
	Emet le signal requestForDiagramPropertiesEdition avec le schema selectionne
*/
void ElementsPanelWidget::editDiagramProperties()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramPropertiesEdition(selected_diagram));
	}
}

/**
	Emet le signal requestForNewDiagram avec le projet selectionne
*/
void ElementsPanelWidget::newDiagram()
{
	if (QETProject *selected_project = elements_panel -> selectedProject()) {
		emit(requestForNewDiagram(selected_project));
	}
}

/**
	Emet le signal requestForTitlePageFolio avec le projet selectionne
*/
void ElementsPanelWidget::newTitlePageFolio()
{
	if (QETProject *selected_project = elements_panel -> selectedProject()) {
		emit(requestForTitlePageFolio(selected_project));
	}
}

/**
	Emet le signal requestForDiagramDeletion avec le schema selectionne
*/
void ElementsPanelWidget::deleteDiagram()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramDeletion(selected_diagram));
	}
}

/**
	Start inline editing of the selected diagram title
*/
void ElementsPanelWidget::renameDiagram()
{
	if (QTreeWidgetItem *item = elements_panel -> currentItem()) {
		if (item -> type() == QET::Diagram) {
			// Make item editable if not already
			Qt::ItemFlags flags = item -> flags();
			if (!(flags & Qt::ItemIsEditable)) {
				item -> setFlags(flags | Qt::ItemIsEditable);
			}
			// Start editing
			elements_panel -> editItem(item, 0);
		}
	}
}

/**
	Start inline editing of the selected project title
*/
void ElementsPanelWidget::renameProject()
{
	if (QTreeWidgetItem *item = elements_panel -> currentItem()) {
		if (item -> type() == QET::Project) {
			// Make item editable if not already
			Qt::ItemFlags flags = item -> flags();
			if (!(flags & Qt::ItemIsEditable)) {
				item -> setFlags(flags | Qt::ItemIsEditable);
			}
			// Start editing
			elements_panel -> editItem(item, 0);
		}
	}
}

/**
	Emet le signal requestForDiagramMoveUpTop avec le schema selectionne
+*/
void ElementsPanelWidget::moveDiagramUpTop()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramMoveUpTop(selected_diagram));
	}
}



/**
	Emet le signal requestForDiagramMoveUp avec le schema selectionne
*/
void ElementsPanelWidget::moveDiagramUp()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramMoveUp(selected_diagram));
	}
}

/**
	Emet le signal requestForDiagramMoveDown avec le schema selectionne
*/
void ElementsPanelWidget::moveDiagramDown()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramMoveDown(selected_diagram));
	}
}

/**
	Emet le signal requestForDiagramMoveUpx10 avec le schema selectionne
*/
void ElementsPanelWidget::moveDiagramUpx10()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramMoveUpx10(selected_diagram));
	}
}

/**
	Emet le signal requestForDiagramMoveUpx100 avec le schema selectionne
*/
void ElementsPanelWidget::moveDiagramUpx100()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramMoveUpx100(selected_diagram));
	}
}

/**
	Emet le signal requestForDiagramMoveDownx10 avec le schema selectionne
*/
void ElementsPanelWidget::moveDiagramDownx10()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramMoveDownx10(selected_diagram));
	}
}

/**
	Emet le signal requestForDiagramMoveDownx100 avec le schema selectionne
*/
void ElementsPanelWidget::moveDiagramDownx100()
{
	if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
		emit(requestForDiagramMoveDownx100(selected_diagram));
	}
}

/**
	Opens a template editor to create a new title block template.
*/
void ElementsPanelWidget::addTitleBlockTemplate()
{
	QTreeWidgetItem *current_item = elements_panel -> currentItem();
	if (!current_item) return;

	if (current_item -> type() == QET::TitleBlockTemplatesCollection) {
		QETApp::instance() -> openTitleBlockTemplate(
			elements_panel -> templateLocationForItem(current_item)
		);
	}
}

/**
	Opens an editor to edit the currently selected title block template, if any.
*/
void ElementsPanelWidget::editTitleBlockTemplate()
{
	QTreeWidgetItem *current_item = elements_panel -> currentItem();
	if (current_item && current_item -> type() == QET::TitleBlockTemplate) {
		QETApp::instance() -> openTitleBlockTemplate(
			elements_panel -> templateLocationForItem(current_item)
		);
	}
}

/**
	Delete the currently selected title block template, if any.
*/
void ElementsPanelWidget::removeTitleBlockTemplate()
{
	QTreeWidgetItem *current_item = elements_panel -> currentItem();
	if (current_item && current_item -> type() == QET::TitleBlockTemplate) {
		TitleBlockTemplateDeleter(
			elements_panel -> templateLocationForItem(current_item),
			this
		).exec();
	}
}

/**
	Met a jour les boutons afin d'assurer la coherence de l'interface
*/
void ElementsPanelWidget::updateButtons()
{
	QTreeWidgetItem *current_item = elements_panel -> currentItem();
	int current_type = elements_panel -> currentItemType();

	if (current_type == QET::Project) {
		bool is_writable = !(elements_panel -> selectedProject() -> isReadOnly());
		prj_add_diagram -> setEnabled(is_writable);
	} else if (current_type == QET::Diagram) {
		Diagram    *selected_diagram         = elements_panel -> selectedDiagram();
		QETProject *selected_diagram_project = selected_diagram -> project();

		bool is_writable           = !(selected_diagram_project -> isReadOnly());
		int project_diagrams_count = selected_diagram_project -> diagrams().count();
		int diagram_position       = selected_diagram_project -> diagrams().indexOf(selected_diagram);

		prj_del_diagram       -> setEnabled(is_writable);
		prj_move_diagram_up   -> setEnabled(is_writable && diagram_position > 0);
		prj_move_diagram_down -> setEnabled(is_writable && diagram_position < project_diagrams_count - 1);
		prj_move_diagram_top   -> setEnabled(is_writable && diagram_position > 0);
		prj_move_diagram_upx10   -> setEnabled(is_writable && diagram_position > 10);
		prj_move_diagram_upx100   -> setEnabled(is_writable && diagram_position > 100);
		prj_move_diagram_downx10 -> setEnabled(is_writable && diagram_position < project_diagrams_count - 10);
		prj_move_diagram_downx100 -> setEnabled(is_writable && diagram_position < project_diagrams_count - 100);
	} else if (current_type == QET::TitleBlockTemplatesCollection) {
		TitleBlockTemplateLocation location = elements_panel -> templateLocationForItem(current_item);
		tbt_add    -> setEnabled(!location.isReadOnly());
		tbt_edit   -> setEnabled(false); // would not make sense
		tbt_remove -> setEnabled(false); // would not make sense
	} else if (current_type == QET::TitleBlockTemplate) {
		QTreeWidgetItem *item = elements_panel -> currentItem();
		TitleBlockTemplateLocation location = elements_panel -> templateLocationForItem(item);
		tbt_add    -> setEnabled(false); // would not make sense
		tbt_edit   -> setEnabled(true); // the tbt editor has a read-only mode
		// deleting a tbt requires its parent collection to be writable
		tbt_remove -> setEnabled(location.parentCollection() && !(location.parentCollection() -> isReadOnly()));
	}
}

/**
	Gere le menu contextuel du panel d'elements
	@param pos Position ou le menu contextuel a ete demande
*/
void ElementsPanelWidget::handleContextMenu(const QPoint &pos) {
	// recupere l'item concerne par l'evenement ainsi que son chemin
	QTreeWidgetItem *item = elements_panel -> itemAt(pos);
	if (!item) return;

	updateButtons();
	context_menu -> clear();

	QString dir_path = elements_panel -> dirPathForItem(item);
	if (!dir_path.isEmpty()) {
		context_menu -> addAction(open_directory);
		context_menu -> addAction(copy_path);
		context_menu -> addSeparator();
	}

	switch(item -> type()) {
		case QET::Project:
			context_menu -> addAction(prj_rename_project);
			context_menu -> addAction(prj_activate);
			context_menu -> addAction(prj_edit_prop);
			context_menu -> addAction(prj_add_diagram);
			context_menu -> addAction(prj_add_title_page);
			context_menu -> addAction(prj_close);
			break;
		case QET::Diagram:
			context_menu -> addAction(prj_rename_diagram);
			context_menu -> addAction(prj_prop_diagram);
			context_menu -> addAction(prj_del_diagram);
			context_menu -> addAction(prj_move_diagram_top);
			context_menu -> addAction(prj_move_diagram_upx10);
			context_menu -> addAction(prj_move_diagram_upx100);
			context_menu -> addAction(prj_move_diagram_up);
			context_menu -> addAction(prj_move_diagram_down);
			context_menu -> addAction(prj_move_diagram_downx10);
			context_menu -> addAction(prj_move_diagram_downx100);
			break;
		case QET::TitleBlockTemplatesCollection:
			context_menu -> addAction(tbt_add);
			break;
		case QET::TitleBlockTemplate:
			context_menu -> addAction(tbt_edit);
			context_menu -> addAction(tbt_remove);
			break;
	}

	// affiche le menu
	if (!context_menu -> isEmpty()) {
		context_menu -> popup(mapToGlobal(elements_panel -> mapTo(this, pos + QPoint(2, 2))));
	}
}

void ElementsPanelWidget::filterEdited(const QString &next_text) {
	if (previous_filter_.isEmpty() && next_text.length() == 1) {
		// the field is not empty anymore: begin filtering
		elements_panel -> filter(next_text, QET::BeginFilter);
	} else if (!previous_filter_.isEmpty() && next_text.isEmpty()) {
		// the field is now empty again: end of filtering
		elements_panel -> filter(QString(), QET::EndFilter);
	} else {
		// regular filtering
		elements_panel -> filter(next_text, QET::RegularFilter);
	}
	previous_filter_ = next_text;
}

/**
	Handle itemChanged signal to update diagram title or project title when user finishes editing
*/
void ElementsPanelWidget::itemChanged(QTreeWidgetItem *item, int column)
{
	if (column != 0) return;
	if (!item) return;
	
	// Prevent recursive updates
	if (m_updating_item_) return;
	
	if (item->type() == QET::Diagram) {
		Diagram *diagram = qvariant_cast<Diagram *>(item->data(0, GenericPanel::Item));
		if (!diagram || diagram->isReadOnly()) return;
		
		QString new_text = item->text(0);
		
		// Parse the text to extract the title part
		// Format is: "1/2 - Title" or "1 - Title"
		QString new_title;
		int dash_pos = new_text.indexOf(" - ");
		if (dash_pos >= 0) {
			new_title = new_text.mid(dash_pos + 3).trimmed();
		} else {
			// No dash found, assume entire text is title
			new_title = new_text.trimmed();
		}
		
		// Get current titleblock properties
		TitleBlockProperties old_props = diagram->border_and_titleblock.exportTitleBlock();
		QString old_title = old_props.title;
		if (old_title.isEmpty()) {
			old_title = tr("Untitled folio", "Fallback label when a diagram has no title");
		}
		
		// Update if title actually changed
		if (old_title != new_title) {
			m_updating_item_ = true;
			TitleBlockProperties new_props = old_props;
			new_props.title = new_title;
			diagram->undoStack().push(new ChangeTitleBlockCommand(diagram, old_props, new_props));
			m_updating_item_ = false;
		}
	} else if (item->type() == QET::Project) {
		QETProject *project = qvariant_cast<QETProject *>(item->data(0, GenericPanel::Item));
		if (!project || project->isReadOnly()) return;
		
		QString new_text = item->text(0);
		QString new_title = extractProjectTitle(new_text);
		
		// Update if title actually changed
		QString current_title = project->title();
		if (current_title != new_title) {
			m_updating_item_ = true;
			project->setTitle(new_title);
			m_updating_item_ = false;
		}
	}
}

/**
	Extract project title from full display text
*/
QString ElementsPanelWidget::extractProjectTitle(const QString &fullText) const
{
	// Format: "Project «title : path»" or "Project «title : path» [Edited]" or "Project «title : path» [Read only]"
	int start_pos = fullText.indexOf("«");
	int colon_pos = fullText.indexOf(" : ");
	
	if (start_pos >= 0 && colon_pos > start_pos) {
		return fullText.mid(start_pos + 1, colon_pos - start_pos - 1);
	}
	
	// Fallback: try to find "Project " prefix and extract what follows
	int project_pos = fullText.indexOf("Project ");
	if (project_pos >= 0) {
		QString rest = fullText.mid(project_pos + 8);
		// Remove [Edited] or [Read only] suffixes
		rest = rest.replace(" [Edited]", "");
		rest = rest.replace(" [Read only]", "");
		return rest.trimmed();
	}
	
	return fullText;
}

/**
	Treat key press event inside elements panel widget
*/
void ElementsPanelWidget::keyPressEvent   (QKeyEvent *e) {
	switch(e -> key()) {
				case Qt::Key_Delete: //delete diagram through elements panel widget
					if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
						emit(requestForDiagramDeletion(selected_diagram));
					}
					break;
				case Qt::Key_F2: //rename diagram or project
					if (elements_panel -> currentItemType() == QET::Diagram) {
						renameDiagram();
					} else if (elements_panel -> currentItemType() == QET::Project) {
						renameProject();
					}
					break;
				case Qt::Key_F3:
					if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
						emit(requestForDiagramMoveUp(selected_diagram));
					}
					break;
					case Qt::Key_F4:
					if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
						emit(requestForDiagramMoveDown(selected_diagram));
					}
					break;
					case Qt::Key_F5:
					if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
						emit(requestForDiagramMoveUpTop(selected_diagram));
					}
					
					break;
					case Qt::Key_F6:
					if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
						emit(requestForDiagramMoveDownx10(selected_diagram));
					}
					
					break;
					case Qt::Key_F7:
					if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
						emit(requestForDiagramMoveDownx100(selected_diagram));
					}
					
					
					break;
					case Qt::Key_F8:
					if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
						emit(requestForDiagramMoveUpx10(selected_diagram));
					}					
					
					break;
					case Qt::Key_F9:
					if (Diagram *selected_diagram = elements_panel -> selectedDiagram()) {
						emit(requestForDiagramMoveUpx100(selected_diagram));
					}
					break;
				}
	return;
}

/**
	@brief DiagramTitleDelegate::DiagramTitleDelegate
	Constructor
*/
DiagramTitleDelegate::DiagramTitleDelegate(QObject *parent) :
	QStyledItemDelegate(parent)
{
}

/**
	@brief DiagramTitleDelegate::createEditor
	Create a line edit editor for editing the title
*/
QWidget *DiagramTitleDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	Q_UNUSED(option);
	// Check if this is a diagram item by checking the text pattern
	// Diagram items always have format "X/Y - Title" or "X - Title"
	QString fullText = index.data(Qt::DisplayRole).toString();
	if (!fullText.contains(" - ")) {
		// Not a diagram item, use default delegate
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
	
	QLineEdit *editor = new QLineEdit(parent);
	return editor;
}

/**
	@brief DiagramTitleDelegate::setEditorData
	Extract only the title part and set it in the editor
*/
void DiagramTitleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
	if (!lineEdit) {
		QStyledItemDelegate::setEditorData(editor, index);
		return;
	}
	
	QString fullText = index.data(Qt::DisplayRole).toString();
	
	// Check if this is a project item (has « and : pattern)
	if (fullText.contains("«") && fullText.contains(" : ")) {
		QString titleOnly = extractProjectTitle(fullText);
		lineEdit->setText(titleOnly);
		lineEdit->selectAll();
		return;
	}
	
	// Check if this is a diagram item (has " - " pattern)
	if (fullText.contains(" - ")) {
		QString titleOnly = extractTitle(fullText);
		lineEdit->setText(titleOnly);
		lineEdit->selectAll();
		return;
	}
	
	// Default: use full text
	QStyledItemDelegate::setEditorData(editor, index);
}

/**
	@brief DiagramTitleDelegate::setModelData
	Reconstruct the full text with page number prefix and set it in the model
*/
void DiagramTitleDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
	if (!lineEdit) {
		QStyledItemDelegate::setModelData(editor, model, index);
		return;
	}
	
	QString originalFullText = index.data(Qt::DisplayRole).toString();
	
	// Check if this is a project item (has « and : pattern)
	if (originalFullText.contains("«") && originalFullText.contains(" : ")) {
		QString newTitle = lineEdit->text().trimmed();
		QString newFullText = reconstructProjectFullText(newTitle, originalFullText);
		model->setData(index, newFullText, Qt::DisplayRole);
		return;
	}
	
	// Check if this is a diagram item (has " - " pattern)
	if (originalFullText.contains(" - ")) {
		QString newTitle = lineEdit->text().trimmed();
		QString newFullText = reconstructFullText(newTitle, originalFullText);
		model->setData(index, newFullText, Qt::DisplayRole);
		return;
	}
	
	// Default: use full text
	QStyledItemDelegate::setModelData(editor, model, index);
}

/**
	@brief DiagramTitleDelegate::extractTitle
	Extract the title part from "1/2 - Title" format
*/
QString DiagramTitleDelegate::extractTitle(const QString &fullText) const
{
	int dash_pos = fullText.indexOf(" - ");
	if (dash_pos >= 0) {
		return fullText.mid(dash_pos + 3);
	}
	return fullText;
}

/**
	@brief DiagramTitleDelegate::reconstructFullText
	Reconstruct the full text with page number prefix
*/
QString DiagramTitleDelegate::reconstructFullText(const QString &title, const QString &originalFullText) const
{
	int dash_pos = originalFullText.indexOf(" - ");
	if (dash_pos >= 0) {
		QString prefix = originalFullText.left(dash_pos + 3);
		return prefix + title;
	}
	return title;
}

/**
	@brief DiagramTitleDelegate::extractProjectTitle
	Extract the project title from "Project «title : path» [Edited]" format
*/
QString DiagramTitleDelegate::extractProjectTitle(const QString &fullText) const
{
	// Format: "Project «title : path»" or "Project «title : path» [Edited]" or "Project «title : path» [Read only]"
	int start_pos = fullText.indexOf("«");
	int colon_pos = fullText.indexOf(" : ");
	
	if (start_pos >= 0 && colon_pos > start_pos) {
		return fullText.mid(start_pos + 1, colon_pos - start_pos - 1);
	}
	
	// Fallback: try to find "Project " prefix and extract what follows
	int project_pos = fullText.indexOf("Project ");
	if (project_pos >= 0) {
		QString rest = fullText.mid(project_pos + 8);
		// Remove [Edited] or [Read only] suffixes
		rest = rest.replace(" [Edited]", "");
		rest = rest.replace(" [Read only]", "");
		return rest.trimmed();
	}
	
	return fullText;
}

/**
	@brief DiagramTitleDelegate::reconstructProjectFullText
	Reconstruct the project full text with new title
*/
QString DiagramTitleDelegate::reconstructProjectFullText(const QString &title, const QString &originalFullText) const
{
	// Format: "Project «title : path»" or "Project «title : path» [Edited]" or "Project «title : path» [Read only]"
	int start_pos = originalFullText.indexOf("«");
	int colon_pos = originalFullText.indexOf(" : ");
	int end_pos = originalFullText.indexOf("»");
	
	if (start_pos >= 0 && colon_pos > start_pos && end_pos > colon_pos) {
		QString prefix = originalFullText.left(start_pos + 1); // "Project «"
		QString middle = originalFullText.mid(colon_pos, end_pos - colon_pos + 1); // " : path»"
		QString suffix = originalFullText.mid(end_pos + 1); // " [Edited]" or " [Read only]" or ""
		
		return prefix + title + middle + suffix;
	}
	
	// Fallback: try to preserve [Edited] or [Read only] suffixes
	QString suffix;
	if (originalFullText.contains(" [Edited]")) {
		suffix = " [Edited]";
	} else if (originalFullText.contains(" [Read only]")) {
		suffix = " [Read only]";
	}
	
	return "Project " + title + suffix;
}
