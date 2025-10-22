# QElectroTech Translation TODO

## Phase 1: Non-Breaking Changes (Comments & Documentation)

### 1.1 Source Code Comments Translation

**Priority**: High | **Risk**: None | **Effort**: Medium

#### Tasks:

- [ ] **Audit French Comments**

  - Search for all `\~French` tagged comments in `sources/`
  - Create inventory of comment types (function docs, inline comments, etc.)
  - Estimate total volume of comments to translate

- [ ] **Translate Function Documentation**

  - Target files: All `.cpp` and `.h` files with `\~French` tags
  - Focus on public API documentation first
  - Maintain bilingual format: `\~English` + `\~French`

- [ ] **Translate Inline Comments**

  - Target: Single-line and multi-line comments explaining code logic
  - Priority: Complex algorithms and business logic
  - Format: Replace French with English equivalents

- [ ] **Update Doxygen Documentation**
  - Ensure all `@brief`, `@param`, `@return` descriptions are in English
  - Maintain consistency with existing English documentation

#### Implementation Strategy:

1. Use grep to find all `\~French` occurrences
2. Process files systematically by module/component
3. Test compilation after each file to ensure no syntax errors
4. Maintain git commits per logical unit (file or component)

### 1.2 UI String Translation

**Priority**: High | **Risk**: None | **Effort**: High

#### Tasks:

- [ ] **Audit UI Files**

  - Scan all `.ui` files for French strings
  - Identify hardcoded French text vs translatable strings
  - Check for missing `tr()` wrappers

- [ ] **Update Translation Files**

  - Modify `qet_en.ts` to make English the source language
  - Update `qet_fr.ts` to contain French translations
  - Ensure all UI strings are properly marked for translation

- [ ] **Fix Hardcoded Strings**
  - Wrap French strings in `tr()` or `QObject::tr()`
  - Move strings from UI files to translation files where appropriate
  - Test UI changes in both languages

#### Implementation Strategy:

1. Use Qt Linguist to manage translation files
2. Update `.ui` files to use English as source
3. Rebuild translation files using `lupdate`
4. Test UI changes in both French and English

## Phase 2: Localization Settings (System Configuration)

### 2.1 Default Language Configuration

**Priority**: High | **Risk**: Low | **Effort**: Low

#### Tasks:

- [ ] **Update Default Language Settings**

  - Change default language from `fr` to `en` in `translations_stat.pl`
  - Update `QETApp::setLanguage()` to default to English
  - Modify system language detection logic

- [ ] **Fix Component Preview Language**

  - Investigate why component previews show in French
  - Update preview generation to use English by default
  - Test component library display in English

- [ ] **Update Configuration Files**
  - Ensure `qelectrotech.sample.conf` uses English defaults
  - Update any hardcoded French strings in config files
  - Test configuration loading in English

#### Implementation Strategy:

1. Modify `misc/translations_stat.pl` line 35: `our $default_language = 'en';`
2. Update `QETApp::langFromSetting()` to default to English
3. Test language switching functionality
4. Verify component previews display in English

### 2.2 Translation System Optimization

**Priority**: Medium | **Risk**: Low | **Effort**: Medium

#### Tasks:

- [ ] **Improve Translation Loading**

  - Optimize translation file loading order
  - Ensure proper fallback from English to French
  - Add better error handling for missing translations

- [ ] **Update Translation Statistics**
  - Regenerate translation statistics with English as source
  - Update translator credits and contact information
  - Ensure all language files are up to date

## Phase 3: Breaking Changes (Code Structure)

### 3.1 Function and Variable Names

**Priority**: Medium | **Risk**: High | **Effort**: Very High

#### Tasks:

- [ ] **Audit French Identifiers**

  - Search for French function names, variables, and class names
  - Create comprehensive list of all French identifiers
  - Analyze dependencies and usage patterns

- [ ] **Plan Systematic Renaming**

  - Group identifiers by module/component
  - Create English equivalents for all French names
  - Plan migration order to minimize conflicts

- [ ] **Implement Renaming**
  - Use IDE refactoring tools where possible
  - Update all references systematically
  - Test compilation after each major change

#### Implementation Strategy:

1. Use grep to find all French identifiers: `[àâäéèêëïîôöùûüÿçÀÂÄÉÈÊËÏÎÔÖÙÛÜŸÇ]`
2. Create mapping table: French → English
3. Use systematic find/replace with careful testing
4. Maintain git history for rollback capability

### 3.2 String Literals and Messages

**Priority**: Medium | **Risk**: Medium | **Effort**: High

#### Tasks:

- [ ] **Audit String Literals**

  - Find all French strings in `tr()` calls
  - Identify hardcoded French messages
  - Plan translation strategy

- [ ] **Update Undo Commands**

  - Translate all undo command messages
  - Ensure consistent terminology
  - Test undo/redo functionality

- [ ] **Update Error Messages**
  - Translate all error and warning messages
  - Ensure clear, consistent English
  - Test error handling scenarios

## Phase 4: Testing and Validation

### 4.1 Comprehensive Testing

**Priority**: High | **Risk**: Medium | **Effort**: High

#### Tasks:

- [ ] **Compilation Testing**

  - Ensure all code compiles without errors
  - Test on multiple platforms (Windows, Linux, macOS)
  - Verify all dependencies are satisfied

- [ ] **Functionality Testing**

  - Test all major application features
  - Verify UI elements display correctly
  - Test language switching functionality

- [ ] **Translation Testing**
  - Verify all strings are properly translated
  - Test fallback mechanisms
  - Check for missing translations

### 4.2 Documentation Updates

**Priority**: Medium | **Risk**: Low | **Effort**: Medium

#### Tasks:

- [ ] **Update Project Documentation**

  - Update README files with English as primary
  - Translate installation and build instructions
  - Update contributor guidelines

- [ ] **Update Code Documentation**
  - Ensure all API documentation is in English
  - Update inline comments and examples
  - Verify Doxygen output is correct

## Implementation Guidelines

### General Principles:

1. **Non-Breaking First**: Always start with non-breaking changes
2. **Test Frequently**: Test after each logical unit of changes
3. **Maintain History**: Use git commits for easy rollback
4. **Consistent Terminology**: Use established terminology dictionary
5. **Incremental Approach**: Make changes in small, manageable chunks

### Quality Assurance:

- All changes must compile without errors
- All UI changes must be tested in both languages
- All functionality must work as expected
- Translation completeness must be verified

### Risk Mitigation:

- Maintain backup branches for each phase
- Test on multiple platforms and configurations
- Have rollback plan for each major change
- Document all changes thoroughly

---

_Last Updated: $(date)_
_Status: Planning Phase_
