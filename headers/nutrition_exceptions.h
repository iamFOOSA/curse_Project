#ifndef NUTRITION_EXCEPTIONS_H
#define NUTRITION_EXCEPTIONS_H

#include <string>
#include <exception>

class NutritionException : public std::exception {
protected:
    std::string message;
    std::string context;

public:
    explicit NutritionException(const std::string& msg, const std::string& ctx = "")
        : message(msg), context(ctx) {}

    virtual ~NutritionException() noexcept = default;

    const char* what() const noexcept override {
        return message.c_str();
    }

    std::string get_message() const { return message; }
    std::string get_context() const { return context; }
    std::string get_full_message() const {
        if (context.empty()) {
            return message;
        }
        return message + " [Context: " + context + "]";
    }
};

class FileException : public NutritionException {
private:
    std::string filename;

public:
    explicit FileException(const std::string& msg, const std::string& fname = "", const std::string& ctx = "")
        : NutritionException(msg, ctx), filename(fname) {}

    std::string get_filename() const { return filename; }
    std::string get_full_message() const {
        std::string full = message;
        if (!filename.empty()) {
            full += " [File: " + filename + "]";
        }
        if (!context.empty()) {
            full += " [Context: " + context + "]";
        }
        return full;
    }
};

class DataException : public NutritionException {
private:
    std::string dataType;

public:
    explicit DataException(const std::string& msg, const std::string& dtype = "", const std::string& ctx = "")
        : NutritionException(msg, ctx), dataType(dtype) {}

    std::string get_data_type() const { return dataType; }
    std::string get_full_message() const {
        std::string full = message;
        if (!dataType.empty()) {
            full += " [Data Type: " + dataType + "]";
        }
        if (!context.empty()) {
            full += " [Context: " + context + "]";
        }
        return full;
    }
};

class ValidationException : public NutritionException {
private:
    std::string fieldName;
    std::string invalidValue;

public:
    explicit ValidationException(const std::string& msg, const std::string& field = "", const std::string& value = "", const std::string& ctx = "")
        : NutritionException(msg, ctx), fieldName(field), invalidValue(value) {}

    std::string get_field_name() const { return fieldName; }
    std::string get_invalid_value() const { return invalidValue; }
    std::string get_full_message() const {
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
};

class InvalidUserDataException : public ValidationException {
public:
    explicit InvalidUserDataException(const std::string& msg, const std::string& field = "", const std::string& value = "")
        : ValidationException(msg, field, value, "User Data Validation") {}
};

class InvalidProductException : public ValidationException {
public:
    explicit InvalidProductException(const std::string& msg, const std::string& field = "", const std::string& value = "")
        : ValidationException(msg, field, value, "Product Data Validation") {}
};

class CalculationException : public NutritionException {
private:
    std::string calculationType;

public:
    explicit CalculationException(const std::string& msg, const std::string& calcType = "", const std::string& ctx = "")
        : NutritionException(msg, ctx), calculationType(calcType) {}

    std::string get_calculation_type() const { return calculationType; }
    std::string get_full_message() const {
        std::string full = message;
        if (!calculationType.empty()) {
            full += " [Calculation: " + calculationType + "]";
        }
        if (!context.empty()) {
            full += " [Context: " + context + "]";
        }
        return full;
    }
};

#endif // NUTRITION_EXCEPTIONS_H

