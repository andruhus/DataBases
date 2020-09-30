#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "wchar.h"
#include <locale.h>
#include <string.h>
#include <stdlib.h>

#define LENGTH_OF_COMMAND 128
#define LENGTH_OF_BIRTH_DATE 11
#define LENGTH_SURNAME 128
#define LENGTH_NAME 32
#define LENGTH_ORGANIZATION_ADDRESS 64 
#define LENGTH_PRINCIPAL 128
#define LENGTH_OF_FIELD 15
#define LENGTH_OF_FILE 10000000
typedef enum Bool
{
	true = 1, false = 0
} bool;
int ID = 0;
int NumberOfDeleted = 0;



typedef struct Member
{
	int id;
	char name[LENGTH_NAME];
	char surname[LENGTH_SURNAME];
	char dateOfBirth[LENGTH_OF_BIRTH_DATE];
	int contribution;
	int nextMemberID;
	bool IsCurrent;
} member;

typedef struct Organization
{
	int id;
	char address[LENGTH_ORGANIZATION_ADDRESS];
	char founder[LENGTH_PRINCIPAL];
	int firstMemberID;
	bool IsCurrent;
} organization;

typedef struct FileHandler
{
	FILE* dataIndexFile;
	FILE* organizationDataFile;
	FILE* memberDataFile;
	char* indexFileName;
	char* dataFileName;
	char* memberFileName;

}fileHandler;

fileHandler handler;
// rubish dealer
bool SizeOfFileIsGreater(FILE* f)
{
	fseek(f, 0, SEEK_END);
	unsigned long length = ftell(f);
	if (length > LENGTH_OF_FILE) return 1;
	return 0;
}

void TakeOutMemberRubish() {
	handler.memberDataFile = fopen(handler.memberFileName, "rb");
	FILE* temp = fopen("member_temp.dat", "wb");
	member ind;
	while (fread((char*)&ind, sizeof(member), 1, handler.memberDataFile))
	{
		if (ind.IsCurrent) {
			fwrite((char*)& ind, sizeof(member), 1, temp);
		}
	}
	fclose(handler.memberDataFile);
	fclose(temp);
	remove(handler.memberFileName);
	rename("member_temp.dat", handler.dataFileName);
}

void TakeOutOrganizationRubish() {
	handler.organizationDataFile = fopen(handler.dataFileName, "rb");
	FILE* temp = fopen("organizaton_temp.dat", "wb");
	organization ind;
	while (fread((char*)& ind, sizeof(organization), 1, handler.organizationDataFile))
	{
		if (ind.IsCurrent) {
			fwrite((char*)& ind, sizeof(organization), 1, temp);
		}
	}
	fclose(handler.organizationDataFile);
	fclose(temp);
	remove(handler.dataFileName);
	rename("organizaton_temp.dat", handler.dataFileName);
}

//ut-m
void ShowOrganization()
{
	organization organization;
	handler.organizationDataFile = fopen(handler.dataFileName, "rb");
	fseek(handler.organizationDataFile, 0L, SEEK_SET);
	for (int i = 1; ; i++)
	{
		fread(&organization, sizeof(organization), 1, handler.organizationDataFile);
		if (feof(handler.organizationDataFile))
			break;
		if(organization.IsCurrent)
			printf("%d. %d %s %s %d\n", i, organization.id, organization.address, organization.founder, organization.firstMemberID);
	}
	fclose(handler.organizationDataFile);
}

//ut-s
void ShowMembers()
{
	member member;
	handler.memberDataFile = fopen(handler.memberFileName, "rb");
	fseek(handler.memberDataFile, 0L, SEEK_SET);
	for (int i = 1; ; i++)
	{
		fread(&member, sizeof(member), 1, handler.memberDataFile);
		if (feof(handler.memberDataFile))
			break;
		if(member.IsCurrent)
			printf("%d. %d %s %s %s %d %d\n", i, member.id, member.name, member.surname, member.dateOfBirth, member.contribution, member.nextMemberID);
	}
	fclose(handler.memberDataFile);
}

