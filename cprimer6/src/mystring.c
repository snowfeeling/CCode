#ifndef MYSTRING_C_GUARD
#define MYSTRING_C_GUARD

#include <stdio.h>
#include "../include/hello.h"
#include "../include/mylog.h"
#include "../include/mybase.h"
#include "../include/mystring.h"

void stringDemo(void)
{
    int ch;
    do
    {
        showManual();
        ch = tolower(getchar());
        if (ch != '\n')
            cleanInputBuffer();
        else
            continue;

        switch (ch)
        {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
            strHandle(ch);
            break;
        case 'q':
        {
            printf("Thank you for using the program!\n");
            break;
        }
        case '\n':
            break;
        default:
            printf("Invalid choice!\n");
            break;
        }
        printf("\n");
    } while (ch != 'q');
}
void strHandle(int ch)
{
    char filename[] = "../data/test.txt";
    char *FUNC_NAMES[] = {"Upper", "Lower", "Reverse", "Capitalized", "SplitWords"};
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Failed to open file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int funIndex = ch - '1';
    char strBuffer[MAX_LINE_LENGTH];
    sprintf(strBuffer, "*****Start the function: %s ****", (char *)FUNC_NAMES[funIndex]);
    logMessage(strBuffer);

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // Remove newline character if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = '\0';
        }
        strHandleOneLine(ch, line);
    }

    fclose(file);
}
void strHandleOneLine(int ch, const char *str)
{
    func_ptr funcPtr[4] = {strUpper, strLower, strReverse, strCapitalized};
    func_ptr fPtr;
    printf("The original string: %s\n", str);
    switch (ch)
    {
    case '1':
    case '2':
    case '3':
    case '4':
    {
        int funcIndex = ch - '1';
        fPtr = funcPtr[funcIndex];
        strHandleString(fPtr, str);
        break;
    }
    case '5':
        strSplitTest(str);
        break;
    default:
        break;
    }
}

void strHandleString(func_ptr fPtr, const char *str)
{
    if (str == NULL)
        return;
    char *newStr = (char *)malloc(strlen(str) + 1);
    if (newStr == NULL)
        return;
    char *strBuffer = (char *)malloc(2 * strlen(str) + 1 + 10);
    if (strBuffer == NULL)
        return;
    strcpy(newStr, str);

    fPtr(newStr);

    sprintf(strBuffer, "[%s] -> [%s]", str, newStr);
    printf("%s\n", strBuffer);
    logMessage(strBuffer);

    SAFE_FREE(newStr);
    SAFE_FREE(strBuffer);
}

int strSplitTest(const char *text)
{
    int count = 0;
    char **words = splitWords(text, &count);

    if (NULL == words)
    {
        printf("Memory allocation failed!\n");
        return 0;
    }

    char strBuffer[MAX_LINE_LENGTH];
    sprintf(strBuffer, "Found %d words:", count);
    printf("%s\n", strBuffer);
    logMessage(strBuffer);
    for (int i = 0; i < count; i++)
    {
        sprintf(strBuffer, "[%s]", words[i]);
        printf("%s\n", strBuffer);
        logMessage(strBuffer);
    }
    // 释放内存
    cleanWords(words, count);
    return count;
}
char **splitWords(const char *str, int *word_count)
{
    if (NULL == str || NULL == word_count)
        return NULL;

    // 初始分配空间
    int capacity = 10;
    char **words = malloc(capacity * sizeof(char *));
    if (NULL == words)
        return NULL;

    *word_count = 0;
    const char *p = str;

    while (1)
    {
        // 跳过所有空格
        while (*p == ' ')
            p++;
        if (*p == '\0')
            break;

        // 找到单词边界
        const char *start = p;
        while (*p != '\0' && *p != ' ')
            p++;
        size_t word_len = p - start;

        // 分配单词内存
        char *word = malloc(word_len + 1); // +1 for null terminator
        if (NULL == word)
            goto cleanup;

        // 复制单词内容
        strncpy(word, start, word_len);
        word[word_len] = '\0';

        // 扩展数组空间
        if (*word_count >= capacity)
        {
            capacity *= 2;
            char **new_words = realloc(words, capacity * sizeof(char *));
            if (NULL == new_words)
                goto cleanup;
            words = new_words;
        }

        words[(*word_count)++] = word;
    }

    return words;

// 清理已分配内存（当发生错误时）
cleanup:
    cleanWords(words, *word_count);

    return NULL;
}
void cleanWords(char **words, int word_count)
{
    for (int i = 0; i < word_count; i++)
    {
        SAFE_FREE(words[i]);
    }
    SAFE_FREE(words);
}

void strCapitalized(char *str)
{
    // 首字母大写
    if (NULL == str)
        return;
    int capitalizeNext = 1; // Flag to determine if the next character should be capitalized

    while (*str)
    {
        if (isspace(*str))
        {
            capitalizeNext = 1; // Set flag to capitalize next non-space character
        }
        else if (capitalizeNext)
        {
            *str = toupper(*str); // Capitalize the current character
            capitalizeNext = 0;   // Reset the flag
        }
        else
        {
            //*str = tolower(*str); // Convert the current character to lowercase
        }
        str++;
    }
}

void cleanInputBuffer()
{
    while (getchar() != '\n')
        continue;
}

void strReverse(char *str)
{
    int len = strlen(str);
    int i;
    char temp;

    for (i = 0; i < len / 2; i++)
    {
        temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}
  
void strUpper(char *str)
{
    while (*str)
    {
        *str = toupper(*str);
        str++;
    }
}

void strLower(char *str)
{
    while (*str)
    {
        *str = tolower(*str);
        str++;
    }
}
void showManual(void)
{
    printf("--%-50s-\n", "--------------------------------------------------");
    printf("- %-50s-\n", "Welcome to the program!");
    printf("- %-50s-\n", "This program will help you change the string.");
    printf("- %-50s-\n", "Please choose one of the following:");
    printf("- %-50s-\n", "1) Upper string    2) Lower string");
    printf("- %-50s-\n", "3) Reverse string  4) FirstLetterUpper");
    printf("- %-50s-\n", "5) Split string    q) Quit");
    printf("--%-50s-\n", "--------------------------------------------------");
    printf("Enter the number of your choice: ");
}

#endif
