#include "asciidoc.h"
#include "common.h"
#include <fstream>
#include <functional>
#include <stdexcept>

/// @brief internal: read file in 8 MB chunks, call cb on each complete-lines chunk
static void readInChunks(const std::string& filename,
                         const std::function<void(const std::string&)>& cb) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        throw std::runtime_error("error: file wasnt open " + filename);
    }

    const size_t CHUNK_SIZE = 8 * 1024 * 1024;
    std::vector<char> buffer(CHUNK_SIZE);
    std::string leftover;

    while (in.read(buffer.data(), CHUNK_SIZE) || in.gcount() > 0) {
        size_t bytesRead = in.gcount();
        if (bytesRead == 0) break;

        std::string chunk = leftover + std::string(buffer.data(), bytesRead);

        size_t lastNewline = chunk.find_last_of('\n');
        if (lastNewline == std::string::npos) {
            leftover = chunk;
            continue;
        }

        cb(chunk.substr(0, lastNewline + 1));
        leftover = chunk.substr(lastNewline + 1);
    }

    if (!leftover.empty()) cb(leftover);
}

bool isAsciiDoc(const std::string& text, std::string& error) {
    bool hasAsciiDocElements = false;
    size_t start = 0;

    while (start < text.size()) {
        size_t end = text.find('\n', start);
        if (end == std::string::npos) end = text.size();

        size_t firstChar = start;
        while (firstChar < end && (text[firstChar] == ' ' || text[firstChar] == '\t' || text[firstChar] == '\r')) {
            firstChar++;
        }

        size_t lastChar = end;
        while (lastChar > firstChar && (text[lastChar - 1] == ' ' || text[lastChar - 1] == '\t' || text[lastChar - 1] == '\r')) {
            lastChar--;
        }

        if (firstChar < lastChar) {
            char c = text[firstChar];
            char lastC = text[lastChar - 1];

            if (c == '<' && lastC == '>') { error = "Обнаружен HTML-тег!"; return false; }
            if (c == '#')                  { error = "Обнаружен Markdown!";  return false; }

            if (c == '=' || c == '*' || c == '.') {
                hasAsciiDocElements = true;
            }
        }
        start = end + 1;
    }

    if (!hasAsciiDocElements) {
        error = "Обычный текст: не найдено ни одного элемента AsciiDoc.";
        return false;
    }
    return true;
}

DocStats countAsciiDocElements(const std::string& text) {
    DocStats stats;
    size_t start = 0;

    while (start < text.size()) {
        size_t end = text.find('\n', start);
        if (end == std::string::npos) end = text.size();

        size_t firstChar = start;
        while (firstChar < end && (text[firstChar] == ' ' || text[firstChar] == '\t' || text[firstChar] == '\r')) {
            firstChar++;
        }

        if (firstChar < end) {
            char c = text[firstChar];
            if (c == '=')                     stats.headings++;
            else if (c == '*' || c == '.')    stats.lists++;
            else if (c != ':')                stats.paragraphs++;
        }
        start = end + 1;
    }
    return stats;
}

std::vector<DocElement> extractAsciiDocElements(const std::string& text) {
    std::vector<DocElement> els;
    size_t start = 0;

    while (start < text.size()) {
        size_t end = text.find('\n', start);
        if (end == std::string::npos) end = text.size();

        std::string line = trim(text.substr(start, end - start));

        if (!line.empty() && line[0] != ':') {
            if (line[0] == '=') {
                int lvl = 0;
                while (lvl < (int)line.length() && line[lvl] == '=') lvl++;
                els.push_back({ ElementType::Heading, trim(line.substr(lvl)), lvl });
            }
            else if (line[0] == '*' || line[0] == '.') {
                els.push_back({ ElementType::List, trim(line.substr(1)), 1 });
            }
            else {
                els.push_back({ ElementType::Paragraph, line, 0 });
            }
        }
        start = end + 1;
    }
    return els;
}

FileResult analyzeAsciiDocFile(const std::string& filename) {
    FileResult res;

    readInChunks(filename, [&](const std::string& chunk) {
        for (char c : chunk) if (c == '\n') res.lines++;

        std::string err;
        if (res.valid && !isAsciiDoc(chunk, err)) {
            res.valid = false;
            res.error = err;
        }

        DocStats s = countAsciiDocElements(chunk);
        res.stats.headings   += s.headings;
        res.stats.paragraphs += s.paragraphs;
        res.stats.lists      += s.lists;

        auto els = extractAsciiDocElements(chunk);
        res.elements.insert(res.elements.end(), els.begin(), els.end());
    });

    if (res.valid && res.elements.empty()) {
        res.valid = false;
        res.error = "Обычный текст: не найдено ни одного элемента AsciiDoc.";
    }

    return res;
}