//insert-m
void InsertToIndexFile(organization* newOrganization)
{
	handler.dataIndexFile = fopen(handler.indexFileName, "rb+");
	int oldIndex;
	fread(&oldIndex, sizeof(int), 1, handler.dataIndexFile);
	newOrganization->id = oldIndex;
	fseek(handler.dataIndexFile, 0L, SEEK_SET);
	int newIndex = oldIndex + 1;
	//oldIndex -= NumberOfDeleted;
	fwrite(&newIndex, sizeof(int), 1, handler.dataIndexFile);
	fseek(handler.dataIndexFile, 0L, SEEK_END);
	fwrite(&(newOrganization->id), sizeof(int), 1, handler.dataIndexFile);
	fwrite(&oldIndex, sizeof(int), 1, handler.dataIndexFile);
	fclose(handler.dataIndexFile);
}

void InsertToMasterFile(char command[LENGTH_OF_COMMAND])
{
	char delims[] = " \n";
	organization newOrganization;
	char* delim = strtok(command, delims);
	strncpy(newOrganization.address, strtok(NULL, delims), LENGTH_ORGANIZATION_ADDRESS);
	strncpy(newOrganization.founder, strtok(NULL, delims), LENGTH_PRINCIPAL);
	newOrganization.firstMemberID = -1;
	newOrganization.IsCurrent = true;
	InsertToIndexFile(&newOrganization);
	handler.organizationDataFile = fopen(handler.dataFileName, "ab+");
	fwrite(&newOrganization, sizeof(organization), 1, handler.organizationDataFile);
	fclose(handler.organizationDataFile);
}

//get-m
void PrintMasterRecord(int index)
{
	organization organization;
	handler.organizationDataFile = fopen(handler.dataFileName, "rb+");
	fseek(handler.organizationDataFile, index * sizeof(organization), SEEK_SET);
	fread(&organization, sizeof(organization), 1, handler.organizationDataFile);
	fclose(handler.organizationDataFile);
	printf("ID: %d Address: %s  Founder: %s\n", organization.id, organization.address, organization.founder);
}

void GetRecordFromMasterFile(char command[LENGTH_OF_COMMAND])
{
	char* delims = " \n";
	char* option = strtok(command, delims);
	int masterID = atoi(strtok(NULL, delims));
	int masterIndex = SearchForMasterIndex(masterID);
	PrintMasterRecord(masterIndex);
}

//update-m
int SearchForMasterIndex(int masterID)
{
	handler.dataIndexFile = fopen(handler.indexFileName, "rb");
	fseek(handler.dataIndexFile,masterID*2*sizeof(int) + sizeof(int), SEEK_SET);
	int masterIndex = -1;
	fread((char*)& masterIndex, sizeof(int), 1, handler.dataIndexFile);
	fread((char*)& masterIndex, sizeof(int), 1, handler.dataIndexFile);
	fclose(handler.dataIndexFile);
	return masterIndex;
	
}

organization OrganizationWithIndex(int index)
{
	handler.organizationDataFile = fopen(handler.dataFileName, "rb+");
	organization organization;
	fseek(handler.organizationDataFile, index * sizeof(organization), SEEK_SET);
	fread(&organization, sizeof(organization), 1, handler.organizationDataFile);
	fclose(handler.organizationDataFile);
	return organization;
}

void DeleteEndOfLine(char* string)
{
	char* ptr = string;
	if ((ptr = strchr(string, '\n')) != NULL)
		* ptr = '\0';
}

void ChangeOrganizationRecord(organization* organization, char* field)
{
	if (!strcmp(field, "Address"))
	{
		fgets(organization->address, LENGTH_ORGANIZATION_ADDRESS, stdin);
		DeleteEndOfLine(organization->address);
	}
	else if (!strcmp(field, "Principal"))
	{
		fgets(organization->founder, LENGTH_PRINCIPAL, stdin);
		DeleteEndOfLine(organization->founder);
	}
	else printf("You can update only existing fields");
}

