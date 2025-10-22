# Actionable Implementation Todo Plan

## Overview

This document provides a concise, actionable todo plan for implementing object inheritance in QElectroTech. It builds on our comprehensive research and focuses on immediate next steps.

## Prerequisites Assessment ✅

- [x] **Current QET Architecture Analysis** - Complete
- [x] **Industry Standards Research** - Complete
- [x] **Breaking Change Risk Assessment** - Complete
- [x] **Conservative Implementation Strategy** - Complete
- [x] **Dependency Analysis** - Complete

## Phase 1: Foundation Setup (Weeks 1-4)

### Week 1: Development Environment Setup

**Goal**: Prepare development environment and create initial code structure

#### Day 1-2: Environment Preparation

- [ ] **Set up QET development environment**

  - Clone QET repository
  - Install build dependencies (Qt, CMake, etc.)
  - Verify existing build works
  - Set up debugging environment

- [ ] **Create feature branch**

  ```bash
  git checkout -b feature/object-inheritance
  ```

- [ ] **Create assembly module structure**
  ```
  sources/assembly/
  ├── assemblymanager.h
  ├── assemblymanager.cpp
  ├── assemblyinfo.h
  ├── assemblyinfo.cpp
  └── CMakeLists.txt
  ```

#### Day 3-5: Core Data Structures

- [ ] **Create AssemblyInfo class**

  ```cpp
  // sources/assembly/assemblyinfo.h
  class AssemblyInfo {
  public:
      QString assembly_id;
      QString assembly_name;
      QString assembly_type;
      QStringList child_elements;
      QString parent_assembly;
      QMap<QString, QVariant> properties;

      // Serialization methods
      QDomElement toXml(QDomDocument& doc) const;
      bool fromXml(const QDomElement& element);
  };
  ```

- [ ] **Create AssemblyManager class skeleton**
  ```cpp
  // sources/assembly/assemblymanager.h
  class AssemblyManager : public QObject {
      Q_OBJECT
  public:
      explicit AssemblyManager(QETProject* project, QObject* parent = nullptr);

      // Core methods (implement later)
      bool createAssembly(const AssemblyInfo& info);
      bool addElementToAssembly(const QString& assembly_id, const QString& element_ref);
      QList<QString> getAssemblies() const;

  private:
      QETProject* m_project;
      QHash<QString, AssemblyInfo> m_assemblies;
  };
  ```

### Week 2: ElementData Extension

**Goal**: Extend ElementData class with assembly support

#### Day 1-3: Extend ElementData Class

- [ ] **Add assembly properties to ElementData**

  ```cpp
  // In sources/properties/elementdata.h
  class ElementData : public PropertiesInterface {
      // ... existing code ...

      // NEW: Assembly support
      QString m_assembly_id;
      QString m_assembly_name;
      QString m_assembly_type;
      QStringList m_child_elements;
      QString m_parent_assembly;

      // NEW: Methods
      void setAssemblyInfo(const QString& id, const QString& name, const QString& type);
      bool isAssembly() const;
      bool isSubAssembly() const;
      QStringList getChildElements() const;
      void addChildElement(const QString& element_ref);
      void removeChildElement(const QString& element_ref);
  };
  ```

- [ ] **Update ElementData serialization**
  ```cpp
  // In sources/properties/elementdata.cpp
  QDomElement ElementData::toXml(QDomDocument &xml_element) const {
      // ... existing code ...

      // NEW: Assembly information
      if (!m_assembly_id.isEmpty()) {
          QDomElement assembly_info = xml_element.createElement("assembly_info");
          assembly_info.setAttribute("assembly_id", m_assembly_id);
          assembly_info.setAttribute("assembly_name", m_assembly_name);
          assembly_info.setAttribute("assembly_type", m_assembly_type);

          if (!m_child_elements.isEmpty()) {
              QDomElement children = xml_element.createElement("child_elements");
              for (const QString& child : m_child_elements) {
                  QDomElement child_elem = xml_element.createElement("child");
                  child_elem.setAttribute("ref", child);
                  children.appendChild(child_elem);
              }
              assembly_info.appendChild(children);
          }

          xml_element.appendChild(assembly_info);
      }

      return xml_element;
  }
  ```

#### Day 4-5: Testing and Validation

- [ ] **Create unit tests for ElementData extension**

  ```cpp
  // tests/test_elementdata_assembly.cpp
  void testElementDataAssembly() {
      ElementData data;

      // Test assembly info setting
      data.setAssemblyInfo("CABINET-01", "Main Cabinet", "Assembly");
      QVERIFY(data.isAssembly());
      QCOMPARE(data.m_assembly_id, QString("CABINET-01"));

      // Test child element management
      data.addChildElement("Q1");
      data.addChildElement("S1");
      QCOMPARE(data.getChildElements().size(), 2);

      // Test XML serialization
      QDomDocument doc;
      QDomElement elem = data.toXml(doc);
      QVERIFY(!elem.firstChildElement("assembly_info").isNull());
  }
  ```

