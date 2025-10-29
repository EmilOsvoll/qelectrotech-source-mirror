/*
	Copyright 2006-2025 The QElectroTech Team
	This file is part of QElectroTech.

	QElectroTech is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	QElectroTech is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with QElectroTech. If not, see <http://www.gnu.org/licenses/>.
*/
#include "borderpropertieswidget.h"

#include "../diagram.h"
#include "ui_borderpropertieswidget.h"
#include <QtCore/QtGlobal>

/**
	@brief BorderPropertiesWidget::BorderPropertiesWidget
	default constructor
	@param bp properties
	@param parent paretn widget
*/
BorderPropertiesWidget::BorderPropertiesWidget(const BorderProperties &bp, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::BorderPropertiesWidget)
{
	ui->setupUi(this);
	
	// Connect signals to update calculated dimensions when inputs change
	connect(ui->m_use_calculated_dimensions_cb, &QCheckBox::toggled,
		this, &BorderPropertiesWidget::onCalculateDimensionsToggled);
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
	connect(ui->m_aspect_ratio_sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_base_area_sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_scale_sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_colums_count_sp, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_rows_count_sp, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
#else
	connect(ui->m_aspect_ratio_sp, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_base_area_sp, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_scale_sp, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_colums_count_sp, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_rows_count_sp, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
#endif
	
	setProperties(bp);
}

/**
	@brief BorderPropertiesWidget::~BorderPropertiesWidget
	default destructor
*/
BorderPropertiesWidget::~BorderPropertiesWidget()
{
	delete ui;
}

/**
	@brief BorderPropertiesWidget::setProperties
	Set the current properties to edit
	@param bp properties to edit
*/
void BorderPropertiesWidget::setProperties(const BorderProperties &bp)
{
	m_properties = bp;
	ui -> m_colums_count_sp    ->setValue   (m_properties.columns_count);
	ui -> m_columns_width_sp   ->setValue   (m_properties.columns_width);
	ui -> m_display_columns_cb ->setChecked (m_properties.display_columns);
	ui -> m_rows_count_sp      ->setValue   (m_properties.rows_count);
	ui -> m_rows_height_sp     ->setValue   (m_properties.rows_height);
	ui -> m_display_rows_cb    ->setChecked (m_properties.display_rows);
	ui -> m_border_all_sides_cb ->setChecked(m_properties.border_all_sides);
	
	// Set new calculated dimension properties
	ui -> m_use_calculated_dimensions_cb ->setChecked(m_properties.use_calculated_dimensions);
	ui -> m_aspect_ratio_sp ->setValue(m_properties.aspect_ratio);
	ui -> m_base_area_sp ->setValue(m_properties.base_area);
	ui -> m_scale_sp ->setValue(m_properties.scale);
	
	// Update UI state based on calculated dimensions
	onCalculateDimensionsToggled(m_properties.use_calculated_dimensions);
}

/**
	@brief BorderPropertiesWidget::properties
	@return the edited border properties
*/
const BorderProperties &BorderPropertiesWidget::properties ()
{
	m_properties.columns_count   = ui -> m_colums_count_sp    -> value();
	m_properties.display_columns = ui -> m_display_columns_cb -> isChecked();
	m_properties.rows_count      = ui -> m_rows_count_sp      -> value();
	m_properties.display_rows    = ui -> m_display_rows_cb    -> isChecked();
	m_properties.border_all_sides = ui -> m_border_all_sides_cb -> isChecked();
	
	// Get calculated dimension properties
	m_properties.use_calculated_dimensions = ui -> m_use_calculated_dimensions_cb -> isChecked();
	m_properties.aspect_ratio = ui -> m_aspect_ratio_sp -> value();
	m_properties.base_area = ui -> m_base_area_sp -> value();
	m_properties.scale = ui -> m_scale_sp -> value();
	
	if (m_properties.use_calculated_dimensions) {
		// Calculate dimensions based on new settings
		m_properties.calculateDimensions();
	} else {
		// Use manual values
		m_properties.columns_width = ui -> m_columns_width_sp -> value();
		m_properties.rows_height = ui -> m_rows_height_sp -> value();
	}
	
	return m_properties;
}

/**
	@brief BorderPropertiesWidget::setReadOnly
	Enable or disable this widget
	@param ro true-disable / false-enable
*/
void BorderPropertiesWidget::setReadOnly(const bool &ro)
{
	ui->border_gb->setDisabled(ro);
}

void BorderPropertiesWidget::onCalculateDimensionsToggled(bool enabled)
{
	// Enable/disable calculated dimension controls
	ui->m_aspect_ratio_sp->setEnabled(enabled);
	ui->m_base_area_sp->setEnabled(enabled);
	ui->m_scale_sp->setEnabled(enabled);
	
	// Make column width and row height read-only when using calculated dimensions
	ui->m_columns_width_sp->setReadOnly(enabled);
	ui->m_rows_height_sp->setReadOnly(enabled);
	
	if (enabled) {
		// Update properties and recalculate
		m_properties.use_calculated_dimensions = true;
		m_properties.aspect_ratio = ui->m_aspect_ratio_sp->value();
		m_properties.base_area = ui->m_base_area_sp->value();
		m_properties.scale = ui->m_scale_sp->value();
		m_properties.columns_count = ui->m_colums_count_sp->value();
		m_properties.rows_count = ui->m_rows_count_sp->value();
		m_properties.calculateDimensions();
		
		// Update the displayed values
		ui->m_columns_width_sp->setValue(m_properties.columns_width);
		ui->m_rows_height_sp->setValue(m_properties.rows_height);
	}
}

void BorderPropertiesWidget::onCalculateDimensionsChanged()
{
	if (ui->m_use_calculated_dimensions_cb->isChecked()) {
		// Update properties
		m_properties.aspect_ratio = ui->m_aspect_ratio_sp->value();
		m_properties.base_area = ui->m_base_area_sp->value();
		m_properties.scale = ui->m_scale_sp->value();
		m_properties.columns_count = ui->m_colums_count_sp->value();
		m_properties.rows_count = ui->m_rows_count_sp->value();
		m_properties.use_calculated_dimensions = true;
		
		// Recalculate dimensions
		m_properties.calculateDimensions();
		
		// Update the displayed values
		ui->m_columns_width_sp->setValue(m_properties.columns_width);
		ui->m_rows_height_sp->setValue(m_properties.rows_height);
	}
}