void UpdateRecord(int index, organization* organization)
{
	handler.organizationDataFile = fopen(handler.dataFileName, "rb+");
	fseek(handler.organizationDataFile, index * sizeof(*organization), SEEK_SET);
	fwrite(organization, sizeof(*organization), 1, handler.organizationDataFile);
	fclose(handler.organizationDataFile);
}

void UpdateMasterFileRecord(char command[LENGTH_OF_COMMAND], char* delims)
{
	char field[LENGTH_OF_FIELD];
	char* delim = strtok(command, delims);
	int id = atoi(strtok(NULL, delims));
	int masterIndex = SearchForMasterIndex(id);
	organization organization = OrganizationWithIndex(masterIndex);
	printf("Which field of record would you like to update?\n(Enter 'Address' to change address, 'Founder' - founder)\n");
	fgets(field, LENGTH_OF_FIELD, stdin);
	DeleteEndOfLine(field);
	ChangeOrganizationRecord(&organization, field);
	UpdateRecord(masterIndex, &organization);
}

//insert-s
member SearchInSlaveFile(int id)
{
	member stud;
	handler.memberDataFile = fopen(handler.memberFileName, "rb");
	for (;;)
	{
		fread(&stud, sizeof(member), 1, handler.memberDataFile);
		if (feof(handler.memberDataFile)) break;
		if (stud.id == id)
		{
			fclose(handler.memberDataFile);
			return stud;
		}
	}
	stud.id = -1;
	fclose(handler.memberDataFile);
	return stud;
}

void AppendAfterLastMemberInOrganization(member* firstMember, member* newMember)
{
	while (firstMember->nextMemberID != -1)
	{
		member memb = (SearchInSlaveFile(firstMember->nextMemberID));
		*firstMember = memb;
	}
	firstMember->nextMemberID = newMember->id;
	handler.memberDataFile = fopen(handler.memberFileName, "rb+");
	for (;;)
	{
		member oldRecord;
		fread(&oldRecord, sizeof(member), 1, handler.memberDataFile);
		if (feof(handler.memberDataFile)) break;
		if (oldRecord.id == firstMember->id)
		{
			oldRecord.nextMemberID = firstMember->nextMemberID;
			fseek(handler.memberDataFile, -1L * (int)sizeof(member), SEEK_CUR);
			fwrite(&oldRecord, sizeof(member), 1, handler.memberDataFile);
			fclose(handler.memberDataFile);
			break;
		}
	}
}

void RewriteOrganizationRecord(organization sch, int index)
{
	handler.organizationDataFile = fopen(handler.dataFileName, "rb+");
	fseek(handler.organizationDataFile, index * sizeof(organization), SEEK_SET);
	fwrite(&sch, sizeof(organization), 1, handler.organizationDataFile);
	fclose(handler.organizationDataFile);
}

void AppendRecordToMaster(int masterID, member* stud)
{
	int index = SearchForMasterIndex(masterID);
	organization organization = OrganizationWithIndex(index);
	if (organization.firstMemberID != -1)
	{
		member firstMember = SearchInSlaveFile(organization.firstMemberID);
		AppendAfterLastMemberInOrganization(&firstMember, stud);
	}
	else
	{
		organization.firstMemberID = stud->id;
		RewriteOrganizationRecord(organization, index);
	}
}

void WriteToSlaveFile(member* stud)
{
	handler.memberDataFile = fopen(handler.memberFileName, "ab+");
	fwrite(stud, sizeof(member), 1, handler.memberDataFile);
	fclose(handler.memberDataFile);
}

