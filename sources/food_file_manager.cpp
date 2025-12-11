#include "../headers/food_file_manager.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QFileInfo>
#include <algorithm>
#include <cctype>

FoodFileManager::FoodFileManager(const std::string& filepath)
    : filename(filepath)
{
}


QString FoodFileManager::normalizeProductName(const QString& name) const
{
    QString normalized = name.trimmed();
    normalized.replace('_', ' ');
    normalized = normalized.toLower();
    return normalized;
}

QStringList FoodFileManager::splitProductName(const QString& input) const
{
    return input.trimmed().toLower().split(' ', Qt::SkipEmptyParts);
}

QString FoodFileManager::findDataFilePath(const QString& relativePath) const
{
    QStringList searchPaths;
    
    QString appDir;
    QString currentDir = QDir::currentPath();
    
    if (const QCoreApplication* app = QCoreApplication::instance()) {
        appDir = QCoreApplication::applicationDirPath();
    }
    
    QDir currentQDir(currentDir);
    QString projectRoot = currentDir;
    
    bool found = false;
    while (!currentQDir.isRoot() && !found) {
        if (QFileInfo playCheck(currentQDir.absoluteFilePath("play/data/products.txt")); playCheck.exists()) {
            projectRoot = currentQDir.absolutePath();
            searchPaths << playCheck.absoluteFilePath();
            found = true;
        } else if (!currentQDir.cdUp()) {
            break;
        }
    }
    
    if (!appDir.isEmpty()) {
        searchPaths << appDir + "/" + relativePath;
        searchPaths << appDir + "/../../play/" + relativePath;
        searchPaths << appDir + "/../../../play/" + relativePath;
    }
    
    searchPaths << projectRoot + "/play/" + relativePath;
    searchPaths << projectRoot + "/" + relativePath;
    searchPaths << currentDir + "/" + relativePath;
    searchPaths << currentDir + "/../play/" + relativePath;
    searchPaths << currentDir + "/../../play/" + relativePath;
    searchPaths << relativePath;
    
    for (const QString& path : searchPaths) {
        QString cleanPath = QDir::cleanPath(path);
        QFileInfo fileInfo(cleanPath);
        if (fileInfo.exists() && fileInfo.isFile()) {
            QString absPath = fileInfo.absoluteFilePath();
            return absPath;
        }
    }
    
    QString createPath = QDir::cleanPath(currentDir + "/" + relativePath);
    QFileInfo createFileInfo(createPath);
    
    if (QDir createDir = createFileInfo.absoluteDir(); !createDir.exists() && !createDir.mkpath(".")) {
        return QString();
    }
    
    return QDir::cleanPath(createPath);
}

bool FoodFileManager::parseProductLine(const QString& line, int lineNumber, ProductData& product) const
{
    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    if (parts.size() != 5) {
        qDebug() << "Ошибка в строке" << lineNumber << ": неверное количество параметров";
        return false;
    }

    QString name = parts[0];
    bool ok;
    double calories = parts[1].toDouble(&ok);
    if (!ok) {
        qDebug() << "Ошибка в строке" << lineNumber << ": неверное значение калорий";
        return false;
    }

    double proteins = parts[2].toDouble(&ok);
    if (!ok) {
        qDebug() << "Ошибка в строке" << lineNumber << ": неверное значение белков";
        return false;
    }

    double fats = parts[3].toDouble(&ok);
    if (!ok) {
        qDebug() << "Ошибка в строке" << lineNumber << ": неверное значение жиров";
        return false;
    }

    double carbs = parts[4].toDouble(&ok);
    if (!ok) {
        qDebug() << "Ошибка в строке" << lineNumber << ": неверное значение углеводов";
        return false;
    }

    QString originalName = name;
    originalName.replace('_', ' ');
    originalName = originalName.trimmed();
    
    product = ProductData(originalName, calories, proteins, fats, carbs);
    return true;
}

void FoodFileManager::addProductVariants(const ProductData& product)
{
    QString normalizedName = product.name.toLower();
    QString nameWithUnderscores = product.name;
    nameWithUnderscores.replace(' ', '_');
    QString nameWithUnderscoresLower = nameWithUnderscores.toLower();
    
    productsDatabase[normalizedName] = product;
    productsDatabase[product.name] = product;
    productsDatabase[product.name.toLower()] = product;
    productsDatabase[nameWithUnderscores] = product;
    productsDatabase[nameWithUnderscoresLower] = product;
}

