#include <vector>

#include "actuator.h"

using namespace std;

FourierGroupFeedbackPtr fourierGroupFeedbackCreate(size_t size) {
  FourierGroupFeedbackPtr ptr = new FourierGroupFeedback_();
  for (int i = 0; i < size; i++) {
    ptr->feedback.push_back(new Feedback_());
  }

  return ptr;
}

size_t fourierGroupFeedbackGetSize(FourierGroupFeedbackPtr feedback) {
  return feedback->feedback.size();
}

FourierFeedbackPtr fourierGroupFeedbackGetModuleFeedback(
    FourierGroupFeedbackPtr feedback, size_t module_index) {
  return feedback->feedback.at(module_index);
}

void fourierGroupFeedbackRelease(FourierGroupFeedbackPtr feedback) {
  for (auto elem : feedback->feedback) {
    delete elem;
  }
  delete feedback;
}
