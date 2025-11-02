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
#include "bordertitleblock.h"
#include <QDebug>

#include "createdxf.h"
#include "diagram.h"
#include "diagramposition.h"
#include "math.h"
#include "qetapp.h"
#include "qetversion.h"
#include "titleblocktemplate.h"
#include "titleblocktemplaterenderer.h"


#include <QLocale>
#include <QPainter>
#include <utility>

#define MIN_COLUMN_COUNT 3
#define MIN_ROW_COUNT 3
#define MIN_COLUMN_WIDTH 5.0
#define MIN_ROW_HEIGHT 5.0

/**
	@brief BorderTitleBlock::BorderTitleBlock
	Simple constructor:
	build a border by recovering the dimensions
	in the application configuration.

	\~French Constructeur simple :
	construit une bordure en recuperant les dimensions
	dans la configuration de l'application.
	\~ @param parent :
	\~French QObject parent de ce BorderTitleBlock
*/
BorderTitleBlock::BorderTitleBlock(QObject *parent) :
	QObject(parent)
{
	// at first, the internal titleblock template renderer uses the default titleblock template
	m_titleblock_template_renderer = new TitleBlockTemplateRenderer(this);
	m_titleblock_template_renderer -> setTitleBlockTemplate(QETApp::defaultTitleBlockTemplate());

	// disable the QPicture-based cache from Qt 4.8 to avoid rendering errors and crashes
#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)	// ### Qt 6: remove
#else
	m_titleblock_template_renderer -> setUseCache(false);
#endif

	// dimensions par defaut du schema
	importBorder(BorderProperties::defaultProperties());

	// contenu par defaut du cartouche
	importTitleBlock(TitleBlockProperties());

	display_titleblock_ = true;
	display_border_ = true;
	setFolioData(1, 1);
	updateRectangles();
}

/**
	@brief BorderTitleBlock::~BorderTitleBlock
	\~French Destructeur - ne fait rien
*/
BorderTitleBlock::~BorderTitleBlock()
{
}

/**
	@brief BorderTitleBlock::titleBlockRect
	@return the rectangle of the titleblock in scene coordinate.
*/
QRectF BorderTitleBlock::titleBlockRect() const
{
	QRectF rect;
	
	if (border_all_sides_) {
		// When drawing borders on all sides, title block is INSIDE the border.
		// Position it above/before the border edge so there's no overlap.
		// Use the diagram margin as inset to maintain consistent spacing with the border.
		if (m_edge == Qt::BottomEdge) {
			// Compute content band (between left/right headers) and place the title block
			qreal content_left   = diagram_rect_.topLeft().x() + rows_header_width_;
			qreal separatorX     = qRound(content_left + (columns_count_ * columns_width_));
			qreal content_width  = separatorX - content_left; // align right edge with headers split
			// Round band_top to match separatorY rounding in draw() method for consistent alignment
			qreal band_top       = qRound(diagram_rect_.topLeft().y() + columns_header_height_ + (rows_count_ * rows_height_));
			qreal tbt_height     = m_titleblock_template_renderer->height();
			// Adjust upward by header_line_thickness_ to align title block bottom with row header bottom
			qreal tbt_top        = band_top - tbt_height - header_line_thickness_;
			rect = QRectF(content_left,
				      tbt_top,
				      content_width,
				      tbt_height);
		} else {
			// Position title block inside, starting from right edge minus width (height when rotated) and margin
			rect = QRectF(diagram_rect_.topRight() - QPointF(m_titleblock_template_renderer->height() + Diagram::margin, Diagram::margin),
				      QSize(m_titleblock_template_renderer -> height(),
					    diagram_rect_.height() - 2 * Diagram::margin
					    ));
		}
	} else {
		// When border_all_sides is false, position title block at same location as when true
		// to maintain consistent aspect ratio - title block extends outside but starts at same position
		if (m_edge == Qt::BottomEdge) {
			// Compute content band (between left/right headers) and place the title block
			// at the same vertical position as when border_all_sides is true
			qreal content_left   = diagram_rect_.topLeft().x() + rows_header_width_;
			qreal separatorX     = qRound(content_left + (columns_count_ * columns_width_));
			qreal content_width  = separatorX - content_left; // align right edge with headers split
			// Round band_top to match separatorY rounding in draw() method for consistent alignment
			qreal band_top       = qRound(diagram_rect_.topLeft().y() + columns_header_height_ + (rows_count_ * rows_height_));
			qreal tbt_height     = m_titleblock_template_renderer->height();
			// Adjust upward by header_line_thickness_ to align title block bottom with row header bottom
			qreal tbt_top        = band_top - tbt_height - header_line_thickness_;
			rect = QRectF(content_left,
				      tbt_top,
				      content_width,
				      tbt_height);
		} else {
			// For right edge, use same positioning as when border_all_sides is true
			rect = QRectF(diagram_rect_.topRight() - QPointF(m_titleblock_template_renderer->height() + Diagram::margin, Diagram::margin),
				      QSize(m_titleblock_template_renderer -> height(),
					    diagram_rect_.height() - 2 * Diagram::margin
					    ));
		}
	}
	
	return rect;
}

/**
	@brief BorderTitleBlock::titleblockInformation
	@return
*/
DiagramContext BorderTitleBlock::titleblockInformation() const
{
	return m_titleblock_template_renderer->context();
}

/**
	@brief BorderTitleBlock::titleBlockRectForQPainter
	@return The title block rect to use with the QPainter in the draw method.
	The returned rect is always horizontal
	(as displayed at the bottom of rect) only the top left change of pos
	according to the edge where the title block needs to be displayed.
	Rect according to edge:
	Bottom : top left is at the bottom left edge of the diagram rect.
	Right : top left is at the bottom right of diagram rect.
	Before using this rect you need to rotate the QPainter by -90°
	to snap the rect at the right edge of diagram.
*/
QRectF BorderTitleBlock::titleBlockRectForQPainter() const
{
	//Rect at bottom have same position and dimension of displayed rect
	if (m_edge == Qt::BottomEdge)
		return titleBlockRect();
	else {
		// For right edge, need to convert the vertical rect from titleBlockRect()
		// to a horizontal rect suitable for rotation
		QRectF rect = titleBlockRect();
		if (border_all_sides_) {
			// When inside, rect is already positioned correctly at topRight
			// After -90 rotation, it will align along the right edge
			return rect;
		} else {
			// Original behavior: position at bottomRight for outside placement
			// After -90 rotation, it extends outside along the right edge
			return QRectF (diagram_rect_.bottomRight(),
				       QSize(diagram_rect_.height(),
					     m_titleblock_template_renderer -> height()
					     ));
		}
	}
}

