#include "DBS.h"
/**
 * @brief   初始化程序
 * @return  无返回值
 */
void init() {
    string databaseDirPath;
    sprintf(databaseDirPath, "%s/%s", rootDir, databaseDirName);
    if (access(databaseDirPath, F_OK))
        mkdir(databaseDirPath);
}
/**
 * @brief   输出程序开始或结束时的提示词
 * @param   mode mode == 0 时输出欢迎词，mode == 1 时输出结束词
 * @return  无返回值
 */
void prompt(int mode) {
    if (mode == 0)
        printf("\nWelcome to DBS (Database System).\n\n");
    else if (mode == 1)
        printf("\nGood bye.\n\n");
}
/**
 * @brief   获取用户在数据库层的选择
 * @return  用户在数据库层的选择（所有的选择类型见函数内部代码）
 */
option_t showDatabaseOptions() {
    int i;
    option_t optionCount = 4;
    option_t option = -1;
    string databaseDirPath;
    sprintf(databaseDirPath, "%s/%s", rootDir, databaseDirName);
    Dir *dir = getDir(databaseDirPath);
    if (dir->fileCount > 0) {
        printf("\nThe following database(s) exist:\n");
        for (i = 0; i < dir->fileCount; i++)
            printf("%s\n", dir->fileList[i]);
    } else {
        printf("\nThere have no database exists:\n");
    }
    printf(
        "\n"
        "Please select one of the following commands:\n"
        "1. create database <databaseName>\n"
        "2. delete database <databaseName>\n"
        "3. access to <databaseName>\n"
        "4. exit the program\n"
    );
    while (!scanf("%d", &option) || !inRange(option, 1, optionCount)) {
        printf("Please select a number between 1 and %d:\n", optionCount);
        fflush(stdin);
    }
    freeDir(dir);
    return option;
}
/**
 * @brief   处理用户在数据库层的选择
 * @param   option 用户在数据库层的选择
 * @return  若选择退回上一级返回 -1 ，否则返回 0 
 */
