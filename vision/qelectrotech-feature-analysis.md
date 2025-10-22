# QElectroTech Feature Analysis

## Overview

QElectroTech (QET) is a libre and open source desktop application designed for creating electrical diagrams and schematics. It serves as a CAD/CAE editor focusing specifically on schematics drawing features, with no embedded simulation or calculation functionalities.

## Core Purpose

- **Primary Use**: Creating electrical documentation and schematics
- **Secondary Uses**: Drawing diagrams for pneumatics, hydraulics, process industries, electronics
- **Target Users**: Electricians, engineers, students, teachers, and professionals worldwide
- **Philosophy**: Libre, easy-to-use, and effective software for schematics drawing

### Version Information

- **Current Stable Version**: 0.90 (released 2023.01.06)
- **Current Development Version**: 0.100-dev (active development)
- **Version Management**: Stable versions are frozen after release, development continues in dev branch
- **License**: GNU GPL v2

## Technical Foundation

### Technology Stack

- **Framework**: Qt 5.x with Qt6 compatibility
- **Language**: C++17
- **Build System**: CMake (primary), qmake (legacy)
- **Dependencies**:
  - KF5 Framework (kcoreaddons, kwidgetsaddons)
  - pugixml (XML parsing)
  - SingleApplication (single instance management)
- **File Format**: XML-based for projects, elements, and titleblocks
- **Platform Support**: Windows, GNU/Linux, macOS, BSD

### Architecture

- **Modular Design**: Well-organized source code with clear separation of concerns
- **Graphics Framework**: Qt Graphics View framework for diagram rendering
- **Plugin Architecture**: Extensible element system
- **Multi-language Support**: 32 language translations (Arabic, Catalan, Czech, Danish, German, Greek, English, Spanish, Finnish, French, Croatian, Hungarian, Italian, Japanese, Mongolian, Norwegian, Dutch, Polish, Portuguese, Romanian, Serbian, Russian, Slovak, Slovenian, Swedish, Turkish, Ukrainian, Chinese)

## Core Features

### 1. Diagram Creation and Editing

#### Main Editor Components

- **Diagram Editor**: Primary interface for creating electrical schematics
- **Element Editor**: Tool for creating custom symbols and components
- **Titleblock Editor**: Customizable document headers and templates

#### Drawing Capabilities

- **Drag & Drop**: Elements can be added via drag-and-drop from collections
- **Automatic Connections**: Elements automatically connect when aligned
- **Manual Connections**: Point-to-point conductor drawing with mouse
- **Conductor Management**:
  - Adjustable conductor paths with handles
  - Customizable thickness, line style, and color
  - Automatic numbering patterns
- **Basic Shapes**: Lines, rectangles, ellipses, polygons
- **Image Import**: Support for .bmp, .jpg, .png, .svg images
- **DXF Import**: Import DXF files for conversion to QET elements

#### Advanced Drawing Features

- **Cross References**: Link elements together for automatic cross-reference text
- **Auto-numbering**: Configurable patterns for conductors, symbols, and folios
- **Sequential Numbering**: Support for unit, ten, and hundred numbering patterns
- **Folio-based Numbering**: Separate numbering contexts per folio
- **Search and Replace**: Project-wide search and replace functionality (Ctrl+F)
- **Undo/Redo**: Complete undo/redo system with animation support
- **Dynamic Text Fields**: Configurable text fields that can display element properties
- **Element Scaling**: Built-in element scaling functionality
- **Rich Text Support**: Rich text formatting capabilities
- **Element Groups**: Grouping functionality for related elements

### 2. Element Library and Management

#### Symbol Collections

