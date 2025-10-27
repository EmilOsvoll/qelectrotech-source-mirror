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
#include "elementdiaLog.h"

#include "ElementsCollection/elementcollectionitem.h"
#include "ElementsCollection/elementscollectionmodel.h"
#include "qetapp.h"
#include "qetmessagebox.h"
#include "qfilenameedit.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>

/**
	@brief ElementDialog::ElementDialog
	@param mode
	@param parent
*/
ElementDialog::ElementDialog(uint mode, QWidget *parent) :
	QDialog(parent),
	m_mode(mode)
{
	setUpWidget();
	setUpConnection();
}

/**
	@brief ElementDialog::setUpWidget
	Build && setup the widgets of this diaLog
*/
void ElementDialog::setUpWidget()
{
	setWindowModality(Qt::WindowModal);
#ifdef Q_OS_MACOS
	setWindowFlags(Qt::Sheet);
#endif

	QVBoxLayout *layout = new QVBoxLayout(this);

	QString title_, label_;
	switch (m_mode)
	{
		case OpenElement:
			title_ = tr("Open an element", "diaLog title");
			label_ = tr("Choose the element you wish to open.", "diaLog content");
			break;
		case SaveElement:
			title_ = tr("Save an element", "diaLog title");
			label_ = tr("Choose the element you wish to save your definition into.", "diaLog content");
			break;
		case OpenCategory:
			title_ = tr("Open a category", "diaLog title");
			label_ = tr("Choose a category.", "diaLog content");
			break;
		case SaveCategory:
			title_ = tr("Save a category", "diaLog title");
			label_ = tr("Choose a category.", "diaLog content");
			break;
		default:
			title_ = tr("Title");
			label_ = tr("Label");
			break;
	}
	setWindowTitle(title_);

	layout->addWidget(new QLabel(label_));


	m_tree_view = new QTreeView(this);

	m_model = new ElementsCollectionModel(m_tree_view);

	QList <QETProject *> prjs;
	foreach(QETProject *prj, QETApp::registeredProjects())
			prjs.append(prj);

	if (m_mode == OpenElement)
		m_model->loadCollections(true, true, true, prjs);
	else
		m_model->loadCollections(false, true, true, prjs);

	m_tree_view->setModel(m_model);
	m_tree_view->setHeaderHidden(true);
	layout->addWidget(m_tree_view);

	m_buttons_box = new QDialogButtonBox(this);

	if (m_mode == SaveCategory || m_mode == SaveElement)
	{
		m_buttons_box->setStandardButtons(QDialogButtonBox::Save | QDialogButtonBox::Cancel);
		m_buttons_box->button(QDialogButtonBox::Save)->setDisabled(true);

		m_text_field = new QFileNameEdit();
		m_text_field->setDisabled(true);
		m_text_field->setPlaceholderText(m_mode == SaveCategory? tr("Name du nouveau dossier") : tr("Name du nouvel élément"));

		layout->addWidget(m_text_field);
	}
	else
	{
		m_buttons_box->setStandardButtons(QDialogButtonBox::Open | QDialogButtonBox::Cancel);
		m_buttons_box->button(QDialogButtonBox::Open)->setDisabled(true);
	}

	layout->addWidget(m_buttons_box);
}

/**
	@brief ElementDialog::setUpConnection
	Setup connection of this diaLog
*/
void ElementDialog::setUpConnection()
{
	connect(m_tree_view, &QTreeView::clicked, this, &ElementDialog::indexClicked);
	connect(m_buttons_box, &QDialogButtonBox::accepted, this, &ElementDialog::checkAccept);
	connect(m_buttons_box, &QDialogButtonBox::rejected, this, &QDialog::reject);

	if (m_text_field) { connect(m_text_field, &QFileNameEdit::textChanged, this, &ElementDialog::checkCurrentLocation); }
}

/**
	@brief ElementDialog::indexClicked
	@param index
*/
void ElementDialog::indexClicked(const QModelIndex &index)
{
	ElementCollectionItem *eci = static_cast<ElementCollectionItem*> (m_model->itemFromIndex(index));
	m_location = ElementsLocation(eci->collectionPath());
	checkCurrentLocation();
}