void InsertRecordToSlaveFile(char command[LENGTH_OF_COMMAND], char* delims)
{
	int neg = -1;
	char* delim = strtok(command, delims);
	member member,membertemp;
	int masterID = atoi(strtok(NULL, delims));
	handler.memberDataFile = fopen(handler.memberFileName, "rb");
	if (DoesFileHasContent(handler.memberDataFile))
	{
		fseek(handler.memberDataFile, neg* sizeof(member), SEEK_END);
		fread((char*)& membertemp, sizeof(member), 1, handler.memberDataFile);
		fclose(handler.memberDataFile);
		member.id = membertemp.id + 1;
	}
	else
	{
		member.id = 0;
		fclose(handler.memberDataFile);
	}
	
	strncpy(member.name, strtok(NULL, delims), LENGTH_NAME);
	strncpy(member.surname, strtok(NULL, delims), LENGTH_SURNAME);
	strncpy(member.dateOfBirth, strtok(NULL, delims), LENGTH_OF_BIRTH_DATE);
	int contribution = atoi(strtok(NULL, delims));
	member.contribution = contribution;
	member.nextMemberID = -1;
	member.IsCurrent = true;
	AppendRecordToMaster(masterID, &member);
	WriteToSlaveFile(&member);
}

//get-s
void PrintSlaveRecord(member* stud)
{
	printf("ID: %d Name: %s Surname: %s Date Of Birth: %s Contribution: %d\n", stud->id, stud->name, stud->surname, stud->dateOfBirth, stud->contribution);
}

void GetRecordFromSlaveFile(char command[LENGTH_OF_COMMAND], char* delims)
{
	char field[LENGTH_OF_FIELD];
	char* delim = strtok(command, delims);
	int id = atoi(strtok(NULL, delims));
	char* memberIDString = strtok(NULL, delims);
	member stud;
	int index = SearchForMasterIndex(id);
	organization organization = OrganizationWithIndex(index);
	if (organization.firstMemberID != -1)
	{
		stud = SearchInSlaveFile(organization.firstMemberID);
		while (stud.id != -1)
		{
			PrintSlaveRecord(&stud);
			stud = SearchInSlaveFile(stud.nextMemberID);
		}
	}
}

// get-num
void GetNumberOfSlaves(char command[LENGTH_OF_COMMAND], char* delims)
{
	char field[LENGTH_OF_FIELD];
	char* delim = strtok(command, delims);
	int id = atoi(strtok(NULL, delims));
	char* memberIDString = strtok(NULL, delims);
	member stud;
	int index = SearchForMasterIndex(id);
	int res = 0;
	organization organization = OrganizationWithIndex(index);
	if (organization.firstMemberID != -1)
	{
		stud = SearchInSlaveFile(organization.firstMemberID);
		while (stud.id != -1)
		{
			res++;
			stud = SearchInSlaveFile(stud.nextMemberID);
		}
	}
	printf("%d\n", res);
}

//update-s
void ChangeSlaveRecord(member* stud, char* field)
{
	if (!strcmp(field, "Name"))
	{
		fgets(stud->name, LENGTH_NAME, stdin);
		DeleteEndOfLine(stud->name);
	}
	else if (!strcmp(field, "Surname"))
	{
		fgets(stud->surname, LENGTH_SURNAME, stdin);
		DeleteEndOfLine(stud->surname);
	}
	else if (!strcmp(field, "Contribution"))
	{
		scanf("%d", &stud->contribution);
		char c = getchar();
	}
	else if (!strcmp(field, "Date Of Birth"))
	{
		fgets(stud->dateOfBirth, LENGTH_OF_BIRTH_DATE, stdin);
		DeleteEndOfLine(stud->dateOfBirth);
	}
	else printf("You can update only existing fields");
}

