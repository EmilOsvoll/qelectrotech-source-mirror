# QElectroTech Manual Translation Fix Plan

## Overview

After automated conversion, we have 44,634 mismatches that need manual fixing. The translator requires exact matches between source files and translation files for proper functionality.

## Current Status

- **Total validations**: 91,584
- **Successful matches**: 46,950 (51.3%)
- **Mismatches**: 44,634 (48.7%)
- **Errors**: 0

## Root Causes of Mismatches

1. **Text extraction limitations** - Complex C++ expressions not fully parsed by validation script
2. **Encoding issues** - Some characters not properly handled (e.g., "Tonsit�", "mod�le")
3. **HTML entity differences** - Minor formatting variations (e.g., "&amp;View contacts" vs "&View contacts")
4. **Incomplete French text replacement** - Some French text still exists in source files
5. **Validation script limitations** - Not all text patterns captured correctly

## Manual Fix Strategy

### Phase 1: High-Impact Files (Priority Order)

Focus on files with the most mismatches first to maximize impact.

#### 1.1 UI Files (.ui)

- **Target**: Fix HTML entities, French text, and formatting issues
- **Files to check**:
  - `sources/ui/xrefpropertieswidget.ui` (352 mismatches)
  - `sources/ui/titleblockpropertieswidget.ui` (95 mismatches)
  - Other UI files with significant mismatches

#### 1.2 C++ Files with French Text

- **Target**: Replace remaining French text with English equivalents
- **Files to check**:
  - `sources/ui/titleblockpropertieswidget.cpp` (64 mismatches)
  - `sources/undocommand/addelementtextcommand.cpp` (96 mismatches)
  - `sources/undocommand/deleteqgraphicsitemcommand.cpp` (32 mismatches)

### Phase 2: Systematic File-by-File Review

Go through each file systematically and fix all mismatches.

## Progress Tracking

### Completed Files

- [ ] `sources/ui/xrefpropertieswidget.ui` - 352 mismatches
- [ ] `sources/ui/titleblockpropertieswidget.ui` - 95 mismatches
- [ ] `sources/ui/titleblockpropertieswidget.cpp` - 64 mismatches
- [ ] `sources/undocommand/addelementtextcommand.cpp` - 96 mismatches
- [ ] `sources/undocommand/deleteqgraphicsitemcommand.cpp` - 32 mismatches

### Current Focus

**Starting with**: `sources/ui/xrefpropertieswidget.ui` (352 mismatches)

## Fix Categories

### 1. HTML Entity Issues

- `&amp;View contacts` → `&View contacts`
- `&lt;html&gt;` → `<html>`
- `&gt;` → `>`

### 2. French Text Remaining

- `Tonsit�` → `Dense` (encoding issue)
- `Greenical` → `Vertical`
- `Tofault` → `Default`
- `Affiche&r en contacts` → `&View contacts`

### 3. Text Extraction Issues

- Complex C++ expressions not properly parsed
- Multi-line strings
- Nested function calls

### 4. Formatting Differences

- Spacing issues
- Line break differences
- Quote style differences

## Validation Process

After each file fix:

1. Run validation script on that specific file
2. Verify mismatch count reduction
3. Test translator functionality
4. Update progress in this file

## Success Criteria

- **Target**: < 1,000 mismatches (99%+ accuracy)
- **Ideal**: 0 mismatches (100% accuracy)
- **Minimum**: < 5,000 mismatches (95%+ accuracy)

## Notes

- Focus on exact string matches
- Preserve original functionality
- Test after each major change
- Document any complex fixes

---

**Last Updated**: [Current Date]
**Current Mismatches**: 44,634
**Files Processed**: 0/642
**Estimated Time**: 2-3 days for complete fix