/**
	@brief BorderTitleBlock::borderAndTitleBlockRect
	@return the bounding rectangle of diagram and titleblock.
	It's like unite outsideBorderRect and titleBlockRect.
	The rect is in scene coordinate
*/
QRectF BorderTitleBlock::borderAndTitleBlockRect() const
{
	// Compute full extent including both left/right and top/bottom header bands
	qreal w = (columns_width_ * columns_count_);
	qreal h = (rows_height_ * rows_count_);
	if (display_rows_) {
		w += rows_header_width_; // left header
		w += rows_header_width_; // right header
		// When not drawing border on all sides, add one more row header width
		// to account for the extra space needed
		if (!border_all_sides_) {
			w -= rows_header_width_;
		}
	}
	if (display_columns_) {
		h += columns_header_height_; // top header
		h += columns_header_height_; // bottom header
		// When not drawing border on all sides, add one more column header height
		// to account for the extra space needed
		if (!border_all_sides_) {
			h -= columns_header_height_;
		}
	}
	QRectF full = QRectF(Diagram::margin, Diagram::margin, w, h);
	return full | titleBlockRect();
}

/**
	@brief BorderTitleBlock::columnsRect
	@return The columns rect in scene coordinate.
	If column is not displayed, return a null QRectF
*/
QRectF BorderTitleBlock::columnsRect() const
{
	if (!display_columns_) return QRectF();
	return QRectF (Diagram::margin,
		       Diagram::margin,
		       (columns_count_*columns_width_) + rows_header_width_,
		       columns_header_height_);
}

/**
	@brief BorderTitleBlock::rowsRect
	@return The rows rect in scene coordinate.
	If row is not displayed, return a null QRectF
*/
QRectF BorderTitleBlock::rowsRect() const
{
	if (!display_rows_) return QRectF();
	return QRectF (Diagram::margin,
		       Diagram::margin,
		       rows_header_width_,
		       (rows_count_*rows_height_) + columns_header_height_);
}

/**
	@brief BorderTitleBlock::outsideBorderRect
	@return The rect of outside border (diagram with columns and rows)
	The rect is in scene coordinate
*/
QRectF BorderTitleBlock::outsideBorderRect() const
{
	return QRectF (Diagram::margin,
		       Diagram::margin,
		       (columns_width_*columns_count_) + rows_header_width_,
		       (rows_height_*rows_count_) + columns_header_height_);
}

/**
	@brief BorderTitleBlock::insideBorderRect
	@return The rect of the inside border, in other word, the drawing area.
	This method take care about if rows or columns are displayed or not.
	The rect is in scene coordinate
*/
QRectF BorderTitleBlock::insideBorderRect() const
{
	qreal left = Diagram::margin;
	qreal top  = Diagram::margin;
	qreal width  = columns_width_*columns_count_;
	qreal height = rows_height_*rows_count_;

	display_rows_ ? left += rows_header_width_ : width += rows_header_width_;
	display_columns_ ? top += columns_header_height_ : height += columns_header_height_;

	return QRectF (left, top, width, height);
}

/**
	@brief BorderTitleBlock::titleBlockToXml
	Exports the title block current values to XML.
	@param xml_elmt the XML element attributes will be added to
*/
void BorderTitleBlock::titleBlockToXml(QDomElement &xml_elmt) {
	exportTitleBlock().toXml(xml_elmt);
}

/**
	@brief BorderTitleBlock::titleBlockFromXml
	Reads the title block values from XML.
	@param xml_elmt the XML element values will be read from
*/
void BorderTitleBlock::titleBlockFromXml(const QDomElement &xml_elmt) {
	TitleBlockProperties tbp;
	tbp.fromXml(xml_elmt);
	importTitleBlock(tbp);
}

/**
	@brief BorderTitleBlock::borderToXml
	Exports the border current settings to XML.
	@param xml_elmt the XML element attributes will be added to
*/
void BorderTitleBlock::borderToXml(QDomElement &xml_elmt) {
	// Use BorderProperties::toXml to save all properties including calculated dimensions
	BorderProperties bp = exportBorder();
	bp.toXml(xml_elmt);
	
	// attribut datant de la version 0.1 - laisse pour retrocompatibilite
	xml_elmt.setAttribute("height", QString("%1").arg(diagramHeight()));
}

/**
	@brief BorderTitleBlock::borderFromXml
	Reads the border settings from XML.
	@param xml_elmt the XML element values will be read from
*/
void BorderTitleBlock::borderFromXml(const QDomElement &xml_elmt) {
	// Handle backward compatibility for old files that only have "height" attribute
	if (!xml_elmt.hasAttribute("rows") && !xml_elmt.hasAttribute("rowsize") && xml_elmt.hasAttribute("height")) {
		// Legacy file format (0.1 version) - use old method
		bool ok;
		int cols_count = xml_elmt.attribute("cols").toInt(&ok);
		if (ok) setColumnsCount(cols_count);

		double cols_width = xml_elmt.attribute("colsize").toDouble(&ok);
		if (ok) setColumnsWidth(cols_width);

		double height = xml_elmt.attribute("height").toDouble(&ok);
		if (ok) setDiagramHeight(height);

		displayColumns(xml_elmt.attribute("displaycols") != "false");
		displayRows(xml_elmt.attribute("displayrows") != "false");

		updateRectangles();
		return;
	}
	
	// Modern file format - use BorderProperties::fromXml to load all properties including calculated dimensions
	// Make a non-const copy since fromXml requires a non-const reference
	QDomElement xml_copy = xml_elmt;  // QDomElement assignment creates a copy
	BorderProperties bp;
	bp.fromXml(xml_copy);
	
	// Import the full BorderProperties
	importBorder(bp);
}

