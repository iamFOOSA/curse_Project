#!/bin/bash
# Скрипт для развертывания приложения на macOS

APP_PATH="/Users/danilaabramcuk/Desktop/PLAY/play/build/debug/NutritionDiary.app"
MACDEPLOYQT="/Users/danilaabramcuk/Desktop/QT/6.9.3/macos/bin/macdeployqt"

if [[ -f "$MACDEPLOYQT" ]]; then
    echo "Запуск macdeployqt..."
    "$MACDEPLOYQT" "$APP_PATH" -verbose=2
    echo "Готово!"
else
    echo "Ошибка: macdeployqt не найден!"
    exit 1
fi

