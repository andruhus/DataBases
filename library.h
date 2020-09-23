#pragma once
#ifndef LABORATORY1_LIBRARY_H
#define LABORATORY1_LIBRARY_H

#include "stdio.h"
#include "wchar.h"
#include <locale.h>
#include <string.h>
#include <stdlib.h>


#define MAX_LENGTH_OF_COMMAND 128
#define LENGTH_OF_BIRTH_DATE 11
#define SURNAME_LEN 128
#define NAME_LEN 32
#define Organization_Adress_LEN 64
#define PHONE_LEN 52
#define MAX_LEN_OF_FIELD 15

typedef struct Member {
	int id;
	char name[NAME_LEN];
	char surname[SURNAME_LEN];
	char dateOfBirth[LENGTH_OF_BIRTH_DATE];
	int contributions;
	int nextMemberID;
	int isDeleted;
} member;

typedef struct Organization {
	int id;
	int budget;
	char address[Organization_Adress_LEN];
	char phone[PHONE_LEN];
	int firstMemberID;
} org;

typedef struct FileHandler {
	FILE* dataIndexFile;
	FILE* MemberDataFile;
	FILE* OrganizationDataFile;
	char* indexFileName;
	char* dataFileName;
	char* MemberFileName;

}fileHandler;
void hello(void);
fileHandler handler;
int handleCommand(void);
void showOrganizationFile();
void updateOrgFileRecord(char[MAX_LENGTH_OF_COMMAND], char*);
int searchForOrganizationIndex(int);
org organizationWithIndex(int);
void changeOrganizationRecord(org*, char*);
void updateRecord(int, org*);
void deleteEndOfLine(char*);
void printMenu();
void getRecordFromMemberFile(char command[MAX_LENGTH_OF_COMMAND], char* delims);
void insertRecordToMemberFile(char command[MAX_LENGTH_OF_COMMAND], char* delims);
void writeToMemberFile(member*);
void appendRecordToOrganization(int, member*);
member searchInMemberFile(int);
void appendAfterLastMemberInOrganization(member*, member*);
void printMemberRecord(member*);
void rewriteOrganizationRecord(org, int);
// void rewriteMemberRecord(member*);
void updateMemberRecord(char[MAX_LENGTH_OF_COMMAND], char*);
void changeMemberRecord(member*, char*);
void deleteOrganizationRecord(char[MAX_LENGTH_OF_COMMAND], char*);
void deleteRecordFromIndexFile(int);
void deleteAllSubrecords(int);
void clearContentOfFiles();
void deleteMemberRecord(char[MAX_LENGTH_OF_COMMAND], char*);
void removeSubrecord(org*, int);
int doesFileHasContent(FILE* f);
int size_of_MemberFile();
void showMemberFile();
#endif //LABORATORY1_LIBRARY_H

