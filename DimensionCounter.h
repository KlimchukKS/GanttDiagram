#ifndef DIMENSIONCOUNTER_H
#define DIMENSIONCOUNTER_H

#include <QObject>

class DimensionCounter : public QObject
{
    Q_OBJECT
public:
    DimensionCounter() = delete;

    explicit DimensionCounter(char value);

    char GetDimension() const;

    void SetDimension(char value);

    void operator=(char value);

    bool operator==(char value) const;

    void operator++();

    void operator--();

signals:
    void DimensionChanged();

private:
    char dimension_;
};


#endif // DIMENSIONCOUNTER_H
