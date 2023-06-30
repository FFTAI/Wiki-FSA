#include "groupFeedback.hpp"

namespace Fourier {

GroupFeedback::GroupFeedback(size_t number_of_modules)
    : internal_(fourierGroupFeedbackCreate(number_of_modules)),
      manage_pointer_lifetime_(true),
      number_of_modules_(number_of_modules) {
  for (size_t i = 0; i < number_of_modules_; i++)
    feedbacks_.emplace_back(
        fourierGroupFeedbackGetModuleFeedback(internal_, i));
}

GroupFeedback::GroupFeedback(FourierGroupFeedbackPtr group_feedback)
    : internal_(group_feedback),
      manage_pointer_lifetime_(false),
      number_of_modules_(fourierGroupFeedbackGetSize(group_feedback)) {
  for (size_t i = 0; i < number_of_modules_; i++)
    feedbacks_.emplace_back(
        fourierGroupFeedbackGetModuleFeedback(internal_, i));
}

GroupFeedback::~GroupFeedback() noexcept {
  if (manage_pointer_lifetime_ && internal_ != nullptr)
    fourierGroupFeedbackRelease(internal_);
}

size_t GroupFeedback::size() const { return number_of_modules_; }

const FourierFeedbackPtr &GroupFeedback::operator[](size_t index) const {
  return feedbacks_[index];
}

}  // namespace Fourier