/**
	@brief BorderTitleBlock::exportTitleBlock
	@return the properties of the titleblock
	\~French les proprietes du cartouches
*/
TitleBlockProperties BorderTitleBlock::exportTitleBlock()
{
	TitleBlockProperties ip;

	ip.author = author();
	ip.date = date();
	ip.title = title();
	ip.filename = fileName();
	ip.plant = plant();
	ip.locmach = locmach();
	ip.indexrev = indexrev();
	ip.version = version();
	ip.folio = folio();
	ip.template_name = titleBlockTemplateName();
	ip.display_at = m_edge;
	ip.auto_page_num = autoPageNum();
	ip.context = additional_fields_;
	ip.collection = QET::QetCollection::Embedded;

	return(ip);
}

/**
	@brief BorderTitleBlock::importTitleBlock
	@param ip the new properties of titleblock
*/
void BorderTitleBlock::importTitleBlock(const TitleBlockProperties &ip)
{
	btb_author_ = ip.author;
	btb_date_ = ip.date;
	btb_filename_ = ip.filename;
	btb_plant_ = ip.plant;
	btb_locmach_ = ip.locmach;
	btb_indexrev_ = ip.indexrev;
	btb_version_ = QetVersion::displayedVersion();
	btb_auto_page_num_ = ip.auto_page_num;
	additional_fields_ = ip.context;
	btb_title_ = ip.title;

	setFolio(ip.folio);

	if (m_edge != ip.display_at)
	{
		m_edge = ip.display_at;
		emit(displayChanged());
	}

	emit(needFolioData()); // Note: we expect additional data to be provided
	// through setFolioData(),
	// which in turn calls updateDiagramContextForTitleBlock().
	emit(needTitleBlockTemplate(ip.template_name));
	emit informationChanged();
}

/**
	@brief BorderTitleBlock::exportBorder
	@return border properties
	\~French les proprietes de la bordure
*/
BorderProperties BorderTitleBlock::exportBorder()
{
	BorderProperties bp;
	bp.columns_count = columnsCount();
	bp.columns_width = columnsWidth();
	bp.columns_header_height = columnsHeaderHeight();
	bp.display_columns = columnsAreDisplayed();
	bp.rows_count = rowsCount();
	bp.rows_height = rowsHeight();
	bp.rows_header_width = rowsHeaderWidth();
	bp.display_rows = rowsAreDisplayed();
	bp.border_all_sides = border_all_sides_;
	bp.header_line_thickness = header_line_thickness_;
	bp.header_thickness = rowsHeaderWidth();
	
	// Calculate and preserve aspect_ratio and base_area from current dimensions
	// so they can be saved to XML and used for future dimension calculations
	qreal drawing_width = bp.columns_count * bp.columns_width;
	qreal drawing_height = bp.rows_count * bp.rows_height;
	qreal calc_base_area = drawing_width * drawing_height;
	qDebug() << "[BorderTitleBlock::exportBorder] Recalculating base_area from dimensions:"
	         << "(" << drawing_width << "x" << drawing_height << "=" << calc_base_area << ")";
	bp.base_area = calc_base_area;
	if (drawing_height > 0.0) {
		bp.aspect_ratio = drawing_width / drawing_height;
	} else {
		bp.aspect_ratio = 1.0;
	}
	bp.scale = 1.0;  // Default scale, can be adjusted if needed
	bp.use_calculated_dimensions = true;  // Enable calculated dimensions mode
	
	return(bp);
}

/**
	@brief BorderTitleBlock::importBorder
	@param bp : the new properties of the border
	\~French les nouvelles proprietes de la bordure
*/
void BorderTitleBlock::importBorder(const BorderProperties &bp) {
	// Store a copy to work with
	BorderProperties working_bp = bp;
    qDebug() << "[BorderTitleBlock::importBorder] incoming:" 
             << "cols" << working_bp.columns_count 
             << "rows" << working_bp.rows_count 
             << "use_calc" << working_bp.use_calculated_dimensions 
             << "aspect" << working_bp.aspect_ratio 
             << "base" << working_bp.base_area 
             << "scale" << working_bp.scale 
             << "hdr" << working_bp.rows_header_width << working_bp.columns_header_height;
    
    // Apply header thickness immediately (affects layout bounds)
    if (working_bp.header_thickness > 0.0) {
        setRowsHeaderWidth(working_bp.header_thickness);
        setColumnsHeaderHeight(working_bp.header_thickness);
    } else {
        setColumnsHeaderHeight(working_bp.columns_header_height);
        setRowsHeaderWidth(working_bp.rows_header_width);
    }
    
    // Apply counts first
    setColumnsCount(working_bp.columns_count);
    setRowsCount(working_bp.rows_count);

    // If using calculated dimensions, (re)calculate based on aspect/base/scale and counts
    if (working_bp.use_calculated_dimensions) {
        working_bp.calculateDimensions();
        setColumnsWidth(working_bp.columns_width);
        setRowsHeight(working_bp.rows_height);
    } else {
        setColumnsWidth(working_bp.columns_width);
        setRowsHeight(working_bp.rows_height);
    }
    
    displayColumns(working_bp.display_columns);
    displayRows(working_bp.display_rows);
    border_all_sides_ = working_bp.border_all_sides;
    header_line_thickness_ = working_bp.header_line_thickness;
    qDebug() << "[BorderTitleBlock::importBorder] applied:" 
             << "columns_width_" << columns_width_ 
             << "rows_height_" << rows_height_ 
             << "rows_header_width_" << rows_header_width_ 
             << "columns_header_height_" << columns_header_height_;
}

/**
	@brief BorderTitleBlock::titleBlockTemplate
	@return the titleblock template used to render the titleblock
	@see TitleBlockTemplateRenderer::titleBlockTemplate()
*/
const TitleBlockTemplate *BorderTitleBlock::titleBlockTemplate()
{
	return(m_titleblock_template_renderer -> titleBlockTemplate());
}

/**
	@brief BorderTitleBlock::setTitleBlockTemplate
	@param titleblock_template:
	The new titleblock template to use to render the titleblock
	@see TitleBlockTemplateRenderer::setTitleBlockTemplate()
*/
void BorderTitleBlock::setTitleBlockTemplate(
		const TitleBlockTemplate *titleblock_template) {
	m_titleblock_template_renderer -> setTitleBlockTemplate(
				titleblock_template);
}

/**
	@brief BorderTitleBlock::titleBlockTemplateName
	@return The name of the template used to render the titleblock.
*/
QString BorderTitleBlock::titleBlockTemplateName() const
{
	QString tbt_name = m_titleblock_template_renderer -> titleBlockTemplate() -> name();
	return((tbt_name == "default") ? "" : tbt_name);
}

