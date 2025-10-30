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
#ifndef BORDER_PROPERTIES_H
#define BORDER_PROPERTIES_H
#include <QtCore>
#include <QtXml>

/**
	@brief The BorderProperties class
	This class is a container for dimensions and display properties of a
	diagram.
	@remark Attributes are public
*/
class BorderProperties {
	public:
		// constructor, destructor, operators
		BorderProperties();
		virtual ~BorderProperties();
	
		bool operator==(const BorderProperties &);
		bool operator!=(const BorderProperties &);
	
		void toXml(QDomElement &) const;
		void fromXml(QDomElement &);
		void toSettings(QSettings &, const QString & = QString()) const;
		void fromSettings(QSettings &, const QString & = QString());

		static BorderProperties defaultProperties();
	
		/**
		 * @brief calculateDimensions
		 * Calculate columns_width and rows_height based on aspect_ratio, base_area, scale
		 * and the counts. This maintains constant area while respecting the aspect ratio.
		 */
		void calculateDimensions();
	
	// attributes
	int columns_count;            ///< Columns count
	qreal columns_width;          ///< Columns width (calculated if use_calculated_dimensions is true)
	qreal columns_header_height;  ///< Column headers height
	bool display_columns;         ///< Backward-compat: whether to display column headers (top)
	bool display_columns_top;     ///< Whether to display column headers on top
	bool display_columns_bottom;  ///< Whether to display column headers on bottom
	
	int rows_count;               ///< Rows count
	qreal rows_height;            ///< Rows height (calculated if use_calculated_dimensions is true)
	qreal rows_header_width;      ///< Row headers width
	bool display_rows;            ///< Backward-compat: whether to display row headers (left)
	bool display_rows_left;       ///< Whether to display row headers on left
	bool display_rows_right;      ///< Whether to display row headers on right
	bool border_all_sides;        ///< Deprecated: Whether to draw border on all sides
	

		// New calculated dimension system
		bool use_calculated_dimensions;  ///< If true, calculate dimensions from aspect ratio and area
		qreal aspect_ratio;              ///< Backward-compat: width/height ratio
		qreal aspect_ratio_w;            ///< Aspect ratio width component (e.g., 297)
		qreal aspect_ratio_h;            ///< Aspect ratio height component (e.g., 210)
		qreal base_area;                 ///< Base drawing area in square pixels (columns_total_width * rows_total_height)
		qreal scale;                     ///< Scale multiplier for base_area (actual area = base_area * scale)
};
#endif