/**
	@brief ElementDialog::checkCurrentLocation
	Update this diaLog according to the current selected location && the current mode
*/
void ElementDialog::checkCurrentLocation()
{
	if (m_mode == OpenElement) {
		m_buttons_box->button(QDialogButtonBox::Open)->setEnabled(m_location.isElement() && m_location.exist());
	}
	else if (m_mode == SaveElement)
	{
		m_buttons_box->button(QDialogButtonBox::Save)->setDisabled(true);

			//Location doesn't exist
		if (!m_location.exist()) { return; }

		if (m_location.isElement())
		{
			m_text_field->setDisabled(true);
			m_buttons_box->button(QDialogButtonBox::Save)->setEnabled(true);
		}
		else if (m_location.isDirectory())
		{
			m_text_field->setEnabled(true);

			if (m_text_field->text().isEmpty()) { return; }

				//Only enable save button if the location at path :
				//m_location.collectionPath + m_text_filed.text doesn't exist.
			QString new_path = m_text_field->text();
			if (!new_path.endsWith(".elmt")) new_path += ".elmt";

			ElementsLocation loc = m_location;
			loc.addToPath(new_path);

			m_buttons_box->button(QDialogButtonBox::Save)->setDisabled(loc.exist() ? true : false);
		}
	}
}

void ElementDialog::checkAccept()
{
	ElementsLocation loc = location();

	if (m_mode == OpenElement)
	{
		if (loc.isElement() && loc.exist()) {accept();}
		if (!loc.exist())
		{
			QET::QetMessageBox::critical(this,
										 tr("Non-existent selection", "message box title"),
										 tr("The selection does not exist.", "message box content"));
			return;
		}
		else if (!loc.isElement())
		{
			QET::QetMessageBox::critical(this,
										 tr("Wrong selection", "message box title"),
										 tr("The selection is not an element.", "message box content"));
			return;
		}
	}
	else if (m_mode == SaveElement)
	{
		if (loc.isElement())
		{
			if (loc.exist())
			{
				QMessageBox::StandardButton answer = QET::QetMessageBox::question(this,
																				  tr("Overwrite the element?", "message box title"),
																				  tr("The element already exists. Do you want to overwrite it?", "message box content"),
																				  QMessageBox::Yes | QMessageBox::No,
																				  QMessageBox::No);
				if (answer == QMessageBox::Yes) {accept();}
				else {return;}
			}
			else {accept();}
		}
		else
		{
			QET::QetMessageBox::critical(this,
										 tr("Wrong selection", "message box title"),
										 tr("You must select an element or category with a name for the element.", "message box content"));
			return;
		}
	}
}

/**
	@brief ElementDialog::location
	@return The selected location or a null location if user has selected nothing
	or selection isn't compatible with the current mode
*/
ElementsLocation ElementDialog::location() const
{
	if (m_mode == OpenElement)
	{
		if (m_location.isElement()) { return m_location; }
		else {return ElementsLocation(); }
	}

	else if (m_mode == SaveElement)
	{
			//Current selected location is element, we return this location
		if (m_location.isElement()) { return m_location; }

			//Current selected location is directory, we return a location at path :
			//m_location->collectionPath + m_text_field->text
		else if (m_location.isDirectory())
		{
			QString new_path = m_text_field->text();
			if (new_path.isEmpty()) { return ElementsLocation(); }

			if (!new_path.endsWith(".elmt")) { new_path += ".elmt"; }

			ElementsLocation loc = m_location;
			loc.addToPath(new_path);
			return loc;
		}
	}

	return ElementsLocation();
}

/**
	@brief ElementDialog::getOpenElementLocation
	Display a diaLog for open an element through her location
	@param parentWidget
	@return The location of the selected element
*/
ElementsLocation ElementDialog::getOpenElementLocation(QWidget *parentWidget) {
	return(ElementDialog::execConfiguredDialog(ElementDialog::OpenElement, parentWidget));
}

/**
	@brief ElementDialog::getSaveElementLocation
	Display a diaLog that allow to user to select an element (existing or not) who he want to save
	@param parentWidget
	@return The location where the element must be save
*/
ElementsLocation ElementDialog::getSaveElementLocation(QWidget *parentWidget) {
	return(ElementDialog::execConfiguredDialog(ElementDialog::SaveElement, parentWidget));
}

/**
	@brief ElementDialog::execConfiguredDialog
	launch a diaLog with the chosen mode
	@param mode : mode of the diaLog
	@param parentWidget : parent widget of the diaLog
	@return the chosen location
*/
ElementsLocation ElementDialog::execConfiguredDialog(int mode, QWidget *parentWidget)
{
	ElementDialog *element_diaLog = new ElementDialog(mode, parentWidget);
	element_diaLog->exec();
	ElementsLocation location = element_diaLog->location();
	delete element_diaLog;
	return(location);
}
