#pragma once
#include <memory>

#include "aios.h"
#include "group.hpp"
#include "util.hpp"

namespace Fourier {

/**
 * @brief Maintains a registry of network-connected modules and returns Group
 * objects to the user.
 *
 * Only one Lookup object is needed per application.
 */
class Lookup final {
 public:
  /**
   * @brief Creates a Lookup object which can create Module and Group
   * references.
   * Typically, only one Lookup object should exist at a time.
   *
   * Note that this call invokes a background thread to query the network for
   * modules at regular intervals.
   */
  Lookup(std::string *addr = nullptr);

  /**
   * @brief Destructor frees all resources created by Lookup object, and stops
   * the background query thread.
   */
  ~Lookup() noexcept;

  /**
   * @brief Sets the broadcast address for the lookup.
   *
   * @param networks A string pointer to a broadcast address.
   *             eg: "192.168.100.255,192.168.101.255"
   */
  void setNetWorks(std::string networks);

  /**
   * @brief Get CtrlBox ip address.
   */
  std::string getCtrlBoxIP();

  /**
   * @brief Get a group from all known modules with the given family.
   *
   * @param family The family of each of the desired group modules.
   * @returns A shared_ptr with no reference if no group found in allotted
   * time, or reference to a newly allocated group object corresponding to
   * the given parameters otherwise.
   */
  std::shared_ptr<Group> getGroupFromFamily(
      const std::string &family, int32_t timeout_ms = DEFAULT_TIMEOUT);

  /**
   * @brief Gets the rate [Hz] at which "discovery" packets are broadcast.
   *
   * Defaults to 5 Hz.
   */
  float getLookupFrequencyHz() const;

  /**
   * @brief Sets the lookup rate [Hz]
   *
   * @param frequency The rate at which "discovery" packets get broadcast on
   * the network to search for modules.
   *
   * \returns true on success, false on failure (e.g., invalid frequency)
   */
  bool setLookupFrequencyHz(float frequency);

  class EntryList final {
    struct Entry final {
      std::string name_;
      std::string family_;
      std::string serial_number_;
    };

   private:
    /**
     * \internal C-style lookup entry list object
     */
    FourierLookupEntryListPtr lookup_list_;

    /**
     * \internal Entry list iterator implementation
     * (see http://anderberg.me/2016/07/04/c-custom-iterators/)
     */
    class Iterator final {
     public:
      // Iterator traits (not from std::iterator to be C++17 compliant)
      using value_type = Entry;
      using difference_type = int;
      using pointer = Entry *;
      using reference = Entry;
      using iterator_category = std::bidirectional_iterator_tag;

      Iterator() = default;
      explicit Iterator(const EntryList &list, size_t current);

      reference operator*() const;

      Iterator &operator++();
      Iterator operator++(int);
      Iterator &operator--();
      Iterator operator--(int);

      bool operator==(const Iterator &rhs) const;
      bool operator!=(const Iterator &rhs) const;

     private:
      const EntryList &list_;
      size_t current_{0};
    };

   public:
    /**
     * \internal Creates entry list from internal C-style object.
     */
    EntryList(FourierLookupEntryListPtr lookup_list)
        : lookup_list_(lookup_list) {}

    ~EntryList() noexcept;

    Entry operator[](size_t index) const;

    size_t size() const;

    Iterator begin() const;
    Iterator end() const;

   private:
    /**
     * Disable copy and move constructors and assignment operators
     */
    FOURIER_DISABLE_COPY_MOVE(EntryList)
  };

  std::shared_ptr<EntryList> getEntryList();

 private:
  /**
   * \internal C-style lookup object
   */
  FourierLookupPtr lookup_;

  static const int32_t DEFAULT_TIMEOUT = 500;
};

}  // namespace Fourier