/**
	@brief BorderTitleBlock::titleBlockTemplateChanged
	This slot may be used to inform this class that the given title block
	template has changed.
	The title block-dedicated rendering cache will thus be flushed.
	@param template_name : Name of the title block template that has changed
*/
void BorderTitleBlock::titleBlockTemplateChanged(const QString &template_name) {
	if (titleBlockTemplateName() != template_name) return;
	m_titleblock_template_renderer -> invalidateRenderedTemplate();
}

/**
	@brief BorderTitleBlock::titleBlockTemplateRemoved
	This slot has to be used to inform this class that the given title block
	template is about to be removed and is no longer accessible. This class
	will either use the provided optional TitleBlockTemplate or the default
	title block provided by QETApp::defaultTitleBlockTemplate()
	@param removed_template_name :
	Name of the title block template that has changed
	@param new_template (Optional) title block template to use instead
*/
void BorderTitleBlock::titleBlockTemplateRemoved(
		const QString &removed_template_name,
		const TitleBlockTemplate *new_template) {
	if (titleBlockTemplateName() != removed_template_name) return;

	if (new_template) {
		setTitleBlockTemplate(new_template);
	} else {
		setTitleBlockTemplate(QETApp::defaultTitleBlockTemplate());
	}
}

/**
	@brief BorderTitleBlock::displayTitleBlock
	@param di : true to display the title block, false otherwise
	\~French true pour afficher le cartouche, false sinon
*/
void BorderTitleBlock::displayTitleBlock(bool di) {
	bool change = (di != display_titleblock_);
	display_titleblock_ = di;
	if (change) emit(displayChanged());
}

/**
	@brief BorderTitleBlock::displayColumns
	@param dc : true to display the column headers, false otherwise
	\~French true pour afficher les entetes des colonnes, false sinon
*/
void BorderTitleBlock::displayColumns(bool dc) {
	bool change = (dc != display_columns_);
	display_columns_ = dc;
	if (change) emit(displayChanged());
}

/**
	@brief BorderTitleBlock::displayRows
	@param dr : true to display line headers, false otherwise
	\~French true pour afficher les entetes des lignes, false sinon
*/
void BorderTitleBlock::displayRows(bool dr) {
	bool change = (dr != display_rows_);
	display_rows_ = dr;
	if (change) emit(displayChanged());
}

/**
	@brief BorderTitleBlock::displayBorder
	@param db : true to display the border of the diagram, false otherwise
	\~French true pour afficher la bordure du schema, false sinon
	\~ @note : if the border display is deactivated,
	the rows and columns will not be drawn.
	\~French si l'affichage de la bordure est ainsi desactivee,
	les lignes et colonnes ne seront pas dessinees.
*/
void BorderTitleBlock::displayBorder(bool db) {
	bool change = (db != display_border_);
	display_border_  = db;
	if (change) emit(displayChanged());
}

/**
	@brief BorderTitleBlock::updateRectangles
	This method update the diagram rect according
	to the value of rows and columns (number and size)
*/
void BorderTitleBlock::updateRectangles()
{
	QRectF previous_diagram = diagram_rect_;
	diagram_rect_ = QRectF(Diagram::margin,
			       Diagram::margin,
			       diagramWidth(),
			       diagramHeight());
	if (diagram_rect_ != previous_diagram)
		emit(borderChanged(previous_diagram, diagram_rect_));
}

