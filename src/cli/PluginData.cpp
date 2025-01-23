// src/cli/PluginData.cpp
#include "PluginData.h"
#include "Utils.h"
#include "core/Group.h"
#include "core/Entry.h"
#include <QCommandLineParser>

const QCommandLineOption PluginData::TitleOption = QCommandLineOption(QStringList() << "t" << "title",
                                                                      QObject::tr("Title of the entry."),
                                                                      QObject::tr("title"));

const QCommandLineOption PluginData::UuidOption = QCommandLineOption(QStringList() << "u" << "uuid",
                                                                     QObject::tr("UUID of the entry."),
                                                                     QObject::tr("uuid"));

const QCommandLineOption PluginData::PropertyOption = QCommandLineOption(QStringList() << "p" << "property", // Updated option name
                                                                         QObject::tr("Name of the property to view or set."),
                                                                         QObject::tr("property"));

const QCommandLineOption PluginData::ValueOption = QCommandLineOption(QStringList() << "v" << "value",
                                                                      QObject::tr("New value for the property."),
                                                                      QObject::tr("value"));

PluginData::PluginData()
{
    name = QString("plugindata");
    description = QObject::tr("View or set custom data for a specified entry.");
    options.append(PluginData::TitleOption);
    options.append(PluginData::UuidOption);
    options.append(PluginData::PropertyOption); // Updated option name
    options.append(PluginData::ValueOption);
}

int PluginData::executeWithDatabase(QSharedPointer<Database> database, QSharedPointer<QCommandLineParser> parser)
{
    auto& out = parser->isSet(Command::QuietOption) ? Utils::DEVNULL : Utils::STDOUT;
    auto& err = Utils::STDERR;

    QString title = parser->value(PluginData::TitleOption);
    QString uuidStr = parser->value(PluginData::UuidOption);
    QString property = parser->value(PluginData::PropertyOption);
    QString value = parser->value(PluginData::ValueOption);

    if (title.isEmpty() && uuidStr.isEmpty()) {
        err << QObject::tr("Either title or uuid must be specified.") << Qt::endl;
        return EXIT_FAILURE;
    }

    Entry* entry = nullptr;
    if (!title.isEmpty()) {
        entry = database->rootGroup()->findEntryByPath(title);
    } else if (!uuidStr.isEmpty()) {
        QUuid uuid(uuidStr);
        entry = database->rootGroup()->findEntryByUuid(uuid);
    }

    if (!entry) {
        err << QObject::tr("Could not find entry with specified title or uuid.") << Qt::endl;
        return EXIT_FAILURE;
    }

    if (property.isEmpty() && value.isEmpty()) {
         // Show all properties
        const auto& customData = entry->customData();
        const auto keys = customData->keys();
        if (keys.isEmpty()) {
            out << QObject::tr("No custom data found for the entry.") << Qt::endl;
        } else {
            out << QObject::tr("Custom data for the entry:") << Qt::endl;
            for (const auto& key : keys) {
                out << QObject::tr("%1: %2").arg(key, customData->value(key)) << Qt::endl;
            }
        }
    } else if (value.isEmpty()) {
        // View specific property
        QString customDataValue = entry->customData()->value(property);
        if (customDataValue.isEmpty()) {
            out << QObject::tr("No custom data found for property %1.").arg(property) << Qt::endl;
        } else {
            out << QObject::tr("Custom data for property %1: %2").arg(property, customDataValue) << Qt::endl;
        }
    } else {
        // Set custom data
        entry->beginUpdate();
        entry->customData()->set(property, value);
        entry->endUpdate();

        QString errorMessage;
        if (!database->save(Database::Atomic, {}, &errorMessage)) {
            err << QObject::tr("Writing the database failed: %1").arg(errorMessage) << Qt::endl;
            return EXIT_FAILURE;
        }

        out << QObject::tr("Successfully set custom data for property %1.").arg(property) << Qt::endl;
    }

    return EXIT_SUCCESS;
}
