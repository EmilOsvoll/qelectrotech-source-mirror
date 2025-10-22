# Current QElectroTech Element Structure Analysis

## Overview

QElectroTech (QET) currently uses a flat element structure where each component is defined as an individual `.elmt` file with no hierarchical relationships or inheritance capabilities.

## Current Element Organization

### Directory Structure

```
elements/
├── 10_electric/           # Main electrical elements
│   ├── 10_allpole/        # Multi-pole components
│   ├── 11_singlepole/     # Single-pole components
│   ├── 20_manufacturers_articles/  # Manufacturer-specific parts
│   ├── 90_american_standards/     # US standards
│   ├── 91_en_60617/       # IEC 60617 standard symbols
│   └── 98_graphics/       # Assembly plan graphics
├── 20_logic/              # Logic elements
├── 30_hydraulic/          # Hydraulic elements
├── 50_pneumatic/          # Pneumatic elements
└── 60_energy/             # Energy elements
```

### Element File Format (.elmt)

Each element is defined in XML format with the following structure:

```xml
<definition version="0.100.0" type="element" link_type="simple"
             width="300" height="60" hotspot_x="45" hotspot_y="25">
    <uuid uuid="{unique-identifier}"/>
    <names>
        <name lang="en">Component Name</name>
        <name lang="fr">Nom du Composant</name>
    </names>
    <informations>Author and license information</informations>
    <description>
        <!-- Graphical elements: lines, circles, polygons, text -->
        <!-- Terminals for electrical connections -->
        <!-- Dynamic text fields -->
    </description>
</definition>
```

### Key Components of Current Structure

#### 1. Element Data Structure

- **ElementData class**: Contains type, master/slave relationships, terminal properties
- **DiagramContext**: Stores element information (label, description, etc.)
- **Element types**: Simple, Master, Slave, Terminal, Report types

#### 2. Element Properties

- **Basic properties**: Width, height, hotspot coordinates
- **Link types**: Simple, Master, Slave, Terminal, Report
- **Terminal properties**: Type (Generic, Fuse, Sectional, Diode, Ground)
- **Dynamic text**: Labels and information fields

#### 3. Project Integration

- **ElementsLocation**: References to element files in collections
- **Project collections**: Automatic inclusion of used elements
- **Element caching**: SQLite-based cache for performance

## Current Limitations

### 1. No Hierarchical Relationships

- Elements exist as independent entities
- No parent-child relationships between components
- No inheritance of properties from base components

### 2. No Component Grouping

- Cannot group multiple elements into assemblies
- No shared properties across component groups
- Manual assembly required for complex components

### 3. Limited Reusability

- Each component variant requires separate element file
- No parameterization or templating system
- Duplication of similar components

### 4. Poor BOM Integration

- No structured component data for procurement
- Limited metadata for manufacturing
- No database integration for parts management

## Current Element Types

### Master/Slave System

QET has a basic master/slave system for related components:

- **Master elements**: Coils, protection devices, commutators
- **Slave elements**: Contacts with different states (NO, NC, SW)
- **Link types**: NextReport, PreviousReport, AllReport

### Terminal Management

- Generic terminals for basic connections
- Specialized terminal types (Fuse, Sectional, Diode, Ground)
- Terminal functions (Phase, Neutral, Generic)

## Code Architecture

### Key Classes

- **Element**: Main element class inheriting from QetGraphicsItem
- **ElementData**: Element metadata and properties
- **ElementsLocation**: File system references
- **Terminal**: Connection points
- **DynamicElementTextItem**: Dynamic text fields

### XML Serialization

- Elements serialize to XML for project files
- Properties stored in DiagramContext
- Terminal information with unique IDs
- Dynamic text with positioning and formatting

## Assessment for Object Inheritance

The current structure provides a solid foundation but lacks:

1. **Hierarchical organization** of components
2. **Property inheritance** mechanisms
3. **Component assembly** capabilities
4. **Structured metadata** for BOM generation
5. **Database integration** for parts management

These limitations make it difficult to implement sophisticated component grouping and inheritance systems that would enable the creation of complex assemblies like electrical cabinets or multi-part components.
