# QElectroTech Translation Conversion Tools

This folder contains all the Python scripts and related files used to convert the QElectroTech translation system from French source to English source.

## Overview

The conversion process involved:

1. Converting English translation file to use English as source language
2. Converting all other language files to use English as source language
3. Updating source code files to replace French strings with English strings
4. Validating the conversion results
5. Archiving original files and promoting converted files

## Script Categories

### Core Conversion Scripts

- **`convert_english.py`** - Converts the English translation file to use English as source
- **`convert_direct.py`** - Converts individual language files using French→English mapping
- **`convert_all.py`** - Orchestrates conversion of all language files
- **`archive_and_promote.py`** - Archives original files and promotes converted files

### Source File Update Scripts

- **`update_source_files.py`** - Updates source code files (.cpp, .ui, .h) with English strings
- **`test_single_update.py`** - Tests source file updates on a single file
- **`fix_all_french_text.py`** - Comprehensive French text fixer for C++ files
- **`fix_all_remaining_french.py`** - Enhanced fixer for UI files and complex patterns
- **`fix_specific_patterns.py`** - Targeted fixes for specific mismatch patterns

### Validation Scripts

- **`validate_conversions.py`** - Provides statistics on converted translation files
- **`validate_location_references.py`** - Compares location tag references with source text
- **`final_summary.py`** - Generates final conversion summary

### Debug and Analysis Scripts

- **`debug_mapping.py`** - Debug script for mapping logic
- **`debug_translations.py`** - Debug script for translation processing
- **`analyze_and_fix_comprehensive.py`** - Analyzes validation results and applies fixes
- **`final_comprehensive_fixer.py`** - Final comprehensive fixer with broad replacements

### Legacy Scripts (Early Attempts)

- **`convert_all_languages.py`** - Early attempt at conversion (replaced by convert_all.py)
- **`convert_translations.py`** - Early conversion script
- **`fix_french_text.py`** - Early French text fixer
- **`fix_source_files.py`** - Early source file fixer
- **`fix_specific_french.py`** - Early specific French fixer

## Result Files

- **`validation_results.txt`** - Initial validation results
- **`validation_results_after.txt`** - Validation results after fixes
- **`validation_results_final.txt`** - Final validation results
- **`MANUAL_FIX_PLAN.md`** - Manual fix plan for remaining mismatches

## Usage

Most scripts are designed to be run from the `lang/` directory. The main conversion process was:

1. **Convert English file**: `python conversion_tools/convert_english.py`
2. **Convert all languages**: `python conversion_tools/convert_all.py`
3. **Update source files**: `python conversion_tools/update_source_files.py`
4. **Validate results**: `python conversion_tools/validate_location_references.py`
5. **Archive and promote**: `python conversion_tools/archive_and_promote.py`

## Dependencies

The scripts require:

- Python 3.x
- lxml library (installed in `conversion_env/` virtual environment)

## Notes

- The conversion process successfully converted 32 language files
- Original files are archived in `../archive/`
- The main `lang/` directory now contains the converted files
- Some validation mismatches remain due to script limitations in extracting complex C++ expressions
- The French translation file was specially handled to maintain proper English→French translations

## Final Status

✅ **Conversion Complete**: All translation files now use English as the source language
✅ **Source Files Updated**: French strings in source code replaced with English equivalents  
✅ **Files Organized**: Original files archived, converted files promoted
✅ **Validation**: Comprehensive validation performed with detailed results
