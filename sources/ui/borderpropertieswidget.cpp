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
	
	// Set minimum values for spinboxes to prevent invalid values
	// Note: Default is 5 for rows, but minimum allowed is 1
	ui->m_colums_count_sp->setMinimum(1);
	ui->m_rows_count_sp->setMinimum(1);
	
    // Connect signals to update calculated dimensions when inputs change
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    connect(ui->m_aspect_ratio_w_sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
        this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
    connect(ui->m_aspect_ratio_h_sp, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
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
    connect(ui->m_aspect_ratio_w_sp, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
        this, &BorderPropertiesWidget::onCalculateDimensionsChanged);
    connect(ui->m_aspect_ratio_h_sp, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
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
	
	// Ensure valid minimum values - enforce defaults if values are invalid
	// IMPORTANT: Counts are NEVER calculated - they are user input that drives calculations
	// Only validate invalid values (<= 0) - preserve all valid user-set values
	if (m_properties.columns_count <= 0) m_properties.columns_count = 8;
	if (m_properties.rows_count <= 0) m_properties.rows_count = 5;
	
	// Ensure calculated dimensions are up to date before displaying
	// This recalculates width/height from counts, but NEVER modifies counts
	if (m_properties.use_calculated_dimensions) {
		m_properties.calculateDimensions();
	}
	
    ui -> m_colums_count_sp    ->setValue   (m_properties.columns_count);
    ui -> m_columns_width_ro   ->setValue   (m_properties.columns_width);
    ui -> m_display_columns_top_cb ->setChecked (m_properties.display_columns_top);
    ui -> m_display_columns_bottom_cb ->setChecked (m_properties.display_columns_bottom);
    ui -> m_rows_count_sp      ->setValue   (m_properties.rows_count);
    ui -> m_rows_height_ro     ->setValue   (m_properties.rows_height);
    ui -> m_display_rows_left_cb ->setChecked (m_properties.display_rows_left);
    ui -> m_display_rows_right_cb ->setChecked (m_properties.display_rows_right);
    // border always drawn on all sides now; no checkbox
	
    // Set new calculated dimension properties
    {
        double widthValue = m_properties.aspect_ratio_w > 0.0 ? m_properties.aspect_ratio_w : 297.0;
        double heightValue = m_properties.aspect_ratio_h > 0.0 ? m_properties.aspect_ratio_h : 210.0;
        ui -> m_aspect_ratio_w_sp ->setValue(widthValue);
        ui -> m_aspect_ratio_h_sp ->setValue(heightValue);
    }
	ui -> m_base_area_sp ->setValue(m_properties.base_area);
	ui -> m_scale_sp ->setValue(m_properties.scale);
	
    // Update the read-only dimension fields to match calculated values
    ui->m_columns_width_ro->setValue(m_properties.columns_width);
    ui->m_rows_height_ro->setValue(m_properties.rows_height);
}

/**
	@brief BorderPropertiesWidget::properties
	@return the edited border properties
*/
const BorderProperties &BorderPropertiesWidget::properties ()
{
	m_properties.columns_count   = ui -> m_colums_count_sp    -> value();
    m_properties.display_columns_top = ui -> m_display_columns_top_cb -> isChecked();
    m_properties.display_columns_bottom = ui -> m_display_columns_bottom_cb -> isChecked();
	m_properties.rows_count      = ui -> m_rows_count_sp      -> value();
    m_properties.display_rows_left = ui -> m_display_rows_left_cb -> isChecked();
    m_properties.display_rows_right = ui -> m_display_rows_right_cb -> isChecked();
    m_properties.border_all_sides = true;
	
    // Get calculated dimension properties
    {
        double ratio_w = ui -> m_aspect_ratio_w_sp -> value();
        double ratio_h = ui -> m_aspect_ratio_h_sp -> value();
        if (ratio_h <= 0.0) ratio_h = 1.0;
        m_properties.aspect_ratio_w = ratio_w;
        m_properties.aspect_ratio_h = ratio_h;
        m_properties.aspect_ratio = ratio_w / ratio_h;
    }
	m_properties.base_area = ui -> m_base_area_sp -> value();
	m_properties.scale = ui -> m_scale_sp -> value();
	
    // Always calculate dimensions based on new settings
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

void BorderPropertiesWidget::onCalculateDimensionsToggled(bool) {}

void BorderPropertiesWidget::onCalculateDimensionsChanged()
{
    // Update properties
        {
            double ratio_w = ui->m_aspect_ratio_w_sp->value();
            double ratio_h = ui->m_aspect_ratio_h_sp->value();
            if (ratio_h <= 0.0) ratio_h = 1.0;
            m_properties.aspect_ratio_w = ratio_w;
            m_properties.aspect_ratio_h = ratio_h;
            m_properties.aspect_ratio = ratio_w / ratio_h;
        }
		m_properties.base_area = ui->m_base_area_sp->value();
		m_properties.scale = ui->m_scale_sp->value();
		m_properties.columns_count = ui->m_colums_count_sp->value();
		m_properties.rows_count = ui->m_rows_count_sp->value();
		
		// Recalculate dimensions
		m_properties.calculateDimensions();
		
		// Update the displayed values
        ui->m_columns_width_ro->setValue(m_properties.columns_width);
        ui->m_rows_height_ro->setValue(m_properties.rows_height);
}
