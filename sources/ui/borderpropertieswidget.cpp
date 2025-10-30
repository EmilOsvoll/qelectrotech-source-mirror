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
#include <numeric>

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

	// Ensure calculated mode is always active and hide related controls
	ui->m_use_calculated_dimensions_cb->setChecked(true);
	ui->m_use_calculated_dimensions_cb->setVisible(false);
	ui->label_base_area->setVisible(false);
	ui->m_base_area_sp->setValue(652800.0);
	ui->m_base_area_sp->setVisible(false);
	// Old numeric aspect ratio control removed from UI; no action needed

	// Connect signals to update calculated dimensions when inputs change
	connect(ui->m_use_calculated_dimensions_cb, &QCheckBox::toggled,
		this, &BorderPropertiesWidget::onCalculateDimensionsToggled);
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
	connect(ui->m_base_area_sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_scale_sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_colums_count_sp, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_rows_count_sp, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_aspect_ratio_width_sp, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onAspectRatioPartsChanged);
	connect(ui->m_aspect_ratio_height_sp, QOverload<int>::of(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onAspectRatioPartsChanged);
#else
	connect(ui->m_base_area_sp, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_scale_sp, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_colums_count_sp, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_rows_count_sp, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
	connect(ui->m_aspect_ratio_width_sp, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onAspectRatioPartsChanged);
	connect(ui->m_aspect_ratio_height_sp, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
		this, &BorderPropertiesWidget::onAspectRatioPartsChanged);
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

	// Force calculated dimension properties
	ui -> m_use_calculated_dimensions_cb ->setChecked(true);
	// Set default aspect ratio parts to 293:210
	ui->m_aspect_ratio_width_sp->setValue(293);
	ui->m_aspect_ratio_height_sp->setValue(210);
	ui -> m_base_area_sp ->setValue(652800.0);
	ui -> m_scale_sp ->setValue(m_properties.scale);

	// Initialize intermediary counts used for calculations
	m_pendingColumnsCount = m_properties.columns_count;
	m_pendingRowsCount = m_properties.rows_count;

	// Now recalc once with the finalized values
	onCalculateDimensionsToggled(true);
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
	
	// Always use calculated dimensions with fixed base area
	m_properties.use_calculated_dimensions = true;
	int rw_prop = std::max(1, ui->m_aspect_ratio_width_sp->value());
	int rh_prop = std::max(1, ui->m_aspect_ratio_height_sp->value());
	m_properties.aspect_ratio = static_cast<double>(rw_prop) / static_cast<double>(rh_prop);
	m_properties.base_area = 652800.0;
	m_properties.scale = ui -> m_scale_sp -> value();
	m_properties.calculateDimensions();
	
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
	Q_UNUSED(enabled);

	// Always-enabled calculated dimension controls (except base area which is fixed)
	ui->m_base_area_sp->setEnabled(false);
	ui->m_scale_sp->setEnabled(true);

	// Make column width and row height read-only (always calculated)
	ui->m_columns_width_sp->setReadOnly(true);
	ui->m_rows_height_sp->setReadOnly(true);

	// Sync intermediaries from UI if valid
	int cols = ui->m_colums_count_sp->value();
	int rows = ui->m_rows_count_sp->value();
	if (cols > 0) m_pendingColumnsCount = cols;
	if (rows > 0) m_pendingRowsCount = rows;

	// Update properties and recalculate using intermediaries
	m_properties.use_calculated_dimensions = true;
	// Aspect ratio from width:height inputs
	int rw = std::max(1, ui->m_aspect_ratio_width_sp->value());
	int rh = std::max(1, ui->m_aspect_ratio_height_sp->value());
	m_properties.aspect_ratio = static_cast<double>(rw) / static_cast<double>(rh);
	m_properties.base_area = 652800.0;
	m_properties.scale = ui->m_scale_sp->value();
	m_properties.columns_count = m_pendingColumnsCount;
	m_properties.rows_count = m_pendingRowsCount;
	m_properties.calculateDimensions();

	// Update the displayed values
	ui->m_columns_width_sp->setValue(m_properties.columns_width);
	ui->m_rows_height_sp->setValue(m_properties.rows_height);
}

void BorderPropertiesWidget::onCalculateDimensionsChanged()
{
	// Update intermediaries first and ignore transient zeros
	int cols = ui->m_colums_count_sp->value();
	int rows = ui->m_rows_count_sp->value();
	if (cols > 0) m_pendingColumnsCount = cols;
	if (rows > 0) m_pendingRowsCount = rows;

	// Always update using calculated mode and fixed base area
	int rw = std::max(1, ui->m_aspect_ratio_width_sp->value());
	int rh = std::max(1, ui->m_aspect_ratio_height_sp->value());
	m_properties.aspect_ratio = static_cast<double>(rw) / static_cast<double>(rh);
	m_properties.base_area = 652800.0;
	m_properties.scale = ui->m_scale_sp->value();
	m_properties.columns_count = m_pendingColumnsCount;
	m_properties.rows_count = m_pendingRowsCount;
	m_properties.use_calculated_dimensions = true;

	// Recalculate dimensions
	m_properties.calculateDimensions();

	// Update the displayed values
	ui->m_columns_width_sp->setValue(m_properties.columns_width);
	ui->m_rows_height_sp->setValue(m_properties.rows_height);
}

void BorderPropertiesWidget::onAspectRatioPartsChanged()
{
	onCalculateDimensionsChanged();
}