void UpdateSlaveRecord(char command[LENGTH_OF_COMMAND], char* delims)
{
	char* option = strtok(command, delims);
	int masterID = atoi(strtok(NULL, delims));
	int memberID = atoi(strtok(NULL, delims));
	int organizationIndex = SearchForMasterIndex(masterID);
	organization sch = OrganizationWithIndex(organizationIndex);
	member stud = SearchInSlaveFile(sch.firstMemberID);
	while (stud.id != memberID && stud.id != -1)
		stud = SearchInSlaveFile(stud.nextMemberID);
	char field[LENGTH_OF_FIELD];
	printf("Enter 'Name' to change name, 'Surname' - surname, 'Contribution' - contribution and 'Date Of Birth' - date of birth\n");
	fgets(field, LENGTH_OF_FIELD, stdin);
	DeleteEndOfLine(field);
	ChangeSlaveRecord(&stud, field);
	handler.memberDataFile = fopen(handler.memberFileName, "rb+");
	for (;;)
	{
		member oldRecord;
		fread(&oldRecord, sizeof(member), 1, handler.memberDataFile);
		if (feof(handler.memberDataFile)) break;
		if (oldRecord.id == stud.id)
		{
			fseek(handler.memberDataFile, -1L * (int)sizeof(member), SEEK_CUR);
			fwrite(&stud, sizeof(member), 1, handler.memberDataFile);
			fclose(handler.memberDataFile);
			break;
		}
	}
}

//del-m
void DeleteRecordFromIndexFile(int id)
{
	handler.dataIndexFile = fopen(handler.indexFileName, "rb");
	FILE* organizationIndexFileTmp = fopen("organization_tmp.ind", "wb");
	int index;
	int masterID;
	int found = 0;
	int nextIndex = -1;
	fread(&nextIndex, sizeof(int), 1, handler.dataIndexFile);
	fwrite(&nextIndex, sizeof(int), 1, organizationIndexFileTmp);
	fseek(handler.dataIndexFile, sizeof(int), SEEK_SET);
	for (;;)
	{
		fread(&masterID, sizeof(int), 1, handler.dataIndexFile);
		fread(&index, sizeof(int), 1, handler.dataIndexFile);
		if (feof(handler.dataIndexFile))
			break;
		if (masterID + found == id)
		{
			printf("Record in index found and deleted\n");
			found = 1;
		}
		else
		{
			index -= found;
			fwrite(&masterID, sizeof(int), 1, organizationIndexFileTmp);
			fwrite(&index, sizeof(int), 1, organizationIndexFileTmp);
		}
	}
	if (!found)
		printf("No records in index file with requested id: %d\n", id);
	fclose(handler.dataIndexFile);
	fclose(organizationIndexFileTmp);
	remove(handler.indexFileName);
	rename("organization_tmp.ind", handler.indexFileName);
}

void DeleteAllSubrecords(int firstMemberID)
{
	int neg = -1;
	if (firstMemberID != -1)
	{
		handler.memberDataFile = fopen(handler.memberFileName, "rb+");
		member stud;
		
		while(firstMemberID != -1)
		{
			
			fseek(handler.memberDataFile, firstMemberID * sizeof(member), SEEK_SET);
			fread(&stud, sizeof(member), 1, handler.memberDataFile);
			/*if (feof(handler.memberDataFile))
				break;*/
			fseek(handler.memberDataFile, firstMemberID * sizeof(member), SEEK_SET);
			printf("Delete subrecord\n");
			firstMemberID = stud.nextMemberID;
			stud.IsCurrent = false;
			

			
			fwrite(&stud, sizeof(member), 1, handler.memberDataFile);
		}
		fclose(handler.memberDataFile);
		
	}
}

void DeleteMasterRecord(char command[LENGTH_OF_COMMAND], char* delims)
{
	int neg = -1;
	char* option = strtok(command, delims);
	int masterID = atoi(strtok(NULL, delims));
	handler.dataIndexFile = fopen(handler.indexFileName, "rb+");
	fseek(handler.dataIndexFile, 2 * (masterID+1) * sizeof(int) + sizeof(int), SEEK_SET);
	int indexID, ID_in_MasterFile;
	while (fread((char*)& indexID, sizeof(int), 1, handler.dataIndexFile))
	{
		fread((char*)& ID_in_MasterFile, sizeof(int), 1, handler.dataIndexFile);
		fseek(handler.dataIndexFile, neg*4*sizeof(int), SEEK_CUR);
		indexID--;
		fwrite((char*)& indexID, sizeof(int), 1, handler.dataIndexFile);
		fwrite((char*)& ID_in_MasterFile, sizeof(int), 1, handler.dataIndexFile);
		fseek(handler.dataIndexFile, 2 * sizeof(int), SEEK_CUR);
	}
	fclose(handler.dataIndexFile);


	handler.organizationDataFile = fopen(handler.dataFileName,"rb+");
	fseek(handler.organizationDataFile, masterID * sizeof(organization), SEEK_SET);
	organization To_del;
	fread((char*)& To_del, sizeof(organization), 1, handler.organizationDataFile);
	To_del.IsCurrent = false;
	
	fseek(handler.organizationDataFile,neg* sizeof(organization), SEEK_CUR);
	fwrite((char*)& To_del, sizeof(organization), 1, handler.organizationDataFile);
	fclose(handler.organizationDataFile);
	DeleteAllSubrecords(To_del.firstMemberID);
	
}

