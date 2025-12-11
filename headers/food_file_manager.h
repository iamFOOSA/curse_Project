#ifndef FOOD_FILE_MANAGER_H
#define FOOD_FILE_MANAGER_H

#include "food.h"
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <QString>
#include <QStringList>
#include <QMap>

class FoodFileManager {
public:
    struct ProductData {
        QString name;
        double calories;  // на 100 грамм
        double proteins;  // на 100 грамм
        double fats;      // на 100 грамм
        double carbs;     // на 100 грамм

        ProductData() : calories(0), proteins(0), fats(0), carbs(0) {}
        ProductData(const QString& n, double cal, double prot, double fat, double carb)
            : name(n), calories(cal), proteins(prot), fats(fat), carbs(carb) {}
    };

private:
    QMap<QString, ProductData> productsDatabase;
    std::string filename;

    // Вспомогательные методы
    QString normalizeProductName(const QString& name) const;
    QStringList splitProductName(const QString& input) const;
    QString findDataFilePath(const QString& relativePath) const;
    bool createDefaultProductsFile(const QString& filepath) const;
    bool parseProductLine(const QString& line, int lineNumber, ProductData& product) const;
    void addProductVariants(const ProductData& product);

public:
    explicit FoodFileManager(const std::string& filepath = "data/products.txt");
    ~FoodFileManager() = default;

    // Загрузка и сохранение продуктов
    bool loadProductsFromFile();
    bool loadProductsFromFile(const std::string& filepath);
    bool saveProductsToFile() const;
    bool saveProductsToFile(const std::string& filepath) const;

    // Работа с продуктами
    bool addProduct(const QString& name, double calories, double proteins, double fats, double carbs);
    bool removeProduct(const QString& name);
    bool productExists(const QString& name) const;
    ProductData findProduct(const QString& name) const;
    QStringList getAllProductNames() const;

    // Получение информации о продукте
    double getCalories(const QString& name, double grams = 100) const;
    double getProteins(const QString& name, double grams = 100) const;
    double getFats(const QString& name, double grams = 100) const;
    double getCarbs(const QString& name, double grams = 100) const;

    // Геттеры
    size_t getProductCount() const {
        QMap<QString, bool> unique;
        for (auto it = productsDatabase.constBegin(); it != productsDatabase.constEnd(); ++it) {
            QString normalizedName = it.value().name.toLower();
            unique[normalizedName] = true;
        }
        return unique.size();
    }
    QMap<QString, ProductData> getAllProducts() const { return productsDatabase; }

    // Поиск продуктов по частичному совпадению
    QStringList searchProducts(const QString& query) const;
};

#endif // FOOD_FILE_MANAGER_H

