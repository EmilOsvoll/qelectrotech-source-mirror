# QElectroTech Translation Project Status

## Project Overview

QElectroTech is a Qt5 application for designing electrical diagrams. The project is currently primarily in French, with extensive localization support for multiple languages.

## Current French Content Areas

### 1. Source Code Comments and Documentation

- **Location**: `sources/` directory
- **Scope**: Extensive French comments throughout the codebase
- **Pattern**: Comments use `\~French` tags for bilingual documentation
- **Examples**:
  - `\~French l'instance de la QETApp` (the instance of QETApp)
  - `\~French Change le langage utilise par l'application` (Change the language used by the application)
  - `\~French Gere les evenements relatifs au QSystemTrayIcon` (Manages QSystemTrayIcon related events)

### 2. User Interface Strings

- **Location**: UI files in `sources/ui/` and `.ui` files
- **Scope**: All user-facing text in French
- **Examples**:
  - "Apparence" (Appearance)
  - "Utiliser les couleurs du système" (Use system colors)
  - "Autoriser le dézoom au delà du folio" (Allow zoom out beyond folio)
  - "Textes d'éléments" (Element texts)

### 3. Translation Files

- **Location**: `lang/` directory
- **Structure**:
  - `qet_fr.ts` - French source strings (19,000+ entries)
  - `qet_en.ts` - English translations (19,000+ entries)
  - Multiple other language files (ar, ca, cs, de, el, es, etc.)
- **Status**: French is the source language, English translations exist

### 4. Configuration Files

- **Location**: `misc/qelectrotech.sample.conf`
- **Content**: Configuration options and default values
- **Language**: Currently in English (good)

### 5. Undo Command Messages

- **Location**: Various undo command files
- **Examples**:
  - `"supprimer %1"` (delete %1)
  - French strings in `QObject::tr()` calls

### 6. Project Documentation

- **Location**: Root directory files
- **Files**: README, ChangeLog, CREDIT, etc.
- **Status**: Multi-language support with French as primary

## Localization System Analysis

### Current Setup

- **Default Language**: French (`fr`) is set as default in `translations_stat.pl`
- **Fallback**: English (`en`) is used as fallback when French fails
- **Translation Loading**: Handled in `QETApp::setLanguage()`
- **Qt Integration**: Uses Qt's translation system with `.ts` and `.qm` files

### Translation Process

1. French strings are marked with `tr()` or `QObject::tr()`
2. Translation files (`.ts`) contain source (French) and target translations
3. Compiled translation files (`.qm`) are loaded at runtime
4. Fallback to English if target language fails

## Key Findings

### Non-Breaking Changes (Step 1)

- **Comments**: Extensive French comments with `\~French` tags
- **UI Strings**: All user interface text in French
- **Documentation**: French documentation strings in code

### Localization Issues (Step 2)

- **Default Language**: System defaults to French
- **Component Previews**: Likely showing French due to default language setting
- **Translation Loading**: Properly implemented but French-first

### Breaking Changes (Step 3)

- **Function Names**: Some French function/variable names found
- **String Literals**: French strings in `tr()` calls
- **Undo Messages**: French undo command descriptions

## File Statistics

- **Total Source Files**: ~640 files in `sources/`
- **Translation Files**: 32 language files in `lang/`
- **UI Files**: Multiple `.ui` files with French strings
- **Configuration**: Minimal French content in config files

## Next Steps

1. Create detailed TODO list for systematic translation
2. Establish terminology dictionary for consistent translation
3. Begin with non-breaking changes (comments and UI strings)
4. Address localization settings
5. Plan systematic approach for breaking changes (function names, etc.)

---

_Generated: $(date)_
_Project: QElectroTech Translation Initiative_
