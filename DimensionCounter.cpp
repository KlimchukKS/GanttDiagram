#include "DimensionCounter.h"

DimensionCounter::DimensionCounter(char value)
    : dimension_{value}
{

}

char DimensionCounter::GetDimension() const {
    return dimension_;
}

void DimensionCounter::SetDimension(char value) {
    dimension_ = value;

    emit DimensionChanged();
}

void DimensionCounter::operator=(char value) {
    this->SetDimension(value);
}

bool DimensionCounter::operator==(char value) const {
    return dimension_ == value;
}

void DimensionCounter::operator++() {
    switch (dimension_) {
    case 'S':
        dimension_ = 'M';
        break;
    case 'M':
        dimension_ = 'H';
        break;
    case 'H':
        dimension_ = 'd';
        break;
    case 'd':
    case 'D':
        dimension_ = 'm';
        break;
    case 'm':
        dimension_ = 'y';
    }

    emit DimensionChanged();
}

void DimensionCounter::operator--() {
    switch (dimension_) {
    case 'y':
        dimension_ = 'm';
        break;
    case 'm':
        dimension_ = 'd';
        break;
    case 'd':
    case 'D':
        dimension_ = 'H';
        break;
    case 'H':
        dimension_ = 'M';
        break;
    case 'M':
        dimension_ = 'S';
    }

    emit DimensionChanged();
}