/**
	@brief BorderTitleBlock::draw
	Draw the border and the titleblock.
	@param painter : QPainter to use for draw this.
*/
void BorderTitleBlock::draw(QPainter *painter)
{
	//Set the QPainter
	painter -> save();
	QPen borderPen(Qt::black);
	painter -> setPen(borderPen);
	painter -> setBrush(Qt::NoBrush);

	QSettings settings;

	// Shared separators to avoid overlaps at grid edges
	qreal separatorX = diagram_rect_.topLeft().x() + rows_header_width_ + (columns_count_ * columns_width_);
	separatorX = qRound(separatorX);
	qreal separatorY = diagram_rect_.topLeft().y() + columns_header_height_ + (rows_count_ * rows_height_);
	separatorY = qRound(separatorY);

	//Draw the border
	qreal halfPen = 0.0; // Initialize halfPen for use in header positioning
	qreal borderPenWidth = 1.0; // Default border pen width for header positioning
	qreal borderBottomY = 0.0; // Store bottom border Y coordinate for header alignment
	if (display_border_) {
		if (border_all_sides_) {
			// Draw all 4 sides around the diagram rect
			// Title block is positioned inside, so border is drawn around diagram_rect_ only
			qreal x = diagram_rect_.x();
			qreal y = diagram_rect_.y();
			qreal w = diagram_rect_.width();
			qreal h = diagram_rect_.height();
			
			// Draw all 4 sides to form a complete border
			// Offset right and bottom edges outward by half the pen width (use 0.5px for cosmetic pens)
			borderPenWidth = (borderPen.widthF() > 0.0) ? borderPen.widthF() : 1.0;
			halfPen = borderPenWidth / 2.0;
			borderBottomY = y + h - halfPen; // Store exact Y coordinate of bottom border (centered on pen)
			painter -> drawLine(x, y, x + w, y);                                  // Top edge (inside)
			painter -> drawLine(x + w + halfPen, y, x + w + halfPen, y + h);      // Right edge (outside)
			painter -> drawLine(x + w + halfPen, borderBottomY, x, borderBottomY); // Bottom edge (outside)
			painter -> drawLine(x, borderBottomY, x, y);                        // Left edge (inside)
		} else {
			// Draw left, top, and right edges (but not bottom to allow title block to extend outside)
			qreal x = diagram_rect_.x();
			qreal y = diagram_rect_.y();
			qreal w = diagram_rect_.width();
			qreal h = diagram_rect_.height();
			
			// Offset right edge outward by half the pen width (use 0.5px for cosmetic pens)
			borderPenWidth = (borderPen.widthF() > 0.0) ? borderPen.widthF() : 1.0;
			halfPen = borderPenWidth / 2.0;
			
			painter -> drawLine(x, y, x + w, y);                      // Top edge
			painter -> drawLine(x, y, x, y + h);                      // Left edge
			painter -> drawLine(x + w + halfPen, y, x + w + halfPen, y + h);  // Right edge (outside)
		}
	}

	painter -> setFont(QETApp::diagramTextsFont());

	// Prepare header pen for header cells/corners
	QPen headerPen(Qt::black);
	headerPen.setWidthF(header_line_thickness_);

	//Draw the empty case at the corners of diagram when there is header
	if (display_border_ && (display_columns_ || display_rows_))
	{
		painter->setPen(headerPen);
		// Top-left corner - only draw outer borders (top and left), skip internal borders
		qreal corner_x = diagram_rect_.topLeft().x();
		qreal corner_y = diagram_rect_.topLeft().y();
		qreal corner_w = rows_header_width_;
		qreal corner_h = columns_header_height_;
		
		// Draw only top and left borders (outer borders)
		// Bottom and right borders will be drawn by adjacent header cells
		painter->drawLine(corner_x, corner_y, corner_x + corner_w, corner_y); // Top
		painter->drawLine(corner_x, corner_y, corner_x, corner_y + corner_h); // Left
		
		// Draw corner rectangles at all four corners when border on all sides
		if (border_all_sides_) {
			// Bottom-left corner - only draw outer borders
			qreal bl_x = diagram_rect_.bottomLeft().x();
			// Position corner to align with the title block's bottom border
			qreal bl_y = separatorY - header_line_thickness_;
			qreal bl_w = rows_header_width_;
			qreal bl_h = columns_header_height_;
			
			// Draw only top and left borders (outer borders)
			painter->drawLine(bl_x, bl_y, bl_x + bl_w, bl_y); // Top
			painter->drawLine(bl_x, bl_y, bl_x, bl_y + bl_h); // Left
			
			// Top-right corner - only draw outer borders
			qreal tr_x = separatorX;
			qreal tr_y = diagram_rect_.topRight().y();
			qreal tr_w = rows_header_width_;
			qreal tr_h = columns_header_height_;
			
			// Draw only top and right borders (outer borders)
			painter->drawLine(tr_x, tr_y, tr_x + tr_w, tr_y); // Top
			painter->drawLine(tr_x + tr_w, tr_y, tr_x + tr_w, tr_y + tr_h); // Right
			
			// Bottom-right corner - only draw outer borders
			qreal br_x = separatorX;
			// Position corner to align with the title block's bottom border
			qreal br_y = separatorY - header_line_thickness_;
			qreal br_w = rows_header_width_;
			qreal br_h = columns_header_height_;
			
			// Draw only top and right borders (outer borders)
			painter->drawLine(br_x, br_y, br_x + br_w, br_y); // Top
			painter->drawLine(br_x + br_w, br_y, br_x + br_w, br_y + br_h); // Right
		}
		// Restore border pen for non-header drawing if needed later
		painter->setPen(borderPen);
	}

		//Draw the nums of columns (top and bottom)
	if (display_border_ && display_columns_) {
		painter->setPen(headerPen);
		// Shared separator X between last column and right row header
		qreal separatorX = qRound(diagram_rect_.topLeft().x() + rows_header_width_ + (columns_count_ * columns_width_));
		for (int i = 1 ; i <= columns_count_ ; ++ i) {
			// Top columns
			qreal cell_x = diagram_rect_.topLeft().x()
					+ (rows_header_width_
					   + ((i - 1) * columns_width_));
			qreal cell_y = diagram_rect_.topLeft().y();
			qreal cell_w = columns_width_;
			qreal cell_h = columns_header_height_;
			// Clamp last column header width to remaining space to avoid overlap
			if (i == columns_count_) {
				qreal startX = diagram_rect_.topLeft().x() + rows_header_width_ + ((i - 1) * columns_width_);
				cell_x = startX;
				cell_w = separatorX - startX;
			}
			
			// Draw borders individually to avoid overlapping shared lines
			QRectF numbered_rectangle(cell_x, cell_y, cell_w, cell_h);
			qreal x = numbered_rectangle.x();
			qreal y = numbered_rectangle.y();
			qreal w = numbered_rectangle.width();
			qreal h = numbered_rectangle.height();
			
			// Draw top border for all cells
			painter->drawLine(x, y, x + w, y);
			// Draw bottom border for all cells
			painter->drawLine(x, y + h, x + w, y + h);
			// Draw right border for all cells (shared vertical lines are only drawn once as right border)
			painter->drawLine(x + w, y, x + w, y + h);
			// Draw left border only for first column (others are the right border of previous cell)
			if (i == 1) {
				painter->drawLine(x, y, x, y + h);
			}
			if (settings.value("border-columns_0", true).toBool()){
			painter -> drawText(numbered_rectangle,
					    Qt::AlignVCenter
					    | Qt::AlignCenter,
					    QString("%1").arg(i - 1));
			}else{
			painter -> drawText(numbered_rectangle,
					    Qt::AlignVCenter
					    | Qt::AlignCenter,
					    QString("%1").arg(i));
			}
			
			// Bottom columns (when border on all sides)
			if (border_all_sides_) {
				qreal bottom_cell_x = diagram_rect_.topLeft().x()
						+ (rows_header_width_
						   + ((i - 1) * columns_width_));
				// Position header so its top border line aligns with the title block's bottom border
				// Title block bottom is at: separatorY - header_line_thickness_
				// The title block's bottom border line is drawn at that position
				// Position header so its top border aligns with the title block's bottom border
				qreal bottom_cell_y = separatorY - header_line_thickness_;
				qreal bottom_cell_w = columns_width_;
				qreal bottom_cell_h = columns_header_height_;
				// Clamp last column header width to remaining space to avoid overlap
				if (i == columns_count_) {
					qreal startX = diagram_rect_.topLeft().x() + rows_header_width_ + ((i - 1) * columns_width_);
					bottom_cell_x = startX;
					bottom_cell_w = separatorX - startX;
				}
				
				// Draw borders individually to avoid overlapping shared lines
				qreal bx = bottom_cell_x;
				qreal by = bottom_cell_y;
				qreal bw = bottom_cell_w;
				qreal bh = bottom_cell_h;
				
				QRectF bottom_numbered_rectangle(bx, by, bw, bh);
				
				// Draw top border for all cells
				painter->drawLine(bx, by, bx + bw, by);
				// Draw bottom border for all cells
				painter->drawLine(bx, by + bh, bx + bw, by + bh);
				// Draw right border for all cells (shared vertical lines are only drawn once as right border)
				painter->drawLine(bx + bw, by, bx + bw, by + bh);
				// Draw left border only for first column (others are the right border of previous cell)
				if (i == 1) {
					painter->drawLine(bx, by, bx, by + bh);
				}
				if (settings.value("border-columns_0", true).toBool()){
				painter -> drawText(bottom_numbered_rectangle,
						    Qt::AlignVCenter
						    | Qt::AlignCenter,
						    QString("%1").arg(i - 1));
				}else{
				painter -> drawText(bottom_numbered_rectangle,
						    Qt::AlignVCenter
						    | Qt::AlignCenter,
						    QString("%1").arg(i));
				}
			}
		}
		painter->setPen(borderPen);
	}

		//Draw the nums of rows (left and right)
	if (display_border_ && display_rows_) {
		painter->setPen(headerPen);
		QString row_string("A");
		for (int i = 1 ; i <= rows_count_ ; ++ i) {
			// Left rows
			qreal cell_x = diagram_rect_.topLeft().x();
			qreal cell_y = diagram_rect_.topLeft().y()
					+ (
						columns_header_height_
						+ ((i - 1)* rows_height_)
						);
			qreal cell_w = rows_header_width_;
			qreal cell_h = rows_height_;
			// Clamp last row header height to remaining space to avoid overlap
			if (i == rows_count_) {
				qreal startY = diagram_rect_.topLeft().y() + columns_header_height_ + ((i - 1) * rows_height_);
				qreal endY = diagram_rect_.bottomLeft().y();
				cell_y = startY;
				cell_h = endY - startY;
			}
			
			// Draw borders individually to avoid overlapping shared lines
			QRectF lettered_rectangle(cell_x, cell_y, cell_w, cell_h);
			qreal x = lettered_rectangle.x();
			qreal y = lettered_rectangle.y();
			qreal w = lettered_rectangle.width();
			qreal h = lettered_rectangle.height();
			
			// Draw left border for all cells
			painter->drawLine(x, y, x, y + h);
			// Draw right border for all cells
			painter->drawLine(x + w, y, x + w, y + h);
			// Draw bottom border for all cells (shared horizontal lines are only drawn once as bottom border)
			painter->drawLine(x, y + h, x + w, y + h);
			// Draw top border only for first row (others are the bottom border of previous row)
			if (i == 1) {
				painter->drawLine(x, y, x + w, y);
			}
			painter -> drawText(lettered_rectangle,
					    Qt::AlignVCenter
					    | Qt::AlignCenter,
					    row_string);
			
			// Right rows (when border on all sides)
			if (border_all_sides_) {
				qreal right_cell_x = separatorX;
				qreal right_cell_y = diagram_rect_.topLeft().y()
						+ (
							columns_header_height_
							+ ((i - 1)* rows_height_)
							);
				qreal right_cell_w = rows_header_width_;
				qreal right_cell_h = rows_height_;
				// Clamp last row header height to remaining space to avoid overlap
				if (i == rows_count_) {
					qreal startY = diagram_rect_.topLeft().y() + columns_header_height_ + ((i - 1) * rows_height_);
					qreal endY = diagram_rect_.bottomLeft().y();
					right_cell_y = startY;
					right_cell_h = endY - startY;
				}
				
				// Draw borders individually to avoid overlapping shared lines
				qreal rx = right_cell_x;
				qreal ry = right_cell_y;
				qreal rw = right_cell_w;
				qreal rh = right_cell_h;
				
				QRectF right_lettered_rectangle(rx, ry, rw, rh);
				
				// Draw left border for all cells
				painter->drawLine(rx, ry, rx, ry + rh);
				// Draw right border for all cells
				painter->drawLine(rx + rw, ry, rx + rw, ry + rh);
				// Draw bottom border for all cells (shared horizontal lines are only drawn once as bottom border)
				painter->drawLine(rx, ry + rh, rx + rw, ry + rh);
				// Draw top border only for first row (others are the bottom border of previous row)
				if (i == 1) {
					painter->drawLine(rx, ry, rx + rw, ry);
				}
				painter -> drawText(right_lettered_rectangle,
						    Qt::AlignVCenter
						    | Qt::AlignCenter,
						    row_string);
			}
			
			row_string = incrementLetters(row_string);
		}
		painter->setPen(borderPen);
	}

		// render the titleblock, using the TitleBlockTemplate object
	if (display_titleblock_) {
		QRectF tbt_rect = titleBlockRectForQPainter();
		// When display_rows_ is true, hide the leftmost border of the title block
		bool hide_left_border = display_rows_;
		if (m_edge == Qt::BottomEdge)
		{
			painter -> translate(tbt_rect.topLeft());
			m_titleblock_template_renderer -> render(
						painter,
						tbt_rect.width(),
						hide_left_border);
			painter -> translate(-tbt_rect.topLeft());
		}
		else
		{
			painter->translate(tbt_rect.topLeft());
			painter->rotate(-90);
			// For rotated title blocks, the concept of "leftmost" depends on rotation
			// For now, we apply the same logic
			m_titleblock_template_renderer -> render(
						painter,
						tbt_rect.width(),
						hide_left_border);
			painter->rotate(90);
			painter -> translate(-tbt_rect.topLeft());
		}
	}

	painter -> restore();
}

