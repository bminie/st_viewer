#ifndef DATASET_H
#define DATASET_H

#include <QString>
#include <QTransform>
#include <QSharedPointer>

class STData;
class DatasetImporter;

// Data model class to store datasets.
class Dataset
{

public:
    Dataset();
    Dataset(const DatasetImporter &importer);
    Dataset(const Dataset &other);
    ~Dataset();

    Dataset &operator=(const Dataset &other);
    bool operator==(const Dataset &other) const;
    // The reference to the ST Data matrix
    const QSharedPointer<STData> data() const;
    // Getters
    const QString name() const;
    const QString dataFile() const;
    const QTransform imageAlignment() const;
    const QString imageAlignmentFile() const;
    const QString imageFile() const;
    const QString spotsFile() const;
    const QString statTissue() const;
    const QString statSpecies() const;
    const QString statComments() const;
    // Setters
    void name(const QString &name);
    void dataFile(const QString &data);
    void imageAlignment(const QTransform &alignmentId);
    void imageAlignmentFile(const QString &aligment_file);
    void imageFile(const QString &image_file);
    void spotsFile(const QString &spots_file);
    void statTissue(const QString &statTissue);
    void statSpecies(const QString &statSpecies);
    void statComments(const QString &statComments);

    // creates the STData object (parse data)
    bool load_data();

private:
    QString m_name;
    QString m_statTissue;
    QString m_statSpecies;
    QString m_statComments;
    QString m_data_file;
    QString m_image_file;
    QString m_alignment_file;
    QString m_spots_file;
    // generated
    QTransform m_alignment;
    QSharedPointer<STData> m_data;
};

#endif // DATASET_H