bool FoodFileManager::createDefaultProductsFile(const QString& filepath) const
{
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось создать файл:" << file.errorString();
        return false;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    QStringList defaultProducts = {
        "Овсяная_каша 350 12 6 57",
        "Куриная_грудка 165 31 3.6 0",
        "Гречка 343 13 3.4 72",
        "Яблоко 52 0.3 0.2 14",
        "Творог 120 17 5 3",
        "Овощной_салат 43 1.8 0.2 10",
        "Йогурт_натуральный 60 4 3.5 5",
        "Рис 130 2.7 0.3 28",
        "Рыба 120 20 4 0",
        "Омлет 180 12 14 2",
        "Банановый_смузи 90 2 0.5 20",
        "Индейка 140 25 3 0",
        "Греческий_салат 130 5 10 6",
        "Орехи_грецкие 650 15 65 14",
        "Макароны 131 5 1.1 25",
        "Хлеб_белый 265 9 3.2 49",
        "Молоко 60 3.2 3.6 4.7",
        "Яйцо 155 13 11 1.1",
        "Сыр 350 25 27 2",
        "Бананы 89 1.1 0.3 23"
    };
    
    for (const QString& product : defaultProducts) {
        out << product << "\n";
    }
    
    file.close();
    return true;
}

bool FoodFileManager::loadProductsFromFile()
{
    return loadProductsFromFile(filename);
}

bool FoodFileManager::loadProductsFromFile(const std::string& filepath)
{
    productsDatabase.clear();
    
    QString qFilePath = findDataFilePath(QString::fromStdString(filepath));
    
    if (qFilePath.isEmpty()) {
        QString currentDir = QDir::currentPath();
        qFilePath = QDir::cleanPath(currentDir + "/data/products.txt");
        QFileInfo info(qFilePath);
        if (!info.exists()) {
            qFilePath = QDir::cleanPath(currentDir + "/play/data/products.txt");
        }
    }
    
    filename = qFilePath.toStdString();

    QFile file(qFilePath);
    
    if (!file.exists()) {
        QFileInfo fileInfo(qFilePath);
        if (QDir dir = fileInfo.absoluteDir(); !dir.exists() && !dir.mkpath(".")) {
            return false;
        }
        
        if (!createDefaultProductsFile(qFilePath)) {
            return false;
        }
        file.setFileName(qFilePath);
    }
    
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    int lineNumber = 0;
    while (!in.atEnd()) {
        lineNumber++;
        QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        ProductData product;
        if (parseProductLine(line, lineNumber, product)) {
            addProductVariants(product);
        }
    }

    file.close();
    
    size_t uniqueCount = 0;
    QMap<QString, bool> seen;
    for (auto it = productsDatabase.constBegin(); it != productsDatabase.constEnd(); ++it) {
        QString normalizedName = it.value().name.toLower();
        if (!seen.contains(normalizedName)) {
            seen[normalizedName] = true;
            uniqueCount++;
        }
    }
    
    qDebug() << "Загружено уникальных продуктов:" << uniqueCount << "из файла:" << qFilePath;
    
    if (uniqueCount == 0) {
        qDebug() << "ВНИМАНИЕ: База продуктов пуста! Файл:" << qFilePath;
    }
    
    return true;
}

bool FoodFileManager::saveProductsToFile() const
{
    return saveProductsToFile(filename);
}

bool FoodFileManager::saveProductsToFile(const std::string& filepath) const
{
    QFile file(QString::fromStdString(filepath));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Ошибка записи файла продуктов:" << file.errorString();
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    QMap<QString, ProductData> uniqueProducts;
    for (auto it = productsDatabase.constBegin(); it != productsDatabase.constEnd(); ++it) {
        QString normalizedName = it.value().name.toLower();
        if (!uniqueProducts.contains(normalizedName)) {
            uniqueProducts[normalizedName] = it.value();
        }
    }

    for (auto it = uniqueProducts.constBegin(); it != uniqueProducts.constEnd(); ++it) {
        const ProductData& product = it.value();
        QString name = product.name;
        name.replace(' ', '_');
        out << name << " "
            << product.calories << " "
            << product.proteins << " "
            << product.fats << " "
            << product.carbs << "\n";
    }

    file.close();
    return true;
}

bool FoodFileManager::addProduct(const QString& name, double calories, double proteins, double fats, double carbs)
{
    if (name.trimmed().isEmpty()) {
        return false;
    }

    QString normalizedName = normalizeProductName(name);
    QString displayName = name.trimmed();

    ProductData product(displayName, calories, proteins, fats, carbs);
    productsDatabase[normalizedName] = product;
    productsDatabase[displayName] = product;

    return true;
}

