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
#ifndef BORDERTITLEBLOCK_H
#define BORDERTITLEBLOCK_H
#include "autoNum/numerotationcontext.h"
#include "borderproperties.h"
#include "diagramcontext.h"
#include "titleblockproperties.h"

#include <QDate>
#include <QObject>
#include <QRectF>
class QPainter;
class DiagramPosition;
class TitleBlockTemplate;
class TitleBlockTemplateRenderer;
/**
	@brief The BorderTitleBlock class
	This class represents the border and the titleblock which frame a
	particular electric diagram.
*/
class BorderTitleBlock : public QObject
{
		Q_OBJECT
	
	public:
		BorderTitleBlock(QObject * = nullptr);
		~BorderTitleBlock() override;
	
	private:
		BorderTitleBlock(const BorderTitleBlock &);
	
		//METHODS
	public:	
		void draw(QPainter *painter);
		void drawDxf(QString &, int);
	
		//METHODS TO GET DIMENSION
		//COLUMNS
		/// @return the number of columns
		int   columnsCount() const { return(columns_count_); }
		/// @return the columns width, in pixels (scaled, for canvas)
		qreal columnsWidth() const { return(columns_width_); }
		/// @return the total width of all columns, headers excluded (scaled, for canvas)
		qreal columnsTotalWidth() const {
			return(columns_count_ * columns_width_); }
		/// @return the base total width of all columns (base dimensions, for viewport/headers)
		qreal baseColumnsTotalWidth() const {
			return(columns_count_ * baseColumnsWidth()); }
		/// @return the column headers height, in pixels
		qreal columnsHeaderHeight() const {
			return(columns_header_height_); }
	
		//ROWS
		/// @return the number of rows
		int rowsCount() const { return(rows_count_); }
		/// @return the rows height, in pixels (scaled, for canvas)
		qreal rowsHeight() const { return(rows_height_); }
		/// @return the total height of all rows, headers excluded (scaled, for canvas)
		qreal rowsTotalHeight() const {
			return(rows_count_ * rows_height_); }
		/// @return the base total height of all rows (base dimensions, for viewport/headers)
		qreal baseRowsTotalHeight() const {
			return(rows_count_ * baseRowsHeight()); }
		/// @return la rows header width, in pixels
		qreal rowsHeaderWidth() const { return(rows_header_width_); }
	
		// border - title block = diagram
		/**
			@brief diagramWidth
			@return the diagram width,
			i.e. the width of the border without title block
			Uses base dimensions to match header boundaries (viewport)
		*/
		qreal diagramWidth() const
{
			return(baseColumnsTotalWidth() + rowsHeaderWidth()); }
		/**
			@brief diagramHeight
			@return the diagram height,
			i.e. the height of the border without title block
			Uses base dimensions to match header boundaries (viewport)
		*/
		qreal diagramHeight() const
{
			return(baseRowsTotalHeight() + columnsHeaderHeight()); }
		
		/**
		 * @brief Get the canvas scale factor
		 * Returns the factor by which canvas items should be visually scaled
		 * relative to headers. The scale value is directly proportional:
		 * - scale = 2.0 → items appear 2x larger (200%)
		 * - scale = 0.5 → items appear 0.5x smaller (50%)
		 * @return scale factor (directly uses scale_), or 1.0 if not applicable
		 */
		qreal canvasScaleFactor() const;

		QRectF titleBlockRect () const;

		DiagramContext titleblockInformation() const;
	private:
		QRectF titleBlockRectForQPainter () const;

	public:
		QRectF borderAndTitleBlockRect () const;
		QRectF columnsRect () const;
		QRectF rowsRect () const;
		QRectF outsideBorderRect() const;
		QRectF insideBorderRect() const;
	
		// methods to get title block basic data
		/// @return the value of the title block "Author" field
		QString author() const { return(btb_author_); }
		/// @return the value of the title block "Date" field
		QDate date() const { return(btb_date_); }
		/// @return the value of the title block "Title" field
		QString title() const { return(btb_title_); }
		/// @return the value of the title block "Folio" field
		QString folio() const { return(btb_folio_); }
		/// @return the value of the title block "Folio" field as displayed
		QString finalfolio() const { return(btb_final_folio_); }
		/// @return the value of the title block "Plant" field
		QString plant() const { return(btb_plant_); }
		/// @return the value of the title block "Locmach" field
		QString locmach() const { return(btb_locmach_); }
		/// @return the value of the revision index block "Folio" field
		QString indexrev() const { return(btb_indexrev_); }
		/// @return the value of the title block "File" field
		QString fileName() const { return(btb_filename_); }
		/// @return the value of the title block Additional Fields
		QString version() const { return(btb_version_); }
		/// @return the value of the title block Additional Fields
		DiagramContext additionalFields() const {
			return (additional_fields_); }
		/// @return the value of the title block
		QString autoPageNum() const { return(btb_auto_page_num_); }
		/// @return the value of the total number of folios
		int folioTotal() const { return(folio_total_);}
	
		// methods to get display options
		/// @return true si le cartouche est affiche, false sinon
		bool titleBlockIsDisplayed() const {
			return(display_titleblock_); }
		/// @return true si les entetes des colonnes sont affiches,
		/// false sinon
		bool columnsAreDisplayed() const { return(display_columns_); }
		/// @return true si les entetes des lignes sont affiches,
		/// false sinon
		bool rowsAreDisplayed() const { return(display_rows_); }
		/// @return true si la bordure est affichee, false sinon
		bool borderIsDisplayed() const { return(display_border_); }
		