//del-s
int DoesFileHasContent(FILE* f)
{
	fseek(f, 0, SEEK_END);
	unsigned long length = ftell(f);
	if (length != 0) return 1;
	return 0;
}

void RemoveSubrecord(organization* scl, int slaveID)
{
	int recordID = scl->firstMemberID;
	if (recordID != -1)
	{
		member stud;
		member previousMember;
		handler.memberDataFile = fopen(handler.memberFileName, "rb");
		FILE* memberDataTemp = fopen("member_temp.dat", "wb+");
		int found = 0;
		for (;;)
		{
			if (feof(handler.memberDataFile))
				break;
			fread(&stud, sizeof(member), 1, handler.memberDataFile);
			if (stud.id == scl->firstMemberID)
				break;
			fseek(memberDataTemp, 0L, SEEK_END);
			fwrite(&stud, sizeof(member), 1, memberDataTemp);
		}
		fseek(handler.memberDataFile, -1 * (int)sizeof(member), SEEK_CUR);
		for (;;)
		{
			fread(&stud, sizeof(member), 1, handler.memberDataFile);
			if (feof(handler.memberDataFile))
				break;
			if (stud.id == slaveID)
			{
				fclose(memberDataTemp);
				memberDataTemp = fopen("member_temp.dat", "rb+");
				found = 1;
				printf("Record found and deleted.\n");
				if (recordID == stud.id)
					scl->firstMemberID = stud.nextMemberID;
				else
				{
					fseek(memberDataTemp, -1 * (int)sizeof(member), SEEK_END);
					fread(&previousMember, sizeof(member), 1, memberDataTemp);
					previousMember.nextMemberID = stud.nextMemberID;
					fseek(memberDataTemp, -1 * (int)sizeof(member), SEEK_END);
					fwrite(&previousMember, sizeof(member), 1, memberDataTemp);
					fseek(memberDataTemp, 0L, SEEK_END);
				}
			}
			else fwrite(&stud, sizeof(member), 1, memberDataTemp);
		}
		if (!found)
			printf("No record with id: %d\n\n", slaveID);
		fclose(handler.memberDataFile);
		fclose(memberDataTemp);
		remove(handler.memberFileName);
		rename("member_temp.dat", handler.memberFileName);
		return;
	}
	printf("This record doesn't have any subrecords\n");
}

void DeleteSlaveRecord(char command[LENGTH_OF_COMMAND], char* delims)
{
	char* option = strtok(command, delims);
	int masterID = atoi(strtok(NULL, delims));
	int slaveID = atoi(strtok(NULL, delims));
	int masterIndex = SearchForMasterIndex(masterID);
	organization scl = OrganizationWithIndex(masterIndex);
	RemoveSubrecord(&scl, slaveID);
	UpdateRecord(masterIndex, &scl);
}

//clear
void ClearContentOfFiles()
{
	handler.organizationDataFile = fopen(handler.dataFileName, "wb");
	fclose(handler.organizationDataFile);
	handler.dataIndexFile = fopen(handler.indexFileName, "wb");
	fclose(handler.dataIndexFile);
	handler.memberDataFile = fopen(handler.memberFileName, "wb");
	fclose(handler.memberDataFile);
}

