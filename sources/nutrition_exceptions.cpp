#include "../headers/nutrition_exceptions.h"

NutritionException::NutritionException(const std::string& msg, const std::string& ctx)
        : message(msg), context(ctx) {}

NutritionException::~NutritionException() noexcept = default;

const char* NutritionException::what() const noexcept {
return message.c_str();
}

std::string NutritionException::get_message() const {
    return message;
}

std::string NutritionException::get_context() const {
    return context;
}

std::string NutritionException::get_full_message() const {
    if (context.empty()) {
        return message;
    }
    return message + " [Context: " + context + "]";
}

FileException::FileException(const std::string& msg, const std::string& fname, const std::string& ctx)
        : NutritionException(msg, ctx), filename(fname) {}

std::string FileException::get_filename() const {
    return filename;
}

std::string FileException::get_full_message() const {
    std::string full = message;
    if (!filename.empty()) {
        full += " [File: " + filename + "]";
    }
    if (!context.empty()) {
        full += " [Context: " + context + "]";
    }
    return full;
}

DataException::DataException(const std::string& msg, const std::string& dtype, const std::string& ctx)
        : NutritionException(msg, ctx), dataType(dtype) {}

std::string DataException::get_data_type() const {
    return dataType;
}

std::string DataException::get_full_message() const {
    std::string full = message;
    if (!dataType.empty()) {
        full += " [Data Type: " + dataType + "]";
    }
    if (!context.empty()) {
        full += " [Context: " + context + "]";
    }
    return full;
}

ValidationException::ValidationException(const std::string& msg, const std::string& field, const std::string& value, const std::string& ctx)
        : NutritionException(msg, ctx), fieldName(field), invalidValue(value) {}

std::string ValidationException::get_field_name() const {
    return fieldName;
}

std::string ValidationException::get_invalid_value() const {
    return invalidValue;
}

std::string ValidationException::get_full_message() const {
    std::string full = message;
    if (!fieldName.empty()) {
        full += " [Field: " + fieldName + "]";
    }
    if (!invalidValue.empty()) {
        full += " [Value: " + invalidValue + "]";
    }
    if (!context.empty()) {
        full += " [Context: " + context + "]";
    }
    return full;
}

InvalidUserDataException::InvalidUserDataException(const std::string& msg, const std::string& field, const std::string& value)
        : ValidationException(msg, field, value, "User Data Validation") {}

InvalidProductException::InvalidProductException(const std::string& msg, const std::string& field, const std::string& value)
        : ValidationException(msg, field, value, "Product Data Validation") {}

CalculationException::CalculationException(const std::string& msg, const std::string& calcType, const std::string& ctx)
        : NutritionException(msg, ctx), calculationType(calcType) {}

std::string CalculationException::get_calculation_type() const {
    return calculationType;
}

std::string CalculationException::get_full_message() const {
    std::string full = message;
    if (!calculationType.empty()) {
        full += " [Calculation: " + calculationType + "]";
    }
    if (!context.empty()) {
        full += " [Context: " + context + "]";
    }
    return full;
}
