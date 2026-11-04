#ifndef ASCIIDOC_H
#define ASCIIDOC_H

#include <string>
#include <vector>

/// @brief kind of AsciiDoc element
enum class ElementType {
    Heading,
    Paragraph,
    List
};

/// @brief one found element with its text and level
struct DocElement {
    ElementType type;
    std::string text;
    int level = 0;
};

/// @brief counters for found elements
struct DocStats {
    int headings = 0;
    int paragraphs = 0;
    int lists = 0;
};

/// @brief result of analyzing a whole file
struct FileResult {
    bool valid = true;
    std::string error;
    int lines = 0;
    DocStats stats;
    std::vector<DocElement> elements;
};

/// @brief check if a piece of text is valid AsciiDoc
/// @param text text to check
/// @param error out: error message if not valid
/// @return true if valid
bool isAsciiDoc(const std::string& text, std::string& error);

/// @brief count headings, paragraphs and lists in text
/// @param text text to analyze
/// @return stats
DocStats countAsciiDocElements(const std::string& text);

/// @brief extract all elements (headings, paragraphs, lists) from text
/// @param text text to analyze
/// @return vector of elements
std::vector<DocElement> extractAsciiDocElements(const std::string& text);

/// @brief read file in 8 MB chunks, check it and extract elements
/// @param filename path to file
/// @return everything: validity, error, line count, stats, elements
/// @throws std::runtime_error if file cannot be opened
FileResult analyzeAsciiDocFile(const std::string& filename);

#endif