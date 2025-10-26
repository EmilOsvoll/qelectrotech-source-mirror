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
#include "selectautonumw.h"

#include "../assignvariables.h"
#include "../numerotationcontextcommands.h"
#include "formulaautonumberingw.h"
#include "numparteditorw.h"
#include "ui_formulaautonumberingw.h"
#include "ui_selectautonumw.h"

#include <QMessageBox>

/**
	@brief SelectAutonumW::SelectAutonumW
	Constructor
	@param type : int m_edited_type
	@param parent : QWidget
*/
SelectAutonumW::SelectAutonumW(int type, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SelectAutonumW),
	m_edited_type(type)
{
	ui->setupUi(this);
	ui->m_comboBox->lineEdit()->setClearButtonEnabled(true);
	if (m_edited_type == 0)
	{
		m_feaw = new FormulaAutonumberingW();
		m_feaw->ui->label->setHidden(true);
		ui->m_widget->layout()->addWidget(m_feaw);
	}
	else if (m_edited_type == 1)
	{
		m_fcaw = new FormulaAutonumberingW();
		m_fcaw->ui->label->setHidden(true);
		ui->m_widget->layout()->addWidget(m_fcaw);
	}
	setContext(NumerotationContext());
}

/**
	@brief SelectAutonumW::SelectAutonumW
	Constructor
	@param context : NumerotationContext
	@param type : int m_edited_type
	@param parent : QWidget
*/
SelectAutonumW::SelectAutonumW(const NumerotationContext &context,
			       int type,
			       QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SelectAutonumW),
	m_edited_type(type)
{
	if (m_edited_type == 0)
	{
		m_feaw = new FormulaAutonumberingW();
		m_feaw->ui->label->setHidden(true);
		ui->m_widget->layout()->addWidget(m_feaw);
	}
	else if (m_edited_type == 1)
	{
		m_fcaw = new FormulaAutonumberingW();
		m_fcaw->ui->label->setHidden(true);
		ui->m_widget->layout()->addWidget(m_fcaw);
	}
	ui->setupUi(this);
	setContext(context);
}

/**
	@brief SelectAutonumW::~SelectAutonumW
	Destructor
*/
SelectAutonumW::~SelectAutonumW()
{
	delete ui;
}

/**
	@brief SelectAutonumW::setContext
	build the context of current diagram
	selected in the diagram_chooser QcomboBox
	@param context
*/
void SelectAutonumW::setContext(const NumerotationContext &context)
{
	m_context = context;

	qDeleteAll(num_part_list_);
	num_part_list_.clear();

	if (m_context.size() == 0) { //@context contain nothing, build a default numPartEditor
		on_add_button_clicked();
	}
	else {
		for (int i=0; i<m_context.size(); ++i) { //build with the content of @context
			NumPartEditorW *part= new NumPartEditorW(m_context, i, m_edited_type, this);
			connect (part, SIGNAL(changed()), this, SLOT(applyEnable()));
			num_part_list_ << part;
			ui -> editor_layout -> addWidget(part);
		}
	}

	num_part_list_.size() == 1 ?
				ui -> remove_button -> setDisabled(true):
				ui -> remove_button -> setEnabled (true);

	applyEnable(false);
}

/**
	@brief SelectAutonumW::toNumContext
	@return the content to num_part_list to NumerotationContext
*/
NumerotationContext SelectAutonumW::toNumContext() const
{
	NumerotationContext nc;
	foreach (NumPartEditorW *npew, num_part_list_)
		nc << npew -> toNumContext();
	return nc;
}

/**
	@brief SelectAutonumW::on_add_button_clicked
 *	Action on add_button, add a NumPartEditor
*/
void SelectAutonumW::on_add_button_clicked()
{
	applyEnable(false);
	NumPartEditorW *part = new NumPartEditorW(m_edited_type, this);
	connect (part, SIGNAL(changed()), this, SLOT(applyEnable()));
	num_part_list_ << part;
	ui -> editor_layout -> addWidget(part);
	ui -> remove_button -> setEnabled(true);
}

/**
	@brief SelectAutonumW::on_remove_button_clicked
 *	Action on remove button, remove the last NumPartEditor
*/
void SelectAutonumW::on_remove_button_clicked()
{
	//remove if @num_part_list contains more than one item
	if (num_part_list_.size() > 1) {
		NumPartEditorW *part = num_part_list_.takeLast();
		disconnect(part, SIGNAL(changed()), this, SLOT(applyEnable()));
		delete part;
		if (num_part_list_.size() == 1) {
			ui -> remove_button -> setDisabled(true);
		}
	}
	applyEnable();
}

/**
	@brief SelectAutonumW::formula
	@return autonumbering widget formula
*/
QString SelectAutonumW::formula()
{
	if (m_edited_type == 0)
		return m_feaw->formula();
	else if (m_edited_type == 1)
		return m_fcaw->formula();
	else
		return "";
}

QComboBox *SelectAutonumW::contextComboBox() const
{
	return ui->m_comboBox;
}

