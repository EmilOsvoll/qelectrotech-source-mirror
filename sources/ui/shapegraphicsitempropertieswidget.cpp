/*
	Copyright 2006-2015 The QElectroTech Team
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
#include "shapegraphicsitempropertieswidget.h"
#include "ui_shapegraphicsitempropertieswidget.h"
#include "qetshapeitem.h"
#include "diagram.h"
#include "changeshapestylecommand.h"

/**
 * @brief ShapeGraphicsItemPropertiesWidget::ShapeGraphicsItemPropertiesWidget
 * Constructor
 * @param item : shape to edit
 * @param parent : parent widget
 */
ShapeGraphicsItemPropertiesWidget::ShapeGraphicsItemPropertiesWidget(QetShapeItem *item, QWidget *parent) :
	PropertiesEditorWidget(parent),
	ui(new Ui::ShapeGraphicsItemPropertiesWidget),
	m_shape(nullptr)
{
	ui->setupUi(this);
	setItem(item);
}

/**
 * @brief ShapeGraphicsItemPropertiesWidget::~ShapeGraphicsItemPropertiesWidget
 * Destructor
 */
ShapeGraphicsItemPropertiesWidget::~ShapeGraphicsItemPropertiesWidget()
{
	delete ui;
}

/**
 * @brief ShapeGraphicsItemPropertiesWidget::setItem
 * Set @shape as the current edited item
 * @param shape
 */
void ShapeGraphicsItemPropertiesWidget::setItem(QetShapeItem *shape)
{
	if (!shape) return;
	if (shape == m_shape) return;

	if (m_shape)
		disconnect(m_shape, &QetShapeItem::styleChanged, this, &ShapeGraphicsItemPropertiesWidget::updateUi);

	m_shape = shape;
	connect(m_shape, &QetShapeItem::styleChanged, this, &ShapeGraphicsItemPropertiesWidget::updateUi);

	m_old_pen_style = m_shape->penStyle();
	updateUi();
}

/**
 * @brief ShapeGraphicsItemPropertiesWidget::apply
 * Apply the current change, by pushing an undo command to the
 * undo stack of the shape diagram.
 */
void ShapeGraphicsItemPropertiesWidget::apply()
{
	if (m_live_edit)
		disconnect(m_shape, &QetShapeItem::styleChanged, this, &ShapeGraphicsItemPropertiesWidget::updateUi);

	if (m_shape->diagram())
		if (QUndoCommand *undo = associatedUndo())
			m_shape->diagram()->undoStack().push(undo);

	m_old_pen_style = m_shape->penStyle();

	if (m_live_edit)
		connect(m_shape, &QetShapeItem::styleChanged, this, &ShapeGraphicsItemPropertiesWidget::updateUi);
}

/**
 * @brief ShapeGraphicsItemPropertiesWidget::reset
 * Reset the change
 */
void ShapeGraphicsItemPropertiesWidget::reset()
{
	m_shape->setStyle(m_old_pen_style);
	updateUi();
}

/**
 * @brief ShapeGraphicsItemPropertiesWidget::associatedUndo
 * @return an undo command that represent the change edited by this widget.
 * The returned undo command is a ChangeShapeStyleCommand.
 * If there isn't change, return nullptr
 */
QUndoCommand* ShapeGraphicsItemPropertiesWidget::associatedUndo() const
{
	Qt::PenStyle new_style = Qt::PenStyle(ui->m_style_cb->currentIndex() + 1);
	if (new_style != m_old_pen_style) return (new ChangeShapeStyleCommand(m_shape, m_old_pen_style, new_style));

	return nullptr;
}

/**
 * @brief ShapeGraphicsItemPropertiesWidget::updateUi
 */
void ShapeGraphicsItemPropertiesWidget::updateUi()
{
	ui->m_style_cb->setCurrentIndex(static_cast<int>(m_shape->penStyle()) - 1);
	ui->m_lock_pos_cb->setChecked(!m_shape->isMovable());
}

/**
 * @brief ShapeGraphicsItemPropertiesWidget::setLiveEdit
 * @param live_edit
 * @return always true
 */
bool ShapeGraphicsItemPropertiesWidget::setLiveEdit(bool live_edit)
{
	if (live_edit == m_live_edit) return true;
	m_live_edit = live_edit;

	if (m_live_edit)
		connect (ui->m_style_cb, SIGNAL(activated(int)), this, SLOT(apply()));
	else
		disconnect (ui->m_style_cb, SIGNAL(activated(int)), this, SLOT(apply()));

	return true;
}

void ShapeGraphicsItemPropertiesWidget::on_m_lock_pos_cb_clicked() {
	m_shape->setMovable(!ui->m_lock_pos_cb->isChecked());
}