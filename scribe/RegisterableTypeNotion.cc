#include <scribe/RegisterableTypeNotion.h>
#include <scribe/makeString.h>

using namespace scribe;

TypeValidationError::TypeValidationError(const std::string& message)
  : ScribeException(makeString() << "Validation failed! " << message)
{}

TypeValidationError::TypeValidationError(const char* message)
  : ScribeException(makeString() << "Validation failed! " << message)
{}