/**
	@brief BorderTitleBlock::drawDxf
	@param file_path
	@param color
*/
void BorderTitleBlock::drawDxf(
		QString &file_path,
		int color)
{
	// Transform to DXF scale.
	columns_header_height_ *= Createdxf::yScale;
	rows_height_           *= Createdxf::yScale;
	rows_header_width_     *= Createdxf::xScale;
	columns_width_         *= Createdxf::xScale;

	// draw the empty box that appears as soon as there is a header
	// dessine la case vide qui apparait des qu'il y a un entete
	if (display_border_ &&
		(display_columns_ ||
		 display_rows_)
		) {
		Createdxf::drawRectangle(
			file_path,
			double(diagram_rect_.topLeft().x()) * Createdxf::xScale,
			Createdxf::sheetHeight
					- double(diagram_rect_.topLeft().y())
					* Createdxf::yScale
					- columns_header_height_,
			rows_header_width_,
			columns_header_height_,
			color
		);
	}

	QSettings settings;

	// draw the numbering of the columns
	// dessine la numerotation des colonnes
	if (display_border_ &&
		display_columns_) {
	int offset = settings.value("border-columns_0", true).toBool() ? -1 : 0;
		for (int i = 1 ; i <= columns_count_ ; ++ i) {
	    double xCoord = diagram_rect_.topLeft().x() * Createdxf::xScale +
					(rows_header_width_ + ((i - 1) *
					 columns_width_));
			double yCoord = Createdxf::sheetHeight
		    - diagram_rect_.topLeft().y()*Createdxf::yScale
					- columns_header_height_;
			double recWidth = columns_width_;
			double recHeight = columns_header_height_;
			Createdxf::drawRectangle(file_path, xCoord, yCoord,
						 recWidth, recHeight, color);

			Createdxf::drawTextAligned(file_path,
			   QString::number(i + offset),
			   xCoord+recWidth/4,
			   yCoord + recHeight*0.5,
						   recHeight*0.7,
						   0,
						   0,
						   1,
						   2,
			   xCoord+recWidth/2,
			   1,
			   color);
	}
	}

	// draw line numbering
	// dessine la numerotation des lignes
	if (display_border_ && display_rows_) {
		QString row_string("A");
		for (int i = 1 ; i <= rows_count_ ; ++ i) {
			double xCoord = diagram_rect_.topLeft().x()
					* Createdxf::xScale;
	    double yCoord = Createdxf::sheetHeight
		    - diagram_rect_.topLeft().y()
					*Createdxf::yScale
					- (
						columns_header_height_
						+ ((i - 1)
						   * rows_height_) )
					- rows_height_;
			double recWidth = rows_header_width_;
			double recHeight = rows_height_;
			Createdxf::drawRectangle(file_path, xCoord, yCoord,
						 recWidth, recHeight, color);
			Createdxf::drawTextAligned(file_path,
						   row_string,
			   xCoord+recWidth*0.1,
			   yCoord + recHeight*0.4,
						   recWidth*0.7,
						   0,
						   0,
						   1,
						   2,
						   xCoord+recWidth/2,
			   1,
			   color);
			row_string = incrementLetters(row_string);
		}
	}

	// render the titleblock, using the TitleBlockTemplate object
	if (display_titleblock_) {
		//qp -> translate(titleblock_rect_.topLeft());
		QRectF rect = titleBlockRect();
		m_titleblock_template_renderer -> renderDxf(rect,
							    rect.width(),
							    file_path,
							    color);
		//qp -> translate(-titleblock_rect_.topLeft());
	}

	// Transform back to QET scale
	columns_header_height_ /= Createdxf::yScale;
	rows_height_		   /= Createdxf::yScale;
	rows_header_width_     /= Createdxf::xScale;
	columns_width_         /= Createdxf::xScale;

}

