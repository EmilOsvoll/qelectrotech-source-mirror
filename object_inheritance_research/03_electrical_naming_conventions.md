# Electrical Schematic Naming Conventions and Standards

## Overview

Proper electrical schematic naming conventions are essential for professional electrical design. This document outlines the key standards and conventions that should be followed in implementing object inheritance for QElectroTech.

## IEC 60617 Standard

### Component Reference Designators

The IEC 60617 standard defines letter codes for different types of electrical components:

| Letter | Component Type                | Examples                                            |
| ------ | ----------------------------- | --------------------------------------------------- |
| **Q**  | Switching devices             | Switches, circuit breakers, contactors              |
| **S**  | Control and signaling devices | Push buttons, selector switches, pilot lights       |
| **K**  | Relays and contactors         | Control relays, contactors, motor starters          |
| **M**  | Motors                        | AC motors, DC motors, servo motors                  |
| **F**  | Protection devices            | Fuses, circuit breakers, surge protectors           |
| **T**  | Transformers                  | Power transformers, current transformers            |
| **R**  | Resistors                     | Fixed resistors, variable resistors, potentiometers |
| **C**  | Capacitors                    | Fixed capacitors, variable capacitors               |
| **L**  | Inductors                     | Coils, chokes, reactors                             |
| **D**  | Semiconductor devices         | Diodes, transistors, integrated circuits            |
| **U**  | Integrated circuits           | Microprocessors, memory, logic ICs                  |
| **X**  | Connectors                    | Plugs, sockets, terminal blocks                     |
| **Y**  | Electromechanical devices     | Solenoids, actuators, valves                        |

### Numbering Conventions

- **Sequential numbering**: Q1, Q2, Q3, etc.
- **Functional grouping**: Q1.1, Q1.2, Q1.3 for related components
- **Location-based**: Q1A, Q1B, Q1C for components in different locations

## Component Assembly Naming

### Hierarchical Naming Structure

For complex assemblies, use hierarchical naming:

```
Assembly Level: CABINET-01
├── Sub-Assembly: POWER-SUPPLY-01
│   ├── Component: T1 (Transformer)
│   ├── Component: D1-D4 (Rectifier Diodes)
│   └── Component: C1-C3 (Filter Capacitors)
├── Sub-Assembly: CONTROL-PANEL-01
│   ├── Component: S1 (Main Switch)
│   ├── Component: S2-S5 (Control Switches)
│   └── Component: H1-H3 (Indicator Lights)
└── Sub-Assembly: MOTOR-CONTROL-01
    ├── Component: K1 (Main Contactor)
    ├── Component: K2 (Auxiliary Contactor)
    └── Component: F1 (Motor Protection)
```

### Assembly Naming Conventions

- **Assembly Prefix**: Use descriptive prefixes (CABINET-, PANEL-, MODULE-)
- **Functional Suffix**: Indicate function (-POWER, -CONTROL, -SIGNAL)
- **Numbering**: Sequential numbering within assembly type
- **Sub-Assembly**: Use hyphenated naming (POWER-SUPPLY, MOTOR-CONTROL)

## Property Naming Standards

### Component Properties

- **label**: Component reference designator (Q1, S2, etc.)
- **description**: Functional description (Main Power Switch)
- **manufacturer**: Component manufacturer
- **part_number**: Manufacturer's part number
- **voltage_rating**: Operating voltage
- **current_rating**: Operating current
- **power_rating**: Power rating where applicable
- **mounting**: Mounting type (panel, DIN rail, etc.)
- **enclosure**: Enclosure rating (IP65, NEMA 4X, etc.)

### Assembly Properties

- **assembly_id**: Unique assembly identifier
- **assembly_name**: Descriptive assembly name
- **assembly_type**: Type of assembly (cabinet, panel, module)
- **location**: Physical location reference
- **responsible**: Engineer responsible for design
- **revision**: Assembly revision number
- **date_created**: Creation date
- **date_modified**: Last modification date

## Database Integration Standards

### Component Database Schema

```sql
CREATE TABLE components (
    id INTEGER PRIMARY KEY,
    reference_designator VARCHAR(20),
    description TEXT,
    manufacturer VARCHAR(100),
    part_number VARCHAR(50),
    category VARCHAR(50),
    subcategory VARCHAR(50),
    voltage_rating DECIMAL(10,2),
    current_rating DECIMAL(10,2),
    power_rating DECIMAL(10,2),
    mounting_type VARCHAR(50),
    enclosure_rating VARCHAR(20),
    supplier VARCHAR(100),
    supplier_part_number VARCHAR(50),
    cost DECIMAL(10,2),
    lead_time INTEGER,
    created_date DATE,
    modified_date DATE
);
```

### Assembly Database Schema

```sql
CREATE TABLE assemblies (
    id INTEGER PRIMARY KEY,
    assembly_id VARCHAR(50),
    assembly_name VARCHAR(100),
    assembly_type VARCHAR(50),
    description TEXT,
    location VARCHAR(100),
    responsible VARCHAR(100),
    revision VARCHAR(20),
    created_date DATE,
    modified_date DATE
);

CREATE TABLE assembly_components (
    id INTEGER PRIMARY KEY,
    assembly_id INTEGER,
    component_id INTEGER,
    quantity INTEGER,
    position VARCHAR(50),
    notes TEXT,
    FOREIGN KEY (assembly_id) REFERENCES assemblies(id),
    FOREIGN KEY (component_id) REFERENCES components(id)
);
```

## BOM (Bill of Materials) Standards

### BOM Structure

- **Level**: Assembly hierarchy level
- **Reference**: Component reference designator
- **Description**: Component description
- **Manufacturer**: Component manufacturer
- **Part Number**: Manufacturer's part number
- **Quantity**: Required quantity
- **Unit**: Unit of measure
- **Supplier**: Preferred supplier
- **Cost**: Unit cost
- **Total Cost**: Quantity × Unit cost

### BOM Export Formats

- **CSV**: Comma-separated values for spreadsheet import
- **Excel**: Microsoft Excel format
- **PDF**: Formatted report for printing
- **XML**: Structured data for system integration

## Implementation Recommendations for QElectroTech

### 1. Naming Convention Enforcement

- **Validation Rules**: Implement validation for reference designators
- **Auto-numbering**: Automatic sequential numbering within component types
- **Conflict Detection**: Detect duplicate reference designators
- **Standards Compliance**: Ensure compliance with IEC 60617

### 2. Property Management

- **Standardized Properties**: Define standard property sets for each component type
- **Custom Properties**: Allow user-defined properties
- **Property Validation**: Validate property values against standards
- **Property Templates**: Predefined property sets for common components

### 3. Assembly Management

- **Hierarchical Structure**: Support multi-level assembly hierarchies
- **Assembly Templates**: Predefined assembly templates
- **Component Grouping**: Logical grouping of related components
- **Assembly Validation**: Ensure assembly completeness and correctness

### 4. Database Integration

- **Component Database**: Link to external component databases
- **Supplier Integration**: Connect to supplier catalogs
- **Cost Tracking**: Track component costs and availability
- **Inventory Management**: Monitor component inventory levels

## Conclusion

Implementing proper electrical schematic naming conventions and standards is crucial for professional electrical design. QElectroTech should adopt these standards to ensure compatibility with industry practices and improve the software's professional capabilities.

The hierarchical naming structure and standardized properties will provide a solid foundation for implementing sophisticated object inheritance and component grouping features.