bool FoodFileManager::removeProduct(const QString& name)
{
    QString normalizedName = normalizeProductName(name);
    bool removed = false;

    if (productsDatabase.contains(normalizedName)) {
        QString displayName = productsDatabase[normalizedName].name;
        productsDatabase.remove(normalizedName);
        productsDatabase.remove(displayName);
        removed = true;
    }

    return removed;
}

bool FoodFileManager::productExists(const QString& name) const
{
    QString normalizedName = normalizeProductName(name);
    return productsDatabase.contains(normalizedName);
}

FoodFileManager::ProductData FoodFileManager::findProduct(const QString& name) const
{
    if (name.trimmed().isEmpty() || productsDatabase.isEmpty()) {
        return ProductData();
    }

    QString inputName = name.trimmed();
    QString inputNormalized = inputName.toLower().trimmed();
    inputNormalized.replace('_', ' ');

    for (auto it = productsDatabase.constBegin(); it != productsDatabase.constEnd(); ++it) {
        QString productName = it.value().name;
        QString productLower = productName.toLower().trimmed();
        
        if (productLower == inputNormalized) {
            return it.value();
        }
    }

    QStringList inputWords = inputNormalized.split(' ', Qt::SkipEmptyParts);
    if (inputWords.isEmpty()) {
        return {};
    }
    
    for (auto it = productsDatabase.constBegin(); it != productsDatabase.constEnd(); ++it) {
        QString productName = it.value().name.toLower();
        bool allMatch = true;
        
        for (const QString& word : inputWords) {
            if (word.length() < 2) {
                continue;
            }
            if (!productName.contains(word)) {
                allMatch = false;
                break;
            }
        }
        
        if (allMatch) {
            return it.value();
        }
    }

    QString inputSimple = inputNormalized;
    inputSimple.remove(' ');
    inputSimple.remove('_');
    
    for (auto it = productsDatabase.constBegin(); it != productsDatabase.constEnd(); ++it) {
        QString productName = it.value().name.toLower();
        QString productSimple = productName;
        productSimple.remove(' ');
        productSimple.remove('_');
        
        if (productSimple.contains(inputSimple) || inputSimple.contains(productSimple)) {
            return it.value();
        }
    }

    return ProductData();
}

QStringList FoodFileManager::getAllProductNames() const
{
    QStringList names;
    QMap<QString, bool> seen;

    for (auto it = productsDatabase.constBegin(); it != productsDatabase.constEnd(); ++it) {
        QString name = it.value().name;
        QString normalizedName = name.toLower();
        if (!seen.contains(normalizedName)) {
            names.append(name);
            seen[normalizedName] = true;
        }
    }

    names.sort(Qt::CaseInsensitive);
    return names;
}

double FoodFileManager::getCalories(const QString& name, double grams) const
{
    ProductData product = findProduct(name);
    if (product.name.isEmpty()) {
        return 0.0;
    }
    return (product.calories * grams) / 100.0;
}

double FoodFileManager::getProteins(const QString& name, double grams) const
{
    ProductData product = findProduct(name);
    if (product.name.isEmpty()) {
        return 0.0;
    }
    return (product.proteins * grams) / 100.0;
}

double FoodFileManager::getFats(const QString& name, double grams) const
{
    ProductData product = findProduct(name);
    if (product.name.isEmpty()) {
        return 0.0;
    }
    return (product.fats * grams) / 100.0;
}

double FoodFileManager::getCarbs(const QString& name, double grams) const
{
    ProductData product = findProduct(name);
    if (product.name.isEmpty()) {
        return 0.0;
    }
    return (product.carbs * grams) / 100.0;
}

QStringList FoodFileManager::searchProducts(const QString& query) const
{
    QStringList results;
    QStringList searchWords = splitProductName(query);

    if (searchWords.isEmpty()) {
        return getAllProductNames();
    }

    QMap<QString, bool> seen;
    for (auto it = productsDatabase.constBegin(); it != productsDatabase.constEnd(); ++it) {
        QString productName = it.value().name;
        QString normalizedName = productName.toLower();
        QString normalizedProduct = productName.toLower();

        if (seen.contains(normalizedName)) {
            continue;
        }

        bool matches = true;
        for (const QString& word : searchWords) {
            if (!normalizedProduct.contains(word)) {
                matches = false;
                break;
            }
        }

        if (matches) {
            results.append(productName);
            seen[normalizedName] = true;
        }
    }

    results.sort(Qt::CaseInsensitive);
    return results;
}