/**
	@brief BorderTitleBlock::setColumnsCount
	Allows you to change the number of columns.
	If this number of columns is less than the minimum required,
	it is this minimum which is used.

	\~French Permet de changer le nombre de colonnes.
	Si ce nombre de colonnes est inferieur au minimum requis,
	c'est ce minimum qui est utilise.
	\~ @param nb_c : new number of columns
	\~French nouveau nombre de colonnes
	\~ @see minNbColumns()
*/
void BorderTitleBlock::setColumnsCount(int nb_c) {
	if (nb_c == columnsCount()) return;
	columns_count_ = qMax(MIN_COLUMN_COUNT , nb_c);
	updateRectangles();
}

/**
	@brief BorderTitleBlock::setColumnsWidth
	Change the width of the columns.
	If the width indicated is less than the minimum required,
	it is this minimum which is used.

	\~French Change la largeur des colonnes.
	Si la largeur indiquee est inferieure au minimum requis,
	c'est ce minimum qui est utilise.
	\~ @param new_cw : new column width
	\~French nouvelle largeur des colonnes
	\~ @see minColumnsWidth()
*/
void BorderTitleBlock::setColumnsWidth(const qreal &new_cw) {
	if (new_cw == columnsWidth()) return;
	columns_width_ = qMax(MIN_COLUMN_WIDTH , new_cw);
	updateRectangles();
}

/**
	@brief BorderTitleBlock::setColumnsHeaderHeight
	Change the height of the headers containing the column numbers.
	This must remain between 5 and 50 px.

	\~French Change la hauteur des en-tetes contenant les numeros de colonnes.
	Celle-ci doit rester comprise entre 5 et 50 px.
	\~ @param new_chh : new height of column headers
	\~French nouvelle hauteur des en-tetes de colonnes
*/
void BorderTitleBlock::setColumnsHeaderHeight(const qreal &new_chh) {
	columns_header_height_ = qBound(qreal(5.0), new_chh, qreal(50.0));
	updateRectangles();
}

/**
	@brief BorderTitleBlock::setRowsCount
	Allows you to change the number of lines.
	If this number of lines is lower than the minimum required,
	this function does nothing

	\~French Permet de changer le nombre de lignes.
	Si ce nombre de lignes est inferieur au minimum requis,
	cette fonction ne fait rien
	\~ @param nb_r : new number of lines
	\~French nouveau nombre de lignes
	\~ @see minNbRows()
*/
void BorderTitleBlock::setRowsCount(int nb_r) {
	if (nb_r == rowsCount()) return;
	rows_count_ = qMax(MIN_ROW_COUNT, nb_r);
	updateRectangles();
}

/**
	@brief BorderTitleBlock::setRowsHeight
	Change the height of the lines.
	If the indicated height is lower than the minimum required,
	it is this minimum which is used.

	\~French Change la hauteur des lignes.
	Si la hauteur indiquee est inferieure au minimum requis,
	c'est ce minimum qui est utilise.
	\~ @param new_rh : new row height
	\~French nouvelle hauteur des lignes
	\~ @see minRowsHeight()
*/
void BorderTitleBlock::setRowsHeight(const qreal &new_rh) {
	if (new_rh == rowsHeight()) return;
	rows_height_ = qMax(MIN_ROW_HEIGHT, new_rh);
	updateRectangles();
}