void PrintMenu()
{
	printf("Hello user!\n");
	printf("There is a list of command you can use to deal with database:\n");
	printf("1.\"insert-m address founder(Surname_N.P.)\" - to add new organization;\n");
	printf("2.\"insert-s orgID name surname birth_date contribution\" - to add new member record with appropriate information to organization with orgID;\n");
	printf("3.\"ut-m\" - to look information about all organization in database;\n");
	printf("4.\"ut-s\" - to look information about all member in database;\n");
	printf("5.\"get-m ID\" - to look information about organization with ID key;\n");
	printf("6.\"get-s ID\" - to look information about all member in organization with ID key;\n");
	printf("7.\"get-num ID\" - to find a number of members in organization;\n");
	printf("8.\"update-m ID\" - to update record of organization with ID;\n");
	printf("9.\"update-s orgID memID\" - to update record of member with memID in organization with orgID;\n");
	printf("10.\"del-m orgID\" - to delete record with orgID and all their subrecord;\n");
	printf("11.\"del-s orgID memID\" - to delete record of member with memberID.\n");
	printf("12.\"exit\" - to finish the program.\n");
}

void GetCommand(int* DO)
{
	char* delims = " \n";
	char command[LENGTH_OF_COMMAND];
	fgets(command, LENGTH_OF_COMMAND, stdin);
	char copyOfcommand[LENGTH_OF_COMMAND];
	memcpy(copyOfcommand, command, LENGTH_OF_COMMAND);
	char* option = strtok(command, delims);
	if (!strcmp(option, "exit"))
	{
		*DO = false;
		
	}
	else if (!strcmp(option, "ut-m"))
		ShowOrganization();
	else if (!strcmp(option, "ut-s"))
		ShowMembers();
	else if (!strcmp(option, "insert-m"))
		InsertToMasterFile(&copyOfcommand);
	else if (!strcmp(option, "get-m"))
		GetRecordFromMasterFile(&copyOfcommand);
	else if (!strcmp(option, "update-m"))
		UpdateMasterFileRecord(&copyOfcommand, delims);
	else if (!strcmp(option, "insert-s"))
		InsertRecordToSlaveFile(copyOfcommand, delims);
	else if (!strcmp(option, "get-s"))
		GetRecordFromSlaveFile(copyOfcommand, delims);
	else if (!strcmp(option, "get-num"))
		GetNumberOfSlaves(copyOfcommand, delims);
	else if (!strcmp(option, "update-s"))
		UpdateSlaveRecord(copyOfcommand, delims);
	else if (!strcmp(option, "del-m"))
		DeleteMasterRecord(copyOfcommand, delims);
	else if (!strcmp(option, "clear"))
		ClearContentOfFiles();
	else if (!strcmp(option, "del-s"))
		DeleteSlaveRecord(copyOfcommand, delims);
}

int main(int argc, char** argv)
{
	
	PrintMenu();
	handler.indexFileName = "organization.ind";
	handler.dataFileName = "organization.dat";
	handler.memberFileName = "member.dat";
	
	handler.memberDataFile = fopen(handler.memberFileName, "rb");
	if (SizeOfFileIsGreater(handler.memberDataFile))
	{
		fclose(handler.memberDataFile);
		TakeOutMemberRubish();
	}
	else
		fclose(handler.memberDataFile);
	handler.organizationDataFile = fopen(handler.dataFileName, "rb");
	if (SizeOfFileIsGreater(handler.organizationDataFile))
	{
		fclose(handler.organizationDataFile);
		TakeOutOrganizationRubish();
	}
	else
		fclose(handler.organizationDataFile);
	
	handler.dataIndexFile = fopen(handler.indexFileName, "ab");
	if (!DoesFileHasContent(handler.dataIndexFile))
	{
		int firstIndex = 0;
		fwrite(&firstIndex, sizeof(int), 1, handler.dataIndexFile);
	}
	fclose(handler.dataIndexFile);


	bool DO = true;
	while (DO)
		GetCommand(&DO);
	return 0;
}
