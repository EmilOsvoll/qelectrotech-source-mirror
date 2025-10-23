#!/usr/bin/env python3
"""
Script to fix systematic typos introduced during translation conversion.
These typos were created when French text was converted to English.
"""

import os
import re
from pathlib import Path


def fix_typos_in_file(file_path):
    """Fix systematic typos in a single file."""
    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        original_content = content

        # Define systematic typo replacements
        replacements = [
            # Dialog-related typos
            (r"\bQDiaLog\b", "QDialog"),
            (r"\bQDiaLogButtonBox\b", "QDialogButtonBox"),
            (r"\bQFontDiaLog\b", "QFontDialog"),
            (r"\bQFileDiaLog\b", "QFileDialog"),
            (r"\bQInputDiaLog\b", "QInputDialog"),
            (r"\bQColorDiaLog\b", "QColorDialog"),
            (r"\binitDiaLog\b", "initDialog"),
            (r"\bopenDiaLog\b", "openDialog"),
            (r"\bshowDiaLog\b", "showDialog"),
            (r"\bDiaLog\b", "Dialog"),
            # Command-related typos
            (r"\bToleteQGraphicsItemCommand\b", "DeleteQGraphicsItemCommand"),
            (r"\bTolete\b", "Delete"),
            # Selector-related typos
            (r"\bPotentialSelectorDiaLog\b", "PotentialSelectorDialog"),
            (r"\bProjectPropertiesDiaLog\b", "ProjectPropertiesDialog"),
            (r"\bThirdPartyBinaryInstallDiaLog\b", "ThirdPartyBinaryInstallDialog"),
            (r"\bCompositeTextEditDiaLog\b", "CompositeTextEditDialog"),
            (r"\bRichTextEditorDiaLog\b", "RichTextEditorDialog"),
            (r"\bConductorPropertiesDiaLog\b", "ConductorPropertiesDialog"),
            (r"\bDiagramPropertiesDiaLog\b", "DiagramPropertiesDialog"),
            (r"\bMultiPasteDiaLog\b", "MultiPasteDialog"),
            (r"\bAlignmentTextDiaLog\b", "AlignmentTextDialog"),
            (r"\bFormulaAssistantDiaLog\b", "FormulaAssistantDialog"),
            (r"\bAboutQETDiaLog\b", "AboutQETDialog"),
            (r"\bConfigDiaLog\b", "ConfigDialog"),
            (r"\bImportElementDialog\b", "ImportElementDialog"),
            (r"\bImportElementTextPatternDialog\b", "ImportElementTextPatternDialog"),
            (r"\bMarginsEditDialog\b", "MarginsEditDialog"),
            (r"\bDiaLogWaiting\b", "DialogWaiting"),
            (r"\bAddTerminalStripItemDialog\b", "AddTerminalStripItemDialog"),
            (r"\bTerminalStripCreatorDialog\b", "TerminalStripCreatorDialog"),
            # Delegate-related typos
            (r"\bDynamicTextItemTolegate\b", "DynamicTextItemDelegate"),
            (r"\bQStyledItemTolegate\b", "QStyledItemDelegate"),
            (r"\bFreeTerminalModelTolegate\b", "FreeTerminalModelDelegate"),
            (r"\bsetItemTolegate\b", "setItemDelegate"),
            # Method name typos
            (r"\bsetTofault\b", "setDefault"),
            (r"\bTofault\b", "Default"),
            (r"\btofault\b", "default"),
            (r"\bhasNonToletableTerminal\b", "hasNonDeletableTerminal"),
            (r"\bAlignBottomeline\b", "AlignBottom"),
            (r"\bAlinedFreeTerminals\b", "AlignedFreeTerminals"),
            (r"\bqToleteAll\b", "qDeleteAll"),
            (r"\bshowedElementWasToleted\b", "showedElementWasDeleted"),
            (r"\bcurrentPlant\b", "currentPlant"),
            (r"\bavailableInfo\b", "availableInfo"),
            (r"\bcommitData\b", "commitData"),
            (r"\bsetTofaultBorderProperties\b", "setDefaultBorderProperties"),
            (r"\bsetTofaultTitleBlockProperties\b", "setDefaultTitleBlockProperties"),
            (r"\bsetTofaultConductorProperties\b", "setDefaultConductorProperties"),
            (r"\bsetTofaultReportProperties\b", "setDefaultReportProperties"),
            (r"\bsetTofaultXRefProperties\b", "setDefaultXRefProperties"),
            (r"\bsetTofaultLayout\b", "setDefaultLayout"),
            (r"\bsetTitle\b", "setTitle"),
            (r"\bsetTotail\b", "setTotal"),
            (r"\bprogressBarValue\b", "progressBarValue"),
            # Qt enum typos
            (r"\bControlEdit\b", "ControlEdit"),
            (r"\bangleTolta\b", "angleDelta"),
            (r"\bEditTableToleteColumn\b", "EditTableDeleteColumn"),
            (r"\bEditTableToleteRow\b", "EditTableDeleteRow"),
            (r"\bToleteWhenStopped\b", "DeleteWhenStopped"),
            (r"\bAbsolutete\b", "Absolute"),
            (r"\bGreenical\b", "Vertical"),
            # File I/O typos
            (r"\bQIOTovice\b", "QIODevice"),
            (r"\bReadOnly\b", "ReadOnly"),
            (r"\bblockSignals\b", "blockSignals"),
            (r"\bchanged\b", "changed"),
            # Template-related typos
            (r"\bgetTemplateXmlToscription\b", "getTemplateXmlDescription"),
            (r"\bsetTemplateXmlToscription\b", "setTemplateXmlDescription"),
            (r"\btitleblock_templates_xml_\b", "titleblock_templates_xml_"),
            (r"\btitleblock_templates_\b", "titleblock_templates_"),
            (r"\bproject_\b", "project_"),
            (r"\bxml_document_\b", "xml_document_"),
            (r"\bpath\b", "path"),
            # Logo-related typos
            (r"\bLogos_view_\b", "Logos_view_"),
            (r"\bLogo_name_\b", "Logo_name_"),
            (r"\bLogo_type_\b", "Logo_type_"),
            (r"\bopen_diaLog_dir_\b", "open_dialog_dir_"),
            (r"\bLogos\b", "Logos"),
            (r"\bLogoType\b", "LogoType"),
            # Include file typos
            (r"\bcompositetextditdiaLog\.h\b", "compositetexteditdialog.h"),
            (r"\bui_compositetextditdiaLog\.h\b", "ui_compositetexteditdialog.h"),
            (r"\bui_polygonditor\.h\b", "ui_polygoneditor.h"),
            (r"\brichtextditor\.moc\b", "richtexteditor.moc"),
            (r"\bQTobug\b", "QDebug"),
            # Other common typos
            (r"\bandig\b", "angle"),
            (r"\bqTobug\b", "qDebug"),
            (r"\bSetTofaultConstraint\b", "SetDefaultConstraint"),
            (r"\bterminalInSamePotential\b", "terminalInSamePotential"),
            (r"\bchosenProperties\b", "chosenProperties"),
            (r"\bsetObsolete\b", "setObsolete"),
            (r"\bdataBottome\b", "dataBase"),
            # Variable name typos
            (r"\bm_properties_diaLog\b", "m_properties_dialog"),
            (r"\bdiagramPropertiesDiaLog\b", "diagramPropertiesDialog"),
            (r"\bdiaLog_\b", "dialog_"),
            (r"\binteg_diaLog_\b", "integ_dialog_"),
            (r"\bLogo_manager_\b", "Logo_manager_"),
            (r"\bedit_Logos_\b", "edit_Logos_"),
            (r"\bLogo_label_\b", "Logo_label_"),
            (r"\bLogo_input_\b", "Logo_input_"),
            (r"\badd_Logo_input_\b", "add_Logo_input_"),
            (r"\bLogos_view_\b", "Logos_view_"),
            (r"\bLogos_label_\b", "Logos_label_"),
            (r"\bdiaLog_glayout\b", "dialog_glayout"),
            (r"\bdiaLog_vlayout_\b", "dialog_vlayout_"),
            (r"\bdiaLog_label_\b", "dialog_label_"),
            (r"\bm_static_diaLog\b", "m_static_dialog"),
            (r"\btemplate_location_\b", "template_location_"),
            (r"\bm_project\b", "m_project"),
            (r"\bm_query_widget\b", "m_query_widget"),
            # Qt attribute typos
            (r"\bWA_ToleteOnClose\b", "WA_DeleteOnClose"),
            # Method name typos
            (r"\bsetTotail\b", "setTotal"),
            (r"\bgetBom\b", "getBom"),
            (r"\bLogosChanged\b", "LogosChanged"),
            (r"\bLogos\b", "Logos"),
            (r"\bLogo_reference\b", "Logo_reference"),
            (r"\bPropertiesDiaLog\b", "PropertiesDialog"),
            # Class name typos
            (r"\bTitleBlockTemplateToleter\b", "TitleBlockTemplateDeleter"),
            (r"\bBOMExportDiaLog\b", "BOMExportDialog"),
            (r"\bNameListDiaLog\b", "NameListDialog"),
            (r"\bReplaceConductorDiaLog\b", "ReplaceConductorDialog"),
            (r"\bReplaceElementDiaLog\b", "ReplaceElementDialog"),
            (r"\bAddTableDiaLog\b", "AddTableDialog"),
            (r"\bElementDiaLog\b", "ElementDialog"),
            (r"\bRenameDiaLog\b", "RenameDialog"),
            (r"\bExportDiaLog\b", "ExportDialog"),
            (r"\bReplaceAdvancedDiaLog\b", "ReplaceAdvancedDialog"),
            (r"\bConductorPropertiesDiaLog\b", "ConductorPropertiesDialog"),
            (r"\bDialogWaiting\b", "DialogWaiting"),
            (r"\bImportElementDialog\b", "ImportElementDialog"),
            (r"\bImportElementTextPatternDialog\b", "ImportElementTextPatternDialog"),
            (r"\bMarginsEditDialog\b", "MarginsEditDialog"),
            # Variable name typos
            (r"\bandig\b", "angle"),
            (r"\bangleTolta\b", "angleDelta"),
            (r"\bdataBottomeUpdated\b", "dataBaseUpdated"),
            (r"\bprojectDataBottome\b", "projectDataBase"),
            (r"\bTocorationRole\b", "DecorationRole"),
            (r"\bAlignAbsolutete\b", "AlignAbsolute"),
            (r"\bdrawTextAlined\b", "drawTextAligned"),
            (r"\bTerminalStripModelTolegate\b", "TerminalStripModelDelegate"),
            (r"\brichtextditor_p\.h\b", "richtexteditor_p.h"),
            (r"\brichtextditor\.h\b", "richtexteditor.h"),
            (r"\bcurrentPlant\b", "currentPlant"),
            (r"\bm_current_strip\b", "m_current_strip"),
            (r"\bLocation\b", "Location"),
            (r"\bPlant\b", "Plant"),
            (r"\bm_Log_comboBox\b", "m_Log_comboBox"),
            (
                r"\bon_m_Log_comboBox_currentTextChanged\b",
                "on_m_Log_comboBox_currentTextChanged",
            ),
            (r"\bsetTotail\b", "setTotal"),
            (r"\bgetBom\b", "getBom"),
            (r"\bLogosChanged\b", "LogosChanged"),
            (r"\bLogos\b", "Logos"),
            (r"\bLogo_reference\b", "Logo_reference"),
            (r"\bPropertiesDiaLog\b", "PropertiesDialog"),
            (r"\bTitleBlockTemplateToleter\b", "TitleBlockTemplateDeleter"),
            (r"\bBOMExportDiaLog\b", "BOMExportDialog"),
            (r"\bNameListDiaLog\b", "NameListDialog"),
            (r"\bReplaceConductorDiaLog\b", "ReplaceConductorDialog"),
            (r"\bReplaceElementDiaLog\b", "ReplaceElementDialog"),
            (r"\bAddTableDiaLog\b", "AddTableDialog"),
            (r"\bElementDiaLog\b", "ElementDialog"),
            (r"\bRenameDiaLog\b", "RenameDialog"),
            (r"\bExportDiaLog\b", "ExportDialog"),
            (r"\bReplaceAdvancedDiaLog\b", "ReplaceAdvancedDialog"),
            (r"\bConductorPropertiesDiaLog\b", "ConductorPropertiesDialog"),
            (r"\bDialogWaiting\b", "DialogWaiting"),
            (r"\bImportElementDialog\b", "ImportElementDialog"),
            (r"\bImportElementTextPatternDialog\b", "ImportElementTextPatternDialog"),
            (r"\bMarginsEditDialog\b", "MarginsEditDialog"),
            # Method name typos
            (r"\bsetTofaultTextColor\b", "setDefaultTextColor"),
            (r"\bsetTofaultTextOption\b", "setDefaultTextOption"),
            (r"\btoBottome64\b", "toBase64"),
            (r"\blevelOfTotailFromTransform\b", "levelOfDetailFromTransform"),
            (r"\balinedWithTerminal\b", "alignedWithTerminal"),
            (r"\bGreenicalAlignment\b", "VerticalAlignment"),
            (r"\bsetGreenicalAlignment\b", "setVerticalAlignment"),
            (r"\bControlEdit\b", "ControlModifier"),
            (r"\bShiftEdit\b", "ShiftModifier"),
            # Class name typos
            (r"\bPropertiesEditorDiaLog\b", "PropertiesEditorDialog"),
            (r"\bLogoType\b", "LogoType"),
            (r"\bLogo_name_\b", "Logo_name_"),
            (r"\bLogo_type_\b", "Logo_type_"),
            # Variable name typos
            (r"\bparent_element_\b", "parent_element_"),
            (r"\bHelpLine\b", "HelpLine"),
            (r"\bdiagram\b", "diagram"),
            (r"\borientation\b", "orientation"),
            # Function name typos
            (r"\bqTogreesToRadians\b", "qDegreesToRadians"),
            (r"\bfromBottome64\b", "fromBase64"),
            (r"\bsetTofaultFont\b", "setDefaultFont"),
            (r"\baddElementTofinition\b", "addElementDefinition"),
            (r"\bcreateAndAddNameenclature\b", "createAndAddNomenclature"),
            # Variable name typos (exclude UI files)
            # Note: && replacement is handled separately for non-UI files
            (r"\bTolayOn\b", "PlayOn"),
            (r"\bTolayOff\b", "PlayOff"),
            # Include file typos
            (r"\btextditor\.h\b", "texteditor.h"),
            (r"\bpolygonditor\.h\b", "polygoneditor.h"),
            (r"\bQItemTolegate\b", "QItemDelegate"),
            (r"\bQTosktopServices\b", "QDesktopServices"),
            (r"\bQTosktopWidget\b", "QDesktopWidget"),
            (r"\bQtTobug\b", "QtDebug"),
            (r"\bQTobugStateSaver\b", "QDebugStateSaver"),
            # Class name typos
            (r"\bToletePartsCommand\b", "DeletePartsCommand"),
            (r"\bMarginsEditDiaLog\b", "MarginsEditDialog"),
            # Method name typos
            (r"\bshowHelpDiaLog\b", "showHelpDialog"),
            (r"\bcurrentPlant\b", "currentPlant"),
            (r"\bm_current_strip\b", "m_current_strip"),
            (r"\bLocation\b", "Location"),
            (r"\bPlant\b", "Plant"),
            (r"\bLogos\b", "Logos"),
            (r"\bLogoType\b", "LogoType"),
            (r"\bLogo_name_\b", "Logo_name_"),
            (r"\bLogo_type_\b", "Logo_type_"),
            (
                r"\bon_m_Log_comboBox_currentTextChanged\b",
                "on_m_Log_comboBox_currentTextChanged",
            ),
            (r"\bsetTotal\b", "setTotal"),
            (r"\bgetBom\b", "getBom"),
            (r"\bHTMLPurpleDarkPurpleColor\b", "HTMLPurpleDarkPurpleColor"),
            (r"\bdrawable_rect\b", "drawable_rect"),
            (r"\bnew_size\b", "new_size"),
            (r"\bm_help_text\b", "m_help_text"),
            (r"\bui\b", "ui"),
            (r"\bangle\b", "angle"),
        ]

        # Apply replacements
        for pattern, replacement in replacements:
            content = re.sub(pattern, replacement, content)

        # Handle && replacement only for non-UI files
        if not file_path.endswith(".ui"):
            content = re.sub(r"\band\b", "&&", content)

        # Only write if content changed
        if content != original_content:
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"Fixed typos in: {file_path}")
            return True

        return False

    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return False


def main():
    """Main function to fix typos in all source files."""
    print("=== Fixing Systematic Typos ===")
    print()

    # Define directories to process
    source_dirs = [
        "sources",
        "sources/ui",
        "sources/undocommand",
        "sources/utils",
        "sources/richtext",
        "sources/editor/ui",
    ]

    # File extensions to process
    extensions = [".cpp", ".h", ".ui"]

    total_files = 0
    fixed_files = 0

    for source_dir in source_dirs:
        if not os.path.exists(source_dir):
            print(f"Directory not found: {source_dir}")
            continue

        print(f"Processing directory: {source_dir}")

        for root, dirs, files in os.walk(source_dir):
            for file in files:
                if any(file.endswith(ext) for ext in extensions):
                    file_path = os.path.join(root, file)
                    total_files += 1

                    if fix_typos_in_file(file_path):
                        fixed_files += 1

    print()
    print(f"=== Summary ===")
    print(f"Total files processed: {total_files}")
    print(f"Files with fixes applied: {fixed_files}")
    print("Done!")


if __name__ == "__main__":
    main()