- **8,200+ Symbols**: Comprehensive library of electrical, logic, pneumatic, hydraulic, and fluid symbols
- **Categories**:
  - **Electric (10_electric)**: 6,743 elements including:
    - All-pole devices (91 elements)
    - Single-pole devices (37 elements)
    - Manufacturer articles (538 elements)
    - American standards (7 elements)
    - EN 60617 standards (130 elements)
    - Graphics symbols (131 elements)
  - **Logic (20_logic)**: 75 elements including:
    - Logic gates (ANSI/IEEE, IEC 60617, DIN 40700, Ladder diagrams)
    - Flow charts and GRAFCET diagrams
  - **Hydraulic (30_hydraulic)**: 94 elements including:
    - Tanks, control valves, cylinders, pumps, exchangers, filters
  - **Pneumatic (50_pneumatic)**: 343 elements
  - **Energy (60_energy)**: 1,325 elements

#### Element Management

- **Search Engine**: Integrated search across all collections
- **Folder Organization**: Hierarchical organization by category
- **Custom Collections**: User-created element collections
- **Element Editor**: Built-in tool for creating custom symbols
- **Element Properties**: Comprehensive metadata system

### 3. Project Management

#### Multi-Project Support

- **Multiple Projects**: Open and edit several projects simultaneously
- **Project Structure**: Organized folio-based documentation
- **Recent Files**: Quick access to recently opened projects and elements

#### Document Structure

- **Folios (Sheets)**: Configurable page sizes and layouts
- **Headers**: Vertical and horizontal printed rulers
- **Grid System**: Configurable columns and rows
- **Titleblocks**: Customizable document headers with variables

### 4. Professional Documentation Features

#### Titleblock System

- **Customizable Templates**: Create and edit titleblock templates
- **Variables**: Custom variables for dynamic information display
- **Professional Layout**: Industry-standard document formatting

#### Table of Contents

- **Automatic Generation**: Two-click automatic table of contents
- **Live Updates**: Changes update automatically
- **Professional Formatting**: Industry-standard presentation

#### Nomenclature System

- **Configurable Tables**: Separate display and content configuration
- **SQLite Database**: Backend database for complex queries
- **Multi-folio Support**: Tables can span multiple pages
- **Export/Import**: Configuration sharing capabilities

### 5. Export and Output Capabilities

#### Print and PDF

- **Print Support**: Direct printing to physical printers
- **PDF Export**: High-quality PDF generation
- **Selective Printing**: Print entire documentation or selected parts

#### Image Export

- **Vector Formats**: SVG export for scalable graphics
- **Raster Formats**: PNG, JPG, BMP export options
- **CAD Formats**: DXF export for CAD software compatibility
- **High Resolution**: Professional-quality image output
- **Export Options**: Full folio or elements-only export

#### Data Export

- **Parts List**: CSV export of element information
- **Bill of Materials**: Integration with spreadsheet applications
- **Conductor Lists**: CSV export of conductor numbering
- **Database Export**: SQLite database export for nomenclature

### 6. Advanced Features

#### Terminal Strip Management

- **Terminal Strip Editor**: Dedicated editor for terminal strip management
- **Free Terminal Editor**: Flexible terminal configuration
- **Tree-based Organization**: Hierarchical terminal strip organization

#### Cross-Reference System

- **Automatic Updates**: Cross-references update automatically
- **Link Management**: Visual indication of linked elements
- **Search Integration**: Easy finding of linkable elements

#### Database Integration

- **SQLite Backend**: Database-driven nomenclature and summary features
- **Query Builder**: Visual SQL query creation
- **Data Export**: Database export capabilities

### 7. User Interface and Experience

#### Modern GUI

- **Customizable Layout**: Toolbars and panels can be enabled/disabled, moved, and docked
- **Multi-display Support**: Works with multiple monitor configurations
- **Responsive Design**: Adapts to different screen sizes
- **System Tray**: Minimize to system tray functionality

#### Internationalization

- **32 Languages**: Comprehensive translation support including Arabic, Catalan, Czech, Danish, German, Greek, English, Spanish, Finnish, French, Croatian, Hungarian, Italian, Japanese, Mongolian, Norwegian, Dutch, Polish, Portuguese, Romanian, Serbian, Russian, Slovak, Slovenian, Swedish, Turkish, Ukrainian, and Chinese
- **Dynamic Language Switching**: Restart application to change language
- **Cultural Adaptation**: Region-specific formatting and conventions

#### Accessibility

