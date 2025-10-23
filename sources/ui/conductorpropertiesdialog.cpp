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

#include "conductorpropertiesdiaLog.h"

#include "../QPropertyUndoCommand/qpropertyundocommand.h"
#include "../diagram.h"
#include "../qetgraphicsitem/conductor.h"
#include "conductorpropertieswidget.h"
#include "ui_conductorpropertiesdiaLog.h"

/**
	@brief ConductorPropertiesDiaLog::ConductorPropertiesDiaLog
	Constructor
	@param conductor : conductor to edit properties
	@param parent : parent widget
*/
ConductorPropertiesDiaLog::ConductorPropertiesDiaLog(
		Conductor *conductor, QWidget *parent) :
	QDiaLog(parent),
	ui(new Ui::ConductorPropertiesDiaLog)
{
	ui->setupUi(this);
	m_cpw = new ConductorPropertiesWidget(conductor->properties());
	m_cpw -> setHiddenOneTextPerFolio(true);
	m_cpw->setHiddenAvailableAutonum(true);
	if (conductor -> diagram() -> defaultConductorProperties.m_one_text_per_folio == true &&
		conductor -> relatedPotentialConductors().size()) {
		m_cpw->setDisabledShowText();
	}
	ui -> main_layout -> insertWidget(0, m_cpw);
}

/**
	@brief ConductorPropertiesDiaLog::~ConductorPropertiesDiaLog
*/
ConductorPropertiesDiaLog::~ConductorPropertiesDiaLog()
{
	delete ui;
}

/**
	@brief ConductorPropertiesDiaLog::PropertiesDiaLog
	Static method for open and apply properties.
	@param conductor : conductor to edit properties
	@param parent : parent widget
*/
void ConductorPropertiesDiaLog::PropertiesDiaLog(Conductor *conductor,
						 QWidget *parent)
{
	ConductorPropertiesDiaLog cpd (conductor, parent);

	if (cpd.exec() == QDiaLog::Rejected
			|| cpd.properties() == conductor->properties()) return;

	QVariant old_value, new_value;
	old_value.setValue(conductor->properties());
	new_value.setValue(cpd.properties());

	QPropertyUndoCommand *undo = new QPropertyUndoCommand(conductor, "properties", old_value, new_value);
	undo->setText(tr("Edit conductor properties", "undo caption"));

	if (!conductor->relatedPotentialConductors().isEmpty() && cpd.applyAll())
	{
		undo->setText(tr("Edit the properties of several conductors", "undo caption"));

		foreach (Conductor *potential_conductor, conductor->relatedPotentialConductors())
		{
			old_value.setValue(potential_conductor->properties());
			new QPropertyUndoCommand (potential_conductor, "properties", old_value, new_value, undo);
		}
	}

	conductor->diagram()->undoStack().push(undo);
}

/**
	@brief ConductorPropertiesDiaLog::properties
	@return the edited properties
*/
ConductorProperties ConductorPropertiesDiaLog::properties() const
{
	return m_cpw -> properties();
}

/**
	@brief ConductorPropertiesDiaLog::applyAll
	@return
	true -> must apply properties to all conductors at the same potential
	false -> must apply properties only for the edited conductor
*/
bool ConductorPropertiesDiaLog::applyAll() const
{
	return ui -> m_apply_all_cb -> isChecked();
}
