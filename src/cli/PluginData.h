// src/cli/PluginData.h
#ifndef KEEPASSXC_PLUGINDATA_H
#define KEEPASSXC_PLUGINDATA_H

#include "DatabaseCommand.h"

class PluginData : public DatabaseCommand
{
public:
    PluginData();
    int executeWithDatabase(QSharedPointer<Database> db, QSharedPointer<QCommandLineParser> parser) override;

    static const QCommandLineOption TitleOption;
    static const QCommandLineOption UuidOption;
    static const QCommandLineOption PropertyOption; // Updated option name
    static const QCommandLineOption ValueOption;
};

#endif // KEEPASSXC_PLUGINDATA_H
