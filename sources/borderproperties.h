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
	bool display_columns;         ///< Whether to display column headers
	
	int rows_count;               ///< Rows count
	qreal rows_height;            ///< Rows height (calculated if use_calculated_dimensions is true)
	qreal rows_header_width;      ///< Row headers width
	bool display_rows;            ///< Whether to display row headers
	bool border_all_sides;        ///< Whether to draw border on all sides (false = only left/top)
	
	// New calculated dimension system
	bool use_calculated_dimensions;  ///< If true, calculate dimensions from aspect_ratio and area
	qreal aspect_ratio;              ///< Width to height ratio of the drawing area (width/height)
	qreal base_area;                 ///< Base drawing area in square pixels (columns_total_width * rows_total_height)
	qreal scale;                     ///< Scale multiplier for base_area (actual area = base_area * scale)
};
#endif