/**
	@brief BorderTitleBlock::setRowsHeaderWidth
	Change the width of the headers containing the line numbers.
	This must remain between 5 and 50 px.

	\~French Change la largeur des en-tetes contenant les numeros de lignes.
	Celle-ci doit rester comprise entre 5 et 50 px.
	\~ @param new_rhw : new width of line headers
	\~French nouvelle largeur des en-tetes des lignes
*/
void BorderTitleBlock::setRowsHeaderWidth(const qreal &new_rhw) {
	rows_header_width_ = qBound(qreal(5.0), new_rhw, qreal(50.0));
	updateRectangles();
}

/**
	@brief BorderTitleBlock::setDiagramHeight
	This method tries to get as close as possible to the given height
	as a parameter by modifying the number of rows in progress.

	\~French Cette methode essaye de se rapprocher le plus possible de la
	hauteur donnee en parametre en modifiant le nombre de lignes en cours.
	\~ @param height :
*/
void BorderTitleBlock::setDiagramHeight(const qreal &height) {
	//          size of rows to use = rows_height
	// taille des lignes a utiliser = rows_height
	setRowsCount(qRound(ceil(height / rows_height_)));
}

/**
	@brief BorderTitleBlock::convertPosition
	Convert a Point in cartesian coordinate (x : 12.5, 56.9)
	to a point in grid coordinate (ex : B2)
	@param pos : position to convert
	@return the converted point in grid coordinate.
*/
DiagramPosition BorderTitleBlock::convertPosition(const QPointF &pos)
{
	if(!insideBorderRect().contains(pos))
		return (DiagramPosition("", 0));

	QPointF relative_pos = pos - insideBorderRect().topLeft();
	int row_number    = int(ceil(relative_pos.x() / columnsWidth()));
	int column_number = int(ceil(relative_pos.y() / rowsHeight()));

	QString letter = "A";
	for (int i = 1 ; i < column_number ; ++ i)
		letter = incrementLetters(letter);

	return(DiagramPosition(letter, row_number));
}

/**
	@brief BorderTitleBlock::setFolio
	@param folio the new value of the "Folio" field
*/
void BorderTitleBlock::setFolio(const QString &folio)
{
	btb_folio_ = folio;
	emit (titleBlockFolioChanged(folio));
}

/**
	@brief BorderTitleBlock::updateDiagramContextForTitleBlock
	Update the informations given to the titleblock template
	by regenerating a DiagramContext object.
	@param initial_context :
	Base diagram context that will be overridden by diagram-wide values
*/
void BorderTitleBlock::updateDiagramContextForTitleBlock(
		const DiagramContext &initial_context) {
	// Our final DiagramContext is the initial one (which is supposed to bring
	// project-wide properties), overridden by the "additional fields" one...
	DiagramContext context = initial_context;
	foreach (QString key, additional_fields_.keys()) {
		context.addValue(key, additional_fields_[key]);
	}

	// ... overridden by the historical and/or dynamically generated fields
	QLocale var;
	var.dateFormat(QLocale::ShortFormat);
	context.addValue("author",      btb_author_);
	context.addValue(
		"date",
		QLocale::system().toString(btb_date_, QLocale::ShortFormat));
	context.addValue("title",       btb_title_);
	context.addValue("filename",    btb_filename_);
	context.addValue("plant",     btb_plant_);
	context.addValue("locmach",     btb_locmach_);
	context.addValue("indexrev",    btb_indexrev_);
	context.addValue("version",     btb_version_);
	context.addValue("folio",       btb_final_folio_);
	context.addValue("folio-id",    folio_index_);
	context.addValue("folio-total", folio_total_);
	context.addValue("auto_page_num", btb_auto_page_num_);
	context.addValue("previous-folio-num", m_previous_folio_num);
	context.addValue("next-folio-num", m_next_folio_num);

	m_titleblock_template_renderer -> setContext(context);
}

/**
	@brief BorderTitleBlock::incrementLetters
	increments string with Letters A to Z
	@param string
	@return string ++Letters
	eg:
	- A-> B
	- Z -> AA
*/
QString BorderTitleBlock::incrementLetters(const QString &string) {
	if (string.isEmpty()) {
		return("A");
	} else {
		// separate previous digits from last digit
		// separe les digits precedents du dernier digit
		QString first_digits(string.left(string.length() - 1));
		char last_digit(string.at(string.length() - 1).unicode());
		if (last_digit != 'Z') {
			// increments the last digit
			// incremente le dernier digit
			last_digit = (char)(string[string.length()-1].unicode()) + 1;
			return(first_digits + QString(last_digit));
		} else {
			return(incrementLetters(first_digits) % "A");
		}
	}
}

/**
	@brief BorderTitleBlock::setFolioData

	\~ @param index : schema number (from 1 to total)
	\~French numero du schema (de 1 a total)

	\~ @param total : total number of diagrams in the project
	\~French nombre total de schemas dans le projet

	\~ @param autonum :

	\~ @param project_properties : Project-wide properties,
	to be merged with diagram-wide ones.
*/
void BorderTitleBlock::setFolioData(
		int index,
		int total,
		const QString& autonum,
		const DiagramContext &project_properties) {
	if (index < 1 || total < 1 || index > total) return;

	// memorize information
	// memorise les informations
	folio_index_ = index;
	folio_total_ = total;

	// regenerate the content of the folio field
	// regenere le contenu du champ folio
	btb_final_folio_ = btb_folio_;

	if (btb_final_folio_.contains("%autonum")){
		btb_final_folio_.replace("%autonum", autonum);
		btb_folio_ = btb_final_folio_;
	}
	btb_final_folio_.replace("%id",    QString::number(folio_index_));
	btb_final_folio_.replace("%total", QString::number(folio_total_));

	updateDiagramContextForTitleBlock(project_properties);
}

/**
	@brief BorderTitleBlock::setPreviousFolioNum
	@param previous the new value of the "previous-folio-num" field
*/
void BorderTitleBlock::setPreviousFolioNum(const QString &previous)
{
	m_previous_folio_num = previous;
	DiagramContext context = m_titleblock_template_renderer->context();
	context.addValue("previous-folio-num", m_previous_folio_num);
	m_titleblock_template_renderer->setContext(context);
}

/**
	@brief BorderTitleBlock::setNextFolioNum
	@param next the new value of the "next-folio-num" field
*/
void BorderTitleBlock::setNextFolioNum(const QString &next)
{
	m_next_folio_num = next;
	DiagramContext context = m_titleblock_template_renderer->context();
	context.addValue("next-folio-num", m_next_folio_num);
	m_titleblock_template_renderer->setContext(context);
}