int handleDatabaseOption(option_t option) {
    string databaseName;
    string databaseDirPath;
    Dir *dir;
    if (option != 4) {
        sprintf(databaseDirPath, "%s/%s", rootDir, databaseDirName);
        dir = getDir(databaseDirPath);
    } else {
        return -1;
    }
    switch (option) {
        case 1:
            printf("Please enter a database name:\n");
            while (!scanf("%s", databaseName)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            if (createDatabase(databaseName)) {
                printf("Database created successfully.\n");
            } else {
                printf("Database failed to create, this database name may already exist.\n");
            }
            break;
        case 2:
            printf("Please enter a database name:\n");
            while (!scanf("%s", databaseName)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            if (deleteDatabase(databaseName)) {
                printf("Database deleted successfully.\n");
            } else {
                printf("Database failed to delete, this database name may not exist.\n");
            }
            break;
        case 3:
            if (dir->fileCount == 0) {
                printf("There is no database.\n");
            } else {
                printf("Please enter a database name:\n");
                while (!scanf("%s", databaseName)) {
                    printf("Please enter correct data.\n");
                    fflush(stdin);
                }
                if (!existsFile(dir, databaseName)) {
                    printf("Database %s does not exist.\n", databaseName);
                } else {
                    strcpy(currentDatabase, databaseName);
                }
            }
            break;
    }
    freeDir(dir);
}
/**
 * @brief   获取用户在数据表层的选择
 * @return  用户在数据表层的选择（所有的选择类型见函数内部代码）
 */
option_t showTableOptions() {
    int i;
    option_t optionCount = 9;
    option_t option = -1;
    string tableDirPath;
    sprintf(tableDirPath, "%s/%s/%s", rootDir, databaseDirName, currentDatabase);
    Dir *dir = getDir(tableDirPath);
    if (dir->fileCount > 0) {
        printf("\nDatabase %s has the following table(s):\n", currentDatabase);
        for (i = 0; i < dir->fileCount; i++)
            printf("%s\n", dir->fileList[i]);
    } else {
        printf("\nDatabase %s does not have any tables.\n", currentDatabase);
    }
    printf(
        "\n"
        "Please select one of the following commands:\n"
        "1. create table {<attributeName1, attributeName2, ...>}\n"
        "2. delete table <tableName>\n"
        "3. insert into <tableName> values (value1, value2, ...)\n"
        "4. delete from <tableName> where <attributeName>=<attributeValue>\n"
        "5. update <tableName> set <attributeName>=<attributeValue> where <attributeName>=<attributeValue>\n"
        "6. select <attributeName1, attributeName2, ...> from <tableName> where <attributeName>=<attributeValue>\n"
        "7. alter table <tableName> add column <attributeName>\n"
        "8. alter table <tableName> drop column <attributeName>\n"
        "9. back to the previous menu\n"
    );
    while (!scanf("%d", &option) || !inRange(option, 1, optionCount)) {
        printf("Please select a number between 1 and %d:\n", optionCount);
        fflush(stdin);
    }
    freeDir(dir);
    return option;
}
/**
 * @brief   处理用户在数据表层的选择
 * @param   option 用户在数据表层的选择
 * @return  若选择退回上一级返回 -1 ，否则返回 0 
 */
int handleTableOption(option_t option) {
    bool success = false;
    int i, itemCount, indexCount, attributeNameCount, *indexList = NULL;
    data_t attributeValue, *valueList = NULL;
    string tableDirPath, tableName, attributeName, *attributeNameList = NULL;
    Dir *dir = NULL;
    Table *table = NULL;
    if (option != 9) {
        sprintf(tableDirPath, "%s/%s/%s", rootDir, databaseDirName, currentDatabase);
        dir = getDir(tableDirPath);
        printf("Please enter table name:\n");
        while (!scanf("%s", tableName)) {
            printf("Please enter correct data.\n");
            fflush(stdin);
        }
        table = loadTable(tableName);
        if (!table && option != 1) {
            printf("Table %s does not exist.\n", tableName);
            return 0;
        }
        if (table && option == 1) {
            printf("Table %s already exists.\n", tableName);
            freeTable(table);
            return 0;
        }
    } else {
        return -1;
    }
    switch (option) {
        case 1:
            printf("Please enter the number of attributes to be set:\n");
            while (!scanf("%d", &attributeNameCount) || attributeNameCount <= 0) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            printf("Please enter %d attribute name(s) separated by spaces:\n", attributeNameCount);
            attributeNameList = (string *)malloc(attributeNameCount * sizeof(string));
            for (i = 0; i < attributeNameCount; ++i)
                scanf("%s", attributeNameList[i]);
            if (createTable(tableName, attributeNameList, attributeNameCount)) {
                printf("Table %s created successfully.\n", tableName);
            } else {
                printf("Failed to create table %s.\n", tableName);
            }
            if (attributeNameList)
                free(attributeNameList);
            break;
        case 2:
            if (deleteTable(tableName)) {
                printf("Table %s deleted successfully.\n", tableName);
            } else {
                printf("Failed to delete table %s.\n", tableName);
            }
            break;
        case 3:
            valueList = (data_t *)malloc(table->attrCount * sizeof(data_t));
            printf("Please enter %d attribute value(s) separated by spaces:\n", table->attrCount);
            for (i = 0; i < table->attrCount; ++i)
                scanf("%d", valueList + i);
            if (insertItem(table, valueList)) {
                printf("Successfully inserted an item to table %s.\n", tableName);
            } else {
                printf("Failed to insert an item to table %s.\n", tableName);
            };
            if (valueList)
                free(valueList);
            break;
        case 4:
            printf("Please enter an attribute name and an attribute value separated by spaces, you can use '*' as a wildcard:\n");
            while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            indexList = filterItem(table, attributeName, attributeValue, &indexCount);
            if (!indexCount) {
                printf("The attribute does not exist or no items are filtered out, no items are deleted");
            } else {
                itemCount = deleteItem(table, indexList, indexCount);
                printf("%d item(s) are deleted in this operation.\n", itemCount);
                if (indexList)
                    free(indexList);
            }
            break;
        case 5:
            printf("Please enter an attribute name and an attribute value separated by spaces, these values are used to filter item(s):\n");
            while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            indexList = filterItem(table, attributeName, attributeValue, &indexCount);
            if (!indexCount) {
                printf("The attribute does not exist or no items are filtered out, no items are updated");
            } else {
                printf("Please enter an attribute name and an attribute value separated by spaces, these values are used to update item(s):\n");
                while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                    printf("Please enter correct data.\n");
                    fflush(stdin);
                }
                itemCount = updateItem(table, indexList, indexCount, attributeName, attributeValue);
                printf("%d item(s) are updated in this operation.\n", itemCount);
                if (indexList)
                    free(indexList);
            }
            break;
        case 6:
            printf("Please enter an attribute name and an attribute value separated by spaces, these values are used to filter item(s), you can use '*' as a wildcard:\n");
            while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            indexList = filterItem(table, attributeName, attributeValue, &indexCount);
            if (!indexList && !indexCount) {
                printf("The attribute does not exist or no items are filtered out, no items are selected.\n");
            } else {
                printf("Please enter the number of attributes to be selected:\n");
                while (!scanf("%d", &attributeNameCount) || attributeNameCount <= 0) {
                    printf("Please enter correct data.\n");
                    fflush(stdin);
                }
                printf("Please enter %d attribute name(s) separated by spaces:\n", attributeNameCount);
                attributeNameList = (string *)malloc(attributeNameCount * sizeof(string));
                for (i = 0; i < attributeNameCount; ++i) {
                    scanf("%s", attributeNameList[i]);
                }
                for (i = 0; i < attributeNameCount; ++i) {
                    if (!strcmp(attributeNameList[i], wildcard)) {
                        free(attributeNameList);
                        attributeNameList = table->attrName;
                        attributeNameCount = table->attrCount;
                        break;
                    }
                }
                itemCount = selectItem(table, indexList, indexCount, attributeNameList, attributeNameCount);
                printf("%d item(s) are selected in this operation.\n", itemCount);
            }
            if (attributeNameList != table->attrName)
                free(attributeNameList);
            if (indexList)
                free(indexList);
            break;
        case 7:
            printf("Please enter an attribute name and an attribute value separated by spaces:\n");
            while (!scanf("%s", attributeName) || !scanf("%d", &attributeValue)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            if (addAttribute(table, attributeName, attributeValue)) {
                printf("Successfully added attribute %s for table %s.\n", attributeName, table->tableName);
            } else {
                printf("Failed to add attribute %s for table %s, perhaps the attribute already exists.\n", attributeName, table->tableName);
            }
            break;
        case 8:
            printf("Please enter an attribute name:\n");
            while (!scanf("%s", attributeName)) {
                printf("Please enter correct data.\n");
                fflush(stdin);
            }
            if (dropAttribute(table, attributeName)) {
                printf("Successfully dropped attribute %s for table %s.\n", attributeName, table->tableName);
            } else {
                printf("Failed to drop attribute %s for table %s, perhaps the attribute does not exist.\n", attributeName, table->tableName);
            }
            break;
    }
    if (table)
        freeTable(table);
    freeDir(dir);
    return 0;
}
int main() {
    char *databaseName;
    int statusCode;
    option_t databaseOption, tableOption;
    init();
    prompt(0);
    while (true) {
        databaseOption = showDatabaseOptions();
        statusCode = handleDatabaseOption(databaseOption);
        if (statusCode == -1)
            break;
        if (strlen(currentDatabase) > 0) {
            while (true) {
                tableOption = showTableOptions();
                statusCode = handleTableOption(tableOption);
                if (statusCode == -1)
                    break;
            }
            memset(currentDatabase, 0, sizeof(currentDatabase));
        }
    }
    prompt(1);
}