		// methods to set dimensions
		void setColumnsCount(int);
		void setRowsCount(int);
		void setColumnsWidth(const qreal &);
		void setRowsHeight(const qreal &);
		void setColumnsHeaderHeight(const qreal &);
		void setRowsHeaderWidth(const qreal &);
		void setDiagramHeight(const qreal &);
		
		DiagramPosition convertPosition(const QPointF &);
		
		// methods to set title block basic data
		void setFolio(const QString &folio);
		void setFolioData(int, int, const QString& = nullptr,
				  const DiagramContext & = DiagramContext());
		void setPreviousFolioNum(const QString &previous);
		void setNextFolioNum(const QString &next);
		
		void titleBlockToXml(QDomElement &);
		void titleBlockFromXml(const QDomElement &);
		void borderToXml(QDomElement &);
		void borderFromXml(const QDomElement &);
		
		TitleBlockProperties exportTitleBlock();
		void importTitleBlock(const TitleBlockProperties &);
		BorderProperties exportBorder();
		void importBorder(const BorderProperties &);
		
		const TitleBlockTemplate *titleBlockTemplate();
		void setTitleBlockTemplate(const TitleBlockTemplate *);
		QString titleBlockTemplateName() const;
		
	public slots:
		void titleBlockTemplateChanged(const QString &);
		void titleBlockTemplateRemoved(
				const QString &,
				const TitleBlockTemplate * = nullptr);
		
		// methods to set display options
		void displayTitleBlock(bool);
		void displayColumns(bool);
		void displayRows(bool);
		void displayBorder(bool);
	
	private:
		void updateRectangles();
		void updateDiagramContextForTitleBlock(
				const DiagramContext & = DiagramContext());
		QString incrementLetters(const QString &);
		
		/**
		 * @brief Calculate base column width (with scale = 1.0)
		 * Used for drawing headers and titleblock which should not be affected by scale
		 */
		qreal baseColumnsWidth() const;
		
	/**
	 * @brief Calculate base row height (with scale = 1.0)
	 * Used for drawing headers and titleblock which should not be affected by scale
	 */
	qreal baseRowsHeight() const;
	
		signals:
		/**
			@brief borderChanged
			Signal emitted after the border has changed
			@param old_border Former border
			@param new_border New border
		*/
		void borderChanged(QRectF old_border, QRectF new_border);
		/**
			@brief displayChanged
			Signal emitted after display options have changed
		*/
		void displayChanged();
		/**
			@brief titleBlockFolioChanged
			Signal emitted after Folio has changed
		*/
		void titleBlockFolioChanged(const QString &);
		/**
			@brief needFolioData
			Signal emitted when the title block
			requires its data to be updated
			in order to generate the folio field.
		*/
		void needFolioData();
		/**
			@brief needTitleBlockTemplate
			Signal emitted when this object needs to
			set a specific title block template.
			This object cannot handle the job
			since it does not know of its parent project.
		*/
		void needTitleBlockTemplate(const QString &);

		/**
		 * @brief infoChanged
		 * Signal emitted when the informations changed.
		 */
		void informationChanged();
	
	// attributes
	private:
		// titleblock basic data
		QString btb_author_;        ///< titleblock author
		QDate   btb_date_;          ///< titleblock date
		QString btb_title_;         ///< titleblock title
		QString btb_folio_;         ///< titleblock folio
		QString btb_plant_;         ///< titleblock plant
		QString btb_locmach_;       ///< titleblock locmach
		QString btb_indexrev_;      ///< titleblock index rev
		QString btb_final_folio_;   ///< titleblock final folio
		QString btb_auto_page_num_; ///< titleblock auto page num
		int folio_index_;           ///< titleblock index
		int folio_total_;           ///< titleblock total
		QString btb_filename_;      ///< titleblock filename
		QString btb_version_;       ///< titleblock version
		/// titleblock additional fields
		DiagramContext additional_fields_;
		Qt::Edge m_edge;              ///< titleblock edge
		QString m_next_folio_num;     ///< titleblock next folio num
		QString m_previous_folio_num; ///< titleblock previous folio num
	
		// border dimensions (rows and columns)
		// columns: number and dimensions
		int columns_count_;           ///< columns count
		qreal columns_width_;         ///< columns width
		qreal columns_header_height_; ///< columns header height
	
		// rows: number and dimensions
		int rows_count_;          ///< rows count
		qreal rows_height_;       ///< rows height
		qreal rows_header_width_; ///< rows header width
		qreal header_line_thickness_ = 1.0; ///< header cells line thickness
	
		// calculated dimensions properties (for base dimension calculations)
		qreal base_area_;         ///< Base drawing area (for calculating base dimensions)
		qreal aspect_ratio_;      ///< Aspect ratio (for calculating base dimensions)
		qreal scale_;             ///< Scale multiplier (affects canvas, not headers/titleblock)
		bool use_calculated_dimensions_; ///< Whether calculated dimensions are used
	
		// title block dimensions
		qreal titleblock_height_;
	
		// rectangles used for drawing operations
		QRectF diagram_rect_;
	
			// display options
		bool display_titleblock_;
		bool display_columns_;
		bool display_rows_;
		bool display_border_;
		bool border_all_sides_;     ///< Whether to draw border on all sides
		bool enable_column_header_spacers_;      ///< Whether to show spacer columns before first and after last column
		qreal column_header_spacer_percentage_;  ///< Percentage of total width each spacer column should take (0-100)
		TitleBlockTemplateRenderer *m_titleblock_template_renderer;
};
#endif