/**
	@brief SelectAutonumW::on_buttonBox_clicked
	Action on button clicked
	@param button
*/
void SelectAutonumW::on_buttonBox_clicked(QAbstractButton *button)
{
	//transform button to int
	int answer = ui -> buttonBox -> buttonRole(button);
	switch (answer) {
			//Reset the current context
		case QDialogButtonBox::ResetRole:
			setContext(m_context);
			break;
			//help dialog
		case QDialogButtonBox::HelpRole:
			if (m_edited_type == 2)
			{
				QMessageBox::information (
							this,
							tr("Folio Auto Numbering",
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

							   "The \"Text\" type represents fixed text. The \"Increment\" field is not used.\n"
							   "help dialog about the folio autonumbering"
							   ));
				break;
			}
			else if (m_edited_type == 1)
			{
				QMessageBox::information (
							this,
							tr("Conductor Auto Numbering",
							   "title window"),
							tr("C'est ici que vous pouvez définir la manière dont seront numérotés les nouveaux conducteurs.\n"
							   "-Une numérotation est composée d'une variable minimum.\n"
							   "-You can add or remove a numbering variable using the - and + buttons.\n"
							   "-A numbering variable consists of: a type, a value, and an increment.\n"

							   "\n-the types \"Digit 1\", \"Digit 01\", and \"Digit 001\", represent a numeric type defined in the \"Value\" field, "
							   "which is incremented with each new conductor by the value of the \"Increment\" field.\n"
							   "-\"Digit 01\" and \"Digit 001\", are respectively represented on the diagram by two and three digits minimum.\n"
							   "If the digit defined in the Value field has fewer digits than the chosen type,"
							   "celui-ci sera précédé par un ou deux 0 afin de respecter son type.\n"

							   "\n-The \"Text\" type represents fixed text.\nThe \"Increment\" field is not used.\n"

							   "\n-The type \"Folio No.\" represents the number of the current folio.\nThe other fields are not used.\n"

							   "The \"Folio\" type represents the name of the current folio. The other fields are not used.\n"
							   "help dialog about the conductor autonumerotation"
							   ));
				break;
			}
			else
			{
				QMessageBox::information (
							this,
							tr("Element Auto Numbering",
							   "title window"),
							tr("C'est ici que vous pouvez définir la manière dont seront numérotés les nouveaux elements.\n"
							   "-Une numérotation est composée d'une variable minimum.\n"
							   "-You can add or remove a numbering variable using the - and + buttons.\n"
							   "-A numbering variable consists of: a type, a value, and an increment.\n"

							   "\n-the types \"Digit 1\", \"Digit 01\", and \"Digit 001\", represent a numeric type defined in the \"Value\" field, "
							   "which is incremented with each new conductor by the value of the \"Increment\" field.\n"
							   "-\"Digit 01\" and \"Digit 001\", are respectively represented on the diagram by two and three digits minimum.\n"
							   "If the digit defined in the Value field has fewer digits than the chosen type,"
							   "celui-ci sera précédé par un ou deux 0 afin de respecter son type.\n"

							   "\n-The \"Text\" type represents fixed text.\nThe \"Increment\" field is not used.\n"

							   "\n-The type \"Folio No.\" represents the number of the current folio.\nThe other fields are not used.\n"

							   "The \"Folio\" type represents the name of the current folio. The other fields are not used."
							   "help dialog about the element autonumbering"
							   ));
				break;
			}
			//apply the context in the diagram displayed by @diagram_chooser.
		case QDialogButtonBox::ApplyRole:
			applyEnable(false);
			emit applyPressed();
			break;
	};
}

/**
	@brief SelectAutonumW::applyEnable
	enable/disable the apply button
*/
void SelectAutonumW::applyEnable(bool b)
{
	if (b){
		bool valid= true;
		foreach (NumPartEditorW *npe, num_part_list_)
			if (!npe -> isValid())
				valid= false;

		ui->buttonBox->button(QDialogButtonBox::Apply)
				->setEnabled(valid);
	}
	else {
		ui->buttonBox->button(QDialogButtonBox::Apply)
				->setEnabled(b);
	}
	if (m_edited_type == 0)
		contextToFormula();
	if (m_edited_type == 1)
		contextToFormula();
}

/**
	@brief SelectAutonumW::contextToFormula
	Apply formula to ElementAutonumbering Widget
*/
void SelectAutonumW::contextToFormula()
{
	FormulaAutonumberingW* m_faw = nullptr;
	if (m_edited_type == 0)
		m_faw = m_feaw;
	else if (m_edited_type == 1)
		m_faw = m_fcaw;

	if (m_faw)
	{
		m_faw->clearContext();
		m_faw->setContext(autonum::numerotationContextToFormula(
					  toNumContext()));
	}
}

/**
	@brief SelectAutonumW::on_m_next_pb_clicked
	Increase NumerotationContext
*/
void SelectAutonumW::on_m_next_pb_clicked()
{
	NumerotationContextCommands ncc (toNumContext());
	setContext(ncc.next());
	applyEnable(true);
}

/**
	@brief SelectAutonumW::on_m_previous_pb_clicked
	Decrease NumerotationContext
*/
void SelectAutonumW::on_m_previous_pb_clicked()
{
	NumerotationContextCommands ncc (toNumContext());
	setContext(ncc.previous());
	applyEnable(true);
}

void SelectAutonumW::on_m_comboBox_currentTextChanged(const QString &arg1)
{
	Q_UNUSED(arg1);
	applyEnable(true);
}

void SelectAutonumW::on_m_remove_pb_clicked()
{
	emit removeClicked();
}
