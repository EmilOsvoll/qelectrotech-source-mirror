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
#include "folioautonumbering.h"

#include "../../diagram.h"
#include "../../qetproject.h"
#include "ui_folioautonumbering.h"

#include <QMessageBox>
#include <QPushButton>
#include <QtWidgets>
/**
	Constructor
*/
FolioAutonumberingW::FolioAutonumberingW(QETProject *project, QWidget *parent) :
	QWidget(parent),
	project_(project),
	ui(new Ui::FolioAutonumberingW)

{
	ui->setupUi(this);
	applyEnable(false);
	ui->m_from_cb->setEnabled(false);
	ui->m_new_tabs_sb->setEnabled(false);
	ui->m_to_cb->setEnabled(false);
}

/**
	Destructor
*/
FolioAutonumberingW::~FolioAutonumberingW()
{
	delete ui;
}

/**
	@brief FolioAutonumberingW::setContext
	construct autonums in the comboBox selected in the autonum_chooser QcomboBox
*/
void FolioAutonumberingW::setContext(QList <QString> autonums)
{
	foreach (QString str, autonums) { ui->m_autonums_cb->addItem(str);}
}

/**
	@brief FolioAutonumberingW::autoNumSelected
	returns the current autonum selected
*/
QString FolioAutonumberingW::autoNumSelected()
{
	return ui->m_autonums_cb->currentText();
}

/**
	@brief FolioAutonumberingW::fromFolio
	returns the current "From Folio" index
*/
int FolioAutonumberingW::fromFolio()
{
	return ui->m_from_cb->currentIndex()-1;
}

/**
	@brief FolioAutonumberingW::toFolio
	returns the current "To Folio" index
*/
int FolioAutonumberingW::toFolio()
{
	return ui->m_to_cb->currentIndex()+this->fromFolio()+1;
}

/**
	@brief FolioAutonumberingW::newFoliosNumber
	returns the number of folios to create
*/
int FolioAutonumberingW::newFoliosNumber()
{
	return ui->m_new_tabs_sb->value();
}
/**
	@brief FolioAutonumberingW::updateFolioList
	update Folio List in From and To ComboBox
*/
void FolioAutonumberingW::updateFolioList()
{
	ui   -> m_from_cb->clear();
	ui   -> m_to_cb->clear();
	if (newFolios){
		this -> on_m_create_new_tabs_rb_clicked();
	} else {
		this -> on_m_autonumber_tabs_rb_clicked();
	}
}

/**
	@brief FolioAutonumberingW::on_m_create_new_tabs_rb_clicked
 *	Enable New Tabs SpinBox
*/
void FolioAutonumberingW::on_m_create_new_tabs_rb_clicked()
{
	ui->m_from_cb->setEnabled(false);
	ui->m_to_cb->setEnabled(false);
	ui->m_new_tabs_sb->setEnabled(true);
	applyEnable();
	newFolios = true;
}

/**
	@brief FolioAutonumberingW::on_m_autonumber_tabs_rb_clicked
 *	Enable From ComboBox, fill From ComboBox
*/
void FolioAutonumberingW::on_m_autonumber_tabs_rb_clicked()
{
	ui->m_new_tabs_sb->setEnabled(false);
	ui->m_from_cb->setEnabled(true);
	ui->m_to_cb->setEnabled(true);
	if (ui->m_from_cb->count()<=0){
		ui->m_from_cb->clear();
		ui->m_from_cb->addItem("");
		foreach (Diagram *diagram, project_->diagrams()){
			ui->m_from_cb->addItem(diagram->title());
		}
	}
	applyEnable();
	newFolios = false;
}

/**
	@brief FolioAutonumberingW::on_m_new_tabs_sb_valueChanged
 *	Enable Apply if any new folio is to be created
*/
void FolioAutonumberingW::on_m_new_tabs_sb_valueChanged(int)
{
	if (ui->m_new_tabs_sb->value()>0) applyEnable(true);
	else applyEnable(false);
}

/**
	@brief FolioAutonumberingW::on_m_from_cb_currentIndexChanged
 *	Enable To ComboBox
*/
void FolioAutonumberingW::on_m_from_cb_currentIndexChanged(int)
{
	int index = ui->m_from_cb->currentIndex();
	ui->m_to_cb->clear();
	if (index > 0){
		ui->m_to_cb->setEnabled(true);
		for (int i=index;i<project_->diagrams().size();i++)
			ui->m_to_cb->addItem(project_
					     ->diagrams().at(i)->title());
		applyEnable(true);
	}
	else{
		applyEnable();
	}

}

/**
	@brief SelectAutonumW::on_buttonBox_clicked
	Action on buttonBox clicked
*/
void FolioAutonumberingW::on_buttonBox_clicked(QAbstractButton *button)
{
	//transform button to int
	int answer = ui -> buttonBox -> buttonRole(button);

	switch (answer) {
			//help dialog - not implemented yet -
		case QDialogButtonBox::HelpRole:
			QMessageBox::information (
						this,
						tr("Folio Autonumbering",
						   "title window"),
						tr("C'est ici que vous pouvez définir la manière dont seront numérotés les nouveaux folios.\n"
						   "-Une numérotation est composée d'une variable minimum.\n"
						   "-You can add or remove a numbering variable using the - and + buttons.\n"
						   "-A numbering variable consists of: a type, a value, and an increment.\n"

						   "\n-the \"Digit 1\", \"Digit 01\", and \"Digit 001\" types represent a numeric type defined in the \"Value\" field, "
						   "which is incremented with each new page by the value of the \"Increment\" field.\n"
						   "-\"Digit 01\" and \"Digit 001\", are respectively represented on the diagram by two and three digits minimum.\n"
						   "If the digit defined in the Value field has fewer digits than the chosen type,"
						   "celui-ci sera précédé par un ou deux 0 afin de respecter son type.\n"

						   "The \"Text\" type represents fixed text. The \"Increment\" field is not used."
						   "help dialog about the folio autonumbering"
						   ));
			break;
		case QDialogButtonBox::ApplyRole:
			applyEnable(true);
			emit applyPressed();
			updateFolioList();
			break;
	}
}

/**
	@brief SelectAutonumW::applyEnable
	enable/disable the apply button
*/
void FolioAutonumberingW::applyEnable(bool b)
{
	if (b){
		bool valid = true;
		if (ui->m_create_new_tabs_rb->isChecked()){
			if (ui->m_new_tabs_sb->value()==0) valid = false;
			ui->buttonBox->button(QDialogButtonBox::Apply)
					->setEnabled(valid);
		}
		else{
			if (ui->m_to_cb->currentText()=="") valid = false;
			ui->buttonBox->button(QDialogButtonBox::Apply)
					->setEnabled(valid);
		}
	}
	else{
		ui->buttonBox->button(QDialogButtonBox::Apply)
				->setEnabled(b);
		}
}
