#include "aios.h"

FourierStatusCode fourierGetLibraryVersion(int32_t *major, int32_t *minor,
                                           int32_t *revision) {
  if (major == nullptr || minor == nullptr || revision == nullptr) {
    return FourierStatusFailure;
  }
  // VERSION
  *major = 0;
  *minor = 7;
  *revision = 1;
  return FourierStatusSuccess;
}