- [ ] **Verify backward compatibility**
  - Test existing element files still load correctly
  - Test existing project files still work
  - Test XML serialization/deserialization

### Week 3: Project Database Extension

**Goal**: Extend project database to support assemblies

#### Day 1-3: Extend projectDataBase Class

- [ ] **Add assembly tables to projectDataBase**

  ```cpp
  // In sources/dataBase/projectdatabase.h
  class projectDataBase : public QObject {
      // ... existing code ...

      // NEW: Assembly management
      QHash<QString, AssemblyInfo> m_assemblies;

      // NEW: Methods
      void addAssembly(const AssemblyInfo& assembly);
      void removeAssembly(const QString& assembly_id);
      AssemblyInfo getAssembly(const QString& assembly_id) const;
      QList<QString> getAllAssemblies() const;
      bool hasAssembly(const QString& assembly_id) const;
  };
  ```

- [ ] **Update project database serialization**
  ```cpp
  // In sources/dataBase/projectdatabase.cpp
  void projectDataBase::toXml(QDomElement& element) const {
      // ... existing code ...

      // NEW: Assembly data
      if (!m_assemblies.isEmpty()) {
          QDomElement assemblies_elem = element.ownerDocument().createElement("assemblies");
          for (auto it = m_assemblies.begin(); it != m_assemblies.end(); ++it) {
              QDomElement assembly_elem = it.value().toXml(element.ownerDocument());
              assemblies_elem.appendChild(assembly_elem);
          }
          element.appendChild(assemblies_elem);
      }
  }
  ```

#### Day 4-5: Integration Testing

- [ ] **Test project database with assemblies**
- [ ] **Verify project file compatibility**
- [ ] **Test assembly persistence**

### Week 4: AssemblyManager Implementation

**Goal**: Implement core assembly management functionality

#### Day 1-3: Core AssemblyManager Methods

- [ ] **Implement assembly creation**

  ```cpp
  bool AssemblyManager::createAssembly(const AssemblyInfo& info) {
      if (info.assembly_id.isEmpty() || m_project->dataBase()->hasAssembly(info.assembly_id)) {
          return false;
      }

      m_project->dataBase()->addAssembly(info);
      emit assemblyCreated(info.assembly_id);
      return true;
  }
  ```

- [ ] **Implement element management**
  ```cpp
  bool AssemblyManager::addElementToAssembly(const QString& assembly_id, const QString& element_ref) {
      if (!m_project->dataBase()->hasAssembly(assembly_id)) {
          return false;
      }

      AssemblyInfo info = m_project->dataBase()->getAssembly(assembly_id);
      info.child_elements.append(element_ref);
      m_project->dataBase()->addAssembly(info);

      emit assemblyModified(assembly_id);
      return true;
  }
  ```

#### Day 4-5: Testing and Documentation

- [ ] **Create comprehensive unit tests**
- [ ] **Document AssemblyManager API**
- [ ] **Test integration with existing QET components**

## Phase 2: User Interface Foundation (Weeks 5-8)

### Week 5: Assembly Properties Dialog

**Goal**: Add assembly tab to element properties dialog

#### Day 1-3: Extend Properties Editor

- [ ] **Add assembly tab to PropertiesEditorDialog**
  ```cpp
  // In sources/PropertiesEditor/propertieseditordialog.h
  class PropertiesEditorDialog : public QDialog {
      // ... existing code ...

      // NEW: Assembly tab
      QWidget* m_assembly_tab;
      QLineEdit* m_assembly_id_edit;
      QLineEdit* m_assembly_name_edit;
      QComboBox* m_assembly_type_combo;
      QListWidget* m_child_elements_list;

      void setupAssemblyTab();
      void loadAssemblyData();
      void saveAssemblyData();
  };
  ```

#### Day 4-5: Testing and Integration

- [ ] **Test assembly properties dialog**
- [ ] **Verify integration with existing properties**
- [ ] **Test data persistence**

### Week 6: Assembly Browser Widget

**Goal**: Create assembly browser for element panel

#### Day 1-3: Create AssemblyBrowser Widget

- [ ] **Create AssemblyBrowser class**
  ```cpp
  // sources/ui/assemblybrowser.h
  class AssemblyBrowser : public QWidget {
      Q_OBJECT
  public:
      explicit AssemblyBrowser(QETProject* project, QWidget* parent = nullptr);

  private:
      QETProject* m_project;
      QTreeWidget* m_assembly_tree;
      QPushButton* m_create_assembly_button;

      void refreshAssemblies();
      void setupUI();
  };
  ```

#### Day 4-5: Integration Testing

- [ ] **Test assembly browser functionality**
- [ ] **Verify integration with element panel**
- [ ] **Test assembly selection and editing**

### Week 7: Element Panel Integration

**Goal**: Integrate assembly browser into element panel

#### Day 1-3: Extend ElementsPanel

