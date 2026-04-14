#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QFile>
#include <QStringList>

class FileManager {
public:
    FileManager();
    ~FileManager();

    // Core methods
    QString openFile(const QString& filePath);
    void createNewFile();
    void saveFileAs(const QString& filePath, const QString& format);
    void saveFile(const QString& currentContent);

    // Getters
    QString getCurrentFilePath() const { return m_currentFilePath; }
    bool hasOpenedFile() const { return !m_currentFilePath.isEmpty(); }
    bool isValidFormat(const QString& format);
private:
    QString m_currentFilePath;
    QStringList m_supportedFormats;
};

#endif // FILEMANAGER_H