- **Keyboard Navigation**: Full keyboard support
- **Context Menus**: Right-click context-sensitive menus
- **Tooltips**: Helpful tooltips throughout the interface

### 8. File Format and Compatibility

#### XML-Based Format

- **Open Standard**: XML format ensures long-term compatibility
- **Human Readable**: Files can be edited with text editors
- **Version Control**: Git-friendly text-based format
- **Custom Tools**: Open format enables third-party tool development

#### Cross-Platform Compatibility

- **OS Independence**: Files created on one OS work on all supported platforms
- **No Conversion**: Direct file compatibility across platforms
- **Portable Installation**: Windows portable version available
- **Packaging Options**: AppImage (Linux), Flatpak, Snap, Windows installer, macOS packages

### 9. Development and Extensibility

#### Open Source

- **GPL License**: Free and open source software
- **Community Driven**: Active community development
- **No Vendor Lock-in**: Complete freedom from proprietary dependencies

#### Extensibility

- **Custom Elements**: Create and share custom symbols
- **Plugin Architecture**: Extensible through custom tools
- **API Access**: XML format enables custom tool development

### 10. Quality and Reliability

#### Professional Standards

- **Industry Standards**: Support for EN 60617, ANSI/IEEE standards
- **International Compliance**: IEC standards for electrical symbols
- **Multiple Standards**: Support for different regional standards (American, European, etc.)
- **Professional Output**: High-quality documentation suitable for professional use
- **Consistent Formatting**: Standardized symbol and document formatting

#### Data Integrity

- **Auto-save**: Automatic backup functionality
- **Version Control**: Git integration for project versioning
- **Backup System**: Automatic backup file management

## Use Cases and Applications

### Professional Applications

- **Industrial Electrical Design**: Manufacturing and industrial facility documentation
- **Building Electrical Systems**: Commercial and residential electrical planning
- **Control Systems**: PLC and automation system documentation
- **Power Distribution**: Electrical distribution system design

### Educational Applications

- **Electrical Engineering Education**: Teaching electrical circuit design
- **Technical Training**: Professional development and certification
- **Student Projects**: Academic electrical design projects

### Specialized Applications

- **Hydraulic Systems**: Fluid power system documentation
- **Pneumatic Systems**: Compressed air system design
- **Process Industries**: Industrial process documentation
- **Electronics**: Electronic circuit documentation

## Strengths and Advantages

1. **Cost-Effective**: Free and open source with no licensing fees
2. **Cross-Platform**: Works on all major operating systems
3. **Professional Quality**: Suitable for commercial and professional use
4. **Extensive Library**: Comprehensive symbol collection
5. **Standards Compliance**: Support for international electrical standards
6. **Community Support**: Active user community and development
7. **Open Format**: XML-based files ensure long-term compatibility
8. **Customizable**: Highly configurable interface and functionality
9. **Multi-language**: Extensive internationalization support
10. **Reliable**: Stable and well-tested software

## Limitations

1. **No Simulation**: No embedded circuit simulation capabilities
2. **No Calculations**: No electrical calculation features
3. **Learning Curve**: Requires understanding of electrical symbols and standards
4. **Limited 3D**: Primarily 2D schematic design tool
5. **No Real-time Collaboration**: Single-user application
6. **No Advanced CAD Features**: Limited compared to full CAD packages
7. **No Electrical Analysis**: No voltage drop, load calculations, or electrical analysis
8. **Limited Automation**: No advanced automation scripting or macros

## Conclusion

QElectroTech is a comprehensive, professional-grade electrical diagramming tool that successfully balances ease of use with powerful functionality. Its open-source nature, extensive symbol library, and professional output capabilities make it an excellent choice for electrical engineers, technicians, educators, and students. The software's focus on schematics drawing rather than simulation makes it ideal for documentation and design purposes, while its XML-based format ensures long-term compatibility and extensibility.

The active community development, regular updates, and comprehensive feature set position QElectroTech as a strong alternative to proprietary electrical CAD software, particularly for users who value open-source solutions and long-term data accessibility.