- [ ] **Add assembly tab to ElementsPanel**
  ```cpp
  // In sources/elementspanel.h
  class ElementsPanel : public QWidget {
      // ... existing code ...

      // NEW: Assembly management
      QTabWidget* m_tab_widget;
      AssemblyBrowser* m_assembly_browser;

      void setupAssemblyTab();
  };
  ```

#### Day 4-5: Testing and Refinement

- [ ] **Test element panel integration**
- [ ] **Verify tab switching functionality**
- [ ] **Test assembly management workflows**

### Week 8: Assembly Creation Wizard

**Goal**: Create wizard for assembly creation

#### Day 1-3: Create Assembly Creation Wizard

- [ ] **Create AssemblyCreationWizard class**
  ```cpp
  // sources/ui/assemblycreationwizard.h
  class AssemblyCreationWizard : public QWizard {
      Q_OBJECT
  public:
      explicit AssemblyCreationWizard(QETProject* project, QWidget* parent = nullptr);

      QString getAssemblyId() const;
      QString getAssemblyName() const;
      QStringList getSelectedElements() const;

  private:
      QETProject* m_project;
  };
  ```

#### Day 4-5: Testing and Documentation

- [ ] **Test assembly creation wizard**
- [ ] **Verify wizard workflow**
- [ ] **Document user workflows**

## Phase 3: Advanced Features (Weeks 9-12)

### Week 9: BOM Generation

**Goal**: Implement basic BOM generation

#### Day 1-3: Create BOMGenerator Class

- [ ] **Create BOMGenerator class**
  ```cpp
  // sources/bom/bomgenerator.h
  class BOMGenerator : public QObject {
  public:
      struct BOMItem {
          QString reference_designator;
          QString description;
          int quantity;
          QString manufacturer;
          QString part_number;
      };

      QList<BOMItem> generateBOM(const QString& assembly_id) const;
      bool exportToCSV(const QString& assembly_id, const QString& filename) const;
  };
  ```

#### Day 4-5: Testing and Integration

- [ ] **Test BOM generation**
- [ ] **Test CSV export**
- [ ] **Verify BOM accuracy**

### Week 10: Property Inheritance

**Goal**: Implement property inheritance system

#### Day 1-3: Property Inheritance Logic

- [ ] **Implement property inheritance in AssemblyManager**
- [ ] **Add property override capabilities**
- [ ] **Test inheritance chains**

#### Day 4-5: Testing and Validation

- [ ] **Test property inheritance**
- [ ] **Test property overrides**
- [ ] **Verify inheritance validation**

### Week 11: Assembly Templates

**Goal**: Create assembly template system

#### Day 1-3: Template System

- [ ] **Create AssemblyTemplates class**
- [ ] **Implement template storage and retrieval**
- [ ] **Create default templates**

#### Day 4-5: Testing and Documentation

- [ ] **Test template system**
- [ ] **Verify template persistence**
- [ ] **Document template usage**

### Week 12: Integration and Testing

**Goal**: Final integration and comprehensive testing

#### Day 1-3: Integration Testing

- [ ] **Test complete assembly workflow**
- [ ] **Test with real project data**
- [ ] **Performance testing**

#### Day 4-5: Documentation and Release Prep

- [ ] **Update user documentation**
- [ ] **Create release notes**
- [ ] **Prepare for code review**

## Immediate Next Steps (This Week)

### Priority 1: Environment Setup

- [ ] **Set up QET development environment**
- [ ] **Verify existing build works**
- [ ] **Create feature branch**

### Priority 2: First Implementation

- [ ] **Create AssemblyInfo class**
- [ ] **Extend ElementData with assembly properties**
- [ ] **Create basic unit tests**

### Priority 3: Validation

- [ ] **Test backward compatibility**
- [ ] **Verify XML serialization works**
- [ ] **Test with existing project files**

## Success Criteria for Phase 1

### Technical Criteria

- [ ] **All existing functionality works unchanged**
- [ ] **New assembly data can be stored and retrieved**
- [ ] **XML serialization/deserialization works**
- [ ] **Unit tests pass with >90% coverage**

### User Experience Criteria

- [ ] **No impact on existing user workflows**
- [ ] **Assembly properties can be set via UI**
- [ ] **Assembly data persists across project saves/loads**
- [ ] **Performance impact is <5%**

## Risk Mitigation

### High-Risk Items

- [ ] **ElementData changes** → Extensive testing, backward compatibility
- [ ] **XML schema changes** → Optional elements only, version checking
- [ ] **Project file compatibility** → Migration tools, fallback mechanisms

### Testing Strategy

- [ ] **Unit tests for all new classes**
- [ ] **Integration tests with existing components**
- [ ] **Regression tests for existing functionality**
- [ ] **Performance tests with large projects**

## Conclusion

This todo plan provides a clear, actionable path forward for implementing object inheritance in QElectroTech. The phased approach ensures minimal risk while delivering incremental value. Each week has specific, measurable goals that build upon the previous week's work.

The key to success is maintaining backward compatibility while gradually introducing new functionality. By starting with core data structures and building up to user interface components, we can ensure a solid foundation for the advanced features.
