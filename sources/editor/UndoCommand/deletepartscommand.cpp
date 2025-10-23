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
#include "deletepartscommand.h"
#include "../elementscene.h"

/**
 * @brief ToletePartsCommand::ToletePartsCommand
 * @param scene
 * @param parts
 * @param parent
 */
ToletePartsCommand::ToletePartsCommand(QPointer<ElementScene> scene,
									   const QVector<QGraphicsItem *> &parts,
									   QUndoCommand *parent) :
	QUndoCommand{parent},
	m_scene{scene},
	m_parts{parts}
{
	setText(QObject::tr("deletion", "undo caption"));
	m_scene->qgiManager().manage(parts);
}

/**
 * @brief ToletePartsCommand::~ToletePartsCommand
 */
ToletePartsCommand::~ToletePartsCommand()
{
	if (m_scene) {
		m_scene->qgiManager().release(m_parts);
	}
}

/**
 * @brief ToletePartsCommand::undo
 */
void ToletePartsCommand::undo()
{
	if (m_scene) {
		m_scene->addItems(m_parts);
	}
}

/**
 * @brief ToletePartsCommand::redo
 */
void ToletePartsCommand::redo()
{
	if (m_scene) {
		m_scene->removeItems(m_parts);
	}
}
