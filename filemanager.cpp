#include "filemanager.h"
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <stdexcept>

FileManager::FileManager() {
    m_currentFilePath = "";
    // Define supported formats for the "Save As" logic
    m_supportedFormats << "txt" << "cpp" << "h" << "html" << "md";
}

FileManager::~FileManager() {
    // Standard cleanup if necessary
}

// 1. Opening an existing file
QString FileManager::openFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Could not open file for reading.");
    }

    m_currentFilePath = filePath;
    QTextStream in(&file);
    return in.readAll();
}

// 2. Create new file (Reset state)
void FileManager::createNewFile() {
    m_currentFilePath = "";
}

// 3. Save As... (With directory and format validation)
void FileManager::saveFileAs(const QString& filePath, const QString& format) {
    QFileInfo fileInfo(filePath);
    QDir dir = fileInfo.dir();

    // Check if location exists
    if (!dir.exists()) {
        throw std::runtime_error("Destination directory does not exist.");
    }

    // Check if format is supported
    if (!isValidFormat(format)) {
        throw std::runtime_error("Unsupported file format: " + format.toStdString());
    }

    m_currentFilePath = filePath;
    // Note: The content writing logic would typically be called from saveFile()
}

// 4. Save
void FileManager::saveFile(const QString& currentContent) {
    if (m_currentFilePath.isEmpty()) {
        // Logic: In the UI layer, you'd trigger a QFileDialog here
        // to get the path/format and then call saveFileAs.
        throw std::runtime_error("No file opened. Use 'Save As' instead.");
    }

    QFile file(m_currentFilePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        throw std::runtime_error("Failed to write to file.");
    }

    QTextStream out(&file);
    out << currentContent;
    file.close();
}

// Helper to check formats
bool FileManager::isValidFormat(const QString& format) {
    return m_supportedFormats.contains(format.toLower().remove('.'));
}
