
#include "lookup.hpp"

#include <string.h>

#include <algorithm>

namespace Fourier {

Lookup::Lookup(std::string *addr) {
  const char *tmp = nullptr;
  size_t size = 0;
  if (addr) {
    tmp = addr->c_str();
    size = strlen(tmp);
  }
  lookup_ = fourierLookupCreate(tmp, size);
}

Lookup::~Lookup() noexcept { fourierLookupRelease(lookup_); }

void Lookup::setNetWorks(std::string networks) {
  fourierLookupSetNetworks(lookup_, strdup(networks.c_str()));
}

std::string Lookup::getCtrlBoxIP() {
  return fourierLookupGetCtrlBoxIP(lookup_);
}

std::shared_ptr<Group> Lookup::getGroupFromFamily(const std::string &family,
                                                  int32_t timeout_ms) {
  std::shared_ptr<Group> ptr;
  FourierGroupPtr group =
      fourierGroupCreateFromFamily(lookup_, family.c_str(), timeout_ms);
  if (group != nullptr) return std::make_shared<Group>(group);
  return ptr;
}

float Lookup::getLookupFrequencyHz() const {
  return fourierLookupGetLookupFrequencyHz(lookup_);
}

bool Lookup::setLookupFrequencyHz(float frequency) {
  return fourierLookupSetLookupFrequencyHz(lookup_, frequency) ==
         FourierStatusSuccess;
}

Lookup::EntryList::Iterator::Iterator(const EntryList &list, size_t current)
    : list_(list), current_(current) {}

Lookup::EntryList::Iterator::reference Lookup::EntryList::Iterator::operator*()
    const {
  return list_[current_];
}

Lookup::EntryList::Iterator &Lookup::EntryList::Iterator::operator++() {
  ++current_;
  return *this;
}

Lookup::EntryList::Iterator Lookup::EntryList::Iterator::operator++(int) {
  Lookup::EntryList::Iterator tmp = *this;
  ++current_;
  return tmp;
}

Lookup::EntryList::Iterator &Lookup::EntryList::Iterator::operator--() {
  --current_;
  return *this;
}

Lookup::EntryList::Iterator Lookup::EntryList::Iterator::operator--(int) {
  Lookup::EntryList::Iterator tmp = *this;
  --current_;
  return tmp;
}

bool Lookup::EntryList::Iterator::operator==(
    const Lookup::EntryList::Iterator &rhs) const {
  return this->current_ == rhs.current_;
}

bool Lookup::EntryList::Iterator::operator!=(
    const Lookup::EntryList::Iterator &rhs) const {
  return !(*this == rhs);
}

Lookup::EntryList::~EntryList() noexcept {
  fourierLookupEntryListRelease(lookup_list_);
  lookup_list_ = nullptr;
}

Lookup::EntryList::Entry Lookup::EntryList::operator[](size_t index) const {
  size_t len;

  fourierLookupEntryListGetName(lookup_list_, index, nullptr, &len);
  char *name_ = new char[len];
  fourierLookupEntryListGetName(lookup_list_, index, name_, &len);
  std::string name(name_, len);
  delete[] name_;

  fourierLookupEntryListGetFamily(lookup_list_, index, nullptr, &len);
  char *family_ = new char[len];
  fourierLookupEntryListGetFamily(lookup_list_, index, family_, &len);
  std::string family(family_, len);
  delete[] family_;

  fourierLookupEntryListGetSerialNumber(lookup_list_, index, nullptr, &len);
  char *serial_number_ = new char[len];
  fourierLookupEntryListGetSerialNumber(lookup_list_, index, serial_number_,
                                        &len);
  std::string serial_number(serial_number_, len);
  delete[] serial_number_;

  Entry e = {name, family, serial_number};
  return e;
}

size_t Lookup::EntryList::size() const {
  return fourierLookupEntryListGetSize(lookup_list_);
}

Lookup::EntryList::Iterator Lookup::EntryList::begin() const {
  return Lookup::EntryList::Iterator(*this, 0);
}

Lookup::EntryList::Iterator Lookup::EntryList::end() const {
  return Lookup::EntryList::Iterator(*this, size());
}

std::shared_ptr<Lookup::EntryList> Lookup::getEntryList() {
  std::shared_ptr<Lookup::EntryList> ptr;
  auto entry_list = fourierCreateLookupEntryList(lookup_);
  if (entry_list != nullptr)
    return std::make_shared<Lookup::EntryList>(entry_list);
  return ptr;
}

}  // namespace Fourier
